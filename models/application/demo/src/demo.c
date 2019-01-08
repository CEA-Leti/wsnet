/**
 *  \file   demo.c
 *  \brief  demo model to show WSNet3 functionnalities
 *  \author Loic Lemaitre
 *  \date   2010
 **/

/****************************************************************************/
/* Demo model is an example of WSNet3 simulation, aimed to demonstrate the  */
/* the ability of WSNet3 to handle several communication mediums. It uses   */
/* also monitor and packet fields new functionnalities.                     */
/*                                                                          */
/* Demo consists in 7 nodes (could be changed), distributed over 2 mediums: */
/* 3 nodes communicated over water, 3 nodes over air, and 1 behaving as a   */
/* gateway between them.                                                    */
/* Each node excepted the gateway, sends alternatively a message containing */
/* its id. This message is directly delivered to the node connected to the  */
/* same medium as the sender, and forwarded to the other nodes by the       */
/* gateway.                                                                 */
/*                                                                          */
/* A monitor displays a message on each node birth event.                   */
/****************************************************************************/


#include <stdio.h>
#include <kernel/include/modelutils.h>
#include <inttypes.h>

/* ************************************************** */
/* ************************************************** */
#define SENSOR  0
#define GATEWAY 1

#define TX_INTERVAL 1000000000 /* 1s */

//#define ENABLE_DEMO_TRACER

/* ************************************************** */
/* ************************************************** */
model_t model = 
{
    "demonstration model",
    "Loic Lemaitre",
    "0.1",
    MODELTYPE_APPLICATION
};


/* ************************************************** */
/* ************************************************** */
struct _demo_node_private 
{
    int type;
    /* Variable for printing*/
    int         log_status;
    uint32_t RX;
    uint32_t TX;
};

struct _demo_class_private
{
    int      source;
    uint64_t next_tx;
};


/* ************************************************** */
/* ************************************************** */
int tx(call_t *to, call_t *from, void *args);


/* ************************************************** */
/* ************************************************** */
int init(call_t *to, void *params) {
  struct _demo_class_private *classdata = malloc(sizeof(struct _demo_class_private));

  classdata->source = 0;
  classdata->next_tx = TX_INTERVAL;

  /* attach classdata to current class */
  set_class_private_data(to, classdata);

  return 0;
}

int destroy(call_t *to) {
  struct _demo_class_private *classdata = get_class_private_data(to);

  if (classdata)
    free(classdata);

  return 0;
}


/* ************************************************** */
/* ************************************************** */
int bind(call_t *to, void *params) {
  struct _demo_node_private *nodedata = malloc(sizeof(struct _demo_node_private));
  param_t *param;

  nodedata->type = SENSOR;
  nodedata->log_status = 0;

  /* get parameters */
  list_init_traverse(params);
  while ((param = (param_t *) list_traverse(params)) != NULL) {

    if (!strcmp(param->key, "type")) {
      if (get_param_integer(param->value, &(nodedata->type))) {
        goto error;
      }
    }
    if (!strcmp(param->key, "log_status")) {
      if (get_param_integer(param->value, &(nodedata->log_status))) {
        goto error;
      }
    }
  }

  nodedata->TX = 0;
  nodedata->RX = 0;

  /* attach nodedata to current node */
  set_node_private_data(to, nodedata);

  return 0;

  error:
  free(nodedata);
  return -1;
}

int unbind(call_t *to) {
  struct _demo_node_private *nodedata = get_node_private_data(to);

  free(nodedata);

  return 0;
}


/* ************************************************** */
/* ************************************************** */
int bootstrap(call_t *to) {
  struct _demo_node_private *nodedata = get_node_private_data(to);
  struct _demo_class_private *classdata = get_class_private_data(to);

  if (nodedata->type == SENSOR) {

    /* schedule tx */
    call_t from = {-1, -1};
    scheduler_add_callback(classdata->next_tx, to, &from, tx, NULL);
    classdata->next_tx += TX_INTERVAL;
  }

  return 0;
}

int ioctl(call_t *to, int option, void *in, void **out) {
  return 0;
}


/* ************************************************** */
/* ************************************************** */
int tx(call_t *to, call_t *from, void *args) {
  struct _demo_node_private  *nodedata  = get_node_private_data(to);
  array_t *down = get_class_bindings_down(to);
  int i = down->size;
  destination_t destination = {-1, {-1,-1,-1}}; /* broadcast */

  while (i--) {
    int *demo_tx_id =  malloc(sizeof(int));
    int demo_header_size = sizeof(int);
    field_t *field_demo = field_create(INT, demo_header_size, demo_tx_id);
    packet_t *packet = packet_create(to, demo_header_size, -1);
    call_t to0   = {down->elts[i], to->object};
    call_t from0 = {to->class, to->object};

    *demo_tx_id = to->object;
    packet_add_field(packet, "_demo_header_tx_id", field_demo);
    packet->destination=destination;

    if (nodedata->log_status){
      fprintf(stderr, "\nTX: node %d mac %s             [time %"PRIu64"]\n",
          to0.object, get_class_by_id(to0.class)->name, get_time());
    }
    TX(&to0, &from0, packet);

    nodedata->TX++;

  }

  return 0;
}


/* ************************************************** */
/* ************************************************** */
void fw(call_t *to, call_t *from, packet_t *packet) {
  struct _demo_node_private  *nodedata  = get_node_private_data(to);
  array_t *down = get_class_bindings_down(to);
  int i = down->size;

  while (i--) {
    /* forward on the other interface */
    if (from->class != down->elts[i]) {
      packet_t *packet1 = packet_clone(packet);
      call_t to0   = {down->elts[i], to->object};
      call_t from0 = {to->class, to->object};

      if (nodedata->log_status){
        fprintf(stderr, "FW: node %d mac %s             [time %"PRIu64"]\n",
            to0.object, get_class_by_id(to0.class)->name, get_time());
      }
      TX(&to0, &from0, packet1);
      nodedata->TX++;

    }
  }
}


/* ************************************************** */
/* ************************************************** */
void rx(call_t *to, call_t *from, packet_t *packet) {
  int *demo_tx_id = (int *) packet_retrieve_field_value_ptr(packet, "_demo_header_tx_id");
  struct _demo_node_private  *nodedata  = get_node_private_data(to);

  if (nodedata->log_status){
    fprintf(stderr, "RX: node %d mac %s from node %d [time %"PRIu64"]\n",
        to->object, get_class_by_id(from->class)->name, *demo_tx_id, get_time());
  }

  nodedata->RX++;

  if (nodedata->type == GATEWAY)
  {
    /* forward */
    fw(to, from, packet);
  }

  /* dealloc packet */
  packet_dealloc(packet);
}


/* ************************************************** */
/* ************************************************** */
application_methods_t methods = {rx};
