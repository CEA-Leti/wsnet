/**
 *  \file   grouparch_config.c
 *  \brief  grouparch configuration module
 *  \author Luiz Henrique Suraty Filho
 *  \date   2016
 */

#if defined(__APPLE__)
#include <stdlib.h>
#else 
#include <malloc.h>
#endif

#include <kernel/include/configuration_parser/grouparch_configuration.h>
#include <kernel/include/configuration_parser/param.h>
#include <kernel/include/definitions/grouparch.h>
#include <kernel/include/definitions/nodearch.h>

// Initialize the default group architecture
grouparch_t *dflt_grouparch = NULL;

/**
 * \brief Print informations about all registered group architectures
 **/
void print_grouparchs(void)
{
	int i;
	int j;

	fprintf(stderr, "\n-----------------------------------");

	for (i = 0; i < grouparchs.size; i++){
		grouparch_t *grouparch;

		grouparch = grouparchs.elts + i;

		fprintf(stderr, "\nGroup architecture '%s' (%d) (nbr_nodes = %d)\n", grouparch->name, grouparch->id, grouparch->nbr_nodes);

		fprintf(stderr, "	- Leader architecture '%s' :(%d)\n", get_nodearch_by_id(grouparch->leader_nodearchid)->name, grouparch->leader_nodearchid);
		fprintf(stderr, "	- Members architecture :\n");
		for (j=0;j<grouparch->membersarch.size;j++){
			nodearch_t *nodearch = get_nodearch_by_id(grouparch->membersarch.elts[j].id);

			fprintf(stderr, "		-> '%s' (%d) - nbr_nodes = %d\n", nodearch->name, nodearch->id ,grouparch->membersarch.elts[j].nbr_nodes);
		}
	}
}

/**
 * \brief Parse the name of a group architecture
 * \param _nd1, the xml group pointer
 * \param grouparch, the group architecture pointer
 * \return 0 in case of success, -1 else
 **/
int parse_grouparch_name(xmlNodePtr _nd1, grouparch_t *_grouparch)
{
  char *grouparch_name;

  if ((grouparch_name = get_xml_attr_content(_nd1, XML_A_NAME)) != NULL)
    {
      _grouparch->name = strdup(grouparch_name);
    }
  else
    {
      fprintf(stderr, "config: grouparch %s: error, while parsing grouparch name\n", _grouparch->name);
      return -1;
    }

  return 0;
}


/**
 * \brief Parse 'default' attribute of a group architecture
 * \param _nd1, the xml group pointer
 * \param grouparch, the group architecture pointer
 * \return 0 in case of success, -1 else
 **/
int parse_grouparch_default(xmlNodePtr _nd1, grouparch_t *_grouparch)
{
	char *grouparch_dflt;

	if ((grouparch_dflt = get_xml_attr_content(_nd1, XML_A_DEFAULT)) != NULL){
		if  (!strcmp(grouparch_dflt, "true") || !strcmp(grouparch_dflt, "1")){
			dflt_grouparch = _grouparch;
			DBG_GROUPARCH("Group architecture '%s': default group architecture\n", _grouparch->name);
		}
	}

	return 0;
}

/**
 * \brief Parse 'default' attribute of a group architecture
 * \param _nd1, the xml group pointer
 * \param grouparch, the group architecture pointer
 * \return 0 in case of success, -1 else
 **/
int parse_grouparch_type(xmlNodePtr _nd1, grouparch_t *_grouparch)
{
	char *grouparch_type;

	if ((grouparch_type = get_xml_attr_content(_nd1, XML_A_TYPE)) != NULL){
		if  (!strcmp(grouparch_type, "BAN") || !strcmp(grouparch_type, "ban")){
			_grouparch->type = GROUP_TYPE_BAN;
			DBG_GROUPARCH("Group architecture '%s': group type %s\n", _grouparch->name, grouparch_type);
		}
	}

	return 0;
}



/**
 * \brief Count the number of classes of the group architecture
 * \param nd1, the xml group pointer
 * \param grouparch, the group architecture pointer
 * \return 0 in case of success, -1 else
 **/
int parse_grouparch_count(xmlNodePtr _nd1, grouparch_t *grouparch)
{
	xmlNodePtr nd2;

	for (nd2 = _nd1->children; nd2; nd2 = nd2->next)
		if (!strcmp((char *) nd2->name, XML_E_MEMBER))
				grouparch->membersarch.size++;

	return 0;
}

/**
 * \brief Allocate memory for class links of the group architecture
 * \param nd1, the xml group pointer
 * \param grouparch, the group architecture pointer
 * \return 0 in case of success, -1 else
 **/
