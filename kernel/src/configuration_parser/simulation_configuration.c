/**
 *  \file   simulation_config.c
 *  \brief  Simulation configuration module
 *  \author Guillaume Chelius & Loic Lemaitre
 *  \date   2007
 */

#include <kernel/include/scheduler/scheduler.h>
#include <kernel/include/model_handlers/topology.h>
#include <kernel/include/tools/math/rng/rng.h>

#include "nodearch_configuration.h"
#include "grouparch_configuration.h"
#include "simulation_configuration.h"
#include "param.h"


/* ************************************************** */
/* ************************************************** */
/**
 * \brief Print informations about simulation
 **/
void print_simulation(void)
{
	int title;
	position_t *g_area = get_topology_area();

	fprintf(stderr, "\n-----------------------------------");
	fprintf(stderr, "\nSimulation will run using:\n");
	fprintf(stderr, "   - nodes      : %d\n", nodes.size);
	fprintf(stderr, "   - groups     : %d\n", groups.size);

	if (scheduler_get_end()) {
		fprintf(stderr, "   - duration   : %" PRId64 " ns\n", scheduler_get_end());
	}
	else {
		fprintf(stderr, "   - duration   : unlimited\n");
	}

	if (global_map != -1) {
		fprintf(stderr, "   - global map : %s -> area = {%g, %g, %g}\n",
				get_class_by_id(global_map)->name, g_area->x, g_area->y, g_area->z);
	}

	if (monitors.elts != NULL) {
		int i;
		title = 0;
		for (i = 0; i < monitors.size; i++)
		{
			if (title == 0)
			{
				fprintf(stderr, "   - monitors   : ");
				title = 1;
			}
			else
			{
				fprintf(stderr, ", ");
			}
			fprintf(stderr, "%s", get_class_by_id(monitors.elts[i])->name);
		}
	}

	fprintf(stderr, "\n-----------------------------------\n\n");
}



/* ************************************************** */
/* ************************************************** */
/**
 * \brief Parse 'node' element of the xml configuration file 
 * \param nd1, the xml node pointer
 * \return 0 in case of success, -1 else
 **/
