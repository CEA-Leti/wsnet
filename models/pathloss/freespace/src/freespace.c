/**
 *  \file   logdistance_pathloss.c
 *  \brief  Log-Distance Pathloss propagation model
 *  \author Guillaume Chelius and Elyes Ben Hamida
 *  \date   2007
 **/

#include <kernel/include/modelutils.h>

/* ************************************************** */
/* ************************************************** */
model_t model =  {
    "Log-Distance Pathloss propagation model",
    "Elyes Ben Hamida",
    "0.1",
    MODELTYPE_PATHLOSS
};


/* ************************************************** */
/* ************************************************** */
struct classdata {
    double pathloss;
    double Pr0;
    double frequency;
};


/* ************************************************** */
/* ************************************************** */
int init(call_t *to, void *params) {
    struct classdata *classdata = malloc(sizeof(struct classdata));
    param_t *param;
    classdata->frequency = 868;

    /* default values */
    classdata->pathloss   = 2.0;
    classdata->Pr0= 0;
    /* get parameters */
    list_init_traverse(params);
    while ((param = (param_t *) list_traverse(params)) != NULL) {
        if (!strcmp(param->key, "frequency_MHz")) {
            if (get_param_double(param->value, &(classdata->frequency))) {
                goto error;
            }
        }
        if (!strcmp(param->key, "pathloss")) {
            if (get_param_double(param->value, &(classdata->pathloss))) {
                goto error;
            }
        }
        if (!strcmp(param->key, "Pr0")) {
            if (get_param_double(param->value, &(classdata->Pr0))) {
                goto error;
            }
        }

    }
    
    /*
     * Compute attenuation at reference distance d=1 with reference power Pt=1 :
     *
     *           Pt * Gt * Gr * lambda^2
     *  Pr(d) = -------------------------
     *             (4 * pi * d)^2 * L
     *
     *  Note: rxmW = Pt * Gt * Gr, and L = 1
     *
     *  cf p71 ref "Wireless Communications: Principles and Practice", Theodore Rappaport, 1996.
     *
     */
    if (classdata->Pr0==0)
    {
    classdata->Pr0 = mW2dBm((300 / (4 * M_PI * classdata->frequency)) * (300 / (4 * M_PI * classdata->frequency)));
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
int bootstrap(call_t *to, void *params) {
  return 0;
}


/* ************************************************** */
/* ************************************************** */
int bind(call_t *to, void *params) {
  DBG_PATHLOSS("model freespace.c: binding class %s with medium %s\n",
	     get_class_by_id(to->class)->name, get_medium_by_id(to->object)->name);
  return 0;
}

int unbind(call_t *to) {
  return 0;
}


/* ************************************************** */
/* ************************************************** */
double pathloss(call_t *to_pathloss, call_t *to_interface ,call_t *from_interface, packet_t *packet, double rxdBm) {
    struct classdata *classdata = get_class_private_data(to_pathloss);
    double dist = distance(get_node_position(from_interface->object), get_node_position(to_interface->object));

    /*
     *  Pr_dBm(d) = Pr_dBm(d0) - 10 * beta * log10(d/d0)
     *
     *  Note: rxdBm = [Pt + Gt + Gr]_dBm, and L = 1
     *
     *  cf p102-104 ref "Wireless Communications: Principles and Practice", Theodore Rappaport, 1996.
     *
     */
    if (dist <= 1) {
        return classdata->Pr0 + rxdBm;
    } else {
        return classdata->Pr0 + rxdBm - 10.0 * classdata->pathloss * log10(dist);
    }
}


/* ************************************************** */
/* ************************************************** */
pathloss_methods_t methods = {pathloss};
