/**
 *  \file   map.h
 *  \brief  Map utilities
 *  \author Luiz Henrique Suraty Filho
 *  \date   2016
 **/

#ifndef WSNET_TEST_FAKES_MODEL_HANDLERS_MAP_H_
#define WSNET_TEST_FAKES_MODEL_HANDLERS_MAP_H_

#include <kernel/include/model_handlers/map.h>

/* ************************************************** */
/* ************************************************** */

#ifdef __cplusplus
extern "C"{
#endif


map_conditions_t map_get_scenario_condition(call_t *to, call_t *from, int src, int dst){
  (void) to;
  (void) from;
  (void) src;
  (void) dst;
	return MAP_SAME_ROOM;
}

int map_get_nbr_walls(call_t *to, call_t *from, nodeid_t src, nodeid_t dst){
  (void) to;
  (void) from;
  (void) src;
  (void) dst;
  return 0;
}

#ifdef __cplusplus
}
#endif

#endif //WSNET_TEST_FAKES_MODEL_HANDLERS_MAP_H_
