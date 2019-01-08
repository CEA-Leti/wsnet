/**
 *  \file   omnidirectional.c
 *  \brief  Omni-directionnal antenna
 *  \author Guillaume Chelius
 *  \date   2007
 **/

#include <kernel/include/modelutils.h>


/* ************************************************** */
/* ************************************************** */
model_t model =  {
  "Omnidirectionnal antenna",
  "Guillaume Chelius",
  "0.1",
  MODELTYPE_INTERFACE,
};


/* ************************************************** */
/* ************************************************** */
struct classdata {
};

struct nodedata {
  double noise;
  angle_t angle;
  double gain_tx;
  double gain_rx;
  medium_t *medium;
  interface_type_t type;
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
  int medium_param_found = 0;

  DBG_INTERFACE("model antenna_omnidirectionnal.c: binding class %d to node %d\n", to->class, to->object);

  /* default values */
  nodedata->noise = 0.0;
  nodedata->gain_tx = 0.0;
  nodedata->gain_rx = 0.0;
  nodedata->type = INTERFACE_TYPE_ANTENNA_OMNIDIRECTIONAL;
  nodedata->angle.xy = get_random_double_range(0, 2 * M_PI);
  nodedata->angle.z = get_random_double_range(0, 2 * M_PI);
    
  /* get parameters */
  list_init_traverse(params);
  while ((param = (param_t *) list_traverse(params)) != NULL) {
    if (!strcmp(param->key, "gain-tx")) {
      if (get_param_double(param->value, &(nodedata->gain_tx))) {
	goto error;
      }
    }
    if (!strcmp(param->key, "gain-rx")) {
      if (get_param_double(param->value, &(nodedata->gain_rx))) {
	goto error;
      }
    }
    if (!strcmp(param->key, "loss")) {
      if (get_param_double(param->value, &(nodedata->noise))) {
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
    if (!strcmp(param->key, "medium")) {
      medium_t *medium;
      if ((medium = get_medium_by_name(param->value)) == NULL) {
	fprintf(stderr, "antenna_omnidirectionnal:error: medium not found\n");
	goto error;
      }
      nodedata->medium = medium;
      medium_param_found = 1;
      DBG_INTERFACE("model antenna_omnidirectionnal.c: medium %s parameter saved\n", medium->name);
    }
  }
  
  if (!medium_param_found) {
    fprintf(stderr, "antenna_omnidirectionnal:error: medium parameter not found\n");
    goto error;
    }
  

  //fprintf(stderr, "gain-tx = %f and gain-rx = %f \n", nodedata->gain_tx, nodedata->gain_rx);

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
  return 0;
}

int ioctl(call_t *to, int option, void *in, void **out) {
  return 0;
}


/* ************************************************** */
/* ************************************************** */
double get_loss(call_t *to, call_t *from) {
  struct nodedata *nodedata = get_node_private_data(to);
  return nodedata->noise;
}

angle_t *get_angle(call_t *to, call_t *from) {
  struct nodedata *nodedata = get_node_private_data(to);
  return &(nodedata->angle);
}

void set_angle(call_t *to, call_t *from, angle_t *angle) {
  struct nodedata *nodedata = get_node_private_data(to);
  nodedata->angle.xy = angle->xy;
  nodedata->angle.z = angle->z;
  return;
}


/* ************************************************** */
/* ************************************************** */
void cs(call_t *to, call_t *from, packet_t *packet) {
  array_t *up = get_class_bindings_up(to);
  int i = up->size;

  while (i--) {
    call_t to_up = {up->elts[i], to->object,};
    transceiver_cs(&to_up, to, packet);
  }
    
  return;
}

void rx(call_t *to, call_t *from, packet_t *packet) {
  array_t *up = get_class_bindings_up(to);
  int i = up->size;

  while (i--) {
    call_t to_up = {up->elts[i], to->object};
    packet_t *packet_up;
        
    if (i > 0) {
      packet_up = packet_clone(packet);           
    } else {
      packet_up = packet;
    }
    RX(&to_up, to, packet_up);
  }
    
  return;
}

void tx(call_t *to, call_t *from, packet_t *packet) {
  medium_t  *medium = get_medium_by_id(interface_get_medium(to, from));

  // if we are still using WSNETv3
  if (medium->spectrum == CLASS_NON_EXISTENT_CLASS_ID){
    MEDIA_TX(from, to, packet);
    return;
  }

  array_t *down = get_class_bindings_down(to);
  int i = down->size;

  while (i--) {
    call_t to_up = {down->elts[i], to->object};
    packet_t *packet_down;

    if (i > 0) {
      packet_down = packet_clone(packet);
    } else {
      packet_down = packet;
    }
    TX(&to_up, to, packet_down);
  }

  return;
}

/* ************************************************** */
/* ************************************************** */
double  gain_tx(call_t *to, call_t *from, position_t *pos) {    
  struct nodedata *nodedata = get_node_private_data(to);
  return nodedata->gain_tx;
}

double  gain_rx(call_t *to, call_t *from, position_t *pos) {    
  struct nodedata *nodedata = get_node_private_data(to);
  return nodedata->gain_rx;
}


/* ************************************************** */
/* ************************************************** */
mediumid_t get_medium(call_t *to, call_t *from) {
  struct nodedata *nodedata = (struct nodedata *) get_node_private_data(to);
  return nodedata->medium->id;
}

interface_type_t get_type(call_t *to){
	struct nodedata *nodedata = (struct nodedata *) get_node_private_data(to);
	return nodedata->type;
}

/* ************************************************** */
/* ************************************************** */
interface_methods_t methods = {rx, 
             tx,
			       cs,
			       get_loss, 
			       get_angle, 
			       set_angle, 
			       gain_tx, 
			       gain_rx,
			       get_medium,
			       get_type};
