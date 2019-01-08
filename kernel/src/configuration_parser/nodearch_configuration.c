/**
 *  \file   nodearch_config.c
 *  \brief  nodearch configuration module
 *  \author Loic Lemaitre
 *  \date   2010
 */

#include "nodearch_configuration.h"


/* ************************************************** */
/* ************************************************** */
nodearch_t *dflt_nodearch = NULL;
int class_position = 0;


/* ************************************************** */
/* ************************************************** */
/* parse a layer class of CLASS_TYPE */
#define PARSE_NODEARCH_LAYER_CLASS(CLASS_TYPE, XML_TAG)			\
    do									\
    {									\
      int prev_class_position = class_position;				\
      int i;								\
      class_t *class;							\
      int position = 0;							\
      \
      /* count and configure*/						\
    for (nd3 = nd2->children; nd3; nd3 = nd3->next)			\
    {									\
      if (!strcmp((char *) nd3->name, XML_TAG))			\
      {								\
        char *class_name;						\
        if ((class_name = get_xml_attr_content(nd3, XML_A_NAME)) != NULL) \
        {								\
          if ((class = get_class_by_name(class_name)) == NULL)	\
          {							\
            fprintf(stderr, "config: wrong class '%s' has been used (parse_nodearch_implem())\n", class_name); \
            return -1;						\
          }							\
          \
          nodearch->CLASS_TYPE.size++;				\
          \
          /* include class in node architecture and vice-versa */	\
    nodearch->classes.elts[class_position] = class->id;	\
    class->nodearchs.elts[nodearch->id]  = class_position;	\
    \
    DBG_NODEARCH("Node architecture '%s': class '%s': **Parsing updown**\n", \
        nodearch->name, class->name);			\
        /* eventually parse up and down layers */		\
    if (parse_nodearch_updown(nd3, nodearch, class_position)) \
    return -1;						\
    \
    DBG_NODEARCH("Node architecture '%s': class '%s': **Parsing parameters**\n",	\
        nodearch->name, class->name);			\
        /* save class parameters for this node architecture */	\
    parse_nodearch_class_params(nd3, nodearch, class, NULL, NULL); \
    \
    class_position++;					\
        }								\
        else							\
        {								\
          return -1;						\
        }								\
      }								\
    }									\
    \
    /* allocate */							\
    if (nodearch->CLASS_TYPE.size != 0)					\
    {									\
      if ((nodearch->CLASS_TYPE.elts = (int *) malloc(sizeof(int) * nodearch->CLASS_TYPE.size)) == NULL) \
      {								\
        fprintf(stderr, "config: malloc error (parse_nodearch_implem())\n"); \
        return -1;							\
      }								\
      \
      /* fill */							\
    for (i = prev_class_position; i < class_position; i++)		\
    {								\
      class = get_class_by_id(nodearch->classes.elts[i]);		\
      nodearch->CLASS_TYPE.elts[position] = class->id;		\
      position++;							\
    }								\
    }								\
    \
    } while(0)


