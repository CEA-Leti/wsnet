/**
 *  \file   medium_config.c
 *  \brief  Medium configuration module
 *  \author Loic Lemaitre
 *  \date   2010
 */

#include "medium_configuration.h"
#include "param.h"


/* ************************************************** */
/* ************************************************** */
/**
 * \brief Print informations about all registered mediums
 **/
void print_mediums(void) {
  int i;

  fprintf(stderr, "\n-----------------------------------");

  for (i = 0; i < mediums.size; i++) {
    medium_t *medium;
    int title;

    medium = mediums.elts + i;

    fprintf(stderr, "\nMedium '%s' (%d)", medium->name, medium->id);

    if (medium->pathloss != CLASS_NON_EXISTENT_CLASS_ID) {
      fprintf(stderr, "\n   - pathloss      : %s", get_class_by_id(medium->pathloss)->name);
    }
    if (medium->spectrum != CLASS_NON_EXISTENT_CLASS_ID) {
          fprintf(stderr, "\n   - spectrum      : %s", get_class_by_id(medium->spectrum)->name);
    }
    if (medium->shadowing != CLASS_NON_EXISTENT_CLASS_ID) {
      fprintf(stderr, "\n   - shadowing     : %s", get_class_by_id(medium->shadowing)->name);
    }
    if (medium->fading != CLASS_NON_EXISTENT_CLASS_ID) {
      fprintf(stderr, "\n   - fading        : %s", get_class_by_id(medium->fading)->name);
    }
    if (medium->interferences != CLASS_NON_EXISTENT_CLASS_ID) {
      fprintf(stderr, "\n   - interferences : %s", get_class_by_id(medium->interferences)->name);
    }
    if (medium->intermodulation != CLASS_NON_EXISTENT_CLASS_ID) {
      fprintf(stderr, "\n   - intermodulation : %s", get_class_by_id(medium->intermodulation)->name);
    }
    if (medium->noise != CLASS_NON_EXISTENT_CLASS_ID) {
      fprintf(stderr, "\n   - noise         : %s", get_class_by_id(medium->noise)->name);
    }
    if (medium->modulations.elts != NULL) {
      int i;
      title = 0;
      for (i = 0; i < medium->modulations.size; i++) {
        if (title == 0) {
          fprintf(stderr, "\n   - modulations   : ");
          title = 1;
        }
        else {
          fprintf(stderr, ", ");
        }
        fprintf(stderr, "%s", get_class_by_id(medium->modulations.elts[i])->name);
      }
    }

    if (medium->monitors.elts != NULL) {
      int i;
      title = 0;
      for (i = 0; i < medium->monitors.size; i++) {
        if (title == 0) {
          fprintf(stderr, "\n   - monitors      : ");
          title = 1;
        }
        else {
          fprintf(stderr, ", ");
        }
        fprintf(stderr, "%s", get_class_by_id(medium->monitors.elts[i])->name);
      }
    }
    fprintf(stderr, "\n");
  }
}


/* ************************************************** */
/* ************************************************** */
/**
 * \brief Init the class parameters for an medium
 * \param medium, the medium pointer
 * \param class, the class pointer
 * \param params, the parameters for the class of the medium
 * \return 0 in case of success, -1 else
 **/
int parse_medium_class_bind(medium_t *medium, class_t *class, void *params) {
  call_t to  = {class->id, medium->id};
  int    ok = 0;

  DBG_MEDIUM("Medium '%s': class '%s': initializing parameters...\n", medium->name, class->name);

  ok = class->bind ? class->bind(&to, params) : 0;

  if (class->create_object) {
      int* key = malloc(sizeof(int));
      *key = medium->id;
      int* index = malloc(sizeof(int));
      *index = class->objects.size++;
      list_insert(class->objects.saved_indexes,(void*)index);
      hashtable_insert(class->objects.indexes,(void*)key, (void*)index);
      class->objects.object[*index] = class->create_object(&to, params);
      ok = SUCCESSFUL;
  }

  return ok;
}


/* ************************************************** */
/* ************************************************** */
/**
 * \brief Parse a class of a medium
 * \param nd1, the xml node pointer
 * \param medium, the medium pointer
 * \param class_position, the position of the class in the medium structure
 * \return 0 in case of success, -1 else
 **/
