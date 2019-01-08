/**
 *  \file   gossip_sensor.c
 *  \brief  Sensor for gossip data dissemination protocols
 *  \author Guillaume Chelius and Elyes Ben Hamida
 *  \date   2007
 **/
#include <kernel/include/modelutils.h>

#include "data_d_common.h"


/**************************************************************************/
/**************************************************************************/
#define GOSSIP_DATA_TYPE 6

struct gossip_data_p {
  int source;
  int metadata;
  int d_seq;
  int d_value;
};


/**************************************************************************/
/**************************************************************************/
struct nodedata {
  int d_source[MAX_METADATA];
  int d_value[MAX_METADATA];
  int d_seq[MAX_METADATA];

  int s_seq[MAX_SOURCE];
  int r_seq[MAX_SINK];

};

/**************************************************************************/
/**************************************************************************/
model_t model =  {
  "Sensor for gossip data dissemination protocol",
  "Guillaume Chelius & Elyes Ben Hamida",
  "0.1",
  MODELTYPE_APPLICATION
};


/**************************************************************************/
/**************************************************************************/
int init(call_t *to, void *params) {
  return 0;
}

int destroy(call_t *to) {
  return 0;
}


/**************************************************************************/
/**************************************************************************/
int bind(call_t *to, void *params) {
  struct nodedata *nodedata = malloc(sizeof(struct nodedata));
  int i = MAX_METADATA;
  int j = MAX_SOURCE;
  int k = MAX_SINK;

  /* init values */
  while (i--) {
    nodedata->d_source[i] = -1;
    nodedata->d_value[i] = -1;
    nodedata->d_seq[i] = -1;
  }
  while (j--) {
    nodedata->s_seq[j] = -1;        
  }
  while (k--) {
    nodedata->r_seq[k] = -1;        
  }

  set_node_private_data(to, nodedata);
  return 0;
}

int unbind(call_t *to) {
  struct nodedata *nodedata = get_node_private_data(to);
  free(nodedata);
  return 0;
}


/**************************************************************************/
/**************************************************************************/
int bootstrap(call_t *to) {

  return 0;
}


/**************************************************************************/
/**************************************************************************/
void rx_source_adv(call_t *to, packet_t *packet) {
  struct nodedata *nodedata = get_node_private_data(to);
  struct source_adv_p *source = (struct source_adv_p *) packet_retrieve_field_value_ptr(packet, "source_adv_p");
  packet_t *packet0;
  destination_t dst = {BROADCAST_ADDR, {-1, -1, -1}};
  call_t to0 = {get_class_bindings_down(to)->elts[0], to->object};

  /* check adv sequence */
  if (source->s_seq <= nodedata->s_seq[source->source]) {
    /* old request */
    packet_dealloc(packet);
    return;
  }
  nodedata->s_seq[source->source] = source->s_seq;
    
  /* reply */
  packet0 = packet_create(to,sizeof(struct data_d_header) + sizeof(struct sensor_adv_p), -1);
  struct data_d_header *header = malloc(sizeof(struct data_d_header));
  field_t *field_header = field_create(INT, sizeof(struct data_d_header), header);
  packet_add_field(packet0, "data_d_header", field_header);
  struct sensor_adv_p *sensor = malloc(sizeof(struct sensor_adv_p));
  field_t *field_adv = field_create(DBLE, sizeof(struct sensor_adv_p), sensor);
  packet_add_field(packet0, "sensor_adv_p", field_adv);
  packet0->destination=dst;

  header->type = SENSOR_ADV_TYPE;
  sensor->sensor = to->object;
  sensor->source = source->source;
  sensor->s_seq = source->s_seq;
  TX(&to0, to, packet0);
  packet_dealloc(packet);

  return;
}


/**************************************************************************/
/**************************************************************************/
void rx_sink_adv(call_t *to, packet_t *packet) {
  struct nodedata *nodedata = get_node_private_data(to);
  struct sink_adv_p *sink = (struct sink_adv_p *) packet_retrieve_field_value_ptr(packet, "sink_adv_p");
  packet_t *packet0;
  destination_t dst = {BROADCAST_ADDR, {-1, -1, -1}};
  call_t to0 = {get_class_bindings_down(to)->elts[0], to->object};

  /* check request sequence */
  if (sink->r_seq <= nodedata->r_seq[sink->sink]) {
    /* old request */
    packet_dealloc(packet);
    return;
  }
  nodedata->r_seq[sink->sink] = sink->r_seq;
    
  /* check wether we have the data */
  if (sink->d_seq > nodedata->d_seq[sink->metadata]) {
    /* our data is not up to date */
    packet_dealloc(packet);
    return;
  }

  /* reply */
  packet0 = packet_create(to, sizeof(struct data_d_header) + sizeof(struct sensor_data_p), -1);
  struct data_d_header *header = malloc(sizeof(struct data_d_header));
  field_t *field_header = field_create(INT, sizeof(struct data_d_header), header);
  packet_add_field(packet0, "data_d_header", field_header);
  struct sensor_data_p *data = malloc(sizeof(struct sensor_data_p));
  field_t *field_data = field_create(DBLE, sizeof(struct sensor_data_p), data);
  packet_add_field(packet0, "sensor_data_p", field_data);
  packet0->destination=dst;

  header->type = SENSOR_DATA_TYPE;
  data->metadata = sink->metadata;
  data->sink = sink->sink;
  data->r_seq = sink->r_seq;
  data->source = nodedata->d_source[sink->metadata];
  data->d_seq = nodedata->d_seq[sink->metadata];
  data->d_value = nodedata->d_value[sink->metadata];
  TX(&to0, to, packet0);
  packet_dealloc(packet);

  return;
}