/* parse a class of CLASS_TYPE, its link is stored in a table (there may be several classes of this type) */
#define PARSE_NODEARCH_CLASS_ARRAY(CLASS_TYPE, XML_TAG)			\
    do									\
    {									\
      int prev_class_position = class_position;				\
      int i;								\
      class_t *class;							\
      int position = 0;							\
      \
      /* count and configure*/						\
    for (nd2 = nd1->children; nd2; nd2 = nd2->next)			\
    {									\
      if (!strcmp((char *) nd2->name, XML_TAG))			\
      {								\
        char *class_name;						\
        if ((class_name = get_xml_attr_content(nd2, XML_A_NAME)) != NULL) \
        {								\
          if ((class = get_class_by_name(class_name)) == NULL)	\
          {							\
            fprintf(stderr, "config: wrong class '%s' has been used (parse_nodearch_classes())\n", class_name); \
            return -1;						\
          }							\
          nodearch->CLASS_TYPE.size++;				\
          \
          /* include class in node architecture and vice-versa */	\
    nodearch->classes.elts[class_position] = class->id;	\
    class->nodearchs.elts[nodearch->id]  = class_position;	\
    \
    DBG_NODEARCH("Node architecture '%s': class '%s': **Parsing parameters**\n",	\
        nodearch->name, class->name);			\
        /* save class parameters for this node architecture */	\
    parse_nodearch_class_params(nd2, nodearch, class, NULL, NULL); \
    \
    class_position++;					\
        }								\
        else							\
        {								\
          return -1;						\
        }								\
      }								\
    }									\
    \
    \
    /* allocate */							\
    if (nodearch->CLASS_TYPE.size != 0)					\
    {									\
      if ((nodearch->CLASS_TYPE.elts = (int *) malloc(sizeof(int) * nodearch->CLASS_TYPE.size)) == NULL) \
      {								\
        fprintf(stderr, "config: malloc error (parse_nodearch_classes())\n"); \
        return -1;							\
      }								\
      \
      /* fill */							\
    for (i = prev_class_position; i < class_position; i++)		\
    {								\
      class = get_class_by_id(nodearch->classes.elts[i]);		\
      nodearch->CLASS_TYPE.elts[position] = class->id;		\
      position++;							\
    }								\
    }									\
    \
    } while(0)


/* parse a class of CLASS_TYPE, its link is stored as an integer id (there may be only one class of this type) */
#define PARSE_NODEARCH_CLASS_INT(CLASS_TYPE, XML_TAG)			\
    do									\
    {									\
      class_t *class;							\
      \
      /* count and configure*/						\
    for (nd2 = nd1->children; nd2; nd2 = nd2->next)			\
    {									\
      if (!strcmp((char *) nd2->name, XML_TAG))			\
      {								\
        char *class_name;						\
        if ((class_name = get_xml_attr_content(nd2, XML_A_NAME)) != NULL) \
        {								\
          if ((class = get_class_by_name(class_name)) == NULL)	\
          {							\
            fprintf(stderr, "config: wrong class '%s' has been used (parse_nodearch_classes())\n", class_name); \
            return -1;						\
          }							\
          nodearch->CLASS_TYPE = class->id;			\
          \
          /* include class in node architecture and vice-versa */	\
    nodearch->classes.elts[class_position] = class->id;	\
    class->nodearchs.elts[nodearch->id]  = class_position;	\
    \
    DBG_NODEARCH("Node architecture '%s': class '%s': **Parsing parameters**\n",	\
        nodearch->name, class->name);			\
        /* save class parameters for this node architecture */	\
    parse_nodearch_class_params(nd2, nodearch, class, NULL, NULL); \
    \
    class_position++;					\
        }								\
        else							\
        {								\
          return -1;						\
        }								\
      }								\
    }									\
    \
    } while(0)


/* print a layer class of CLASS_TYPE */
#define PRINT_NODEARCH_CLASS_LAYER(CLASS_TYPE, CLASS_NAME)		\
    do									\
    {									\
      if (nodearch->CLASS_TYPE.size != 0)				\
      {								\
        int i;							\
        for (i = 0; i < nodearch->CLASS_TYPE.size; i++)		\
        {								\
          class_t *class;						\
          int class_position;					\
          int j;							\
          class = get_class_by_id(nodearch->CLASS_TYPE.elts[i]);	\
          fprintf(stderr, "       -> "CLASS_NAME" : %s\n", class->name); \
          class_position = class->nodearchs.elts[nodearch->id];	\
          if (nodearch->up[class_position].size)			\
          {							\
            fprintf(stderr, "             up :");			\
            int comma = 0;					\
            for (j = 0; j < nodearch->up[class_position].size; j++) \
            {							\
              if (comma)					\
              fprintf(stderr, ",");				\
              else						\
              comma = 1;					\
              fprintf(stderr, " %s", get_class_by_id(nodearch->up[class_position].elts[j])->name); \
            }							\
            fprintf(stderr, "\n");				\
          }							\
          if (nodearch->down[class_position].size)			\
          {							\
            fprintf(stderr, "             down :");		\
            int comma = 0;					\
            for (j = 0; j < nodearch->down[class_position].size; j++) \
            {							\
              if (comma)					\
              fprintf(stderr, ",");				\
              else						\
              comma = 1;					\
              fprintf(stderr, " %s", get_class_by_id(nodearch->down[class_position].elts[j])->name); \
            }							\
            fprintf(stderr, "\n");				\
          }							\
        }								\
      }								\
      \
    } while(0)


