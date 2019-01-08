/**
 *  \file   indoor.c
 *  \brief  ITU Indoor Propagation Model
 *  \author Doreid Ammar and Elyès Ben Hamida
 *  \date   2009
 **/

#include <kernel/include/modelutils.h>


/* ************************************************** */
/* ************************************************** */
model_t model =  {
    "ITU Indoor Propagation Model",
    "Doreid Ammar and Elyès Ben Hamida",
    "0.1",
    MODELTYPE_PATHLOSS
};


/* ************************************************** */
/* ************************************************** */
struct classdata {
    double frequency;		/* The carrier frequency in MHz           */
    double distpower_lc;	/* The distance power loss coefficient    */
    int n_floors;		/* The number of floors penetrated        */
    double Lf;			/* The floor penetration loss factor (dB) */
    double cst;
};


/* ************************************************** */
/* ************************************************** */
int init(call_t *to, void *params) {
    struct classdata *classdata = malloc(sizeof(struct classdata));
    param_t *param;
    
    /* default values */
    classdata->frequency    = 868;
    classdata->distpower_lc = 33;
    classdata->n_floors     = 2;
    classdata->Lf           = -1;
    classdata->cst          = 0;

    /* get parameters */
    list_init_traverse(params);
    while ((param = (param_t *) list_traverse(params)) != NULL) {
        if (!strcmp(param->key, "frequency_MHz")) {
            if (get_param_double(param->value, &(classdata->frequency))) {
                goto error;
            }
        }
        if (!strcmp(param->key, "distpower_lc")) {
            if (get_param_double(param->value, &(classdata->distpower_lc))) {
                goto error;
            }
        }
        if (!strcmp(param->key, "n_floors")) {
            if (get_param_integer(param->value, &(classdata->n_floors))) {
                goto error;
            }
        }
        if (!strcmp(param->key, "Lf")) {
            if (get_param_double(param->value, &(classdata->Lf))) {
                goto error;
            }
        }
    }

    if (classdata->Lf == -1) {
	classdata->Lf = 15 + 4 * (classdata->n_floors - 1);
    } 

    /* precomputed value for optimizing the simulation speedup */
    classdata->cst = 20 * log10(classdata->frequency) + classdata->Lf - 28;

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
int bootstrap(call_t *to, void *params) {
  return 0;
}


/* ************************************************** */
/* ************************************************** */
int bind(call_t *to, void *params) {
  DBG_PATHLOSS("model itu_indoor.c: binding class %s with medium %s\n",
	     get_class_by_id(to->class)->name, get_medium_by_id(to->object)->name);
  return 0;
}

int unbind(call_t *to) {
  return 0;
}


/* ************************************************** */
/* ************************************************** */
double pathloss(call_t *to_pathloss, call_t *to_interface ,call_t *from_interface, packet_t *packet, double rxdBm){
    struct classdata *classdata = get_class_private_data(to_pathloss);
    double dist = distance(get_node_position(from_interface->object), get_node_position(to_interface->object));
    double L_dB = 0;

    /*
     * Pr_dBm = Pt + Gt + Gr - L
     *
     * L_dB = 20 * log10(f) + N * log10(dist) + Lf(n) - 28
     * 
     * For the frequency 1.8-2.0 GHz
     * Lf(n) is the floor penetration loss factor (dB)
     * Lf(n) = 15 + 4(n-1); (n >= 1)
     *
     * Note: rxdBm = [Pt + Gt + Gr]_dBm
     *
     * ref1: recommendation ITU-R P.1238-1, propagation data and prediction methods for the planning of indoor radiocommunication systems, 1999.
     * ref2: http://en.wikipedia.org/wiki/ITU_Model_for_Indoor_Attenuation, 2009
     *
     */

     L_dB = classdata->cst + classdata->distpower_lc * log10(dist);

     return (rxdBm - L_dB);
}

/* ************************************************** */
/* ************************************************** */
pathloss_methods_t methods = {pathloss};

