/*
 *  \file   epidemic.c
 *  \brief  An epidemic data dissemination protocol
 *  \author Luiz Henrique Suraty Filho
 *  \date   2013
 */
#include <stdio.h>
#include <kernel/include/modelutils.h>
#include <inttypes.h>


// ************************************************** //
// ************************************************** //

// Defining module informations//
model_t model =  {
    "Epidemic protocol",
    "Luiz Henrique Suraty Filho",
    "0.1",
    MODELTYPE_APPLICATION,
};


// ************************************************** //
// ************************************************** //

// Defining node type //
#define SENSOR 0
#define SOURCE 1
#define SINK   2

// ************************************************** //
// ************************************************** //

// Node private data //
struct _epidemic_node_private {
  int type;
  int seq;
  uint64_t period;
  int last_seq;
  // for stats //
  int packet_tx;
  int packet_rx;
};

// Class private data //
struct _epidemic_class_private {
  double p;
  double q;
};


// Data Packet header //
struct _epidemic_packet_header {
  nodeid_t origin;
  nodeid_t source;
  int seq;
};

int _epidemic_packet_header_size = sizeof(nodeid_t) + sizeof(nodeid_t) + sizeof(int);
/* header origin, header src , seq number*/


// Function to create a new broadcast at the callback event from the WSNET core
// i.e. it is the function provided in the scheduling and the core calls it in
// the appropriate time.
int broadcastTX(call_t *to, call_t *from, void *args);


// Here we read the class variables from the xml config file
int init(call_t *to, void *params) {
  struct _epidemic_class_private *classdata= malloc(sizeof(struct _epidemic_class_private));
  param_t *param;

  // init class variables
  classdata->p = 1;
  classdata->q = 1;

  // reading the "init" markup from the xml config file
  list_init_traverse(params);
  while ((param = (param_t *) list_traverse(params)) != NULL) {

    if (!strcmp(param->key, "p")) {
      if (get_param_double_range(param->value, &(classdata->p), 0, 1)) {
        goto error;
      }
    }
    if (!strcmp(param->key, "q")) {
      if (get_param_double_range(param->value, &(classdata->q), 0, 1)) {
        goto error;
      }
    }
  }

  set_class_private_data(to, classdata);
  return 0;


  error:
  free(classdata);
  return -1;

}


// Destroy class, i.e. free the used memory 
int destroy(call_t *to) {
  struct _epidemic_class_private *classdata = get_class_private_data(to);

  if (classdata)
    free(classdata);

  return 0;
}


