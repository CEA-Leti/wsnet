/**
 *  \file   tworayground.c
 *  \brief  Two-Ray ground propagation model
 *  \author Guillaume Chelius and Elyes Ben Hamida
 *  \date   2007
 **/

#include <kernel/include/modelutils.h>



/**
 *  TODO : include ht & hr (heights of transmit and receive antennas) in the antenna module
 *         
 **/

/* ************************************************** */
/* ************************************************** */
model_t model =  {
    "Two-Ray ground propagation model",
    "Elyes Ben Hamida",
    "0.1",
    MODELTYPE_PATHLOSS
};


/* ************************************************** */
/* ************************************************** */
struct classdata {
	double factor;
    double Pr0;
};


/* ************************************************** */
/* ************************************************** */
int init(call_t *to, void *params) {
    struct classdata *classdata = malloc(sizeof(struct classdata));
    param_t *param;
    double frequency = 868;

    /* get parameters */
    list_init_traverse(params);
    while ((param = (param_t *) list_traverse(params)) != NULL) {
        if (!strcmp(param->key, "frequency_MHz")) {
            if (get_param_double(param->value, &(frequency))) {
                goto error;
            }
        }
    }

    /* update factor */
    classdata->factor = 300 / (4 * M_PI * frequency);
    classdata->Pr0 = mW2dBm(classdata->factor * classdata->factor);

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
  DBG_PATHLOSS("model tworayground.c: binding class %s with medium %s\n",
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
    double h_src = get_node_position(from_interface->object)->z, h_dst = get_node_position(to_interface->object)->z;
    double crossover = h_src * h_dst / classdata->factor;

    /*
     * Cross over distance:
     *
     *        4 * pi * ht * hr
     *  dc = ------------------
     *             lambda
     *                               Pt * Gt * Gr * (ht * hr)^2
     *  Case 1: d >= dc  => Pr(d) = -----------------------------
     *                                        d^4 * L
     *
     *                               Pt * Gt * Gr * lambda^2
     *  Case 2: d < dc   => Pr(d) = -------------------------
     *                                 (4 * pi * d)^2 * L
     *
     *  Note: rxmW = Pt * Gt * Gr, and L = 1
     *  
     *  cf p89 ref "Wireless Communications: Principles and Practice", Theodore Rappaport, 1996.
     *
     */

    if ((crossover == 0) || (dist <= crossover)) {
        if (dist <= 1) {
            return  classdata->Pr0 + rxdBm;
        } else {
            return classdata->Pr0 + rxdBm - 10.0 * 2 * log10(dist);
        }    
    } else {
        return rxdBm + mW2dBm(h_src * h_dst * h_src * h_dst / pow(dist, 4.0));
    }
}


/* ************************************************** */
/* ************************************************** */
pathloss_methods_t methods = {pathloss};