int parse_simulation_nodes(xmlNodePtr nd1, int *birth_spcfd)
{
	xmlNodePtr nd2;
	int i;
	nodearch_t *nodearch;
	node_t *node;

	/* create nodes */
	if(nodes_create()) {
		fprintf(stderr, "config: error when creating nodes (parse_simulation_nodes())\n");
		return -1;
	}

	/* check for a default node architecture*/
	if (dflt_nodearch == NULL) {
		//fprintf(stderr, "config: no default node architecture defined, assigning one node for each architecture\n");
		/* assigning one node for each architecture */
		for (i = 0; i < nodes.size; i++) {
			//fprintf(stderr, "Node %d with Arch %s (arch_id=%d)\n", i, nodearchs.elts[i%nodearchs.size].name, i%nodearchs.size);
			node = get_node_by_id(i);
			node->nodearch = nodearchs.elts[i%nodearchs.size].id;
			node->birth = 0;
			birth_spcfd[i] = 0;
		}

	}
	else {

		/* init node architecture to default one */
		for (i = 0; i < nodes.size; i++) {
			node = get_node_by_id(i);
			node->nodearch = dflt_nodearch->id;
			node->birth = 0;
			birth_spcfd[i] = 0;
		}
	}

	/* parse nodes */
	for (nd2 = nd1->children; nd2; nd2 = nd2->next)	{
		if (!strcmp((char *) nd2->name, XML_E_NODE)) {
			/* get node id */
			char *nodeid_str;
			int nodeid;
			if ((nodeid_str = get_xml_attr_content(nd2, XML_A_ID)) != NULL) {
				nodeid = strtoll(nodeid_str, NULL, 10);
				node = get_node_by_id(nodeid);
				DBG_SIMULATION("Simulation: **Parsing node %u**\n", nodeid);
			}
			else {
				fprintf(stderr, "config: error when parsing node: id not found (parse_simulation_nodes())\n");
				return -1;
			}

			/* get node architecture */
			char *arch;
			if ((arch = get_xml_attr_content(nd2, XML_A_ARCHITECTURE)) == NULL) {
				fprintf(stderr, "config: error when parsing node: node architecture not found (parse_simulation_nodes())\n");
				return -1;
			}

			/* eventually override node architecture */
			if ((arch != NULL) && (nodeid != -1)) {
				nodearch = get_nodearch_by_name(arch);
				node->nodearch = nodearch->id;
				DBG_SIMULATION("Simulation: Node %u: architecture = %s\n", nodeid, get_nodearch_by_id(node->nodearch)->name);
			}

			/* get node birth */
			char *birth_str;
			if ((birth_str = get_xml_attr_content(nd2, XML_A_BIRTH)) != NULL) {
				birth_spcfd[nodeid] = 1;
				get_param_time(birth_str, &(node->birth));
				DBG_SIMULATION("Simulation: Node %u: birth = %s\n", nodeid, birth_str);
			}

			/* parse node parameters */
			xmlNodePtr nd3;
			for (nd3 = nd2->children; nd3; nd3 = nd3->next) {
				if (!strcmp((char *) nd3->name, XML_E_PARAMETERS)) {
					char    *class_name;
					class_t *class;
					if ((class_name = get_xml_attr_content(nd3, XML_A_CLASS)) != NULL) {
						class = get_class_by_name(class_name);
						/* check if this class exist */
						if (class == NULL) {
							fprintf(stderr, "config: error: class '%s' not declared (parse_simulation_nodes())\n",
									class_name);
							return -1;
						}
						/* check if this class is in the node architecture */
						else {
							if (class->nodearchs.elts[node->nodearch] == -1) {
								fprintf(stderr, "config: error: class '%s' not declared in the node architecture '%s' (parse_simulation_nodes())\n", class_name, get_nodearch_by_id(node->nodearch)->name);
								return -1;
							}
						}
					}
					else {
						fprintf(stderr, "config: error when parsing node: wrong parameter (parse_simulation_nodes())\n");
						return -1;
					}

					/* get default class parameters and duplicate them */
					dflt_param_t *dflt_param;
					if ((dflt_param = (dflt_param_t *) malloc(sizeof(dflt_param_t))) == NULL) {
						fprintf(stderr, "config: malloc error (parse_simulation_nodes())\n");
						return -1;
					}
					if ((dflt_param->params = list_create()) == NULL) {
						free(dflt_param);
						fprintf(stderr, "config: error when list parameter list creation (parse_simulation_nodes())\n");
						return -1;
					}
					dflt_param_t *dflt_class_param = get_class_params(-1, class->id, node->nodearch, -1, -1);
					if (dflt_class_param) {
						/* this class has default parameters, so duplicate them */
						if (duplicate_class_params(dflt_class_param->params, dflt_param->params)) {
							//free(dflt_param);
							fprintf(stderr, "config: error when duplicating parameters (parse_simulation_nodes())\n");
							return -1;
						}
						dflt_param->mediumid      = dflt_class_param->mediumid;
						dflt_param->environmentid = dflt_class_param->environmentid;
					}
					else {
						dflt_param->mediumid      = -1;
						dflt_param->environmentid = -1;
					}
					dflt_param->nodeid        = node->id;
					dflt_param->classid       = class->id;
					dflt_param->nodearchid    = node->nodearch;

					/* parse node class parameters */
					xmlNodePtr nd4;
					for (nd4 = nd3->children; nd4; nd4 = nd4->next)	{
						if (!strcmp((char *) nd4->name, XML_E_PARAM)) {
							param_t *node_class_param;
							char    *key_content;
							char    *val_content;
							if (((key_content = get_xml_attr_content(nd4, XML_A_KEY)) != NULL)
									&& ((val_content = get_xml_attr_content(nd4, XML_A_VALUE)) != NULL)){
								/* check if parameter is present into class default parameter, if yes, override it */
								if ((node_class_param = find_class_param_key(dflt_param->params, key_content)) != NULL) {
									/* override medium class value */
									node_class_param->value = val_content;
									DBG_SIMULATION("Simulation: Node %d: class '%s': Override param key '%s'\n",
											node->id, class->name, key_content);
									DBG_SIMULATION("Simulation: Node %d: class '%s': Override param value '%s'\n",
											node->id, class->name, val_content);
								}
								else {
									/* parameter not in class default parameter, so add parameter it */
									param_t *param;
									if ((param = (param_t *) malloc(sizeof(param_t))) == NULL) {
										free(dflt_param);
										fprintf(stderr, "config: malloc error (parse_simulation_nodes())\n");
										return -1;
									}
									param->key   = key_content;
									param->value = val_content;
									DBG_SIMULATION("Simulation: Node %d: class '%s': Save param key '%s'\n",
											node->id, class->name, key_content);
									DBG_SIMULATION("Simulation: Node %d: class '%s': Save param value '%s'\n",
											node->id, class->name, val_content);
									list_insert(dflt_param->params, param);
								}
							}
							else {
								free(dflt_param);
								fprintf(stderr, "config: error when parsing node: 'key' word not found (parse_simulation_nodes())\n");
								return -1;
							}
						}
					}
					list_insert(dflt_params, dflt_param);
				}
			}
		}
	}

	return 0;
}


