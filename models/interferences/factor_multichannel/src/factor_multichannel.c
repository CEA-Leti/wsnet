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
/*struct classdata {
  double factor1;
  double factor2;
  double factor3;
};
*/

/* ************************************************** */
/* ************************************************** */
int init(call_t *to, void *params) {
//  struct classdata *classdata = malloc(sizeof(struct classdata));
//  param_t *param;

  /* default value */
//  classdata->factor1 = 0.5;
//  classdata->factor2 = 0.5;
//  classdata->factor3 = 0.5;

  /* get parameters */
/*  list_init_traverse(params);
  while ((param = (param_t *) list_traverse(params)) != NULL) {
    if (!strcmp(param->key, "factor1")) {
      if (get_param_double(param->value, &(classdata->factor1))) {
	goto error;
      }
    }
  }

  while ((param = (param_t *) list_traverse(params)) != NULL) {
    if (!strcmp(param->key, "factor2")) {
      if (get_param_double(param->value, &(classdata->factor2))) {
	goto error;
      }
    }
  }

  while ((param = (param_t *) list_traverse(params)) != NULL) {
    if (!strcmp(param->key, "factor3")) {
      if (get_param_double(param->value, &(classdata->factor3))) {
	goto error;
      }
    }
  }

  set_class_private_data(to, classdata);
  return 0;

 error:
  free(classdata);
  return -1;
*/
	return 0;
}

int destroy(call_t *to) {
//  free(get_class_private_data(to));
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
//  struct classdata *classdata = get_class_private_data(to);
  double adj,altr0,altr1;
  double tmp, tmpfact = 0;

  adj = dBm2mW(-49);
  altr0 = dBm2mW(-54);
  altr1 = dBm2mW(-55);

  tmp = abs(channel0-channel1);

  if(tmp == 0)
	  tmpfact = 1;
  else if(tmp == 1)
	  tmpfact = adj;
  else if(tmp == 2)
	  tmpfact = altr0;
  else if(tmp >= 3)
	  tmpfact = altr1;

  return tmpfact;
}


/* ************************************************** */
/* ************************************************** */
interferences_methods_t methods = {interfere};
