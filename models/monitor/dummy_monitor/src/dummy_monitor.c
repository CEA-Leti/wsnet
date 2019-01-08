/**
 *  \file   dummy_monitor.c
 *  \brief  Dummy monitor model
 *  \author Loic Lemaitre
 *  \date   2010
 **/

#include <kernel/include/modelutils.h>


/* ************************************************** */
/* ************************************************** */
model_t model =  {
    "Dummy monitor model",
    "Loic Lemaitre",
    "0.1",
    MODELTYPE_MONITOR
};


/* ************************************************** */
/* ************************************************** */
struct classdata {
};


/* ************************************************** */
/* ************************************************** */
int init(call_t *to, void *params) {
  DBG_MONITOR("model dummy_monitor.c: initializing class %s\n",
	     get_class_by_id(to->class)->name);
  monitor_register(to, SIG_NODE_BIRTH);

  return 0;
}

int destroy(call_t *to) {
  monitor_unregister(to);

  return 0;
}

int bootstrap(call_t *to, void *params) {
  return 0;
}


/* ************************************************** */
/* ************************************************** */
int bind(call_t *to, void *params) {
  return 0;
}

int unbind(call_t *to) {
  return 0;
}


/* ************************************************** */
/* ************************************************** */
void fire(call_t *to, call_t *from, void *data) {
  DBG_MONITOR("model dummy_monitor.c: node %d birth!\n", *((nodeid_t*) data));
}

/* ************************************************** */
/* ************************************************** */
monitor_methods_t methods = {fire};
