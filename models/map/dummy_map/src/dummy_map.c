/**
 *  \file   dummy_map.c
 *  \brief  Dummy map model
 *  \author Loic Lemaitre
 *  \date   2010
 **/

#include <kernel/include/definitions/types.h>
#include <kernel/include/definitions/class.h>
#include <kernel/include/definitions/environment.h>
#include <models/models_dbg.h>


/* ************************************************** */
/* ************************************************** */
model_t model =  {
    "Dummy map model",
    "Loic Lemaitre",
    "0.1",
    MODELTYPE_MAP
};


/* ************************************************** */
/* ************************************************** */
struct classdata {
};


/* ************************************************** */
/* ************************************************** */
int init(call_t *to, void *params) {
  (void) to;
  (void) params;
  DBG_MAP("model dummy_map.c: initializing class %s\n",
          get_class_by_id(to->class)->name);
  return 0;
}

int destroy(call_t *to) {
  (void) to;
  return 0;
}

int bootstrap(call_t *to, void *params) {
  (void) to;
  (void) params;
  return 0;
}


/* ************************************************** */
/* ************************************************** */
int bind(call_t *to, void *params) {
  (void) to;
  (void) params;
  DBG_MAP("model dummy_map.c: binding class %s with environment %s\n",
          get_class_by_id(to->class)->name, get_environment_by_id(to->object)->name);
  return 0;
}

int unbind(call_t *to) {
  (void) to;
  return 0;
}

map_conditions_t get_scenario_condition(call_t *to, call_t *from, nodeid_t src, nodeid_t dst) {
  (void) to;
  (void) from;
  (void) src;
  (void) dst;
  return MAP_SAME_ROOM;
}

int get_nbr_walls(call_t *to, call_t *from, nodeid_t src, nodeid_t dst) {
  (void) to;
  (void) from;
  (void) src;
  (void) dst;
  return 0;
}

map_methods_t methods = {get_scenario_condition,
                         get_nbr_walls};
