/**
 *  \file   classic oqpsk.c
 *  \brief  Classic OQPSK
 *  \author Guillaume Chelius
 *  \date   2007
 **/
#include <math.h>

#include <kernel/include/modelutils.h>


/* ************************************************** */
/* ************************************************** */
model_t model =  {
  "classic oqpsk",
  "Guillaume Chelius & Mickael Maman",
  "0.1",
  MODELTYPE_MODULATIONS
};


/* ************************************************** */
/* ************************************************** */
int init(call_t *c, void *params) {
  return 0;
}

int destroy(call_t *c) {
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
double modulate(call_t *to, call_t *from, double snr) {
  if (snr == MAX_SNR) {
    return 0;
  } else {
    return 0.5 * erfc(sqrt(2*snr)); 
	//BER not Symbol error rate (1 - (1 - bpsk_ber) * (1- bpsk_ber));
  }
}


int bit_per_symbol(call_t *to, call_t *from){
  return 2;
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