/* print a class of CLASS_TYPE, its link is stored in a table (there may be several classes of this type) */
#define PRINT_NODEARCH_CLASS_ARRAY(CLASS_TYPE, CLASS_NAME)		\
    do									\
    {									\
      if (nodearch->CLASS_TYPE.size != 0)				\
      {								\
        fprintf(stderr, "   - "CLASS_NAME" :");			\
        int comma = 0;						\
        int i;							\
        for (i = 0; i < nodearch->CLASS_TYPE.size; i++)		\
        {								\
          if (comma)						\
          fprintf(stderr, ",");					\
          else							\
          comma = 1;						\
          fprintf(stderr, " %s", get_class_by_id(nodearch->CLASS_TYPE.elts[i])->name); \
        }								\
        fprintf(stderr, "\n");					\
      }								\
      \
    } while(0)


/* print a class of CLASS_TYPE, its link is stored as an integer id (there may be only one class of this type) */
#define PRINT_NODEARCH_CLASS_INT(CLASS_TYPE, CLASS_NAME)		\
    do									\
    {									\
      if (nodearch->CLASS_TYPE != -1)					\
      {								\
        fprintf(stderr, "   - "CLASS_NAME" : %s\n", get_class_by_id(nodearch->CLASS_TYPE)->name); \
      }								\
      \
    } while(0)


/* ************************************************** */
/* ************************************************** */
/**
 * \brief Print informations about all registered node architectures
 **/
void print_nodearchs(void)
{
  int i;

  fprintf(stderr, "\n-----------------------------------");

  for (i = 0; i < nodearchs.size; i++)
  {
    nodearch_t *nodearch;

    nodearch = nodearchs.elts + i;

    fprintf(stderr, "\nNode architecture '%s' (%d)\n", nodearch->name, nodearch->id);

    fprintf(stderr, "   - layers :\n");
    PRINT_NODEARCH_CLASS_LAYER(applications, "application");
    PRINT_NODEARCH_CLASS_LAYER(routings,     "routing");
    PRINT_NODEARCH_CLASS_LAYER(macs,         "mac");
    PRINT_NODEARCH_CLASS_LAYER(phys,         "phy");
    PRINT_NODEARCH_CLASS_LAYER(codings,       "coding");
    PRINT_NODEARCH_CLASS_LAYER(interferences,       "interference");
    PRINT_NODEARCH_CLASS_LAYER(modulators,    "modulators");
    PRINT_NODEARCH_CLASS_LAYER(errors,        "error");
    PRINT_NODEARCH_CLASS_LAYER(signal_trackers, "signal_tracker");
    PRINT_NODEARCH_CLASS_LAYER(transceivers, "transceivers");

    PRINT_NODEARCH_CLASS_INT  (energy,     "energy    ");
    PRINT_NODEARCH_CLASS_INT  (mobility,   "mobility  ");
    PRINT_NODEARCH_CLASS_ARRAY(interfaces, "interfaces");
    PRINT_NODEARCH_CLASS_ARRAY(sensors,    "sensors   ");
  }
}


/* ************************************************** */
/* ************************************************** */
/**
 * \brief Parse up and down classes of a layer class in a node architecture
 * \param nd1, the xml node pointer
 * \param nodearch, the node architecture pointer
 * \param class_position, the position of the class in the node architecture structure
 * \return 0 in case of success, -1 else
 **/
