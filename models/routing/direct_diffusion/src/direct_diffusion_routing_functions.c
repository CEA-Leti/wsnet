/**
 *  \file   direct_diffusion_routing_functions.c
 *  \brief  Direct Diffusion based routing approach: Auxiliary Functions Source Code File
 *  \author Luiz Henrique Suraty Filho (CEA-LETI)
 *  \date   May 2013
 *  \version 1.0
 **/

#include <stdio.h>
#include <kernel/include/modelutils.h>
#include "direct_diffusion_routing_interest_management.h"
#include "direct_diffusion_routing_print_logs.h"
#include "direct_diffusion_routing_types.h"
#include "direct_diffusion_routing_route_management.h"
#include "direct_diffusion_routing_functions.h"

/* ************************************************** */
/* ************************************************** */

/** \brief Set default values of the node.
 *  \fn void set_node_default_values(struct _direct_diffusion_routing_node_private *nodedata)
 *  \param nodedata is a pointer to the node data
 *  \return nothing
 **/
void set_node_default_values(struct _direct_diffusion_routing_node_private *nodedata){
  int i = 0;

  /* set the default values for global parameters */
  nodedata->node_type = SENSOR_NODE;
  nodedata->routing_table = list_create();
  nodedata->packet_table = list_create();
  nodedata->path_establishment_delay = -1;
  nodedata->sink_id = -1;
  nodedata->seq = 0;

  for (i=0; i<5; i++) {
    nodedata->rx_nbr[i] = 0;
    nodedata->tx_nbr[i] = 0;
  }


  /* set the default values for the SINK interest propagation protocol */
  nodedata->interest_status = STATUS_OFF;
  nodedata->interest_propagation_probability = 1.0;
  nodedata->interest_propagation_backoff = 1000000000ull; /* 1s */
  nodedata->interest_nbr = -1;
  nodedata->seq = 0;
  nodedata->interest_start = 0;
  nodedata->interest_period = 10000000000ull; /* 10s */
  nodedata->ttl = 100;
  nodedata->interest_data_type = -1;


  nodedata->log_status = 0x0000;
  return;
}


/** \brief Read parameters from the xml file.
 *  \fn int read_xml_file(struct _direct_diffusion_routing_node_private *nodedata, void *params)
 *  \param nodedata is a pointer to the node data
 *  \param params is a pointer to the parameters read from the XML configuration file
 *  \return SUCCESSFUL if success, UNSUCCESSFUL otherwise
 **/
int read_nodedata_from_xml_file(struct _direct_diffusion_routing_node_private *nodedata, void *params){

  param_t *param;

  /* get module parameters from the XML configuration file*/
  list_init_traverse(params);

  while ((param = (param_t *) list_traverse(params)) != NULL) {

    /* reading the parameter related to the node type from the xml file */
    if (!strcmp(param->key, "node_type")) {
      if (get_param_integer(param->value, &(nodedata->node_type))) {
        return UNSUCCESSFUL;
      }
    }
    /* reading the parameter related to the interest dissemination protocol from the xml file */
    if (!strcmp(param->key, "interest_status")) {
      if (get_param_integer(param->value, &(nodedata->interest_status))) {
        return UNSUCCESSFUL;
      }
    }
    if (!strcmp(param->key, "interest_propagation_probability")) {
      if (get_param_double_range(param->value, &(nodedata->interest_propagation_probability), 0.0, 1.0)) {
        return UNSUCCESSFUL;
      }
    }
    if (!strcmp(param->key, "interest_propagation_backoff")) {
      if (get_param_time(param->value, &(nodedata->interest_propagation_backoff))) {
        return UNSUCCESSFUL;
      }
    }
    if (!strcmp(param->key, "interest_nbr")) {
      if (get_param_integer(param->value, &(nodedata->interest_nbr))) {
        return UNSUCCESSFUL;
      }
    }
    if (!strcmp(param->key, "interest_start")) {
      if (get_param_time(param->value, &(nodedata->interest_start))) {
        return UNSUCCESSFUL;
      }
    }
    if (!strcmp(param->key, "interest_period")) {
      if (get_param_time(param->value, &(nodedata->interest_period))) {
        return UNSUCCESSFUL;
      }
    }
    if (!strcmp(param->key, "ttl")) {
      if (get_param_integer(param->value, &(nodedata->ttl))) {
        return UNSUCCESSFUL;
      }
    }
    if (!strcmp(param->key, "interest_data_type")) {
      if (get_param_integer(param->value, &(nodedata->interest_data_type))) {
        return UNSUCCESSFUL;
      }
    }


    /* reading the parameter related to the node type from the xml file */
    if (!strcmp(param->key, "sink_id")) {
      if (get_param_integer(param->value, &(nodedata->sink_id))) {
        return UNSUCCESSFUL;
      }
    }


    /* reading the parameter related to the node type from the xml file */
    if (!strcmp(param->key, "log_status")) {
      nodedata->log_status = strtoul(param->value,NULL,16);
    }

  }

  return SUCCESSFUL;
}

