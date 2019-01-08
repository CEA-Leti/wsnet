/**
 *  \file   environment_config.c
 *  \brief  Environment configuration module
 *  \author Loic Lemaitre
 *  \date   2010
 */

#include "environment_configuration.h"


/* ************************************************** */
/* ************************************************** */
/**
 * \brief Print informations about all registered environments
 **/
void print_environments(void)
{ 
  int i;

  fprintf(stderr, "\n-----------------------------------");

  for (i = 0; i < environments.size; i++)
  {
    environment_t *environment;
    int title;

    environment = environments.elts + i;

    fprintf(stderr, "\nEnvironment '%s' (%d)", environment->name, environment->id);

    if (environment->map != -1)
      fprintf(stderr, "\n   - map       : %s", get_class_by_id(environment->map)->name);

    if (environment->physicals.elts != NULL)
    {
      int i;
      title = 0;
      for (i = 0; i < environment->physicals.size; i++)
      {
        if (title == 0)
        {
          fprintf(stderr, "\n   - physicals : ");
          title = 1;
        }
        else
        {
          fprintf(stderr, ", ");
        }
        fprintf(stderr, "%s", get_class_by_id(environment->physicals.elts[i])->name);
      }
    }

    if (environment->monitors.elts != NULL)
    {
      int i;
      title = 0;
      for (i = 0; i < environment->monitors.size; i++)
      {
        if (title == 0)
        {
          fprintf(stderr, "\n   - monitors  : ");
          title = 1;
        }
        else
        {
          fprintf(stderr, ", ");
        }
        fprintf(stderr, "%s", get_class_by_id(environment->monitors.elts[i])->name);
      }
    }
    fprintf(stderr, "\n");
  }
}


/* ************************************************** */
/* ************************************************** */
/**
 * \brief Init the class parameters for an environment
 * \param environment, the environment pointer
 * \param class, the class pointer
 * \param params, the parameters for the class of the environement
 * \return 0 in case of success, -1 else
 **/
int parse_environment_class_bind(environment_t *environment, class_t *class, void *params)
{
  call_t to  = {class->id, environment->id};
  int    ok = 0;

  DBG_ENVIRONMENT("Environment '%s': class '%s': initializing parameters...\n", environment->name, class->name);

  ok = class->bind ?  class->bind(&to, params) : 0;

  return ok;
}


/* ************************************************** */
/* ************************************************** */
/**
 * \brief Parse a class of an environment
 * \param nd1, the xml node pointer
 * \param environment, the environment pointer
 * \param class_position, the position of the class in the environment structure
 * \return 0 in case of success, -1 else
 **/
