/**
 *  \file   grouparch_config.h
 *  \brief  grouparch definitions
 *  \author Luiz Henrique Suraty Filho
 *  \date   2016
 */

#ifndef WSNET_KERNEL_CONFIGURATION_PARSER_GROUPARCH_CONFIGURATION_H_
#define WSNET_KERNEL_CONFIGURATION_PARSER_GROUPARCH_CONFIGURATION_H_

#include <kernel/include/configuration_parser/xmlparser.h>


/* ************************************************** */
/* ************************************************** */
extern grouparch_t *dflt_grouparch;


/* ************************************************** */
/* ************************************************** */
#ifdef __cplusplus
extern "C"{
#endif
/**
 * \brief Parse 'group_architecture' elements of the xml configuration file
 * \param groupset, the xml pointer on class groups in the xml file
 * \return 0 in case of success, -1 else
 **/
int parse_grouparchs(xmlNodeSetPtr groupset);

#ifdef __cplusplus
}
#endif

#endif //WSNET_KERNEL_CONFIGURATION_PARSER_GROUPARCH_CONFIGURATION_H_