/* ************************************************** */
/* ************************************************** */
/**
 * \brief Parse 'node' element of the xml configuration file 
 * \param nd1, the xml node pointer
 * \return 0 in case of success, -1 else
 **/
int parse_simulation_set_nodes(xmlNodePtr nd1, int *birth_spcfd)
{
	int i;
	node_t *node;
	nodearch_t *nodearch;

  /* for all nodes */
	for (i = 0; i < nodes.size; i++) {
		int j;

		node = get_node_by_id(i);
		nodearch = get_nodearch_by_id(node->nodearch);

		/* allocate pointer array for private memory */
		if ((node->private = malloc(sizeof(void *) * nodearch->classes.size)) == NULL) {
			fprintf(stderr, "config: malloc error (parse_simulation_nodes())\n");
			return -1;
		}

		/* for all classes, call setnode */
		for (j  = 0; j < nodearch->classes.size; j++) {
			class_t *class = get_class_by_id(nodearch->classes.elts[j]);
			dflt_param_t *dflt_param = get_class_params(node->id, class->id, node->nodearch, -1, -1);
			void *params = NULL;
			int create = 0;
			int ok = 0;


			if (dflt_param == NULL) {
				create = 1;
				if ((params = list_create()) == NULL)
				{
					fprintf(stderr, "config: error when parsing node: list creation failed (parse_simulation_nodes())\n");
					return -1;
				}
			}
			else {
				params = dflt_param->params;
			}
			/* call bind */
			if (class->bind) {
				call_t to = {class->id, node->id};
				ok = class->bind(&to, params);
			}
			if (class->create_object) {
				call_t to = {class->id, node->id};
				void* key = (void*) &node->id;
				int* index = malloc(sizeof(int));
				*index = class->objects.size++;
				list_insert(class->objects.saved_indexes,(void*)index);
				hashtable_insert(class->objects.indexes,key, (void*)index);
				class->objects.object[*index] = class->create_object(&to, params);
			}

			if (create) {
				list_destroy(params);
			}

			if (ok) {
				fprintf(stderr, "config: error: binding class %s with node %d failed (parse_simulation_nodes())\n",
						class->name, node->id);
				return -1;
			}
		}

		/* program birth */
		if (birth_spcfd[i] == 0) {
			get_param_time(nodearch->birth?nodearch->birth:"0", &(node->birth));
		}

		// update node types
		node->types = nodearch->types;

	}


	return 0;
} 


/* ************************************************** */
/* ************************************************** */
/**
 * \brief Parse 'monitor' element of the xml configuration file 
 * \param nd1, the xml node pointer
 * \return 0 in case of success, -1 else
 **/
