/**
 *  \file   map.c
 *  \brief  Map utilities
 *  \author Luiz Henrique Suraty Filho
 *  \date   2016
 **/

#include <kernel/include/definitions/class.h>
#include <kernel/include/model_handlers/map.h>


map_conditions_t map_get_scenario_condition(call_t *to, call_t *from, int src, int dst){
	class_t *class = get_class_by_id(to->class);
	if (class->model->type != MODELTYPE_MAP)
		return 0;
	return class->methods->map.get_scenario_condition(to, from, src, dst);
}

int map_get_nbr_walls(call_t *to, call_t *from, nodeid_t src, nodeid_t dst){
  class_t *class = get_class_by_id(to->class);
  if (class->model->type != MODELTYPE_MAP)
    return 0;
  return class->methods->map.get_nbr_walls(to, from, src, dst);
}
