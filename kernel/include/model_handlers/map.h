/**
 *  \file   map.h
 *  \brief  Map declarations
 *  \author Luiz Henrique Suraty Filho
 *  \date   2016
 **/

#ifndef WSNET_KERNEL_MODEL_HANDLERS_MAP_H_
#define WSNET_KERNEL_MODEL_HANDLERS_MAP_H_

#include <kernel/include/definitions/types.h>

#ifdef __cplusplus
extern "C"{
#endif

map_conditions_t map_get_scenario_condition(call_t *to, call_t *from, int src, int dst);

int map_get_nbr_walls(call_t *to, call_t *from, nodeid_t src, nodeid_t dst);

#ifdef __cplusplus
}
#endif

#endif // WSNET_KERNEL_MODEL_HANDLERS_MAP_H_
