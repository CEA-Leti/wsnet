/**
 *  \file   medium_config.h
 *  \brief  Medium configuration declarations
 *  \author Loic Lemaitre
 *  \date   2010
 **/

#ifndef __medium_config__
#define __medium_config__

#include "xmlparser.h"


/* ************************************************** */
/* ************************************************** */
#ifdef __cplusplus
extern "C"{
#endif
/**
 * \brief Parse 'medium' elements of the xml configuration file
 * \param nodeset, the xml pointer on class nodes in the xml file
 * \return 0 in case of success, -1 else
 **/
int parse_mediums(xmlNodeSetPtr nodeset);

#ifdef __cplusplus
}
#endif

#endif //__medium_config__
