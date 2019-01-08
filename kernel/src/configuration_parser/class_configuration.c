/**
 *  \file   class_config.c
 *  \brief  class configuration module
 *  \author Loic Lemaitre
 *  \date   2010
 */

#include <kernel/include/definitions/models.h>
#include "class_configuration.h"
#include "simulation_configuration.h"
#include <kernel/include/definitions/class.h>
#include <kernel/include/data_structures/hashtable/hashtable.h>
#include <kernel/include/data_structures/list/list.h>


/**
 * \brief Print informations about all registered classes
 **/
void print_classes(void)
{
  int i;

  fprintf(stderr, "\n-----------------------------------");

  for (i = 0; i < classes.size; i++)
  {
    class_t *class = get_class_by_id(i);

    fprintf(stderr, "\nClass '%s' (%d)", class->name, class->id);

    fprintf(stderr, "\n   - implementation : ");
    fprintf(stderr, "%s", implem_type_to_str(class->implem.all.implem_type));

    switch(class->implem.all.implem_type)
    {
      case CXX_IMPLEM:
        fprintf(stderr, "\n   - type           : %s", model_type_to_str(class->model->type));
        fprintf(stderr, "\n   - using model    : %s", class->implem.cxx.library.name);
        fprintf(stderr, "\n   - using plugin   : %s", class->implem.cxx.library.file);
        fprintf(stderr, "\n   - author         : %s", class->model->author);
        fprintf(stderr, "\n   - version        : %s", class->model->version);
        fprintf(stderr, "\n   - description    : %s", class->model->oneline);
        break;
      case C_IMPLEM:
        fprintf(stderr, "\n   - type           : %s", model_type_to_str(class->model->type));
        fprintf(stderr, "\n   - using model    : %s", class->implem.c.library.name);
        fprintf(stderr, "\n   - using plugin   : %s", class->implem.c.library.file);
        fprintf(stderr, "\n   - author         : %s", class->model->author);
        fprintf(stderr, "\n   - version        : %s", class->model->version);
        fprintf(stderr, "\n   - description    : %s", class->model->oneline);
        break;
      case PYTHON_IMPLEM:
        /* add here informations to print about this python class */
        break;
      case RUBY_IMPLEM:
        /* add here informations to print about this ruby class */
        break;
      default:
        break;
    }

    fprintf(stderr, "\n");
  }
}


/* ************************************************** */
/* ************************************************** */
/**
 * \brief Parse library parameter for a class implemented in c
 * \param nd1, the xml node pointer
 * \param class, the class pointer
 * \return 0 in case of success, -1 else
 **/
int parse_class_library_c(xmlNodePtr nd1, class_t *class)
{
  xmlNodePtr nd2;

  for (nd2 = nd1->children; nd2; nd2 = nd2->next)
  {
    if (!strcmp((char *) nd2->name, XML_E_PARAM))
    {
      if(!strcmp(get_xml_attr_content(nd2, XML_A_KEY), XML_C_LIBRARY))
      {
        class->implem.c.library.name = strdup(get_xml_attr_content(nd2, XML_A_VALUE));
        if (class->implem.c.library.name != NULL)
        {
          DBG_CLASS("Class '%s': Library name = '%s'\n", class->name, class->implem.c.library.name);
          return 0;
        }
      }
    }
  }
  fprintf(stderr, "config: class '%s': error when parsing library name (parse_class_library_c())\n", class->name);
  return -1;
}


/**
 * \brief Load module for a class implemented in c
 * \param class, the class pointer
 * \return 0 in case of success, -1 else
 **/
