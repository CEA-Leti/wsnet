/**
 *  \file   white.c
 *  \brief  White noise model
 *  \author Luiz Henrique Suraty Filho
 *  \date   2018
 **/
#include <kernel/include/modelutils.h>


/* ************************************************** */
/* ************************************************** */
model_t model =  {
    "White noise",
    "Luiz Henrique Suraty Filho",
    "0.1",
    MODELTYPE_NOISE
};


/* ************************************************** */
/* ************************************************** */
typedef struct classdata {
	double white_mW;
	double white_dBm;
} white_noise_classdata_t;


/* ************************************************** */
/* ************************************************** */
int init(call_t *to, void *params) {
    white_noise_classdata_t *classdata = malloc(sizeof(white_noise_classdata_t));
    param_t *param;

    classdata->white_dBm = -174.0;

    list_init_traverse(params);
    while ((param = (param_t *) list_traverse(params)) != NULL) {
        if (!strcmp(param->key, "white-noise-dBm")) {
            if (get_param_double(param->value, &( classdata->white_dBm))) {
                goto error;
            }
        }
    }

    if (classdata->white_dBm != MIN_DBM) {
      classdata->white_mW = dBm2mW( classdata->white_dBm);
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
double noise(call_t *to, call_t *from, nodeid_t node, int channel) {
    white_noise_classdata_t *classdata = (white_noise_classdata_t *) get_class_private_data(to);;
    return classdata->white_dBm;
}


/* ************************************************** */
/* ************************************************** */
noise_methods_t methods = {noise};
