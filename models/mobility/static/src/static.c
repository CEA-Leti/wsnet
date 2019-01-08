/**
 *  \file   static.c
 *  \brief  no mobility
 *  \author Guillaume Chelius
 *  \date   2007
 **/
#include <kernel/include/modelutils.h>


/* ************************************************** */
/* ************************************************** */
model_t model =  {
    "No mobility",
    "Guillaume Chelius",
    "0.1",
    MODELTYPE_MOBILITY, 
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
    param_t *param;

    /* default values */
    get_node_position(to->object)->x = get_random_x_position();
    get_node_position(to->object)->y = get_random_y_position();
    get_node_position(to->object)->z = get_random_z_position();
    
   /* get parameters */
    list_init_traverse(params);
    while ((param = (param_t *) list_traverse(params)) != NULL) {
        if (!strcmp(param->key, "x")) {
            if (get_param_x_position(param->value, &(get_node_position(to->object)->x))) {
                goto error;
            }
        }
        if (!strcmp(param->key, "y")) {
            if (get_param_y_position(param->value, &(get_node_position(to->object)->y))) {
                goto error;
            }
        }
        if (!strcmp(param->key, "z")) {
            if (get_param_z_position(param->value, &(get_node_position(to->object)->z))) {
                goto error;
            }
        }
    }

    return 0;
    
 error:
    return -1;
}

int unbind(call_t *to) {
    return 0;
}


/* ************************************************** */
/* ************************************************** */
int bootstrap(call_t *to) {
    PRINT_REPLAY("mobility %"PRId64" %d %lf %lf %lf\n", get_time(), to->object, 
                 get_node_position(to->object)->x, get_node_position(to->object)->y, 
                 get_node_position(to->object)->z);
    return 0;
}

int ioctl(call_t *to, int option, void *in, void **out) {
    return 0;
}


/* ************************************************** */
/* ************************************************** */
void update_position(call_t *to, call_t *from) {
    return;
}

double get_speed(call_t *to) {

return 0.0;
}


angle_t get_angle(call_t *to) {

angle_t angle;
angle.xy=0.0;
angle.z=0.0;

return angle;

}

/* ************************************************** */
/* ************************************************** */
mobility_methods_t methods = {update_position,
							  get_speed,
							  get_angle};