/** \brief Process a HELLO packet received.
 *  \fn void process_hello_packet(call_t *to, call_t *from, packet_t *packet)
 *  \param to is a pointer to the called class
 *  \param from is a pointer to the class who invoked
 *  \param packet is a pointer to the packet to be processed
 *  \return nothing
 **/
void process_hello_packet(call_t *to, call_t *from, packet_t *packet){

  /* destroy hello packet */
  packet_dealloc(packet);

  return;
}

/** \brief Process a DATA packet received.
 *  \fn void process_data_packet(call_t *to, call_t *from, packet_t *packet)
 *  \param to is a pointer to the called class
 *  \param from is a pointer to the class who invoked
 *  \param packet is a pointer to the packet to be processed
 *  \return nothing
 **/
void process_data_packet(call_t *to, call_t *from, packet_t *packet){

  /* extract packet header */
  struct _direct_diffusion_routing_packet_header *header = (struct _direct_diffusion_routing_packet_header *) packet_retrieve_field_value_ptr(packet, "_direct_diffusion_routing_packet_header");

  /* check if packet have been already treated, i.e., there is an entry in packet table*/
  if (packet_table_lookup(to, from, header->origin, header->final_dst, header->packet_type, header->seq) == SUCCESSFUL){

    /*discard data packet*/
    packet_dealloc(packet);

    return;

  }

  /* update routing table according to received packet*/
  route_update_from_packet(to, from, packet);

  /* update packet table to avoid processing same packet */
  packet_table_update(to, from, header->origin, header->final_dst, header->packet_type, header->seq);

  /* check if data packet has arrived to destination and forwards to upper layers */
  if (header->final_dst  == to->object) {

    /* print logs*/
    print_log_rx(to, DATA_PACKET, packet);

    call_t to0   = {header->app_id, to->object};
    call_t from0 = {to->class, to->object};

    RX(&to0, &from0, packet);

  }

  /* forward the data packet to the closest/particular sink */
  else {
    /* print log */
    print_log_fwd(to,DATA_PACKET, packet);

    route_forward_packet_to_destination(to, from, packet);
  }

}

/** \brief Process an INTEREST packet received.
 *  \fn void process_data_packet(call_t *to, call_t *from, packet_t *packet)
 *  \param to is a pointer to the called class
 *  \param from is a pointer to the class who invoked
 *  \param packet is a pointer to the packet to be processed
 *  \return nothing
 **/
void process_interest_packet(call_t *to, call_t *from, packet_t *packet){

  struct _direct_diffusion_routing_node_private *nodedata = get_node_private_data(to);
  /* extract packet header */
  struct _direct_diffusion_routing_packet_header *header = (struct _direct_diffusion_routing_packet_header *) packet_retrieve_field_value_ptr(packet, "_direct_diffusion_routing_packet_header");

  /* check if packet have been already treated, i.e., there is an entry in packet table*/
  /* sink and anchor will destroy received packet */
  if (packet_table_lookup(to, from, header->origin, header->final_dst, header->packet_type, header->seq) == SUCCESSFUL || nodedata->node_type != SENSOR_NODE){

    /*discard data packet*/
    packet_dealloc(packet);

    return;

  }

  /* update routing table of sensor nodes and schedule packet retransmission using a random backoff period */
  /* update routing table according to received packet*/
  if (route_update_from_packet(to, from, packet) == SUCCESSFUL){

    print_log_rx(to, header->packet_type, packet);

    scheduler_add_callback(get_time() + get_random_double() * nodedata->interest_propagation_backoff, to, from, interest_propagation, (void *)(packet));
  }

}

/** \brief Forward packets to lower layers
 *  \fn int forward_to_lower_layers(call_t *to, packet_t *packet, int packet_type)
 *  \param to is a pointer to the called class
 *  \param from is a pointer to the class who invoked
 *  \param packet is a pointer to the packet to be forwarded
 *  \param packet_type indicates which type of packet will be forwarded
 *  \return SUCCESSFUL if success, UNSUCCESSFUL otherwise
 **/
