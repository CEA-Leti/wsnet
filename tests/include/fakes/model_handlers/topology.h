/**
 *  \file   topology.h
 *  \brief  Topology support 
 *  \author Luiz Henrique Suraty Filho
 *  \date   2018
 **/

#ifndef WSNET_TEST_FAKES_MODEL_HANDLERS_TOPOLOGY_H_
#define WSNET_TEST_FAKES_MODEL_HANDLERS_TOPOLOGY_H_

#include <kernel/include/model_handlers/link.h>


/* ************************************************** */
/* ************************************************** */

#ifdef __cplusplus
extern "C"{
#endif


int __wrap_topology_init(void) {
    return 0;
}

int __wrap_topology_bootstrap(void) {
    return 0;
}


void __wrap_topology_clean(void) {
    return;
}


/* ************************************************** */
/* ************************************************** */
position_t *__wrap_get_topology_area(void) {
  return nullptr;
}


#ifdef __cplusplus
}
#endif

#endif //WSNET_TEST_FAKES_MODEL_HANDLERS_LINK_H_
