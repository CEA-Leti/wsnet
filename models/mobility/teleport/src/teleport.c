/**
 *  \file   teleport.c
 *  \brief  Teleport mobility
 *  \author Guillaume Chelius and Elyes Ben Hamida
 *  \date   2007
 **/
#include <kernel/include/modelutils.h>


/* ************************************************** */
/* ************************************************** */
model_t model =  {
  "Teleport mobility",
  "Elyes Ben Hamida",
  "0.1",
  MODELTYPE_MOBILITY
};


/* ************************************************** */
/* ************************************************** */

struct nodedata {
  uint64_t pausetime;
  uint64_t lupdate;
  uint64_t nupdate;
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
  struct nodedata *nodedata = malloc(sizeof(struct nodedata));
  param_t *param;
  uint64_t min_pausetime, max_pausetime;
    
  /* default values */
  get_node_position(to->object)->x = get_random_x_position();
  get_node_position(to->object)->y = get_random_y_position();
  get_node_position(to->object)->z = get_random_z_position();

  nodedata->pausetime = 2000000000;
  min_pausetime       = 0;
  max_pausetime       = 0;

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
    if (!strcmp(param->key, "pausetime")) {
      if (get_param_time(param->value, &(nodedata->pausetime))) {
	goto error;
      }
    }
    if (!strcmp(param->key, "min-pausetime")) {
      if (get_param_time(param->value, &min_pausetime)) {
	goto error;
      }
    }
    if (!strcmp(param->key, "max-pausetime")) {
      if (get_param_time(param->value, &max_pausetime)) {
	goto error;
      }
    }
  }
    
  if (min_pausetime < max_pausetime ) {
    nodedata->pausetime = get_random_time_range(min_pausetime, max_pausetime);
  }
    
  set_node_private_data(to, nodedata);
  return 0;

 error:
  free(nodedata);
  return -1;
}

int unbind(call_t *to) {
  free(get_node_private_data(to));
  return 0;
}


/* ************************************************** */
/* ************************************************** */
int bootstrap(call_t *to) {
  struct nodedata *nodedata = get_node_private_data(to);

  PRINT_REPLAY("mobility %"PRId64" %d %lf %lf %lf\n", get_time(), to->object, 
	       get_node_position(to->object)->x, get_node_position(to->object)->y, 
	       get_node_position(to->object)->z);
    
  nodedata->lupdate = get_time();
  nodedata->nupdate = get_time() + nodedata->pausetime;
  return 0;
}

int ioctl(call_t *to, int option, void *in, void **out) {
  return 0;
}


/* ************************************************** */
/* ************************************************** */
void update_position(call_t *to, call_t *from) {
  struct nodedata *nodedata = get_node_private_data(to);
        
  while  (nodedata->lupdate < get_time()) {
      
    if (nodedata->nupdate <= get_time()) {
      nodedata->lupdate = nodedata->nupdate;
      nodedata->nupdate += nodedata->pausetime;

      get_node_position(to->object)->x = get_random_x_position();
      get_node_position(to->object)->y = get_random_y_position();
      get_node_position(to->object)->z = get_random_z_position();
      PRINT_REPLAY("mobility %"PRId64" %d %lf %lf %lf\n", nodedata->lupdate, to->object, get_node_position(to->object)->x, get_node_position(to->object)->y, get_node_position(to->object)->z);
    } else {
      nodedata->lupdate = get_time();
      break;
    }
  }

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

