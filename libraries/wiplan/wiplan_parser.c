/**
 *  \file   wiplan_parser.c
 *  \brief  XML parser for wiplan configuration file
 *  \author Loic Lemaitre
 *  \date   2010
 */

#include <string.h>
#include "wiplan_parser.h"

/* ************************************************** */
/* ************************************************** */
typedef struct _xmlpathobj
{
  xmlXPathObjectPtr *ptr;
  xmlChar *expr;
}  xmlpathobj_t;


/* ************************************************** */
/* ************************************************** */
static xmlParserCtxtPtr   p_ctxt     = NULL;
static xmlDocPtr          doc        = NULL;
static xmlXPathContextPtr xp_ctx     = NULL;
static xmlXPathObjectPtr  nodes_xobj = NULL;
static xmlpathobj_t       xpathobj[] = {{&nodes_xobj , (xmlChar *) XML_X_NODE}};

static int reading_count; 


/* ************************************************** */
/* ************************************************** */
void wiplan_parser_init(void)
{
  reading_count = 0;
}


/* ************************************************** */
/* ************************************************** */
void wiplan_parser_clean(void)
{
}


/* ************************************************** */
/* ************************************************** */
xmlNodeSetPtr wiplan_parser_start(char *file)
{
  int i;
  
  if (reading_count == 0)
    {
      /* Check XML version */
      LIBXML_TEST_VERSION;

      /* Initialise and parse document */
      p_ctxt = xmlNewParserCtxt();
      if (p_ctxt == NULL)
	{
	  fprintf(stderr, "config: XML parser initialisation failure (wiplan_parser_init())\n");
	  wiplan_parser_close();
	  return NULL;
	}
    
      doc = xmlCtxtReadFile(p_ctxt, file, NULL, XML_PARSE_NONET | XML_PARSE_NOBLANKS | XML_PARSE_NSCLEAN);
      if (doc == NULL)
	{
	  fprintf(stderr, "config: failed to parse %s (wiplan_parser_init())\n", file);
	  wiplan_parser_close();
	  return NULL;
	}

      /* Create xpath context */
      xp_ctx = xmlXPathNewContext(doc);
      if (xp_ctx == NULL)
	{
	  fprintf(stderr, "config: XPath initialisation failure (wiplan_parser_init())\n");
	  wiplan_parser_close();
	  return NULL;
	}
      xmlXPathRegisterNs(xp_ctx, (xmlChar *) XML_NS_ID, (xmlChar *) XML_NS_URL);

      /* Get xpath obj */
      for (i = 0 ; i < (int) (sizeof(xpathobj) / sizeof(xpathobj[0])); i++)
	{
	  *xpathobj[i].ptr = xmlXPathEvalExpression(xpathobj[i].expr, xp_ctx);
	  if (*xpathobj[i].ptr == NULL)
	    {
	      fprintf(stderr, "config: unable to evaluate xpath \"%s\" (do_configuration())\n", xpathobj[i].expr);
	      wiplan_parser_close();
	      return NULL;
	    }
	}
    }
  reading_count++;  

  return nodes_xobj->nodesetval;
}


/* ************************************************** */
/* ************************************************** */
void wiplan_parser_close(void)
{
  int i;

  if (reading_count == 1)
    {
      for (i = 0 ; i < (int) (sizeof(xpathobj) / sizeof(xpathobj[0])); i++)
	{
	  xmlXPathFreeObject(*xpathobj[i].ptr);
	}

      if (xp_ctx)
	{
	  xmlXPathFreeContext(xp_ctx);
	}

      if (doc) 
	{
	  xmlFreeDoc(doc);
	}

      if (p_ctxt) 
	{
	  xmlFreeParserCtxt(p_ctxt);
	}

      xmlCleanupParser();
    }

  reading_count--;
}


/* ************************************************** */
/* ************************************************** */
char *wiplan_parser_get_xml_attr_content(xmlNodePtr nd1, const char *attr_name)
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

