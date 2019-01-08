/**
 *  \file   topology.c
 *  \brief  Topology support 
 *  \author Guillaume Chelius
 *  \date   2007
 **/
#include "topology.h"
#include <kernel/include/definitions/simulation.h>
#include <kernel/include/definitions/class.h>


/* ************************************************** */
/* ************************************************** */
int topology_init(void) {
    return 0;
}

int topology_bootstrap(void) {
    return 0;
}


void topology_clean(void) {
    return;
}


/* ************************************************** */
/* ************************************************** */
position_t *get_topology_area(void) {
  call_t to   = {global_map, -1};
  call_t from = {-1, -1};
  class_t *class = get_class_by_id(global_map);
  return class->methods->global_map.get_global_area(&to, &from);
}
