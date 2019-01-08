/**
 *  \file   nakagami_m.c
 *  \brief  Nakagami_m fading model
 *  \author Doreen and Ruifeng
 *  \date   2008
 **/

#include <kernel/include/modelutils.h>

#define VARIANCE 0.6366197723676


/* ************************************************** */
/* ************************************************** */
model_t model =  {
    "Nakagami Fading model",
    "Doreen and Ruifeng",
    "0.1",
    MODELTYPE_FADING
};

/* ************************************************** */
/* ************************************************** */
struct classdata {
    double m;           //strength of fading
};


/* ************************************************** */
/* ************************************************** */
int init(call_t *to, void *params) {
    struct classdata *classdata = malloc(sizeof(struct classdata));
    param_t *param;

    /* default value */
    classdata->m           = 1.0;

    /* get parameters */
    list_init_traverse(params);
    while ((param = (param_t *) list_traverse(params)) != NULL) {
	if (!strcmp(param->key, "m")) {
            if (get_param_double(param->value, &(classdata->m))) {
                goto error;
            }
        }
    }

    set_class_private_data(to, classdata);
    return 0;

 error:
    free(classdata);
    return -1;
	printf("Parameters for nakagmi-m model are error.\n");
}

int destroy(call_t *to) {
    free(get_class_private_data(to));
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
  DBG_FADING("model dummy_fading.c: binding class %s with medium %s\n",
	     get_class_by_id(to->class)->name, get_medium_by_id(to->object)->name);
  return 0;
}

int unbind(call_t *to) {
  return 0;
}


/* ************************************************** */
/* ************************************************** */
double fading(call_t *to_fading, call_t *to_interface, call_t *from_interface, packet_t *packet, double rxdBm) {
  struct classdata *classdata = get_class_private_data(to_fading);
	double sum = 0;
	int i;    	

	for(i=0; i < classdata->m; i++){
		sum = sum - log(get_random_double());
	}
	return 10 * log10(sum/classdata->m);
}


/* ************************************************** */
/* ************************************************** */
fading_methods_t methods = {fading};