int parse_nodearch_updown(xmlNodePtr nd1, nodearch_t *nodearch, int class_position)
{
  xmlNodePtr nd2;
  int up_c   = 0;
  int down_c = 0;

  /* count attributes */
  for (nd2 = nd1->children; nd2; nd2 = nd2->next) {
    if (!strcmp((char *) nd2->name, XML_E_UP)) {
      nodearch->up[class_position].size++;
    }
    else if (!strcmp((char *) nd2->name, XML_E_DOWN)) {
      nodearch->down[class_position].size++;
    }
  }

  /* allocate */
  if ((nodearch->up[class_position].size) &&
      (nodearch->up[class_position].elts =
          (int *) malloc(sizeof(int) * nodearch->up[class_position].size)) == NULL)  {
    fprintf(stderr, "config: malloc error (parse_nodearch_updown())\n");
    return -1;
  }
  if ((nodearch->down[class_position].size) &&
      (nodearch->down[class_position].elts =
          (int *) malloc(sizeof(int) * nodearch->down[class_position].size)) == NULL)
  {
    fprintf(stderr, "config: malloc error (parse_nodearch_updown())\n");
    return -1;
  }

  /* fill */
  for (nd2 = nd1->children; nd2; nd2 = nd2->next )
  {
    if (!strcmp((char *) nd2->name, XML_E_UP))
    {
      char *class_name;
      if ((class_name = get_xml_attr_content(nd2, XML_A_NAME)) != NULL)
      {
        class_t *class;
        if ((class = get_class_by_name(class_name)) == NULL)
        {
          fprintf(stderr, "config: wrong class '%s' has been used (parse_nodearch_updown())\n", class_name);
          return -1;
        }
        nodearch->up[class_position].elts[up_c++] = class->id;
        DBG_NODEARCH("Node architecture '%s': up class '%s' found\n", nodearch->name, class->name);
      }
    }
    else if (!strcmp((char *) nd2->name, XML_E_DOWN))
    {
      char *class_name;
      if ((class_name = get_xml_attr_content(nd2, XML_A_NAME)) != NULL)
      {
        class_t *class;
        if ((class = get_class_by_name(class_name)) == NULL)
        {
          fprintf(stderr, "config: wrong class '%s' has been used (parse_nodearch_updown())\n", class_name);
          return -1;
        }
        nodearch->down[class_position].elts[down_c++] = class->id;
        DBG_NODEARCH("Node architecture '%s': down class '%s' found\n", nodearch->name, class->name);
      }
    }
  }

  return 0;
}        


/* ************************************************** */
/* ************************************************** */
/**
 * \brief Parse a class parameters in a node architecture
 * \param nd1, the xml node pointer
 * \param nodearch, the node architecture pointer
 * \param class, the class pointer
 * \param medium, the medium pointer
 * \param environment, the environment pointer
 * \return 0 in case of success, -1 else
 **/
