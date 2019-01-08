/**
 *  \file   nodearch_config.h
 *  \brief  nodearch declarations
 *  \author Guillaume Chelius & Elyes Ben Hamida
 *  \date   2007
 **/

#ifndef __nodearch_config__
#define __nodearch_config__

#include "xmlparser.h"


/* ************************************************** */
/* ************************************************** */
extern nodearch_t *dflt_nodearch;


/* ************************************************** */
/* ************************************************** */
#ifdef __cplusplus
extern "C"{
#endif
/**
 * \brief Parse 'node_architecture' elements of the xml configuration file
 * \param nodeset, the xml pointer on class nodes in the xml file
 * \return 0 in case of success, -1 else
 **/
int parse_nodearchs(xmlNodeSetPtr nodeset);

#ifdef __cplusplus
}
#endif

#endif //__nodearch_config__
