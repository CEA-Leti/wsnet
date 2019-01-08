/**
 *  \file   dummy_physical.c
 *  \brief  Dummy physical model
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
    "Dummy physical model",
    "Loic Lemaitre",
    "0.1",
    MODELTYPE_PHYSICAL
};


/* ************************************************** */
/* ************************************************** */
struct classdata {
};


/* ************************************************** */
/* ************************************************** */
int init(call_t *to, void *params) {
  DBG_PHYSICAL("model dummy_physical.c: initializing class %s\n",
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
  DBG_PHYSICAL("model dummy_physical.c: binding class %s with environment %s\n",
	     get_class_by_id(to->class)->name, get_environment_by_id(to->object)->name);
  return 0;
}

int unbind(call_t *to) {
  return 0;
}
