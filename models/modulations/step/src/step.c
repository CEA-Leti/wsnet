/**
 *  \file   step.c
 *  \brief  Step modulation model
 *  \author Guillaume Chelius
 *  \date   2007
 **/
#include <kernel/include/modelutils.h>


/* ************************************************** */
/* ************************************************** */
model_t model =  {
  "Step modulation",
  "Guillaume Chelius",
  "0.1",
  MODELTYPE_MODULATIONS
};


/* ************************************************** */
/* ************************************************** */
struct classdata {
  double step;
};


/* ************************************************** */
/* ************************************************** */
int init(call_t *to, void *params) {
  struct classdata *classdata = malloc(sizeof(struct classdata));
  param_t *param;
    
  /* default values */
  classdata->step = 2;

  /* get parameters */
  list_init_traverse(params);
  while ((param = (param_t *) list_traverse(params)) != NULL) {
    if (!strcmp(param->key, "step")) {
      if (get_param_double(param->value, &(classdata->step))) {
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
double modulate(call_t *to, call_t *from, double snr) {
  struct classdata *classdata = get_class_private_data(to);
  if (snr >= classdata->step) {
    return 0;
  } else {
    return 0.5;
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