int parse_grouparch_allocate(xmlNodePtr _nd1, grouparch_t *_grouparch)
{

	if ( _grouparch->membersarch.size == 0){
		_grouparch->membersarch.elts = NULL;
		return 0;
	}
	// allocate node architectures
	if ((_grouparch->membersarch.elts = (memberarch_t *) malloc(sizeof(memberarch_t) * _grouparch->membersarch.size)) == NULL){
		fprintf(stderr, "config4: malloc error (parse_grouparch_allocate())\n");
		return -1;
	}

	return 0;
}

/**
 * \brief Parse the node architecture of the leader of the group architecture
 * \param nd1, the xml group pointer
 * \param grouparch, the group architecture pointer
 * \return 0 in case of success, -1 else
 **/
int parse_grouparch_leader(xmlNodePtr _nd1, grouparch_t *_grouparch)
{
	xmlNodePtr nd2;
	char *leader_name;

	for (nd2 = _nd1->children; nd2; nd2 = nd2->next){
		if ( (!strcmp((char *) nd2->name, XML_E_LEADER)) && ( (leader_name = get_xml_attr_content(nd2, XML_A_ARCHITECTURE)) != NULL ) ){
			_grouparch->leader_nodearchid = get_nodearchid_by_name(leader_name);
			_grouparch->nbr_nodes++;
			return 0;
		}
	}

	fprintf(stderr, "config: grouparch '%s': no element 'leader' found (parse_grouparch_leader())\n", _grouparch->name);
	return -1;
}

/**
 * \brief Parse the node architecture of the members of the group architecture
 * \param nd1, the xml group pointer
 * \param grouparch, the group architecture pointer
 * \return 0 in case of success, -1 else
 **/
int parse_grouparch_members(xmlNodePtr _nd1, grouparch_t *_grouparch)
{
	xmlNodePtr nd2;
	int i =0;
	char *member_name;
	char *max_number;

	if (_grouparch->membersarch.size == 0)
		return 0;

	for (nd2 = _nd1->children; nd2; nd2 = nd2->next){
		if ( (!strcmp((char *) nd2->name, XML_E_MEMBER)) && ( (member_name = get_xml_attr_content(nd2, XML_A_ARCHITECTURE) ) != NULL ) ){
			_grouparch->membersarch.elts[i].id = get_nodearchid_by_name(member_name);
			if ((max_number = get_xml_attr_content(nd2, XML_A_NBR_NODES) ) != NULL){
				get_param_integer(max_number,&(_grouparch->membersarch.elts[i].nbr_nodes));
				_grouparch->nbr_nodes += _grouparch->membersarch.elts[i].nbr_nodes;
			}
			i++;
		}
	}

	if (i>0)
		return 0;

	fprintf(stderr, "config: grouparch '%s': no element 'members' found (parse_grouparch_members())\n", _grouparch->name);
	return -1;
}


/**
 * \brief Parse a group architecture
 * \param nd1, the xml group pointer
 * \param id, the identifier of the group architecture
 * \return 0 in case of success, -1 else
 **/
int parse_grouparch(xmlNodePtr _nd1, int _id)
{
  grouparch_t *grouparch = get_grouparch_by_id(_id);

  if (parse_grouparch_name(_nd1, grouparch))
      return -1;

  if (parse_grouparch_type(_nd1, grouparch))
	  return -1;

  DBG_GROUPARCH("\n===Parsing group architecture '%s'===\n", grouparch->name);

  if (parse_grouparch_default(_nd1, grouparch))
      return -1;



  /* count classes */
  if (parse_grouparch_count(_nd1, grouparch))
      return -1;

  if (parse_grouparch_allocate(_nd1, grouparch))
      return -1;

  if (parse_grouparch_leader(_nd1, grouparch))
       return -1;

  if (parse_grouparch_members(_nd1, grouparch))
       return -1;

  return 0;
}

int parse_grouparchs(xmlNodeSetPtr _groupset){
	int i;

	DBG_GROUPARCH("\n\n==============GROUP ARCHITECTURES===============\n");

	if(grouparchs.size == 0)
	  return 0;

	// allocate memory
	if (grouparchs_malloc())
		return -1;

	// init grouparchs
	for (i = 0 ; i < grouparchs.size; i++)
		grouparch_init(i);

	// parse grouparchs
	for (i = 0 ; i < grouparchs.size; i++)
		if (parse_grouparch(_groupset->nodeTab[i], i))
			return -1;

	// print the parsed values
	print_grouparchs();

	return 0;
}
