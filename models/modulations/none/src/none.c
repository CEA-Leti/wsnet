/**
 *  \file   none.c
 *  \brief  No modulation model
 *  \author Guillaume Chelius
 *  \date   2007
 **/
#include <kernel/include/modelutils.h>


/* ************************************************** */
/* ************************************************** */
model_t model =  {
    "No modulation",
    "Guillaume Chelius",
    "0.1",
    MODELTYPE_MODULATIONS
};


/* ************************************************** */
/* ************************************************** */
int init(call_t *to, void *params) {
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
    DBG_MODULATIONS("model none.c: binding class %s with medium %s\n",
		  get_class_by_id(to->class)->name, get_medium_by_id(to->object)->name);
    return 0;
}

int unbind(call_t *to) {
    return 0;
}


/* ************************************************** */
/* ************************************************** */
double modulate(call_t *to, call_t *from, double snr) {
  return 0;
}

int bit_per_symbol(call_t *to, call_t *from){
    return 1;
}

int get_modulation_type(call_t *to, call_t *from)
{
    return 1;
}

void set_modulation_type(call_t *to, call_t *from, int modulation_type)
{
	return;
}
/* ************************************************** */
/* ************************************************** */
modulation_methods_t methods = {modulate, bit_per_symbol, get_modulation_type, set_modulation_type};
