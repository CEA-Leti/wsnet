/**
 *  \file   class_config.h
 *  \brief  class declarations
 *  \author Guillaume Chelius & Elyes Ben Hamida
 *  \date   2007
 **/

#ifndef __class_config__
#define __class_config__

#include "xmlparser.h"


/* ************************************************** */
/* ************************************************** */
#ifdef __cplusplus
extern "C"{
#endif
/**
 * \brief Parse 'class' elements of the xml configuration file
 * \param nodeset, the xml pointer on class nodes in the xml file
 * \return 0 in case of success, -1 else
 **/
int parse_classes(xmlNodeSetPtr nodeset);

#ifdef __cplusplus
}
#endif
#endif //__class_config__
