/**
 *  \file   wiplan_parser.h
 *  \brief  wiplan XML parser header
 *  \author Loic Lemaitre
 *  \date   2010
 */

#ifndef WIPLAN_PARSER_H
#define WIPLAN_PARSER_H

#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include <libxml/xmlschemas.h>


/* ************************************************** */
/* ************************************************** */

/*************/
/* Namespace */
/*************/
#define XML_NS_ID		"w"
#define XML_NS_URL		"http://www.cea.fr"


/************/
/* Elements */
/************/
#define XML_E_CONFIG            "config"
#define XML_E_NODE              "node"
#define XML_E_POSITION          "pos"
#define XML_E_ANTENNA           "antenna"
#define XML_E_RADIO             "radio"
#define XML_E_RXNODE            "rxnode"


/**************/
/* Attributes */
/**************/
#define XML_A_ID                "id"
#define XML_A_X                 "x"
#define XML_A_Y                 "y"
#define XML_A_Z                 "z"
#define XML_A_POWER             "power"
#define XML_A_DBM               "dbm"


/*********/
/* XPath */
/*********/
#define XML_X_NODE  "/" XML_NS_ID ":" XML_E_CONFIG\
                    "/" XML_NS_ID ":" XML_E_NODE


/* ************************************************** */
/* ************************************************** */
#ifdef __cplusplus
extern "C"{
#endif


void          wiplan_parser_init                (void);
void          wiplan_parser_clean               (void);
xmlNodeSetPtr wiplan_parser_start               (char *file);
void          wiplan_parser_close               (void);
char         *wiplan_parser_get_xml_attr_content(xmlNodePtr nd1, const char *attr_name);

#ifdef __cplusplus
}
#endif

#endif /* WIPLAN_PARSER_H */
