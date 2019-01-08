/**
 *  \file   cbr_v2.c
 *  \brief  CBR application
 *  \author Elyes Ben Hamida & Quentin Lampin
 *  \date   2009
 **/
#include <stdio.h>

#include <kernel/include/modelutils.h>


/* ************************************************** */
/* ************************************************** */
model_t model =  {
  "CBR application v2",
  "Elyes Ben Hamida & Quentin Lampin",
  "0.1",
  MODELTYPE_APPLICATION
};


/* ************************************************** */
/* ************************************************** */
struct _cbr_private {
  uint64_t start;
  uint64_t period;
  nodeid_t destination;
  position_t position;
  int sequence;
  void *timer_id;
};

/* ************************************************** */
/* ************************************************** */
struct data_packet_header {
  int source;
  int sequence;
  double delay;
  double source_pos_x;
  double source_pos_y;
};


/* ************************************************** */
/* ************************************************** */
int send_message(call_t *to, call_t *from, void *args);
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
  nodedata->sequence = 0;

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
  }

  set_node_private_data(to, nodedata);
  return 0;
  
 error:
  free(nodedata);
  return -1;
}

int unbind(call_t *to) {
  struct _cbr_private *nodedata = get_node_private_data(to);
  destroy_timer(nodedata->timer_id);
  free(nodedata);    
  return 0;
}


/* ************************************************** */
/* ************************************************** */
int bootstrap(call_t *to) {
  struct _cbr_private *nodedata = get_node_private_data(to);
  double random_double = get_random_double();
  uint64_t start = get_time() + nodedata->start + random_double * nodedata->period;

  /* create a periodic timer for the function send_message */
  nodedata->timer_id = create_timer(to, send_message, never_stop, periodic_trigger, &(nodedata->period));    

  /* start timer */
  start_timer(nodedata->timer_id, start); 

  return 0;
}

int ioctl(call_t *to, int option, void *in, void **out) {
  return 0;
}


/* ************************************************** */
/* ************************************************** */
int send_message(call_t *to, call_t *from, void *args) {
  tx(to, from);
  return 0;
}


/* ************************************************** */
/* ************************************************** */
void tx(call_t *to, call_t *from) {
  struct _cbr_private *nodedata = get_node_private_data(to);
  array_t *down = get_class_bindings_down(to);
  packet_t *packet = packet_create(to, sizeof(struct data_packet_header), -1);
  struct data_packet_header *header = malloc(sizeof(struct data_packet_header));
  field_t *field_cbr_v2 = field_create(DBLE, sizeof(struct data_packet_header), header);

  packet_add_field(packet, "data_packet_header", field_cbr_v2);

  call_t to0 = {down->elts[0], to->object};
  destination_t destination = {nodedata->destination, 
			       {nodedata->position.x, 
				nodedata->position.y, 
				nodedata->position.z}};
  packet->destination=destination;


  header->source = to->object;
  header->sequence = nodedata->sequence++;
  header->delay = get_time() * 0.000000001;
  header->source_pos_x = get_node_position(to->object)->x;
  header->source_pos_y = get_node_position(to->object)->y;

  PRINT_APPLICATION("[CBRV2] node %d sends a data packet to %d: packet_size=%d \n", to->object, destination.id, packet->size);
  TX(&to0, to, packet);
}


/* ************************************************** */
/* ************************************************** */
void rx(call_t *to, call_t *from, packet_t *packet) {  
  struct data_packet_header *header = (struct data_packet_header *) packet_retrieve_field_value_ptr(packet, "data_packet_header");

  printf("[CBRV2] node %d (%.2lf,%.2lf) received a packet from %d (%.2lf,%.2lf): seq=%d delay=%lfs size=%d bytes real_size=%d bits rxdBm=%lf dBm\n", to->object, get_node_position(to->object)->x, get_node_position(to->object)->y, header->source, header->source_pos_x, header->source_pos_y, header->sequence, ((get_time()*0.000000001)-header->delay), packet->size, packet->real_size, packet->rxdBm);

  packet_dealloc(packet);
}


/* ************************************************** */
/* ************************************************** */
application_methods_t methods = {rx};
