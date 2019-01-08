/**
 *  \file   Intermodulation.c
 *  \brief  Intermodulation
 *  \author Amine DIDIOUI
 *  \date   2011
 **/

#include <kernel/include/modelutils.h>


/* ************************************************** */
/* ************************************************** */
model_t model =  {
  "Intermodulation",
  "Amine DIDIOUI",
  "0.1",
   MODELTYPE_INTERMODULATION
};


/* ************************************************** */
/* ************************************************** */
struct classdata {
  double factor;
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
int bind(call_t *to, void *params) {
  return 0;
}

int unbind(call_t *to) {
  return 0;
}


/* ************************************************** */
/* ************************************************** */
double intermod(call_t *to, call_t *from, int channel0, int channel1, int channel2) {
	return 0;
}


/* ************************************************** */
/* ************************************************** */
intermodulation_methods_t methods = {intermod};