/**************************************************************************/
/**************************************************************************/
void rx_source_data(call_t *to, packet_t *packet) {
  struct nodedata *nodedata = get_node_private_data(to);
  struct source_data_p *data = (struct source_data_p *) packet_retrieve_field_value_ptr(packet, "source_data_p");
  packet_t *packet0;
  destination_t dst = {BROADCAST_ADDR, {-1, -1, -1}};
  call_t to0 = {get_class_bindings_down(to)->elts[0], to->object};

  /* check sensor */
  if (data->sensor != to->object) {
    /* not for us */
    packet_dealloc(packet);
    return;
  }

  /* check data sequence */
  if (data->d_seq <= nodedata->d_seq[data->metadata]) {
    /* old data */
    packet_dealloc(packet);
    return;
  }
  nodedata->d_seq[data->metadata] = data->d_seq;
  nodedata->d_source[data->metadata] = data->source;
  nodedata->d_value[data->metadata] = data->d_value;
    
  /* start gossip */
  packet0 = packet_create(to, sizeof(struct data_d_header) + sizeof(struct gossip_data_p), -1);
  struct data_d_header *header = malloc(sizeof(struct data_d_header));
  field_t *field_header = field_create(INT, sizeof(struct data_d_header), header);
  packet_add_field(packet0, "data_d_header", field_header);
  struct gossip_data_p *gossip = malloc(sizeof(struct gossip_data_p));
  field_t *field_gossip = field_create(DBLE, sizeof(struct gossip_data_p), gossip);
  packet_add_field(packet0, "gossip_data_p", field_gossip);
  packet0->destination=dst;

  header->type = GOSSIP_DATA_TYPE;
  gossip->source = data->source;
  gossip->metadata = data->metadata;
  gossip->d_seq = data->d_seq;
  gossip->d_value = data->d_value;
  TX(&to0, to, packet0);
  packet_dealloc(packet);

  return;
}


/**************************************************************************/
/**************************************************************************/
void rx_gossip_data(call_t *to, packet_t *packet) {
  struct nodedata *nodedata = get_node_private_data(to);
  struct gossip_data_p *data = (struct gossip_data_p *) packet_retrieve_field_value_ptr(packet, "gossip_data_p");
  destination_t dst = {BROADCAST_ADDR, {-1, -1, -1}};
  call_t to0 = {get_class_bindings_down(to)->elts[0], to->object};

  /* check data sequence */
  if (data->d_seq <= nodedata->d_seq[data->metadata]) {
    /* old data */
    packet_dealloc(packet);
    return;
  }
  nodedata->d_seq[data->metadata] = data->d_seq;
  nodedata->d_source[data->metadata] = data->source;
  nodedata->d_value[data->metadata] = data->d_value;
    
  /* forward gossip */
  packet->destination=dst;

  TX(&to0, to, packet);    
}


/**************************************************************************/
/**************************************************************************/
void rx(call_t *to, call_t *from, packet_t *packet) {
  struct data_d_header *header = (struct data_d_header *) packet_retrieve_field_value_ptr(packet, "data_d_header");

  switch (header->type) {
  case SOURCE_ADV_TYPE:
    /* for us */
    rx_source_adv(to, packet);
    break;
  case SENSOR_ADV_TYPE:
    /* not for us */
    packet_dealloc(packet);
    break;
  case SOURCE_DATA_TYPE:
    /* for us */
    rx_source_data(to, packet);
    break;
  case SINK_ADV_TYPE:
    /* for us */
    rx_sink_adv(to, packet);
    break;
  case SENSOR_DATA_TYPE:
    /* not for us */
    packet_dealloc(packet);
    break;
  case HOME_ADV_TYPE:
    /* not for us */
    packet_dealloc(packet);
    break;
  case GOSSIP_DATA_TYPE:
    /* for us */
    rx_gossip_data(to, packet);
    break;
  default:
    /* not for us */
    packet_dealloc(packet);
    break;
  }

  return;
}


/**************************************************************************/
/**************************************************************************/
application_methods_t methods = {rx};
