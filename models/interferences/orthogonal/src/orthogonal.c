/**
 *  \file   orthogonal.c
 *  \brief  Orthogonal interference
 *  \author Guillaume Chelius
 *  \date   2007
 **/

#include <kernel/include/modelutils.h>


/* ************************************************** */
/* ************************************************** */
model_t model =  {
    "Orthogonal",
    "Guillaume Chelius",
    "0.1",
    MODELTYPE_INTERFERENCES
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
double interfere(call_t *to, call_t *from, int channel0, int channel1) {
    if (channel0 == channel1) {
        return 1;
    } else {
        return 0;
    }
}

/* ************************************************** */
/* ************************************************** */
interferences_methods_t methods = {interfere};
