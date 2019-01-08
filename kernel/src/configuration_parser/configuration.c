/**
 *  \file   configuration.c
 *  \brief  XML parser & configuration module
 *  \author Guillaume Chelius & Stephane d'Alu & Loic Lemaitre
 *  \date   2007
 */

#include "class_configuration.h"
#include "medium_configuration.h"
#include "environment_configuration.h"
#include "nodearch_configuration.h"
#include "simulation_configuration.h"
#include <kernel/include/configuration_parser/grouparch_configuration.h>


/* ************************************************** */
/* ************************************************** */
#define DEFAULT_CONFIGFILE "config_test.xml"
#define DEFAULT_SCHEMAFILE DATADIR"/config.xsd"
#define DEFAULT_MODULESDIR LIBDIR"/"
#define ENV_MODDIR         "WSNET_MODDIR"

static char *schemafile      = DEFAULT_SCHEMAFILE;
static char *configfile      = DEFAULT_CONFIGFILE;
static char *user_modulesdir = NULL;
static char *sys_modulesdir  = DEFAULT_MODULESDIR;

gchar **user_path_list;
gchar **sys_path_list;

void *dflt_params = NULL;


/* ************************************************** */
/* ************************************************** */
void config_set_configfile(char *c)
{
  configfile = c;
}

void config_set_schemafile(char *c)
{
  schemafile = c;
}

void config_set_sys_modulesdir(char *c)
{
  sys_modulesdir = c;
}

void config_set_usr_modulesdir(void)
{
  char *env_moddir = getenv(ENV_MODDIR);
  user_modulesdir = env_moddir ? env_moddir : "";
}


/* ************************************************** */
/* ************************************************** */
char *get_xml_attr_content(xmlNodePtr nd1, const char *attr_name)
{
  xmlAttrPtr attr;

  for (attr = nd1->properties; attr; attr = attr->next)
    {
      if (!strcmp((char *) attr->name, attr_name))
	{
	  return (char *) attr->children->content;
	}
    }

  return NULL;
}


/* ************************************************** */
/* ************************************************** */
static void xml_error(void *dummy, const char *format, ...)
{
  va_list args;
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);
}

static void xml_warning(void *dummy, const char *format, ...)
{
  va_list args;
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);
}


/* ************************************************** */
/* ************************************************** */
typedef struct _xmlpathobj
{
  xmlXPathObjectPtr *ptr;
  xmlChar *expr;
}  xmlpathobj_t;


/* ************************************************** */
/* ************************************************** */
void clean_params()
{
  if (dflt_params)
    {
      dflt_param_t *dflt_param;
        
      while ((dflt_param = (dflt_param_t *) list_pop(dflt_params)) != NULL)
	{
	  param_t *param;
            
	  while ((param = (param_t *) list_pop(dflt_param->params)) != NULL)
	    {
	      free(param);
	    }
	  list_destroy(dflt_param->params);
	  free(dflt_param);
	}
        
      list_destroy(dflt_params);
    }
}


dflt_param_t *get_class_params(nodeid_t node, classid_t class, nodearchid_t nodearch, 
		       mediumid_t medium, environmentid_t environment)
{
  dflt_param_t *dflt_param;
  dflt_param_t *best = NULL;
  int match = 0;

  list_init_traverse(dflt_params);

  while ((dflt_param = (dflt_param_t *) list_traverse(dflt_params)) != NULL)
    {
      int c_match = 0;

      if (dflt_param->classid != class)
	continue;
      if ((dflt_param->nodeid        != -1) && (dflt_param->nodeid        != node))
	continue;
      if ((dflt_param->nodearchid    != -1) && (dflt_param->nodearchid    != nodearch))
	continue;
      if ((dflt_param->mediumid      != -1) && (dflt_param->mediumid      != medium))
	continue;
      if ((dflt_param->environmentid != -1) && (dflt_param->environmentid != environment))
	continue;
        
      if (dflt_param->classid == class) c_match++;
      if (node        != -1   &&  dflt_param->nodeid        == node)        c_match++;
      if (nodearch    != -1   &&  dflt_param->nodearchid    == nodearch)    c_match++;
      if (medium      != -1   &&  dflt_param->mediumid      == medium)      c_match++;
      if (environment != -1   &&  dflt_param->environmentid == environment) c_match++;

      if (c_match > match)
	{
	  best  = dflt_param;
	  match = c_match;
	}
    }
 
  if (best == NULL)
    return best;
  return best;
}


int  duplicate_class_params(void *params1, void *params2)
{
  param_t *param1;
  param_t *param2;

  list_init_traverse(params1);
  while ((param1 = (param_t *) list_traverse(params1)) != NULL)
    {
      if ((param2 = (param_t *) malloc(sizeof(param_t))) == NULL)
	{
	  fprintf(stderr, "config: malloc error (duplicate_class_params())\n");
	  return -1;
	}
      param2->key   = param1->key;
      param2->value = param1->value;
      list_insert(params2, param2);
    }
  
  return 0;
}