int parse_medium_class(xmlNodePtr nd1, medium_t *medium, int class_position) {
  xmlNodePtr    nd2;
  dflt_param_t *dflt_param;
  param_t      *param;
  class_t      *class = get_class_by_name(get_xml_attr_content(nd1, XML_A_NAME));

  if (class == NULL) {
    fprintf(stderr, "config: error: class '%s' not declared (parse_medium_class())\n",
        get_xml_attr_content(nd1, XML_A_NAME));
    return -1;
  }

  /* include class in medium and vice-versa */
  medium->classes.elts[class_position] = class->id;
  class->mediums.elts[medium->id]      = class_position;

  DBG_MEDIUM("Medium '%s': class '%s': **Parsing parameters**\n", medium->name, class->name);

  /* bind medium to class */
  if (!strcmp((char *) nd1->name, XML_E_SPECTRUM)) {
      medium->spectrum = class->id;
  }
  else if (!strcmp((char *) nd1->name, XML_E_PATHLOSS)) {
    medium->pathloss = class->id;
  }
  else if (!strcmp((char *) nd1->name, XML_E_SHADOWING)) {
    medium->shadowing = class->id;
  }
  else if (!strcmp((char *) nd1->name, XML_E_FADING)) {
    medium->fading = class->id;
  }
  else if (!strcmp((char *) nd1->name, XML_E_INTERFERENCES)) {
    medium->interferences = class->id;
  }
  else if (!strcmp((char *) nd1->name, XML_E_INTERMODULATION)) {
    medium->intermodulation = class->id;
  }
  else if (!strcmp((char *) nd1->name, XML_E_NOISE)) {
    medium->noise = class->id;
  }
  else if (!strcmp((char *) nd1->name, XML_E_MODULATION)) {
    medium->modulations.elts[medium->modulations.size] = class->id;
    medium->modulations.size++;
  }
  else if (!strcmp((char *) nd1->name, XML_E_MONITOR)) {
    medium->monitors.elts[medium->monitors.size] = class->id;
    medium->monitors.size++;
  }
  else if (!strcmp((char *) nd1->name, XML_E_LINK)) {
    medium->links.elts[medium->links.size] = class->id;
    medium->links.size++;
  }
  else {
    fprintf(stderr, "config: medium '%s': error, unknown medium class (parse_medium_class())\n", medium->name);
    return -1;
  }

  /* get default class parameters and duplicate them */
  if ((dflt_param = (dflt_param_t *) malloc(sizeof(dflt_param_t))) == NULL) {
    fprintf(stderr, "config: malloc error (parse_medium_class())\n");
    return -1;
  }
  if ((dflt_param->params = list_create()) == NULL) {
    free(dflt_param);
    return -1;
  }
  dflt_param_t *dflt_class_param = get_class_params(-1, class->id, -1, -1, -1);
  if (dflt_class_param) {
    /* this class has default parameters, so duplicate them */
    if (duplicate_class_params(dflt_class_param->params, dflt_param->params)) {
      free(dflt_param);
      return -1;
    }
  }
  dflt_param->nodeid        = -1;
  dflt_param->classid       = class->id;
  dflt_param->nodearchid    = -1;
  dflt_param->mediumid      = medium->id;
  dflt_param->environmentid = -1;

  /* parse xml medium class parameters */
  for (nd2 = nd1->children ; nd2 ; nd2 = nd2->next) {
    if (!strcmp((char *) nd2->name, XML_E_PARAM)) {
      param_t *medium_class_param;
      char    *key_content;
      char    *val_content;
      if (((key_content = get_xml_attr_content(nd2, XML_A_KEY)) != NULL)
          && ((val_content = get_xml_attr_content(nd2, XML_A_VALUE)) != NULL)) {
        /* check if parameter is present into class default parameter, if yes, override it */
        if ((medium_class_param = find_class_param_key(dflt_param->params, key_content)) != NULL) {
          /* override medium class value */
          medium_class_param->value = val_content;
          DBG_MEDIUM("Medium '%s': class '%s': Override param key '%s'\n",
              medium->name, class->name, key_content);
          DBG_MEDIUM("Medium '%s': class '%s': Override param value '%s'\n",
              medium->name, class->name, val_content);
        }
        else {
          /* parameter not in class default parameter, so add parameter it */
          if ((param = (param_t *) malloc(sizeof(param_t))) == NULL) {
            fprintf(stderr, "config: malloc error (parse_medium_class())\n");
            free(dflt_param);
            return -1;
          }

          param->key   = key_content;
          param->value = val_content;
          DBG_MEDIUM("Medium '%s': class '%s': Save param key '%s'\n",
              medium->name, class->name, key_content);
          DBG_MEDIUM("Medium '%s': class '%s': Save param value '%s'\n",
              medium->name, class->name, val_content);

          list_insert(dflt_param->params, param);
        }
      }
      else {
        free(dflt_param);
        return -1;
      }

    }
  }
  list_insert(dflt_params, dflt_param);

  /* init medium parameters classes */
  if (parse_medium_class_bind(medium, class, dflt_param->params)) {
    fprintf(stderr, "config: medium '%s': error, while initializing medium parameters of class '%s'\n",
        medium->name, class->name);
  }

  return 0; 
}


/* ************************************************** */
/* ************************************************** */
/**
 * \brief Allocate memory for the classes links to store
 * \param nd1, the xml node pointer
 * \param medium, the medium pointer
 * \return 0 in case of success, -1 else
 **/
