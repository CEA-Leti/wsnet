/**
 *  \file   cbr.c
 *  \brief  CBR application
 *  \author Guillaume Chelius and Elyes Ben Hamida
 *  \date   2007
 **/
#include <stdio.h>

#include <kernel/include/modelutils.h>


/* ************************************************** */
/* ************************************************** */
model_t model =  {
  "CBR application",
  "Guillaume Chelius and Elyes Ben Hamida",
  "0.1",
  MODELTYPE_APPLICATION
};


/* ************************************************** */
/* ************************************************** */
struct _cbr_private {
  uint64_t start;
  uint64_t period;
  int size;
  nodeid_t destination;
  position_t position;
};


// Data Packet //
struct _cbr_packet_header {
    int data;
};


/* ************************************************** */
/* ************************************************** */
int callmeback(call_t *to, call_t *from, void *args);
void tx(call_t *to, call_t *from);


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
  struct _cbr_private *nodedata = malloc(sizeof(struct _cbr_private));
  param_t *param;
        
  /* default values */
  nodedata->destination = get_random_node(to->object);
  nodedata->position.x = get_random_x_position();
  nodedata->position.y = get_random_y_position();
  nodedata->position.z = get_random_z_position();
  nodedata->start = 0;
  nodedata->period = 1000000000;
  nodedata->size = 1000;

  /* get parameters */
  list_init_traverse(params);
  while ((param = (param_t *) list_traverse(params)) != NULL) {

    if (!strcmp(param->key, "destination")) {
      if (get_param_nodeid(param->value, &(nodedata->destination), to->object)) {
	goto error;
      }
    }
    if (!strcmp(param->key, "destination-x")) {
      if (get_param_x_position(param->value, &(nodedata->position.x))) {
	goto error;
      }
    }
    if (!strcmp(param->key, "destination-y")) {
      if (get_param_y_position(param->value, &(nodedata->position.y))) {
	goto error;
      }
    }
    if (!strcmp(param->key, "destination-z")) {
      if (get_param_z_position(param->value, &(nodedata->position.z))) {
	goto error;
      }
    }
    if (!strcmp(param->key, "start")) {
      if (get_param_time(param->value, &(nodedata->start))) {
	goto error;
      }
    }
    if (!strcmp(param->key, "period")) {
      if (get_param_time(param->value, &(nodedata->period))) {
	goto error;
      }
    }
    if (!strcmp(param->key, "size")) {
      if (get_param_integer(param->value, &(nodedata->size))) {
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
  struct _cbr_private *nodedata = get_node_private_data(to);

  free(nodedata);
    
  return 0;
}


/* ************************************************** */
/* ************************************************** */
int bootstrap(call_t *to) {
  struct _cbr_private *nodedata = get_node_private_data(to);
  uint64_t start = get_time() + nodedata->start + get_random_double() * nodedata->period;
  
  /* eventually schedule callback */
  call_t from ={-1, -1};
  scheduler_add_callback(start, to, &from, callmeback, NULL);
    
  return 0;
}

int ioctl(call_t *to, int option, void *in, void **out) {
  return 0;
}


/* ************************************************** */
/* ************************************************** */
int callmeback(call_t *to, call_t *from, void *args) {
  struct _cbr_private *nodedata = get_node_private_data(to);

  scheduler_add_callback(get_time() + nodedata->period, to, from, callmeback, NULL);
  tx(to, from);

  return 0;
}


/* ************************************************** */
/* ************************************************** */
void tx(call_t *to, call_t *from) {
  struct _cbr_private *nodedata = get_node_private_data(to);
  array_t *down = get_class_bindings_down(to);
  call_t to0 = {down->elts[0], to->object};
  destination_t destination = {nodedata->destination, 
			       {nodedata->position.x, 
				nodedata->position.y, 
				nodedata->position.z}};

   // insert data into the packet
   int _cbr_packet_header_size = sizeof(struct _cbr_packet_header);
   struct _cbr_packet_header *packet_header = malloc(_cbr_packet_header_size);
   field_t *field_cbr_header = field_create(INT, _cbr_packet_header_size, packet_header);

   packet_t *packet = packet_create(to, nodedata->size, -1);
   packet->destination=destination;
   packet_add_field(packet, "_data", field_cbr_header);

  PRINT_APPLICATION("[CBR] node %d sends a data packet to %d: packet_size=%d \n", to->object, destination.id, packet->size);
  TX(&to0, to, packet);
}


/* ************************************************** */
/* ************************************************** */
void rx(call_t *to, call_t *from, packet_t *packet) {
  PRINT_APPLICATION("[CBR] node %d received a data packet from %d: packet_size=%d  rxdBm=%lf RSSI=%lf \n", to->object, packet->node, packet->size, packet->rxdBm, packet->RSSI);
  packet_dealloc(packet);
}


/* ************************************************** */
/* ************************************************** */
application_methods_t methods = {rx};