int parse_simulation_monitor(xmlNodePtr nd1) {
	xmlNodePtr nd2;
	xmlNodePtr nd3;
	param_t *param;
	void *params;
	char *monitor_name;
	class_t *class;
	int count = 0;

	DBG_SIMULATION("Simulation: **Parsing monitor**\n");

	/* count */
	for (nd2 = nd1->children; nd2; nd2 = nd2->next) {
		if (!strcmp((char *) nd2->name, XML_E_MONITOR))
		{
			monitors.size++;
		}
	}

	/* allocate */
	if (monitors.size != 0) {
		if ((monitors.elts = (int *) malloc(sizeof(int) * monitors.size)) == NULL) {
			fprintf(stderr, "config: malloc error (parse_environments())\n");
			return -1;
		}
	}

	/* parse */
	for (nd2 = nd1->children; nd2; nd2 = nd2->next) {
		if (!strcmp((char *) nd2->name, XML_E_MONITOR)) {
			if ((monitor_name = get_xml_attr_content(nd2, XML_A_NAME)) != NULL) {
				/* retrieve class */
				class = get_class_by_name(monitor_name);
				if (class == NULL) {
					fprintf(stderr, "config: error: class '%s' not declared (parse_simulation_monitor())\n", monitor_name);
					return -1;
				}

				monitors.elts[count++] = class->id;

				/* get parameters */
				if ((params = list_create()) == NULL) {
					return -1;
				}
				for (nd3 = nd2->children ; nd3 ; nd3 = nd3->next) {
					if (!strcmp((char *) nd2->name, XML_E_PARAM)) {
						if ((param = (param_t *) malloc(sizeof(param_t))) == NULL) {
							fprintf(stderr, "config: malloc error (parse_class_param())\n");
							return -1;
						}

						char *content;
						if ((content = get_xml_attr_content(nd3, XML_A_KEY)) != NULL) {
							param->key = content;
							DBG_SIMULATION("Simulation: class '%s': Save param key '%s'\n", class->name, content);
						}
						if ((content = get_xml_attr_content(nd3, XML_A_VALUE)) != NULL) {
							param->value = content;
							DBG_SIMULATION("Simulation: class '%s': Save param value '%s'\n", class->name, content);
						}

						list_insert(params, param);
					}
				}

				/* bind */
				if (class->bind) {
					call_t to = {class->id, -1};
					if (class->bind(&to, params))
					{
						fprintf(stderr, "config: error when binding class (parse_simulation_monitor())\n");
						return -1;
					}
				}
				list_destroy(params);
			}
			else {
				fprintf(stderr, "config: attribute name not found (parse_simulation_monitor())\n");
				return -1;
			}
		}
	}

	return 0;
}


/* ************************************************** */
/* ************************************************** */
/**
 * \brief Parse 'global_map' element of the xml configuration file 
 * \param nd1, the xml node pointer
 * \return 0 in case of success, -1 else
 **/
int parse_simulation_global_map(xmlNodePtr nd1) {
	xmlNodePtr nd2;
	char *map_name;
	class_t *class;

	DBG_SIMULATION("Simulation: **Parsing global map**\n");

	for (nd2 = nd1->children; nd2; nd2 = nd2->next)
	{
		if (!strcmp((char *) nd2->name, XML_E_GLOBAL_MAP)) {
			if ((map_name = get_xml_attr_content(nd2, XML_A_NAME)) != NULL) {
				class = get_class_by_name(map_name);
				if (class == NULL) {
					fprintf(stderr, "config: error: class '%s' not declared (parse_simulation_global_map())\n", map_name);
					return -1;
				}
				//global_map = class->id;
				if (class->bind) {
					call_t to = {class->id, -1};
					if (class->bind(&to, NULL))
					{
						fprintf(stderr, "config: error when binding class (parse_simulation_global_map())\n");
						return -1;
					}
				}
			}
			else {
				fprintf(stderr, "config: attribute name not found (parse_simulation_global_map())\n");
				return -1;
			}
		}
	}

	return 0;
}