int forward_to_lower_layers(call_t *to, call_t *from, packet_t *packet, int packet_type){

  /* get class and node private data*/
  struct _direct_diffusion_routing_node_private *nodedata = get_node_private_data(to);
  struct _direct_diffusion_routing_class_private *classdata = get_class_private_data(to);

  /* encapsulate packet with layer's header */
  if (set_direct_diffusion_routing_packet_header(to, from, packet, packet_type) == UNSUCCESSFUL){

    /* dealloc original packet */
    packet_dealloc(packet);

    /*return error*/
    return UNSUCCESSFUL;
  }

  /* clone packet and send to lower layers for further forwarding */
  /* send to all lower layer if no channel was previously decided*/
  array_t *down = get_class_bindings_down(to);
  if (packet->channel == -1){

    int i = down->size;
    while (i--){
      packet_t *packet_cloned = packet_clone(packet);
      call_t to0   = {down->elts[i], to->object};
      call_t from0 = {to->class, to->object};

      TX(&to0, &from0, packet_cloned);
    }
  }
  /* send to specific layer, if channel have been decided*/
  else{

    packet_t *packet_cloned = packet_clone(packet);
    call_t to0   = {down->elts[packet->channel], to->object};
    call_t from0 = {to->class, to->object};

    TX(&to0, &from0, packet_cloned);

  }

  /* update stats*/
  nodedata->tx_nbr[packet_type]++;

  /* update global stats */
  classdata->current_tx_control_packet ++;
  classdata->current_tx_control_packet_bytes += (packet->real_size/8);

  /* print logs */
  print_log_tx(to, packet_type, packet);

  /* dealloc original packet */
  packet_dealloc(packet);

  return SUCCESSFUL;
}


/** \brief Encapsulate the packet with layer's header and, in order to do so, verify the existance of a route to the destination
 *  \fn int set_direct_diffusion_routing_packet_header(call_t *to, packet_t *packet, int packet_type) 
 *  \param to is a pointer to the called class
 *  \param from is a pointer to the class who invoked
 *  \param packet is a pointer to the packet to be forwarded
 *  \param packet_type indicates which type of packet will be forwarded
 *  \return SUCCESSFUL if success, UNSUCCESSFUL otherwise
 **/
int set_direct_diffusion_routing_packet_header(call_t *to, call_t *from, packet_t *packet, int packet_type) {
  struct _direct_diffusion_routing_node_private *nodedata = get_node_private_data(to);

  field_t *field = packet_retrieve_field(packet, "_direct_diffusion_routing_packet_header");

  /* check if packet header does not exists yet*/
  if ( field == NULL ){

    /* create and insert Direct Diffusion header into the packet */
    int _direct_diffusion_routing_packet_header_size = sizeof(struct _direct_diffusion_routing_packet_header);
    struct _direct_diffusion_routing_packet_header *header = malloc(_direct_diffusion_routing_packet_header_size);
    field_t *field_direct_diffusion_routing_packet_header = field_create(INT, _direct_diffusion_routing_packet_header_size, header);

    /* increase header size*/
    packet->size += sizeof(struct _direct_diffusion_routing_packet_header);
    packet->real_size += packet->size*8;

    /* set origin and final destination*/
    header->final_dst = packet->destination.id;
    header->origin = to->object;
    header->packet_type = packet_type;
    header->ttl = nodedata->ttl;
    header->ttl_max = nodedata->ttl;
    header->seq = ++nodedata->seq;
    header->app_id = from->class;

    /* add field to the packet*/
    packet_add_field(packet, "_direct_diffusion_routing_packet_header", field_direct_diffusion_routing_packet_header);

  }

  struct _direct_diffusion_routing_packet_header *header = (struct _direct_diffusion_routing_packet_header *) packet_retrieve_field_value_ptr(packet, "_direct_diffusion_routing_packet_header");

  /* check for a route to the destination */
  struct _direct_diffusion_routing_route *route = NULL;
  if (packet->destination.id != BROADCAST_ADDR) {
    /* if no route, launch discovery procedure(RREQ) and return UNSUCCESSFUL */
    if ( (route = route_get_nexthop_to_destination(to, from, header->final_dst)) == NULL) {
      return UNSUCCESSFUL;
    }
    header->dst = route->nexthop_id;
  }
  else {
    header->dst = BROADCAST_ADDR;
  }

  /* update packet header values*/
  header->src = to->object;
  header->sensor_type = nodedata->node_type;
  header->ttl--;

  /* update packet destination for further layer*/
  packet->destination.id  = header->dst;

  return SUCCESSFUL;
}

