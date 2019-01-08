/**
 *  \file   dummy_mobility.c
 *  \brief  Dummy mobility model
 *  \author Loic Lemaitre
 *  \date   2010
 **/

#include <kernel/include/definitions/types.h>
#include <kernel/include/definitions/class.h>
#include <kernel/include/definitions/node.h>
#include <models/models_dbg.h>


/* ************************************************** */
/* ************************************************** */
model_t model =  {
    "Dummy sensor model",
    "Loic Lemaitre",
    "0.1",
    MODELTYPE_SENSOR
};


/* ************************************************** */
/* ************************************************** */
struct classdata {
};


/* ************************************************** */
/* ************************************************** */
int init(call_t *to, void *params) {
  DBG_MOBILITY("model dummy_mobility.c: initializing class %s\n",
	     get_class_by_id(to->class)->name);
  return 0;
}

int destroy(call_t *to) {
  return 0;
}

int bootstrap(call_t *to, void *params) {
  return 0;
}


/* ************************************************** */
/* ************************************************** */
int bind(call_t *to, void *params) {
  DBG_MOBILITY("model dummy_mobility.c: binding class %s with node %d\n",
	     get_class_by_id(to->class)->name, get_node_by_id(to->object)->id);
  return 0;
}

int unbind(call_t *to) {
  return 0;
}