/* ************************************************** */
/* ************************************************** */
/**
 * \brief Parse 'implementation' element of the xml configuration file 
 * \param nd1, the xml node pointer
 * \return 0 in case of success, -1 else
 **/
int parse_simulation_implem(xmlNodePtr nd1) {
	xmlNodePtr nd2;

	DBG_SIMULATION("Simulation: **Parsing implementation**\n");

	for (nd2 = nd1->children; nd2; nd2 = nd2->next) {
		if (!strcmp((char *) nd2->name, XML_E_IMPLEMENTATION)) {
			xmlNodePtr nd3;
			for (nd3 = nd2->children; nd3; nd3 = nd3->next) {
				if (!strcmp((char *) nd2->name, XML_E_PARAM)){
					char *key_content;
					//char *val_content;
					if ((key_content = get_xml_attr_content(nd1, XML_A_KEY)) != NULL) {
						/* implementation parameter */
						/* 		      if (!strcmp(key_content, XML_C_XXXXX)) */
						/* 			{ */

						/* 			} */

						/* other implementation parameter */
						/* 		      if (!strcmp(key_content, XML_C_XXXXX)) */
						/* 			{ */

						/* 			} */

					}
				}
			}
			return 0;
		}

	}

	return 0;
}


/* ************************************************** */
/* ************************************************** */
int parse_simulation_node_nbr(xmlNodeSetPtr nodeset) {
	xmlNodePtr nd1 = nodeset->nodeTab[0];
	char *nodes_number_str;

	/* get number of nodes */
	if ((nodes_number_str = get_xml_attr_content(nd1, XML_A_NODES)) != NULL) {
		nodes.size = strtoll(nodes_number_str, NULL, 10);
		DBG_SIMULATION("Simulation: number of nodes = %d\n", nodes.size);
	}
	else {
		return -1;
	}

	return 0;
}

/* ************************************************** */
/* ************************************************** */
int parse_simulation_group_nbr(xmlNodeSetPtr groupset) {
	xmlNodePtr nd1 = groupset->nodeTab[0];
	char *groups_number_str;

	/* get number of groups */
	if ((groups_number_str = get_xml_attr_content(nd1, XML_A_GROUPS)) != NULL) {
		groups.size = strtoll(groups_number_str, NULL, 10);
		DBG_SIMULATION("Simulation: number of groups = %d\n", groups.size);
	}
	else {
		groups.size = calculate_default_number_of_groups();
	}

	if (check_groups_size()){
		fprintf(stderr, "config: error when counting groups (parse_simulation_group_nbr())\n");
		return -1;
	}
	else{
		return 0;
	}
}