int parse_nodearch_class_params(xmlNodePtr nd1, nodearch_t *nodearch, class_t *class,
    medium_t *medium, environment_t *environment)
{
  xmlNodePtr nd2;
  dflt_param_t *dflt_param;
  param_t *param;
  environmentid_t environmentid = -1;
  mediumid_t      mediumid      = -1;

  /* get default class parameters and duplicate them */
  if (medium != NULL)
  {
    mediumid = medium->id;
  }
  if (environment != NULL)
  {
    environmentid = environment->id;
  }
  if ((dflt_param = (dflt_param_t *) malloc(sizeof(dflt_param_t))) == NULL)
  {
    fprintf(stderr, "config: malloc error (parse_nodearch_class_params())\n");
    return -1;
  }
  if ((dflt_param->params = list_create()) == NULL)
  {
    free(dflt_param);
    return -1;
  }
  dflt_param_t *dflt_class_param = get_class_params(-1, class->id, -1, mediumid, environmentid);
  if (dflt_class_param)
  {
    /* this class has default parameters, so duplicate them */
    if (duplicate_class_params(dflt_class_param->params, dflt_param->params))
    {
      free(dflt_param);
      return -1;
    }
  }
  dflt_param->nodeid        = -1;
  dflt_param->classid       = class->id;
  dflt_param->nodearchid    = nodearch->id;
  dflt_param->mediumid      = mediumid;
  dflt_param->environmentid = environmentid;

  /* parse xml class parameters */
  for (nd2 = nd1->children ; nd2 ; nd2 = nd2->next)
  {
    if (!strcmp((char *) nd2->name, XML_E_PARAMETERS))
    {
      xmlNodePtr nd3;
      for (nd3 = nd2->children ; nd3 ; nd3 = nd3->next)
      {
        if (!strcmp((char *) nd3->name, XML_E_PARAM))
        {
          param_t *nodearch_class_param;
          char    *key_content;
          char    *val_content;
          if (((key_content = get_xml_attr_content(nd3, XML_A_KEY)) != NULL)
              && ((val_content = get_xml_attr_content(nd3, XML_A_VALUE)) != NULL))
          {
            /* check if parameter is present into class default parameter, if yes, override it */
            if ((nodearch_class_param = find_class_param_key(dflt_param->params, key_content)) != NULL)
            {
              /* override medium class value */
              nodearch_class_param->value = val_content;
              DBG_NODEARCH("Node architecture '%s': class '%s': Override param key '%s'\n",
                  nodearch->name, class->name, key_content);
              DBG_NODEARCH("Node architecture '%s': class '%s': Override param value '%s'\n",
                  nodearch->name, class->name, val_content);
            }
            else
            {
              /* parameter not in class default parameter, so add parameter it */
              if ((param = (param_t *) malloc(sizeof(param_t))) == NULL)
              {
                free(dflt_param);
                fprintf(stderr, "config: malloc error (parse_medium_class())\n");
                return -1;
              }
              param->key   = key_content;
              param->value = val_content;
              DBG_NODEARCH("Node architecture '%s': class '%s': Save param key '%s'\n",
                  nodearch->name, class->name, key_content);
              DBG_NODEARCH("Node architecture '%s': class '%s': Save param value '%s'\n",
                  nodearch->name, class->name, val_content);
              list_insert(dflt_param->params, param);
            }
          }
          else
          {
            free(dflt_param);
            return -1;
          }

        }
      }
      list_insert(dflt_params, dflt_param);
      DBG_NODEARCH("Node architecture '%s': class '%s': Parameters saved\n", nodearch->name, class->name);

      return 0;
    }
  }

  list_destroy(dflt_param->params);
  free(dflt_param);

  return 0;
}


/* ************************************************** */
/* ************************************************** */
/**
 * \brief Parse the name of a node architecture
 * \param nd1, the xml node pointer
 * \param nodearch, the node architecture pointer
 * \return 0 in case of success, -1 else
 **/
int parse_nodearch_name(xmlNodePtr nd1, nodearch_t *nodearch)
{
  char *nodearch_name;

  if ((nodearch_name = get_xml_attr_content(nd1, XML_A_NAME)) != NULL)
  {
    nodearch->name = strdup(nodearch_name);
  }
  else
  {
    fprintf(stderr, "config: nodearch %s: error, while parsing nodearch name\n", nodearch->name);
    return -1;
  }

  return 0;
}


/* ************************************************** */
/* ************************************************** */
/**
 * \brief Parse the birth of a node architecture
 * \param nd1, the xml node pointer
 * \param nodearch, the node architecture pointer
 * \return 0 in case of success, -1 else
 **/
int parse_nodearch_birth(xmlNodePtr nd1, nodearch_t *nodearch)
{
  char *nodearch_birth;

  if ((nodearch_birth = get_xml_attr_content(nd1, XML_A_BIRTH)) != NULL)
  {
    nodearch->birth = strdup(nodearch_birth);
    DBG_NODEARCH("Node architecture '%s': birth = %s\n", nodearch->name, nodearch_birth);
  }

  return 0;
}


/* ************************************************** */
/* ************************************************** */
/**
 * \brief Parse 'default' attribute of a node architecture
 * \param nd1, the xml node pointer
 * \param nodearch, the node architecture pointer
 * \return 0 in case of success, -1 else
 **/
int parse_nodearch_default(xmlNodePtr nd1, nodearch_t *nodearch)
{
  char *nodearch_dflt;

  if ((nodearch_dflt = get_xml_attr_content(nd1, XML_A_DEFAULT)) != NULL)
  {
    if  (!strcmp(nodearch_dflt, "true") || !strcmp(nodearch_dflt, "1"))
    {
      dflt_nodearch = nodearch;
      DBG_NODEARCH("Node architecture '%s': default node architecture\n", nodearch->name);
    }
  }

  return 0;
}


