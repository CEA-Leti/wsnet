/**
 *  \file   rayleigh.c
 *  \brief  Rayleigh fading model
 *  \author Guillaume Chelius and Elyes Ben Hamida and Ruifeng Zhang
 *  \date   2007
 **/

#include <kernel/include/modelutils.h>

#define VARIANCE 0.6366197723676

/* ************************************************** */
/* ************************************************** */
model_t model =  {
    "Rayleigh Fading model",
    "Elyes Ben Hamida",
    "0.1",
    MODELTYPE_FADING
};


/* ************************************************** */
/* ************************************************** */
struct classdata {
};


/* ************************************************** */
/* ************************************************** */
int init(call_t *to, void *params) {
  DBG_FADING("model dummy_fading.c: initializing class %s\n",
	     get_class_by_id(to->class)->name);
  return 0;
}


int destroy(call_t *to) {
  return 0;
}


/* ************************************************** */
/* ************************************************** */
int bootstrap(call_t *to, void *params) {
  return 0;
}


/* ************************************************** */
/* ************************************************** */
int bind(call_t *to, void *params) {
  DBG_FADING("model rayleigh.c: binding class %s with medium %s\n",
	     get_class_by_id(to->class)->name, get_medium_by_id(to->object)->name);
  return 0;
}

int unbind(call_t *to) {
  return 0;
}


/* ************************************************** */
/* ************************************************** */
double fading(call_t *to_fading, call_t *to_interface, call_t *from_interface, packet_t *packet, double rxdBm) {
  //    return 5.0 * log10(-2.0 * VARIANCE * log(get_random_double()));
  return 10*log10(-1.55 * VARIANCE * log(get_random_double()));
}


/* ************************************************** */
/* ************************************************** */
fading_methods_t methods = {fading};