/* ************************************************** */
/* ************************************************** */
int parse_simulation_groups(xmlNodePtr nd1){

	int i;
	int j;
	int k;
	int l;

	if (groups.size == 0)
		return 0;

	// create groups
	if(groups_create()){
		fprintf(stderr, "config: error when creating groups (parse_simulation_groups())\n");
		return -1;
	}

	array_t default_leaders = get_nodesid_by_nodearchid(dflt_grouparch->leader_nodearchid);

	array_t *default_members = (array_t *) malloc(sizeof(array_t)*dflt_grouparch->membersarch.size);

	for (j=0;j<dflt_grouparch->membersarch.size;j++){
		*(default_members+j) = get_nodesid_by_nodearchid(dflt_grouparch->membersarch.elts[j].id);
		// for each member architecture check whether we have the number of nodes with the architecture
		if ( groups.size*dflt_grouparch->membersarch.elts[j].nbr_nodes > (default_members+j)->size ) {
			fprintf(stderr, "config: Number of elements of architecture %s declared on group %s does not correspond (parse_simulation_groups())\n", get_nodearch_by_id(dflt_grouparch->membersarch.elts[j].id)->name, get_grouparch_by_id(dflt_grouparch->id)->name);
			return -1;
		}
	}

	// set all groups with default value
	for (i=0;i<groups.size;i++){
		l=0;
		group_t *group 	= get_group_by_id(i);
		group->grouparch = dflt_grouparch->id;
		group->leaderid = default_leaders.elts[i];
		add_group_to_node(default_leaders.elts[i], group->id);
		group->size = dflt_grouparch->nbr_nodes;
		group->type = dflt_grouparch->type;
		// memory allocation
		for (j=0;j<dflt_grouparch->membersarch.size;j++){
			group->membersid.size += dflt_grouparch->membersarch.elts[j].nbr_nodes ;
		}
		group->membersid.elts = (int*) malloc (sizeof(int)*group->membersid.size);

		fprintf(stderr, "\nGroupArch %s with id=%d has a total of %d nodes (%d leader(s) and %d members)\n", get_grouparch_by_id(group->grouparch )->name,group->id, group->size, 1, group->membersid.size );
		fprintf(stderr, "     Leader with arch %s -> [%d]\n", get_nodearch_by_id(dflt_grouparch->leader_nodearchid)->name,group->leaderid );


		// set default values
		for (j=0;j<dflt_grouparch->membersarch.size;j++) {
			int nbr_nodes = dflt_grouparch->membersarch.elts[j].nbr_nodes;
			fprintf(stderr, "     Members with arch %s (nbr_nodes=%d)-> [ ", get_nodearch_by_id(dflt_grouparch->membersarch.elts[j].id)->name,nbr_nodes);
			for (k=0;k<nbr_nodes;k++){
				group->membersid.elts[l] = (default_members+j)->elts[i*nbr_nodes+k];
				fprintf(stderr, "%d ",group->membersid.elts[l]);
				l++;
				if (add_group_to_node((default_members+j)->elts[i*nbr_nodes+k], group->id)==-1){
					fprintf(stderr, "config: Could not realloc group to node\n");
					return -1;
				}

			}
			fprintf(stderr, "]\n");
		}


	}

	// free memory
	free(default_leaders.elts);
	for (j=0;j<dflt_grouparch->membersarch.size;j++)
		free((default_members+j)->elts);

	free(default_members);

	return 0;

}


void simulation_set_global_map(int id) {
	global_map = id;
}


int parse_simulation_begin(xmlNodeSetPtr nodeset) {
	xmlNodePtr nd1 = nodeset->nodeTab[0];
	char *duration_str;

	DBG_SIMULATION("\n\n===============SIMULATION================\n");
	DBG_SIMULATION("\n===Begin parsing simulation===\n");

	/* init */
	monitors.size = 0;
	monitors.elts = NULL;

	/* get duration of the simulation */
	if ((duration_str = get_xml_attr_content(nd1, XML_A_DURATION)) != NULL) {
		uint64_t duration;
		get_param_time(duration_str, &duration);
		scheduler_set_end(duration);
		DBG_SIMULATION("Simulation: duration = %s\n", duration_str);
	}

	/* parse implementation element */
	/*   if (parse_simulation_implem(nd1)) */
	/*     { */
	/*       return -1; */
	/*     } */

	/* parse monitor element */
	if (parse_simulation_monitor(nd1)) {
		return -1;
	}

	/* parse map element */
	if (parse_simulation_global_map(nd1)) {
		return -1;
	}

	return 0;
}


int parse_simulation_end(xmlNodeSetPtr nodeset) {
	xmlNodePtr nd1 = nodeset->nodeTab[0];
	int birth_spcfd[nodes.size];

	DBG_SIMULATION("\n\n===============SIMULATION================\n");
	DBG_SIMULATION("\n===End parsing simulation===\n");

	/* parse node elements */
	if (parse_simulation_nodes(nd1, birth_spcfd)) {
		return -1;
	}

	/* display simulation informations */
	print_simulation();

	if (parse_simulation_groups(nd1)) {
		return -1;
	}
	/* set node parameters */
	if (parse_simulation_set_nodes(nd1, birth_spcfd)) {
		return -1;
	}

	return 0;
}
