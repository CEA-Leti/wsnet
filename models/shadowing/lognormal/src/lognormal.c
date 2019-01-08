/**
 *  \file   lognormal.c
 *  \brief  Log normal shadowing model
 *  \author Loic Lemaitre
 *  \date   2010
 **/

#include <kernel/include/modelutils.h>


/* ************************************************** */
/* ************************************************** */
model_t model = 
  {
    "Log normal shadowing model",
    "Loic Lemaitre",
    "0.1",
    MODELTYPE_SHADOWING
  };

/* ************************************************** */
/* ************************************************** */
struct classdata
{
  double deviation;   /* Shadowing deviation (dB) */
};


/* ************************************************** */
/* ************************************************** */
int init(call_t *to, void *params)
{
  struct classdata *classdata = malloc(sizeof(struct classdata));
  param_t *param;

  /* default values */
  classdata->deviation  = 4.0;

  /* get parameters */
  list_init_traverse(params);
  while ((param = (param_t *) list_traverse(params)) != NULL)
    {
      if (!strcmp(param->key, "deviation"))
	{
	  if (get_param_double(param->value, &(classdata->deviation)))
	    {
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

int destroy(call_t *to)
{
  free(get_class_private_data(to));
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
  struct classdata *classdata = get_class_private_data(to_shadowing);
  
  return get_gaussian(0.0, classdata->deviation);
}


/* ************************************************** */
/* ************************************************** */
shadowing_methods_t methods = {shadowing};
