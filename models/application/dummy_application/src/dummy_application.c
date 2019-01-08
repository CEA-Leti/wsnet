/**
 *  \file   dummy_application.c
 *  \brief  Dummy application model
 *  \author Loic Lemaitre
 *  \date   2010
 **/

#include <kernel/include/definitions/types.h>
#include <kernel/include/definitions/class.h>
#include <kernel/include/definitions/node.h>
#include <kernel/include/definitions/packet.h>
#include <models/models_dbg.h>



/* ************************************************** */
/* ************************************************** */
model_t model =  {
    "Dummy application model",
    "Loic Lemaitre",
    "0.1",
    MODELTYPE_APPLICATION
};


/* ************************************************** */
/* ************************************************** */
struct classdata {
};


/* ************************************************** */
/* ************************************************** */
int init(call_t *to, void *params) {
  DBG_APPLICATION("model dummy_application.c: initializing class %s\n",
	     get_class_by_id(to->class)->name);
  return 0;
}

int destroy(call_t *to) {
  return 0;
}

int bootstrap(call_t *to, void *params) {
  return 0;
}

int ioctl(call_t *to, int option, void *in, void **out) {
    return 0;
}

/* ************************************************** */
/* ************************************************** */
int bind(call_t *to, void *params) {
  DBG_APPLICATION("model dummy_application.c: binding class %s with node %d\n",
	     get_class_by_id(to->class)->name, get_node_by_id(to->object)->id);
  return 0;
}

int unbind(call_t *to) {
  return 0;
}

/* ************************************************** */
/* ************************************************** */
void rx(call_t *to, call_t *from, packet_t *packet) {
    packet_dealloc(packet);
}

/* ************************************************** */
/* ************************************************** */
application_methods_t methods = {rx};