int parse_environment_class(xmlNodePtr nd1, environment_t *environment, int class_position)
{
  xmlNodePtr    nd2;
  dflt_param_t *dflt_param;
  param_t      *param;
  class_t      *class = get_class_by_name(get_xml_attr_content(nd1, XML_A_NAME));

  if (class == NULL)
  {
    fprintf(stderr, "config: error: class '%s' not declared (parse_environment_class())\n",
        get_xml_attr_content(nd1, XML_A_NAME));
    return -1;
  }

  /* include class in environment and vice-versa */
  environment->classes.elts[class_position] = class->id;
  class->environments.elts[environment->id] = class_position;

  DBG_ENVIRONMENT("Environment '%s': class '%s': **Parsing parameters**\n", environment->name, class->name);

  /* bind environment to class */
  if (!strcmp((char *) nd1->name, XML_E_MAP))
  {
    environment->map = class->id;
  }
  else if (!strcmp((char *) nd1->name, XML_E_PHYSICAL))
  {
    environment->physicals.elts[environment->physicals.size] = class->id;
    environment->physicals.size++;
  }
  else if (!strcmp((char *) nd1->name, XML_E_MONITOR))
  {
    environment->monitors.elts[environment->monitors.size] = class->id;
    environment->monitors.size++;
  }
  else
  {
    fprintf(stderr, "config: environment '%s': error, unknown environment class (parse_environment_class())\n",
        environment->name);
    return -1;
  }

  /* get default class parameters and duplicate them */
  if ((dflt_param = (dflt_param_t *) malloc(sizeof(dflt_param_t))) == NULL)
  {
    fprintf(stderr, "config: malloc error (parse_environment_class())\n");
    return -1;
  }
  if ((dflt_param->params = list_create()) == NULL)
  {
    free(dflt_param);
    return -1;
  }
  dflt_param_t *dflt_class_param = get_class_params(-1, class->id, -1, -1, -1);
  if (dflt_class_param)
  {
    /* this class has default parameter, so duplicate them */
    if (duplicate_class_params(dflt_class_param->params, dflt_param->params))
    {
      free(dflt_param);
      return -1;
    }
  }
  dflt_param->nodeid        = -1;
  dflt_param->classid       = class->id;
  dflt_param->nodearchid    = -1;
  dflt_param->mediumid      = -1;
  dflt_param->environmentid = environment->id;

  /* parse xml environment class parameters */
  for (nd2 = nd1->children ; nd2 ; nd2 = nd2->next)
  {
    if (!strcmp((char *) nd2->name, XML_E_PARAM))
    {
      param_t *environment_class_param;
      char    *key_content;
      char    *val_content;
      if (((key_content = get_xml_attr_content(nd2, XML_A_KEY)) != NULL)
          && ((val_content = get_xml_attr_content(nd2, XML_A_VALUE)) != NULL))
      {
        /* check if parameter is present into class default parameter, if yes, override it */
        if ((environment_class_param = find_class_param_key(dflt_param->params, key_content)) != NULL)
        {
          /* override environment class value */
          environment_class_param->value = val_content;
          DBG_ENVIRONMENT("Environment '%s': class '%s': Override param key '%s'\n",
              environment->name, class->name, key_content);
          DBG_ENVIRONMENT("Environment '%s': class '%s': Override param value '%s'\n",
              environment->name, class->name, val_content);
        }
        else
        {
          /* parameter not in class default parameter, so add parameter it */
          if ((param = (param_t *) malloc(sizeof(param_t))) == NULL)
          {
            fprintf(stderr, "config: malloc error (parse_environment_class())\n");
            free(dflt_param);
            return -1;
          }

          param->key   = key_content;
          param->value = val_content;
          DBG_ENVIRONMENT("Environment '%s': class '%s': Save param key '%s'\n",
              environment->name, class->name, key_content);
          DBG_ENVIRONMENT("Environment '%s': class '%s': Save param value '%s'\n",
              environment->name, class->name, val_content);

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

  /* init environment parameters classes */
  if ( parse_environment_class_bind(environment, class, dflt_param->params))
  {
    fprintf(stderr, "config: environment '%s': error, while initializing environment parameters of class '%s'\n",
        environment->name, class->name);
  }

  return 0; 
}


/* ************************************************** */
/* ************************************************** */
/**
 * \brief Allocate memory for the classes links to store
 * \param nd1, the xml node pointer
 * \param environment, the environment pointer
 * \return 0 in case of success, -1 else
 **/
int parse_environment_allocate(xmlNodePtr nd1, environment_t *environment)
{
  xmlNodePtr nd2;

  /* count */
  for (nd2 = nd1->children; nd2; nd2 = nd2->next)
  {
    environment->classes.size++;

    if (!strcmp((char *) nd2->name, XML_E_PHYSICAL))
      environment->physicals.size++;

    if (!strcmp((char *) nd2->name, XML_E_MONITOR))
      environment->monitors.size++;
  }

  /* allocate classes */
  if ((environment->classes.elts = (int *) malloc(sizeof(int) * environment->classes.size)) == NULL)
  {
    fprintf(stderr, "config: malloc error (parse_environment_allocate())\n");
    return -1;
  }

  /* allocate physicals */
  if ((environment->physicals.elts = (int *) malloc(sizeof(int) * environment->physicals.size)) == NULL)
  {
    fprintf(stderr, "config: malloc error (parse_environment_allocate())\n");
    return -1;
  }

  /* allocate monitors */
  if ((environment->monitors.elts = (int *) malloc(sizeof(int) * environment->monitors.size)) == NULL)
  {
    fprintf(stderr, "config: malloc error (parse_environment_allocate())\n");
    return -1;
  }

  /* allocate private memory */
  if ((environment->private = malloc(sizeof(void *) * environment->classes.size)) == NULL)
  {
    fprintf(stderr, "config: malloc error (parse_environment_allocate())\n");
    return -1;
  }

  /* set to 0 to know where to store the next class */
  environment->physicals.size = 0;
  environment->monitors.size  = 0;

  return 0;
}


/* ************************************************** */
/* ************************************************** */
/**
 * \brief Parse an environment
 * \param nd1, the xml node pointer
 * \param id, the identifier of the environment to parse
 * \return 0 in case of success, -1 else
 **/
int parse_environment(xmlNodePtr nd1, int id)
{
  environment_t *environment = get_environment_by_id(id);
  xmlNodePtr     nd2;
  char          *environment_name;
  int            class_position = 0;

  /* get environment name */
  if ((environment_name = get_xml_attr_content(nd1, XML_A_NAME)) != NULL)
  {
    environment->name = strdup(environment_name);
  }
  else
    return -1;

  /* count and allocate */
  if (parse_environment_allocate(nd1, environment))
  {
    return -1;
  }

  DBG_ENVIRONMENT("\n===Parsing environment '%s'===\n", environment->name);

  /* parse environment classes */
  for (nd2 = nd1->children; nd2; nd2 = nd2->next)
  {
    if (parse_environment_class(nd2, environment, class_position))
    {
      fprintf(stderr, "config: environment '%s': error, while parsing environment class '%s'\n",
          environment->name, nd2->name);
      return -1;
    }
    class_position++;
  }

  return 0;
}


/* ************************************************** */
/* ************************************************** */
/**
 * \brief Init the structure of this environment
 * \param id, the identifier of the environment to init 
 * \return 0 in case of success, -1 else
 **/
void parse_environment_init(int id)
{
  environment_t *environment = get_environment_by_id(id);

  environment->id               = id;
  environment->name             = NULL;
  environment->classes.size     = 0;
  environment->classes.elts     = NULL;
  environment->physicals.size   = 0;
  environment->physicals.elts   = NULL;
  environment->monitors.size    = 0;
  environment->monitors.elts    = NULL;
  environment->map              = -1;
}


/* ************************************************** */
/* ************************************************** */
int parse_environments(xmlNodeSetPtr nodeset) {
  int i;

  DBG_ENVIRONMENT("\n\n==================ENVIRONMENTS================\n");

  /* allocate memory for environment list */
  if ((environments.elts = (environment_t *) malloc(sizeof(environment_t) * environments.size)) == NULL)
  {
    fprintf(stderr, "config: malloc error (parse_environments())\n");
    return -1;
  }

  /* init environment structure */
  for (i = 0 ; i < environments.size; i++)
  {
    parse_environment_init(i);
  }

  /* parse environments */
  for (i = 0 ; i < environments.size; i++)
  {
    if (parse_environment(nodeset->nodeTab[i], i))
    {
      return -1;
    }
  }

  print_environments();

  return 0;
}
