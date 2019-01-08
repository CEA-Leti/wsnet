/**
 *  \file   dummy_mobility.c
 *  \brief  Dummy mobility model
 *  \author Loic Lemaitre
 *  \date   2010
 **/

#include <kernel/include/modelutils.h>

//#define ENABLE_DUMMY_MOBILITY_TRACER

/* ************************************************** */
/* ************************************************** */
model_t model =  {
    "Dummy mobility model",
    "Loic Lemaitre",
    "0.1",
    MODELTYPE_MOBILITY
};


/* ************************************************** */
/* ************************************************** */
struct classdata {
};

struct nodedata {
};


/* ************************************************** */
/* ************************************************** */
int init(call_t *to, void *params) {
  DBG_MOBILITY("model dummy_mobility.c: initializing class %s\n",
	     get_class_by_id(to->class)->name);

  return 0;
}

int destroy(call_t *to) {
  return 0;
}

int bootstrap(call_t *to, void *params) {

  return 0;
}


/* ************************************************** */
/* ************************************************** */
int bind(call_t *to, void *params) {
  param_t* param;
  DBG_MOBILITY("model dummy_mobility.c: binding class %s with node %d\n",
	     get_class_by_id(to->class)->name, get_node_by_id(to->object)->id);

  get_node_position(to->object)->x = get_random_x_position();
    get_node_position(to->object)->y = get_random_y_position();
    get_node_position(to->object)->z = get_random_z_position();

  /* get parameters if set*/
  list_init_traverse(params);
  while ((param = (param_t *) list_traverse(params)) != NULL) {
    if (!strcmp(param->key, "x")) {
      if (get_param_x_position(param->value, &(get_node_position(to->object)->x))) {
	return -1;
      }
    }
    if (!strcmp(param->key, "y")) {
      if (get_param_y_position(param->value, &(get_node_position(to->object)->y))) {
	return -1;
      }
    }
    if (!strcmp(param->key, "z")) {
      if (get_param_z_position(param->value, &(get_node_position(to->object)->z))) {
	return -1;
      }
    }
  }

  return 0;
}

int unbind(call_t *to) {

  return 0;
}


/* ************************************************** */
/* ************************************************** */
void update_position(call_t *to, call_t *from) {

  DBG_MOBILITY("model dummy_mobility.c: updating node %d position\n", to->object);

  /* move */
  get_node_by_id(to->object)->position.x = get_node_by_id(to->object)->position.x + 1;
  get_node_by_id(to->object)->position.y = get_node_by_id(to->object)->position.y + 1;
  get_node_by_id(to->object)->position.z = get_node_by_id(to->object)->position.z + 1;

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