int parse_class_module_c(class_t *class) {
  char **path = NULL;
  char *file = NULL;

  for (path = user_path_list ; *path ; path++) {
    class->implem.c.library.file = file = g_module_build_path(*path, class->implem.c.library.name);
    if (!g_file_test(file, G_FILE_TEST_EXISTS)) {
      free(file);
      continue;
    }
    if ((class->implem.c.library.module = g_module_open(file, G_MODULE_BIND_LOCAL))) {
      return 0;
    }
    goto load_model_error;
  }

  for (path = sys_path_list ; *path ; path++) {
    class->implem.c.library.file = file = g_module_build_path(*path, class->implem.c.library.name);
    if (!g_file_test(file, G_FILE_TEST_EXISTS)) {
      free(file);
      continue;
    }
    if ((class->implem.c.library.module = g_module_open(file, G_MODULE_BIND_LOCAL))) {
      return 0;
    }
    goto load_model_error;
  }

  if (class->implem.c.library.module == NULL) {
    fprintf(stderr, "config: class '%s': Unable to load model '%s' (%s) (parse_class_module_c())\n",
        class->name, class->implem.c.library.name, "Not found");
    return -1;
  }

  return 0;

  load_model_error:
  fprintf(stderr, "config: class '%s', unable to load library '%s' (%s) (parse_class_module_c())\n",
      class->name, class->implem.c.library.name, g_module_error());
  return -1;
}


/**
 * \brief Init pointer on functions for a class implemented in c
 * \param class, the class pointer
 * \return 0 in case of success, -1 else
 **/
int parse_class_symbols_c(class_t *class) {
  char *s;

  if (g_module_symbol(class->implem.c.library.module, s = "model",  (gpointer *) &(class->model)) != TRUE) {
    fprintf(stderr, "config: model '%s': unable to find symbol '%s' (parse_class_symbols_c())\n",
        class->implem.c.library.name, s);
    return -1;
  }
  if (g_module_symbol(class->implem.c.library.module, s = "init",  (gpointer *) &(class->init)) != TRUE) {
    class->init = NULL;
  }
  if (g_module_symbol(class->implem.c.library.module, s = "destroy",  (gpointer *) &(class->destroy)) != TRUE) {
    class->destroy = NULL;
  }
  if (g_module_symbol(class->implem.c.library.module, s = "bootstrap",  (gpointer *) &(class->bootstrap)) != TRUE) {
    class->bootstrap = NULL;
  }
  if (g_module_symbol(class->implem.c.library.module, s = "bind",  (gpointer *) &(class->bind)) != TRUE) {
    class->bind = NULL;
  }
  if (g_module_symbol(class->implem.c.library.module, s = "unbind",  (gpointer *) &(class->unbind)) != TRUE) {
    class->unbind = NULL;
  }
  if (g_module_symbol(class->implem.c.library.module, s = "ioctl",  (gpointer *) &(class->ioctl)) != TRUE) {
    class->ioctl = NULL;
  }
  if (g_module_symbol(class->implem.c.library.module, s = "methods",  (gpointer *) &(class->methods)) != TRUE) {
    class->methods = NULL;
  }

  return 0;
}

/* ************************************************** */
/* ************************************************** */
/**
 * \brief Parse library parameter for a class implemented in c
 * \param nd1, the xml node pointer
 * \param class, the class pointer
 * \return 0 in case of success, -1 else
 **/
int parse_class_library_cxx(xmlNodePtr nd1, class_t *class)
{
  xmlNodePtr nd2;

  for (nd2 = nd1->children; nd2; nd2 = nd2->next)
  {
    if (!strcmp((char *) nd2->name, XML_E_PARAM))
    {
      if(!strcmp(get_xml_attr_content(nd2, XML_A_KEY), XML_C_LIBRARY))
      {
        class->implem.cxx.library.name = strdup(get_xml_attr_content(nd2, XML_A_VALUE));
        if (class->implem.cxx.library.name != NULL)
        {
          DBG_CLASS("Class '%s': Library name = '%s'\n", class->name, class->implem.cxx.library.name);
          return 0;
        }
      }
    }
  }
  fprintf(stderr, "config: class '%s': error when parsing library name (parse_class_library_cxx())\n", class->name);
  return -1;
}


/**
 * \brief Load module for a class implemented in c
 * \param class, the class pointer
 * \return 0 in case of success, -1 else
 **/
