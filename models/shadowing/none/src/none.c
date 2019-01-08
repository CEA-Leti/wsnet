/**
 *  \file   none.c
 *  \brief  None shadowing model
 *  \author Loic Lemaitre
 *  \date   2010
 **/

#include <kernel/include/modelutils.h>


/* ************************************************** */
/* ************************************************** */
model_t model =  {
    "None shadowing model",
    "Loic Lemaitre",
    "0.1",
    MODELTYPE_SHADOWING
};


/* ************************************************** */
/* ************************************************** */
struct classdata {
};


/* ************************************************** */
/* ************************************************** */
int init(call_t *to, void *params) {
  DBG_SHADOWING("model none.c: initializing class %s\n",
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
  DBG_SHADOWING("model none.c: binding class %s with medium %s\n",
	     get_class_by_id(to->class)->name, get_medium_by_id(to->object)->name);
  return 0;
}

int unbind(call_t *to) {
  return 0;
}


/* ************************************************** */
/* ************************************************** */
double shadowing(call_t *to_shadowing, call_t *to_interface, call_t *from_interface, packet_t *packet, double rxdBm)
{
  return 0;
}


/* ************************************************** */
/* ************************************************** */
shadowing_methods_t methods = {shadowing};