/* ************************************************** */
/* ************************************************** */
/**
 * \brief Count the number of classes of the node architecture
 * \param nd1, the xml node pointer
 * \param nodearch, the node architecture pointer
 * \return 0 in case of success, -1 else
 **/
int parse_nodearch_count(xmlNodePtr nd1, nodearch_t *nodearch)
{
  xmlNodePtr nd2;
  xmlNodePtr nd3;

  for (nd2 = nd1->children; nd2; nd2 = nd2->next){
    if (!strcmp((char *) nd2->name, XML_E_NODE_TYPES)){
      for (nd3 = nd2->children; nd3; nd3 = nd3->next){
        if (!strcmp((char *) nd3->name, XML_E_TYPE_FAMILY))
          nodearch->types.size++;
      }
    }

    if (!strcmp((char *) nd2->name, XML_E_IMPLEMENTATION)){
      for (nd3 = nd2->children; nd3; nd3 = nd3->next){
        if (strcmp((char *) nd3->name, XML_E_WSIM))
          nodearch->classes.size++;
      }
    }
    else /* other elements than implementation */
      nodearch->classes.size++;
  }

  return 0;
}


/* ************************************************** */
/* ************************************************** */
/**
 * \brief Allocate memory for class links of the node architecture
 * \param nd1, the xml node pointer
 * \param nodearch, the node architecture pointer
 * \return 0 in case of success, -1 else
 **/
int parse_nodearch_allocate(xmlNodePtr nd1, nodearch_t *nodearch)
{
  int i;

  /* allocate classes */
  if ((nodearch->classes.elts = (int *) malloc(sizeof(int) * nodearch->classes.size)) == NULL)
  {
    fprintf(stderr, "config1: malloc error (parse_nodearch_allocate())\n");
    return -1;
  }
  if ((nodearch->up = (array_t *) malloc(sizeof(array_t) * nodearch->classes.size)) == NULL)
  {
    fprintf(stderr, "config2: malloc error (parse_nodearch_allocate())\n");
    return -1;
  }
  if ((nodearch->down = (array_t *) malloc(sizeof(array_t) * nodearch->classes.size)) == NULL)
  {
    fprintf(stderr, "config3: malloc error (parse_nodearch_allocate())\n");
    return -1;
  }

  // allocate types
  if (nodearch->types.size == 0 ){
    nodearch->types.size = 1;
  }
  if ((nodearch->types.elts = (node_type_t *) malloc(sizeof(node_type_t) * nodearch->types.size)) == NULL)
  {
    fprintf(stderr, "config4: malloc error (parse_nodearch_allocate())\n");
    return -1;
  }

  /* initialize */
  for (i = 0; i < nodearch->classes.size; i++) {
    nodearch->up[i].size   = 0;
    nodearch->up[i].elts   = NULL;
    nodearch->down[i].size = 0;
    nodearch->down[i].elts = NULL;
  }

  return 0;
}


/* ************************************************** */
/* ************************************************** */
/**
 * \brief Parse 'implementation' element of the node architecture
 * \param nd1, the xml node pointer
 * \param nodearch, the node architecture pointer
 * \return 0 in case of success, -1 else
 **/
int parse_nodearch_implem(xmlNodePtr nd1, nodearch_t *nodearch)
{
  xmlNodePtr nd2,nd3;

  for (nd2 = nd1->children; nd2; nd2 = nd2->next)
  {
    if (!strcmp((char *) nd2->name, XML_E_IMPLEMENTATION))
    {
      /* WSNet node architecture */
      DBG_NODEARCH("Node architecture '%s': parsing implementation : wsnet node arch\n", nodearch->name);
      PARSE_NODEARCH_LAYER_CLASS(phys,            XML_E_PHY);
      PARSE_NODEARCH_LAYER_CLASS(codings,         XML_E_CODING);
      PARSE_NODEARCH_LAYER_CLASS(interferences,   XML_E_INTERFERENCE);
      PARSE_NODEARCH_LAYER_CLASS(modulators,   	  XML_E_MODULATOR);
      PARSE_NODEARCH_LAYER_CLASS(errors,          XML_E_ERROR);
      PARSE_NODEARCH_LAYER_CLASS(signal_trackers, XML_E_SIGNAL_TRACKER);
      PARSE_NODEARCH_LAYER_CLASS(interfaces,      XML_E_INTERFACE);
      PARSE_NODEARCH_LAYER_CLASS(transceivers,    XML_E_TRANSCEIVER);
      PARSE_NODEARCH_LAYER_CLASS(macs,            XML_E_MAC);
      PARSE_NODEARCH_LAYER_CLASS(routings,        XML_E_ROUTING);
      PARSE_NODEARCH_LAYER_CLASS(applications,    XML_E_APPLICATION);
      return 0;
    }
  }
  fprintf(stderr, "config: nodearch '%s': no element 'implementation' found (parse_nodearch_implem())\n", nodearch->name);
  return -1;
}