int parse_class_module_cxx(class_t *class) {
  char **path = NULL;
  char *file = NULL;

  for (path = user_path_list ; *path ; path++) {
    class->implem.cxx.library.file = file = g_module_build_path(*path, class->implem.cxx.library.name);
    if (!g_file_test(file, G_FILE_TEST_EXISTS)) {
      free(file);
      continue;
    }
    if ((class->implem.cxx.library.module = g_module_open(file, G_MODULE_BIND_LOCAL))) {
      return 0;
    }
    goto load_model_error;
  }

  for (path = sys_path_list ; *path ; path++) {
    class->implem.cxx.library.file = file = g_module_build_path(*path, class->implem.cxx.library.name);
    if (!g_file_test(file, G_FILE_TEST_EXISTS)) {
      free(file);
      continue;
    }
    if ((class->implem.cxx.library.module = g_module_open(file, G_MODULE_BIND_LOCAL))) {
      return 0;
    }
    goto load_model_error;
  }

  if (class->implem.cxx.library.module == NULL) {
    fprintf(stderr, "config: class '%s': Unable to load model '%s' (%s) (parse_class_module_cxx())\n",
        class->name, class->implem.cxx.library.name, "Not found");
    return -1;
  }

  return 0;

  load_model_error:
  fprintf(stderr, "config: class '%s', unable to load library '%s' (%s) (parse_class_module_cxx())\n",
      class->name, class->implem.cxx.library.name, g_module_error());
  return -1;
}


/**
 * \brief Init pointer on functions for a class implemented in c
 * \param class, the class pointer
 * \return 0 in case of success, -1 else
 **/
int parse_class_symbols_cxx(class_t *class) {
  char *s;

  if (g_module_symbol(class->implem.cxx.library.module, s = "model",  (gpointer *) &(class->model)) != TRUE) {
    fprintf(stderr, "config: model '%s': unable to find symbol '%s' (parse_class_symbols_cxx())\n",
        class->implem.cxx.library.name, s);
    return -1;
  }
  if (g_module_symbol(class->implem.cxx.library.module, s = "create_object",  (gpointer *) &(class->create_object)) != TRUE) {
    class->create_object = NULL;
  }
  if (g_module_symbol(class->implem.cxx.library.module, s = "bootstrap",  (gpointer *) &(class->bootstrap)) != TRUE) {
    class->bootstrap = NULL;
  }
  if (g_module_symbol(class->implem.cxx.library.module, s = "destroy_object",  (gpointer *) &(class->destroy_object)) != TRUE) {
    class->destroy = NULL;
  }
  if (g_module_symbol(class->implem.cxx.library.module, s = "methods",  (gpointer *) &(class->methods)) != TRUE) {
    class->methods = NULL;
  }

  return 0;
}

/* ************************************************** */
/* ************************************************** */

/****************************************/
/* Functions to parse Python class here */
/****************************************/


/* ************************************************** */
/* ************************************************** */

/****************************************/
/* Functions to parse Ruby class here   */
/****************************************/


/* ************************************************** */
/* ************************************************** */
/**
 * \brief Parse a class implemented in c
 * \param nd1, the xml node pointer
 * \param class, the class pointer
 * \return 0 in case of success, -1 else
 **/
int parse_class_implem_c(xmlNodePtr nd1, class_t *class)
{
  class->implem.c.implem_type = C_IMPLEM;

  if (parse_class_library_c(nd1, class)) {
    return -1;
  }

  if (parse_class_module_c(class)) {
    return -1;
  }

  if (parse_class_symbols_c(class)) {
    return -1;
  }

  return 0;
}

/* ************************************************** */
/**
 * \brief Parse a class implemented in C++
 * \param nd1, the xml node pointer
 * \param class, the class pointer
 * \return 0 in case of success, -1 else
 **/