// Here we read the node variables from the xml config file
int bind(call_t *to, void *params) {
  struct _epidemic_node_private *nodedata = malloc(sizeof(struct _epidemic_node_private));
  param_t *param;

  // default values
  nodedata->period    = 1000000000;
  nodedata->type      = SENSOR;
  nodedata->seq       = 0;
  nodedata->last_seq  = -1;
  nodedata->packet_tx = 0;
  nodedata->packet_rx = 0;

  // reading the "default" markup from the xml config file
  list_init_traverse(params);
  while ((param = (param_t *) list_traverse(params)) != NULL) {

    if (!strcmp(param->key, "type")) {
      if (get_param_integer(param->value, &(nodedata->type))) {
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


// Before being unbinded (destroyed), each node prints its final stats
int unbind(call_t *to) {
  struct _epidemic_node_private *nodedata = get_node_private_data(to);

  // we print node stats before exit
  if (nodedata->packet_tx > 0 || nodedata->packet_rx > 0) {
    fprintf(stderr, "[UNBINDNODE] Node %d type %d => total rx = %d , total tx = %d \n", to->object, nodedata->type, nodedata->packet_rx, nodedata->packet_tx);
  }

  free(nodedata);
  return 0;
}

// On initialization
int bootstrap(call_t *to) {
  struct _epidemic_node_private *nodedata = get_node_private_data(to);

  // if the node type is source, we schedule a new callback
  call_t from = {-1, -1};

  if (nodedata->type == SOURCE) {
    scheduler_add_callback(nodedata->period, to, &from, broadcastTX, NULL);
  }

  return 0;
}

int ioctl(call_t *to, int option, void *in, void **out) {
  return 0;
}

// Function to create a new broadcast at the callback event from the WSNET core
// i.e. it is the function provided in the scheduling and the core calls it in
// the appropriate time.
int broadcastTX(call_t *to, call_t *from, void *args) {
  struct _epidemic_node_private *nodedata = get_node_private_data(to);

  array_t *down = get_class_bindings_down(to);
  int i = down->size;

  // broadcast a new data packet
  while (i--){
    call_t to0   = {down->elts[i], to->object};
    call_t from0 = {to->class, to->object};
    destination_t destination = {BROADCAST_ADDR, {-1,-1,-1}}; /* broadcast */

    // insert packet header into the packet
    int _epidemic_header_size = sizeof(struct _epidemic_packet_header);
    struct _epidemic_packet_header *packet_header = malloc(_epidemic_header_size);
    field_t *field_epidemic_header = field_create(INT, _epidemic_header_size, packet_header);

    packet_t *packet = packet_create(to, _epidemic_packet_header_size ,-1);
    packet->destination=destination;

    packet_header->origin = to->object;
    packet_header->source = to->object;
    packet_header->seq = ++nodedata->seq;
    packet_add_field(packet, "_epidemic_header", field_epidemic_header);

    // Send to lower-layer
    TX(&to0, &from0, packet);

  }

  fprintf(stderr,"[SOURCE] Node %d (%lf %lf) broadcast a packet, seq=%d \n", to->object, get_node_position(to->object)->x, get_node_position(to->object)->y, nodedata->seq);

  // for stats
  nodedata->packet_tx++;

  // we schedule a new callback after actualtime+period
  scheduler_add_callback(get_time() + nodedata->period, to, from, broadcastTX, NULL);

  return 0;
}

// Function to forward packets
void fwd(call_t *to, call_t *from, packet_t *packet) {
  struct _epidemic_node_private *nodedata = get_node_private_data(to);
  array_t *down = get_class_bindings_down(to);
  int i = down->size;

  fprintf(stderr, "[FWD] Node %d is forwarding packets\n", to->object);

  // send to lower layers for further forwarding
  while (i--){
    //packet_t *packet1 = packet_clone(packet);
    struct _epidemic_packet_header *packet_header = (struct _epidemic_packet_header *) packet_retrieve_field_value_ptr(packet, "_epidemic_header");
    packet_header->source = to->object;
    call_t to0   = {down->elts[i], to->object};

    call_t from0 = {to->class, to->object};
    TX(&to0, &from0, packet);

    nodedata->packet_tx++;
  }

}

// Funtion to receive packets from lower-layers
void rx(call_t *to, call_t *from, packet_t *packet) {

  struct _epidemic_packet_header *packet_header = (struct _epidemic_packet_header *) packet_retrieve_field_value_ptr(packet, "_epidemic_header");
  struct _epidemic_node_private *nodedata = get_node_private_data(to);
  struct _epidemic_class_private *classdata = get_class_private_data(to);

  fprintf(stderr, "[RX] Node %d (%lf %lf) received a packet originated in %d from %d with sequence = %d !\n", to->object, get_node_position(to->object)->x, get_node_position(to->object)->y, packet_header->origin , packet_header->source , packet_header->seq);

  if (nodedata->type != SINK){
    // stores the received data according to the q probability
    if ( get_random_double_range(0.0,1.0) <= classdata->q ) {
      nodedata->packet_rx++;
    }

    // forwards the received data according to the p probability and if it's a new data
    if ( (nodedata->last_seq < packet_header->seq) && (get_random_double_range(0.0,1.0) <= classdata->p) ) {

      fwd(to, from, packet);

      nodedata->last_seq = packet_header->seq;

      return;
    }

  } else {
    nodedata->packet_rx++;
  }

  // else dealloc the packet
  packet_dealloc(packet);
}

application_methods_t methods = {rx};

