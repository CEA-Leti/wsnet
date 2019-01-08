/**
 *  \file   factor.c
 *  \brief  Correlation factor
 *  \author Guillaume Chelius
 *  \date   2007
 **/

#include <kernel/include/modelutils.h>


/* ************************************************** */
/* ************************************************** */
model_t model =  {
  "Correlation factor",
  "Guillaume Chelius",
  "0.1",
  MODELTYPE_INTERFERENCES
};


/* ************************************************** */
/* ************************************************** */
struct classdata {
  double factor;
};


/* ************************************************** */
/* ************************************************** */
int init(call_t *to, void *params) {
  struct classdata *classdata = malloc(sizeof(struct classdata));
  param_t *param;
    
  /* default value */
  classdata->factor = 0.5;

  /* get parameters */
  list_init_traverse(params);
  while ((param = (param_t *) list_traverse(params)) != NULL) {
    if (!strcmp(param->key, "factor")) {
      if (get_param_double(param->value, &(classdata->factor))) {
	goto error;
      }
    }
  }
    
  set_class_private_data(to, classdata);
  return 0;

 error:
  free(classdata);
  return -1;
}

int destroy(call_t *to) {
  free(get_class_private_data(to));
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
  struct classdata *classdata = get_class_private_data(to);
  if (channel0 == channel1) {
    return 1;
  } else {
    return classdata->factor;
  }
}


/* ************************************************** */
/* ************************************************** */
interferences_methods_t methods = {interfere};