param_t *find_class_param_key(void *params, char *key)
{
  param_t *param;

  list_init_traverse(params);
  while ((param = (param_t *) list_traverse(params)) != NULL)
    {
      if (!strcmp(param->key, key))
	{
	  return param;
	}
    }
  return NULL;
}


/* ************************************************** */
/* ************************************************** */
int do_configuration(void)
{
  xmlSchemaValidCtxtPtr sv_ctxt      = NULL;
  xmlSchemaParserCtxtPtr sp_ctxt     = NULL;
  xmlSchemaPtr schema                = NULL;
  xmlParserCtxtPtr p_ctxt            = NULL;
  xmlDocPtr doc                      = NULL;
  xmlXPathContextPtr xp_ctx          = NULL; 
  xmlXPathObjectPtr classes_xobj     = NULL; 
  xmlXPathObjectPtr medium_xobj      = NULL; 
  xmlXPathObjectPtr environment_xobj = NULL;
  xmlXPathObjectPtr nodearch_xobj    = NULL;
  xmlXPathObjectPtr grouparch_xobj   = NULL;
  xmlXPathObjectPtr simulation_xobj  = NULL;
  xmlNodeSetPtr nodeset;
  xmlpathobj_t xpathobj[] = {{&classes_xobj    , (xmlChar *) XML_X_CLASSES          }, 
			     {&medium_xobj     , (xmlChar *) XML_X_MEDIUM           },
			     {&environment_xobj, (xmlChar *) XML_X_ENVIRONMENT      },
			     {&nodearch_xobj  ,  (xmlChar *) XML_X_NODE_ARCHITECTURE},
				 {&grouparch_xobj  ,  (xmlChar *) XML_X_GROUP_ARCHITECTURE},
			     {&simulation_xobj , (xmlChar *) XML_X_SIMULATION       }};


  int ok = 0, i;

  /* Check XML version */
  LIBXML_TEST_VERSION;

  /* Initialise and parse schema */
  sp_ctxt = xmlSchemaNewParserCtxt(schemafile);
  if (sp_ctxt == NULL)
    {
      fprintf(stderr, "config: XML schema parser initialisation failure (do_configuration())\n");
      ok = -1;
      goto cleanup;
    }
  xmlSchemaSetParserErrors(sp_ctxt,
			   (xmlSchemaValidityErrorFunc)   xml_error,
			   (xmlSchemaValidityWarningFunc) xml_warning,
			   NULL);
    
  schema = xmlSchemaParse(sp_ctxt);
  if (schema == NULL)
    {
      fprintf(stderr, "config: error in schema %s (do_configuration())\n", schemafile);
      ok = -1;
      goto cleanup;
    }
  xmlSchemaSetValidErrors(sv_ctxt,
			  (xmlSchemaValidityErrorFunc)   xml_error,
			  (xmlSchemaValidityWarningFunc) xml_warning,
			  NULL);
  
  sv_ctxt = xmlSchemaNewValidCtxt(schema);
  if (sv_ctxt == NULL)
    {
      fprintf(stderr, "config: XML schema validator initialisation failure (do_configuration())\n");
      ok = -1;
      goto cleanup;
    }

  /* Initialise and parse document */
  p_ctxt = xmlNewParserCtxt();
  if (p_ctxt == NULL)
    {
      fprintf(stderr, "config: XML parser initialisation failure (do_configuration())\n");
      ok = -1;
      goto cleanup;
    }
    
  doc = xmlCtxtReadFile(p_ctxt, configfile, NULL, XML_PARSE_NONET | XML_PARSE_NOBLANKS | XML_PARSE_NSCLEAN);
  if (doc == NULL)
    {
      fprintf(stderr, "config: failed to parse %s (do_configuration())\n", configfile);
      ok = -1;
      goto cleanup;
    }

  /* Validate document */
  if (xmlSchemaValidateDoc(sv_ctxt, doc))
    {
      fprintf(stderr, "config: error in configuration file %s (do_configuration())\n", configfile);
      ok = -1;
      goto cleanup;
    }

  /* Create xpath context */
  xp_ctx = xmlXPathNewContext(doc);
  if (xp_ctx == NULL)
    {
      fprintf(stderr, "config: XPath initialisation failure (do_configuration())\n");
      ok = -1;
      goto cleanup;
    }
  xmlXPathRegisterNs(xp_ctx, (xmlChar *) XML_NS_ID, (xmlChar *) XML_NS_URL); 

  /* Get xpath obj */
  for (i = 0 ; i < (int) (sizeof(xpathobj) / sizeof(xpathobj[0])); i++)
    {
      *xpathobj[i].ptr = xmlXPathEvalExpression(xpathobj[i].expr, xp_ctx);
      if (*xpathobj[i].ptr == NULL)
	{
	  fprintf(stderr, "config: unable to evaluate xpath \"%s\" (do_configuration())\n", xpathobj[i].expr);
	  ok = -1;
	  goto cleanup; 
	}
    }

  /***************/
  /* Counting... */
  /***************/
  nodeset = classes_xobj->nodesetval;
  if ((classes.size = (nodeset) ? nodeset->nodeNr : 0) == 0)
    {
      fprintf(stderr, "config: no class defined (do_configuration())\n");
      ok = -1; 
      goto cleanup;
    }
  fprintf(stderr, "Found %d classes...\n", classes.size);
  
  nodeset = medium_xobj->nodesetval;
  if ((mediums.size = (nodeset) ? nodeset->nodeNr : 0) == 0)
    {
      fprintf(stderr, "config: no medium defined (do_configuration())\n");
      ok = -1; 
      goto cleanup;
    }
  fprintf(stderr, "Found %d mediums...\n", mediums.size);

  nodeset = environment_xobj->nodesetval;
  if ((environments.size = (nodeset) ? nodeset->nodeNr : 0) == 0)
    {
      fprintf(stderr, "config: no environment defined (do_configuration())\n");
      ok = -1; 
      goto cleanup;
    }
  fprintf(stderr, "Found %d environments...\n", environments.size);

  nodeset = nodearch_xobj->nodesetval;
  if ((nodearchs.size = (nodeset) ? nodeset->nodeNr : 0) == 0) 
    {
      fprintf(stderr, "config: no node architecture defined (do_configuration())\n");
      ok = -1; 
      goto cleanup;
    }
  fprintf(stderr, "Found %d node architectures...\n", nodearchs.size);
    

  nodeset = grouparch_xobj->nodesetval;
    if ((grouparchs.size = (nodeset) ? nodeset->nodeNr : 0) == 0)
      {
        fprintf(stderr, "config: no group architecture defined (do_configuration())\n");
      }
  fprintf(stderr, "Found %d group architectures...\n", grouparchs.size);

  if ((dflt_params = list_create()) == NULL)
    {
      ok = -1;
      goto cleanup;
    }

  /*******************************************************/
  /* Number of nodes (needed by model for init function) */
  /*******************************************************/
  parse_simulation_node_nbr(simulation_xobj->nodesetval);

  /***********/
  /* Classes */
  /***********/
  /* initialize library paths */
  config_set_usr_modulesdir();
  user_path_list = g_strsplit(user_modulesdir, ":", 0); /* TOCLEAN */
  sys_path_list  = g_strsplit(sys_modulesdir,  ":", 0); /* TOCLEAN */
  
  if (parse_classes(classes_xobj->nodesetval)) {
    ok = -1;
    goto cleanup;
  }

  /******************************************************************************************/
  /* Simulation begin parsing (global_map needs to be parse before mediums, environment...) */
  /******************************************************************************************/
  if (parse_simulation_begin(simulation_xobj->nodesetval)) {
    ok = -1;
    goto cleanup;
  }

  /**************/
  /* Mediums    */
  /**************/
  if (parse_mediums(medium_xobj->nodesetval)) {
    ok = -1;
    goto cleanup;
  }

  /****************/
  /* Environments */
  /****************/
  if (parse_environments(environment_xobj->nodesetval)) {
    ok = -1;
    goto cleanup;
  }

  /**********************/
  /* Node architectures */
  /**********************/
  if (parse_nodearchs(nodearch_xobj->nodesetval)) {
    ok = -1;
    goto cleanup;
  }

  /**********************/
  /* Group architectures */
  /**********************/
  if (parse_grouparchs(grouparch_xobj->nodesetval)) {
    ok = -1;
    goto cleanup;
  }

  /*********************/
  /* Number of groups  */
  /*********************/
  parse_simulation_group_nbr(simulation_xobj->nodesetval);

  /**************************/
  /* Simulation end parsing */
  /**************************/
  if (parse_simulation_end(simulation_xobj->nodesetval)) {
    ok = -1;
    goto cleanup;
  }


 cleanup:
  clean_params();

  g_strfreev(user_path_list);
  g_strfreev(sys_path_list);


  for (i = 0 ; i < (int) (sizeof(xpathobj) / sizeof(xpathobj[0])); i++) {
    xmlXPathFreeObject(*xpathobj[i].ptr);
  }

  if (xp_ctx) {
    xmlXPathFreeContext(xp_ctx);
  }

  if (sp_ctxt) {
    xmlSchemaFreeParserCtxt(sp_ctxt);		
  }

  if (schema) {
    xmlSchemaFree(schema);
  }

  if (sv_ctxt) {
    xmlSchemaFreeValidCtxt(sv_ctxt);
  }

  if (doc) {
    xmlFreeDoc(doc);
  }

  if (p_ctxt) {
    xmlFreeParserCtxt(p_ctxt);
  }

  xmlCleanupParser();
  return ok;
}