int parse_class_implem_cxx(xmlNodePtr nd1, class_t *class)
{
  class->implem.cxx.implem_type = CXX_IMPLEM;

  if (parse_class_library_cxx(nd1, class)) {
    return -1;
  }

  if (parse_class_module_cxx(class)) {
    return -1;
  }

  if (parse_class_symbols_cxx(class)) {
    return -1;
  }

  return 0;
}


/**
 * \brief Parse a class implemented in python
 * \param nd1, the xml node pointer
 * \param class, the class pointer
 * \return 0 in case of success, -1 else
 **/
int parse_class_implem_python(xmlNodePtr nd1, class_t *class)
{
  class->implem.python.implem_type = PYTHON_IMPLEM;

  DBG_CLASS("Class '%s': Class implemented in python: not implemented yet.\n", class->name);

  /************************************************************/
  /* TODO : run python interpreter for the class being parsed */
  /************************************************************/

  return 0;
}


/**
 * \brief Parse a class implemented in ruby
 * \param nd1, the xml node pointer
 * \param class, the class pointer
 * \return 0 in case of success, -1 else
 **/
int parse_class_implem_ruby(xmlNodePtr nd1, class_t *class)
{
  class->implem.ruby.implem_type = RUBY_IMPLEM;

  DBG_CLASS("Class '%s': Class implemented in ruby: not implemented yet.\n", class->name);

  /************************************************************/
  /* TODO : run ruby interpreter for the class being parsed   */
  /************************************************************/

  return 0;
}


/**
 * \brief Parse the 'implementation' element of a class
 * \param nd1, the xml pointer
 * \param class, the class pointer
 * \return 0 in case of success, -1 else
 **/
/* ************************************************** */
/* ************************************************** */
int parse_class_implem(xmlNodePtr nd1, class_t *class)
{
  xmlNodePtr nd2;

  DBG_CLASS("Class '%s': **Parsing implementation**\n", class->name);

  for (nd2 = nd1->children; nd2; nd2 = nd2->next) {
    // class implemented in C++
    if (!strcmp((char *) nd2->name, XML_E_CXX)) {
      return parse_class_implem_cxx(nd2, class);
    }


    // class implemented in C
    if (!strcmp((char *) nd2->name, XML_E_C)) {
      return parse_class_implem_c(nd2, class);
    }

    // class implemented in Python
    if (!strcmp((char *) nd2->name, XML_E_PYTHON)) {
      return parse_class_implem_python(nd2, class);
    }

    // class implemented in Ruby
    if (!strcmp((char *) nd2->name, XML_E_RUBY)) {
      return parse_class_implem_ruby(nd2, class);
    }
  }

  return 0;
}


/**
 * \brief Parse the 'parameters' element of a class
 * \param nd1, the xml pointer
 * \param class, the class pointer
 * \return 0 in case of success, -1 else
 **/
int parse_class_param(xmlNodePtr nd1, class_t *class)
{
  void *params;
  param_t *param;
  xmlNodePtr nd2;
  xmlNodePtr nd3;
  int ok = 0;
  call_t to = {class->id, -1};

  DBG_CLASS("Class '%s': **Parsing parameters**\n", class->name);

  if ((params = list_create()) == NULL) {
    return -1;
  }

  for (nd2 = nd1->children; nd2; nd2 = nd2->next)
  {
    if (!strcmp((char *) nd2->name, XML_E_CLASS_PARAMETERS))
    {
      for (nd3 = nd2->children; nd3; nd3 = nd3->next)
      {
        if (!strcmp((char *) nd3->name, XML_E_PARAM))
        {
          if ((param = (param_t *) malloc(sizeof(param_t))) == NULL)
          {
            fprintf(stderr, "config: malloc error (parse_class_param())\n");
            return -1;
          }

          char *content;
          if ((content = get_xml_attr_content(nd3, XML_A_KEY)) != NULL)
          {
            param->key = content;
            DBG_CLASS("Class '%s': Save param key '%s'\n", class->name, content);
          }
          if ((content = get_xml_attr_content(nd3, XML_A_VALUE)) != NULL)
          {
            param->value = content;
            DBG_CLASS("Class '%s': Save param value '%s'\n", class->name, content);
          }

          list_insert(params, param);
        }
      }
    }
  }

  ok = class->init ? class->init(&to, params) : 0;

  while ((param = (param_t *) list_pop(params)) != NULL)
  {
    free(param);
  }
  list_destroy(params);

  return ok;
}