int parse_medium_allocate(xmlNodePtr nd1, medium_t *medium) {
  xmlNodePtr nd2;

  /* count */
  for (nd2 = nd1->children; nd2; nd2 = nd2->next) {
    medium->classes.size++;

    if (!strcmp((char *) nd2->name, XML_E_MODULATION))
      medium->modulations.size++;

    if (!strcmp((char *) nd2->name, XML_E_MONITOR))
      medium->monitors.size++;

    if (!strcmp((char *) nd2->name, XML_E_LINK))
      medium->links.size++;
  }

  /* allocate classes */
  if ((medium->classes.elts = (int *) malloc(sizeof(int) * medium->classes.size)) == NULL) {
    fprintf(stderr, "config: malloc error (parse_medium_allocate())\n");
    return -1;
  }

  /* allocate modulations */
  if ((medium->modulations.elts = (int *) malloc(sizeof(int) * medium->modulations.size)) == NULL) {
    fprintf(stderr, "config: malloc error (parse_medium_allocate())\n");
    return -1;
  }

  /* allocate monitors */
  if ((medium->monitors.elts = (int *) malloc(sizeof(int) * medium->monitors.size)) == NULL) {
    fprintf(stderr, "config: malloc error (parse_medium_allocate())\n");
    return -1;
  }

  /* allocate links */
  if ((medium->links.elts = (int *) malloc(sizeof(int) * medium->links.size)) == NULL) {
    fprintf(stderr, "config: malloc error (parse_medium_allocate())\n");
    return -1;
  }


  /* allocate private memory */
  if ((medium->private = malloc(sizeof(void *) * medium->classes.size)) == NULL) {
    fprintf(stderr, "config: malloc error (parse_medium_allocate())\n");
    return -1;
  }

  /* set to 0 to know where to store the next class */
  medium->modulations.size = 0;
  medium->monitors.size    = 0;
  medium->links.size	   = 0;

  return 0;
}


/* ************************************************** */
/* ************************************************** */
/**
 * \brief Parse a medium
 * \param nd1, the xml node pointer
 * \param id, the identifier of the medium to parse
 * \return 0 in case of success, -1 else
 **/
int parse_medium(xmlNodePtr nd1, int id)
{
  medium_t  *medium = get_medium_by_id(id);
  xmlNodePtr nd2;
  char      *medium_name;
  char      *medium_propagation_range;
  int        class_position = 0;

  /* get medium name */
  if ((medium_name = get_xml_attr_content(nd1, XML_A_NAME)) != NULL) {
    medium->name = strdup(medium_name);
  }
  else
    return -1;

  /* get propagation range */
  if ((medium_propagation_range = get_xml_attr_content(nd1, XML_A_PROPAGATION_RANGE)) != NULL) {
    get_param_double(medium_propagation_range, &(medium->propagation_range));
  }

  /* count and allocate */
  if (parse_medium_allocate(nd1, medium)) {
    return -1;
  }

  DBG_MEDIUM("\n\n===Parsing medium '%s'===\n", medium->name);

  /* parse medium classes */
  for (nd2 = nd1->children; nd2; nd2 = nd2->next) {
    if (parse_medium_class(nd2, medium, class_position)) {
      fprintf(stderr, "config: medium '%s': error, while parsing medium class '%s'\n", medium->name, nd2->name);
      return -1;
    }
    class_position ++;
  }

  return 0;
}


/* ************************************************** */
/* ************************************************** */
/**
 * \brief Init the structure of this medium
 * \param id, the identifier of the medium to init 
 * \return 0 in case of success, -1 else
 **/
void parse_medium_init(int id) {
  medium_t  *medium = get_medium_by_id(id);

  medium->id                 = id;
  medium->name               = NULL;
  medium->propagation_range  = 0;
  medium->speed_of_light	   = 0.3;
  medium->spectrum           = CLASS_NON_EXISTENT_CLASS_ID;
  medium->pathloss           = CLASS_NON_EXISTENT_CLASS_ID;
  medium->shadowing          = CLASS_NON_EXISTENT_CLASS_ID;
  medium->fading             = CLASS_NON_EXISTENT_CLASS_ID;
  medium->interferences      = CLASS_NON_EXISTENT_CLASS_ID;
  medium->intermodulation    = CLASS_NON_EXISTENT_CLASS_ID;
  medium->noise              = CLASS_NON_EXISTENT_CLASS_ID;
  medium->classes.size       = 0;
  medium->classes.elts       = NULL;
  medium->modulations.size   = 0;
  medium->modulations.elts   = NULL;
  medium->monitors.size      = 0;
  medium->monitors.elts      = NULL;
  medium->links.size         = 0;
  medium->links.elts         = NULL;
}


/* ************************************************** */
/* ************************************************** */
int parse_mediums(xmlNodeSetPtr nodeset) {
  int i;

  DBG_MEDIUM("\n====================MEDIUMS====================\n");

  if (mediums.size > MAX_MEDIUMS) {
    fprintf(stderr, "config: too many mediums (%d), max allowed %d\n", mediums.size, MAX_MEDIUMS);
    return -1;
  }

  /* allocate memory for medium list */
  if ((mediums.elts = (medium_t *) malloc(sizeof(medium_t) * mediums.size)) == NULL) {
    fprintf(stderr, "config: malloc error (parse_mediums())\n");
    return -1;
  }

  /* init medium structures */
  for (i = 0 ; i < mediums.size; i++) {
    parse_medium_init(i);
  }

  /* parse mediums */
  for (i = 0 ; i < mediums.size; i++) {
    if (parse_medium(nodeset->nodeTab[i], i)) {
      return -1;
    }
  }

  print_mediums();

  return 0;
}
