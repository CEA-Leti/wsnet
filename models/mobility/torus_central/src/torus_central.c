/**
 *  \file   torus_central.c
 *  \brief  Torus with central symetry mobility
 *  \author Guillaume Chelius
 *  \date   2007
 **/
#include <kernel/include/modelutils.h>


/* ************************************************** */
/* ************************************************** */
model_t model =  {
  "Torus central mobility",
  "Guillaume Chelius",
  "0.1",
  MODELTYPE_MOBILITY
};


/* ************************************************** */
/* ************************************************** */
struct classdata {
  double max_speed;
};

struct nodedata {
  uint64_t lupdate;
  double speed;
  angle_t angle;
};

/* ************************************************** */
/* ************************************************** */
int init(call_t *to, void *params) {
  struct classdata *classdata = malloc(sizeof(struct classdata));
  param_t *param;

  /* default values */
  classdata->max_speed = 30;

  /* get parameters */
  list_init_traverse(params);
  while ((param = (param_t *) list_traverse(params)) != NULL) {
    if (!strcmp(param->key, "max-speed")) {
      if (get_param_double(param->value, &(classdata->max_speed)))
	goto error;
    }
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
  struct nodedata *nodedata = malloc(sizeof(struct nodedata));
  struct classdata *classdata = get_class_private_data(to);
  param_t *param;
    
  /* default values */
  get_node_position(to->object)->x = get_random_x_position();
  get_node_position(to->object)->y = get_random_y_position();
  get_node_position(to->object)->z = get_random_z_position();
  nodedata->speed = get_random_double_range(0, classdata->max_speed);
  nodedata->angle.xy = get_random_double_range(0, 2 * M_PI);
  nodedata->angle.z = get_random_double_range(0, 2 * M_PI);

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
    if (!strcmp(param->key, "speed")) {
      if (get_param_double_range(param->value, &(nodedata->speed), 0, classdata->max_speed)) {
	goto error;
      }
    }
    if (!strcmp(param->key, "angle-xy")) {
      if (get_param_double_range(param->value, &(nodedata->angle.xy), 0, 2*M_PI)) {
	goto error;
      }
    }
    if (!strcmp(param->key, "angle-z")) {
      if (get_param_double_range(param->value, &(nodedata->angle.z), 0, 2*M_PI)) {
	goto error;
      }
    }
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

  return 0;
}

int ioctl(call_t *to, int option, void *in, void **out) {
  return 0;
}


/* ************************************************** */
/* ************************************************** */
void update_position(call_t *to, call_t *from) {
  struct nodedata *nodedata = get_node_private_data(to);
  position_t *pos = get_node_position(to->object);
  position_t *area = get_topology_area();
        
  while  (nodedata->lupdate < get_time()) {
    uint64_t t_0, t_1 = 0, t_2 = 0, t_3 = 0, t_4 = 0, t_5 = 0, t_6 = 0;
    int bounce = 0;
        
    if (cos(nodedata->angle.xy)*cos(nodedata->angle.z) != 0)
      t_1 = ((0 - pos->x) / (nodedata->speed * cos(nodedata->angle.xy)*cos(nodedata->angle.z))) * 1000000000;
    if (cos(nodedata->angle.xy)*cos(nodedata->angle.z) != 0)
      t_2 = ((area->x - pos->x) / (nodedata->speed * cos(nodedata->angle.xy)*cos(nodedata->angle.z))) * 1000000000;
    if (sin(nodedata->angle.xy)*cos(nodedata->angle.z) != 0)
      t_3 = ((0 - pos->y) / (nodedata->speed * sin(nodedata->angle.xy)*cos(nodedata->angle.z))) * 1000000000;
    if (sin(nodedata->angle.xy)*cos(nodedata->angle.z) != 0)
      t_4 = ((area->y - pos->y) / (nodedata->speed * sin(nodedata->angle.xy)*cos(nodedata->angle.z))) * 1000000000;
    if (sin(nodedata->angle.z) != 0)
      t_5 = ((0 - pos->z) / (nodedata->speed * sin(nodedata->angle.z))) * 1000000000;
    if (sin(nodedata->angle.z) != 0)
      t_6 = ((area->z - pos->z) / (nodedata->speed * sin(nodedata->angle.z))) * 1000000000;

    t_0 = get_time() - nodedata->lupdate;
    if ((t_1 > 0) && (t_1 < t_0)) {
      t_0 = t_1;
      bounce = 1;
    }
    if ((t_2 > 0) && (t_2 < t_0)) {
      t_0 = t_2;
      bounce = 2;
    }
    if ((t_3 > 0) && (t_3 < t_0)) {
      t_0 = t_3;
      bounce = 3;
    }
    if ((t_4 > 0) && (t_4 < t_0)) {
      t_0 = t_4;
      bounce = 4;
    }
    if ((t_5 > 0) && (t_5 < t_0)) {
      t_0 = t_5;
      bounce = 5;
    }
    if ((t_6 > 0) && (t_6 < t_0)) {
      t_0 = t_6;
      bounce = 6;
    }

    pos->x = (nodedata->speed * cos(nodedata->angle.xy)*cos(nodedata->angle.z)*t_0 / 1000000000 + pos->x);
    pos->y = (nodedata->speed * sin(nodedata->angle.xy)*cos(nodedata->angle.z)*t_0 / 1000000000 + pos->y);
    pos->z = (nodedata->speed * sin(nodedata->angle.z)*t_0 / 1000000000 + pos->z);

    if (pos->x > area->x)
      pos->x = area->x;
    if (pos->y > area->y)
      pos->y = area->y;
    if (pos->z > area->z)
      pos->z = area->z;
    if (pos->x < 0)
      pos->x = 0;
    if (pos->y < 0)
      pos->y = 0;
    if (pos->z < 0)
      pos->z = 0;
    
    /* if t_0 is g_time - last_update we have to leave  the loop.
     * we have to check this because of the imprecision on the double addition */
         
    nodedata->lupdate += t_0;
    if (nodedata->lupdate > get_time()) {
      nodedata->lupdate = get_time();
    }
    PRINT_REPLAY("mobility %"PRId64" %d %lf %lf %lf\n", nodedata->lupdate, to->object, get_node_position(to->object)->x, get_node_position(to->object)->y, get_node_position(to->object)->z);
        
    if (bounce) {
      if (pos->x > (area->x / 2))
	pos->x = (area->x / 2) - fabs(pos->x - (area->x / 2));
      else 
	pos->x = (area->x / 2) + fabs(pos->x - (area->x / 2));
      if (pos->y > (area->y / 2))
	pos->y = (area->y / 2) - fabs(pos->y - (area->y / 2));
      else 
	pos->y = (area->y / 2) + fabs(pos->y - (area->y / 2));
      if (pos->z > (area->z / 2))
	pos->z = (area->z / 2) - fabs(pos->z - (area->z / 2));
      else 
	pos->z = (area->z / 2) + fabs(pos->z - (area->z / 2));
            
      PRINT_REPLAY("mobility %"PRId64" %d %lf %lf %lf\n", nodedata->lupdate, to->object, get_node_position(to->object)->x, get_node_position(to->object)->y, get_node_position(to->object)->z);
    }
  }
    
  return;
}

double get_speed(call_t *to) {

struct nodedata *nodedata = get_node_private_data(to);

return nodedata->speed;
}


angle_t get_angle(call_t *to) {

struct nodedata *nodedata = get_node_private_data(to);

return nodedata->angle;

}

/* ************************************************** */
/* ************************************************** */
mobility_methods_t methods = {update_position,
							  get_speed,
							  get_angle};