/**
 * \brief Parse the 'default_parameters' element of a class
 * \param nd1, the xml pointer
 * \param class, the class pointer
 * \return 0 in case of success, -1 else
 **/
int parse_class_dflt_param(xmlNodePtr nd1, class_t *class)
{
  xmlNodePtr nd2;
  xmlNodePtr nd3;
  param_t *param;

  DBG_CLASS("Class '%s': **Parsing default parameters**\n", class->name);

  for (nd2 = nd1->children; nd2; nd2 = nd2->next)
  {
    if (!strcmp((char *) nd2->name, XML_E_DEFAULT_PARAMETERS))
    {
      dflt_param_t *dflt_param;

      if ((dflt_param = (dflt_param_t *) malloc(sizeof(dflt_param_t))) == NULL)
      {
        fprintf(stderr, "config: malloc error (parse_class_dflt_param())\n");
        return -1;
      }

      dflt_param->classid = class->id;
      dflt_param->nodearchid = -1;
      dflt_param->nodeid = -1;
      dflt_param->mediumid = -1;
      dflt_param->environmentid = -1;

      if ((dflt_param->params = list_create()) == NULL)
      {
        return -1;
      }

      for (nd3 = nd2->children; nd3; nd3 = nd3->next)
      {
        if (!strcmp((char *) nd3->name, XML_E_PARAM))
        {
          if ((param = (param_t *) malloc(sizeof(param_t))) == NULL)
          {
            fprintf(stderr, "config: malloc error (parse_class_dflt_param())\n");
            return -1;
          }

          char *content;
          if ((content = get_xml_attr_content(nd3, XML_A_KEY)) != NULL)
          {
            param->key = content;
            DBG_CLASS("Class '%s': Save default param key '%s'\n", class->name, content);
          }
          if ((content = get_xml_attr_content(nd3, XML_A_VALUE)) != NULL)
          {
            param->value = content;
            DBG_CLASS("Class '%s': Save default param value '%s'\n", class->name, content);
          }

          list_insert(dflt_param->params, param);
        }
      }
      list_insert(dflt_params, dflt_param);
      DBG_CLASS("Class '%s': Parameters saved\n", class->name);
    }
  }

  return 0;
}


/* ************************************************** */
/* ************************************************** */
/**
 * \brief Allocate memory for the class pointers on mediums, environments, and node architectures
 * \param class, the class pointer
 * \return 0 in case of success, -1 else
 **/
int parse_class_pointers(class_t *class)
{
  int i;

  /* nodearch */
  class->nodearchs.size = nodearchs.size;
  if ((class->nodearchs.elts = (int *) malloc(sizeof(int) * nodearchs.size)) == NULL)
  {
    fprintf(stderr, "config: malloc error (parse_class())\n");
    return -1;
  }

  for (i = 0; i < nodearchs.size; i++)
  {
    class->nodearchs.elts[i] = -1;
  }

  /* medium */
  class->mediums.size = mediums.size;
  if ((class->mediums.elts = (int *) malloc(sizeof(int) * mediums.size)) == NULL)
  {
    fprintf(stderr, "config: malloc error (parse_class())\n");
    return -1;
  }

  for (i = 0; i < mediums.size; i++)
  {
    class->mediums.elts[i] = -1;
  }

  /* environment */
  class->environments.size = environments.size;
  if ((class->environments.elts = (int *) malloc(sizeof(int) * environments.size)) == NULL)
  {
    fprintf(stderr, "config: malloc error (parse_class())\n");
    return -1;
  }

  if ((class->objects.object = (void **) malloc(sizeof(void*) *nodes.size)) == NULL) {
    fprintf(stderr, "config: malloc error (parse_classes())\n");
    return -1;
  }

  for (i = 0; i < environments.size; i++)
  {
    class->environments.elts[i] = -1;
  }

  return 0;
}


