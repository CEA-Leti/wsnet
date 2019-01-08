/**
 *  \file   simulation_config.h
 *  \brief  Simulation declarations
 *  \author Guillaume Chelius & Elyes Ben Hamida & Loic Lemaitre
 *  \date   2007
 **/
#ifndef __simulation_config__
#define __simulation_config__

#include "xmlparser.h"

#ifdef __cplusplus
extern "C"{
#endif
/* ************************************************** */
/* ************************************************** */
/**
 * \brief Parse node attribute of'simulation' element of the xml configuration file
 * \param nodeset, the xml pointer on class nodes in the xml file
 * \return 0 in case of success, -1 else
 **/
int parse_simulation_node_nbr(xmlNodeSetPtr nodeset);

int parse_simulation_group_nbr(xmlNodeSetPtr nodeset);

/**
 * \brief Parse classes in 'simulation' element of the xml configuration file (global map, monitors...)
 * \param nodeset, the xml pointer on class nodes in the xml file
 * \return 0 in case of success, -1 else
 **/
int parse_simulation_begin(xmlNodeSetPtr nodeset);

/**
 * \brief Parse nodes in 'simulation' element of the xml configuration file
 * \param nodeset, the xml pointer on class nodes in the xml file
 * \return 0 in case of success, -1 else
 **/
int parse_simulation_end(xmlNodeSetPtr nodeset);

/**
 * \brief Set global map class id
 * \param id, the class id
 **/
void simulation_set_global_map(int id);


#ifdef __cplusplus
}
#endif

#endif //__simulation_config__