/* ************************************************** */
/* ************************************************** */
/**
 * \brief Parse classes of a node architecture (other than layer classes)
 * \param nd1, the xml node pointer
 * \param nodearch, the node architecture pointer
 * \return 0 in case of success, -1 else
 **/
int parse_nodearch_classes(xmlNodePtr nd1, nodearch_t *nodearch)
{
  xmlNodePtr nd2;


  PARSE_NODEARCH_CLASS_INT  (mobility,   XML_E_MOBILITY);
  PARSE_NODEARCH_CLASS_ARRAY(sensors,    XML_E_SENSOR);
  PARSE_NODEARCH_CLASS_INT  (energy,     XML_E_ENERGY);
  PARSE_NODEARCH_CLASS_ARRAY(monitors,   XML_E_MONITOR);

  return 0;
}

/* ************************************************** */
/* ************************************************** */
/**
 * \brief Parse the types of a node architecture
 * \param nd1, the xml node pointer
 * \param nodearch, the node architecture pointer
 * \return 0 in case of success, -1 else
 **/
int parse_nodearch_types(xmlNodePtr nd1, nodearch_t *nodearch)
{
  int i=0;

  xmlNodePtr nd2;

  for (nd2 = nd1->children; nd2; nd2 = nd2->next)
  {
    if (!strcmp((char *) nd2->name, XML_E_NODE_TYPES)){
      xmlNodePtr nd3;


      for (nd3 = nd2->children; nd3; nd3 = nd3->next){
        char *nodearch_family;
        char *nodearch_type;
        if (!strcmp((char *) nd3->name, XML_E_TYPE_FAMILY)){
          if ((nodearch_family = get_xml_attr_content(nd3, XML_A_NAME)) != NULL){
            if ((!strcmp((char *) nd3->children->name, XML_E_TYPE)) && ((nodearch_type = get_xml_attr_content(nd3->children, XML_A_NAME)) != NULL) ){
              DBG_NODEARCH("Node architecture '%s': parsing types :\n", nodearch->name);

              if ( (!strcmp(nodearch_family,"BAN")) && (!strcmp(nodearch_type,"BELT")) ) {
                nodearch->types.elts[i].family=NODE_TYPE_FAMILY_BAN;
                nodearch->types.elts[i].type=NODE_TYPE_BAN_BELT;
              }
              if ( (!strcmp(nodearch_family,"BAN")) && (!strcmp(nodearch_type,"HAND")) ) {
                nodearch->types.elts[i].family=NODE_TYPE_FAMILY_BAN;
                nodearch->types.elts[i].type=NODE_TYPE_BAN_HAND;
              }
              if ( (!strcmp(nodearch_family,"BAN")) && (!strcmp(nodearch_type,"EAR")) ) {
                nodearch->types.elts[i].family=NODE_TYPE_FAMILY_BAN;
                nodearch->types.elts[i].type=NODE_TYPE_BAN_EAR;
              }
              i++;
            }

          }
        }
      }


      return 0;
    }
  }




  //fprintf(stderr, "config: nodearch '%s': no element 'node_types' found (parse_nodearch_implem())\n", nodearch->name);

  nodearch->types.elts[0].family = NODE_TYPE_FAMILY_15_4;
  nodearch->types.elts[0].type = NODE_TYPE_15_4_RFD;
  return 0;

}