/* ************************************************** */
/* ************************************************** */
/**
 * \brief Parse a class
 * \param nd1, the xml pointer
 * \param class, the class pointer
 * \return 0 in case of success, -1 else
 **/
int parse_class(xmlNodePtr nd1, classid_t id)
{
  class_t *class = get_class_by_id(id);
  char    *class_name;

  /* get class name */
  if ((class_name = get_xml_attr_content(nd1, XML_A_CLASS)) != NULL)
  {
    class->name = strdup(class_name);
  }
  else
    return -1;

  DBG_CLASS("\n===Parsing class '%s'===\n", class->name);

  /* parse implementation element of the class */
  if (parse_class_implem(nd1, class))
  {
    //fprintf(stderr, "config: class '%s': error when parsing class implementation (parse_class())\n", class->name);
    return -1;
  }

  /* parse class_parameters element of the class */
  if (parse_class_param(nd1, class))
  {
    fprintf(stderr, "config: class '%s': error when parsing class parameters (parse_class())\n", class->name);
    return -1;
  }

  /* parse default_parameters element of the class */
  if (parse_class_dflt_param(nd1, class))
  {
    fprintf(stderr, "config: class '%s': error when parsing class default parameters (parse_class())\n", class->name);
    return -1;
  }

  /* init class pointers */
  if (parse_class_pointers(class))
  {
    fprintf(stderr, "config: class '%s': error when initializing class pointers (parse_class())\n", class->name);
    return -1;
  }

  return 0;
}


/**
 * \brief Init the class elements
 * \param id, the identifier of the class to init
 **/
/* ************************************************** */
/* ************************************************** */
void parse_class_init(int id) {
  class_t *class = get_class_by_id(id);

  class->id                = id;
  class->name              = NULL;
  class->nodearchs.size    = 0;
  class->nodearchs.elts    = NULL;
  class->mediums.size      = 0;
  class->mediums.elts      = NULL;
  class->environments.size = 0;
  class->environments.elts = NULL;
  class->create_object	 = NULL;
  class->destroy_object	 = NULL;
  class->init              = NULL;
  class->destroy           = NULL;
  class->bootstrap         = NULL;
  class->bind              = NULL;
  class->unbind            = NULL;
  class->ioctl             = NULL;
  class->model             = NULL;
  class->objects.size    = 0;
  class->objects.object  = NULL;
  class->objects.indexes   = hashtable_create(class_index_hash, class_index_equal, NULL, NULL);
  class->objects.saved_indexes = list_create();

  memset(&(class->implem), 0, sizeof(class->implem));
}


/* ************************************************** */
/* ************************************************** */
int parse_classes(xmlNodeSetPtr nodeset)
{
  int i;

  DBG_CLASS("\n===================CLASSES=====================\n");

  if ((classes.elts = (class_t *) malloc(sizeof(class_t) * classes.size)) == NULL) {
    fprintf(stderr, "config: malloc error (parse_classes())\n");
    return -1;
  }

  for (i = 0 ; i < classes.size ; i++) {
    parse_class_init(i);
  }

  /* parse global map in first (to register global area dimension) */
  for (i = 0 ; i < classes.size ; i++) {
    if (!strcmp((char *) nodeset->nodeTab[i]->name, XML_E_GLOBAL_MAP)) {
      if (parse_class(nodeset->nodeTab[i], i)) {
        return -1;
      }
      else {
        simulation_set_global_map(i);
      }
    }
  }

  /* parse all classes, excepted global map */
  for (i = 0 ; i < classes.size ; i++) {
    if (strcmp((char *) nodeset->nodeTab[i]->name, XML_E_GLOBAL_MAP)) {
      if (parse_class(nodeset->nodeTab[i], i)) {
        return -1;
      }
    }
  }


  print_classes();

  return 0;
}
