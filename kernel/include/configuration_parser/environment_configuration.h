/**
 *  \file   environment_config.h
 *  \brief  Environment declarations
 *  \author Guillaume Chelius & Elyes Ben Hamida
 *  \date   2007
 **/

#ifndef __environment_config__
#define __environment_config__

#include "xmlparser.h"


/* ************************************************** */
/* ************************************************** */
#ifdef __cplusplus
extern "C"{
#endif
/**
 * \brief Parse 'environment' elements of the xml configuration file
 * \param nodeset, the xml pointer on class nodes in the xml file
 * \return 0 in case of success, -1 else
 **/
int parse_environments(xmlNodeSetPtr nodeset);

#ifdef __cplusplus
}
#endif

#endif //__environment_config__