/* ************************************************** */
/* ************************************************** */
/**
 * \brief Parse a node architecture
 * \param nd1, the xml node pointer
 * \param id, the identifier of the node architecture
 * \return 0 in case of success, -1 else
 **/
int parse_nodearch(xmlNodePtr nd1, int id)
{
  nodearch_t *nodearch = get_nodearch_by_id(id);

  /* initialize */
  class_position = 0;

  if (parse_nodearch_name(nd1, nodearch))
  {
    return -1;
  }

  DBG_NODEARCH("\n===Parsing node architecture '%s'===\n", nodearch->name);

  if (parse_nodearch_birth(nd1, nodearch))
  {
    return -1;
  }

  if (parse_nodearch_default(nd1, nodearch))
  {
    return -1;
  }

  /* count classes */
  if (parse_nodearch_count(nd1, nodearch))
  {
    return -1;
  }

  if (parse_nodearch_allocate(nd1, nodearch))
  {
    return -1;
  }

  /* parse implementation element */
  if (parse_nodearch_implem(nd1, nodearch))
  {
    return -1;
  }

  /* parse classes other than layers */
  if (parse_nodearch_classes(nd1, nodearch))
  {
    return -1;
  }

  // parse the node types
  if (parse_nodearch_types(nd1, nodearch))
    return -1;

  return 0;
}


/* ************************************************** */
/* ************************************************** */
/**
 * \brief Init a node architecture structure
 * \param id, the identifier of the node architecture to init
 **/
void parse_nodearch_init(int id)
{
  nodearch_t *nodearch = get_nodearch_by_id(id);

  nodearch->id                    = id;
  nodearch->name                  = NULL;
  nodearch->birth                 = NULL;

  nodearch->classes.size          = 0;
  nodearch->classes.elts          = NULL;

  nodearch->transceivers.size     = 0;
  nodearch->transceivers.elts     = NULL;
  nodearch->macs.size             = 0;
  nodearch->macs.elts             = NULL;
  nodearch->routings.size         = 0;
  nodearch->routings.elts         = NULL;
  nodearch->applications.size     = 0;
  nodearch->applications.elts     = NULL;

  nodearch->up                    = NULL;
  nodearch->down                  = NULL;

  nodearch->phys.size             = 0;
  nodearch->phys.elts             = NULL;
  nodearch->codings.size          = 0;
  nodearch->codings.elts          = NULL;
  nodearch->interferences.size    = 0;
  nodearch->interferences.elts    = NULL;
  nodearch->modulators.size    	  = 0;
  nodearch->modulators.elts    	  = NULL;
  nodearch->errors.size           = 0;
  nodearch->errors.elts           = NULL;
  nodearch->signal_trackers.size  = 0;
  nodearch->signal_trackers.elts  = NULL;
  nodearch->interfaces.size       = 0;
  nodearch->interfaces.elts       = NULL;
  nodearch->sensors.size          = 0;
  nodearch->sensors.elts          = NULL;
  nodearch->monitors.size         = 0;
  nodearch->monitors.elts         = NULL;

  nodearch->types.size            = 0;
  nodearch->types.elts            = NULL;

  nodearch->energy                = -1;
  nodearch->mobility              = -1;
}


/* ************************************************** */
/* ************************************************** */
int parse_nodearchs(xmlNodeSetPtr nodeset)
{
  int i;

  DBG_NODEARCH("\n\n==============NODE ARCHITECTURES===============\n");

  /* allocate memory for class list */
  if ((nodearchs.elts = (nodearch_t *) malloc(sizeof(nodearch_t) * nodearchs.size)) == NULL)
  {
    fprintf(stderr, "config: malloc error (parse_nodearchs())\n");
    return -1;
  }

  /* init nodearchs */
  for (i = 0 ; i < nodearchs.size; i++)
  {
    parse_nodearch_init(i);
  }

  /* parse nodearchs */
  for (i = 0 ; i < nodearchs.size; i++)
  {
    if (parse_nodearch(nodeset->nodeTab[i], i))
    {
      return -1;
    }
  }

  print_nodearchs();

  return 0;
}