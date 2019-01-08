/**
 *  \file   bpsk.c
 *  \brief  BPSK
 *  \author Guillaume Chelius
 *  \date   2007
 **/
#include <math.h>

#include <kernel/include/modelutils.h>


/* ************************************************** */
/* ************************************************** */
model_t model =  {
    "bpsk",
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
  DBG_MODULATIONS("model bpsk.c: binding class %s with medium %s\n",
	     get_class_by_id(to->class)->name, get_medium_by_id(to->object)->name);
  return 0;
}

int unbind(call_t *to) {
  return 0;
}


/* ************************************************** */
/* ************************************************** */
double modulate(call_t *to, call_t *from, double snr) {
    if (snr == MAX_SNR) {
      return 0;
    } else {
      return 0.5 * erfc(sqrt(snr));
    }
}

int bit_per_symbol(call_t *to, call_t *from){
    return 1;
}

int  get_modulation_type(call_t *to, call_t *from)
{
    return 1;
}

void set_modulation_type(call_t *to, call_t *from, int modulation_type)
{
	return;
}

/* ************************************************** */
/* ************************************************** */
modulation_methods_t methods = {modulate, bit_per_symbol, get_modulation_type,set_modulation_type};
