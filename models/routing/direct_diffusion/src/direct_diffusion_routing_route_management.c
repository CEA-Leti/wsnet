/**
 *  \file   direct_diffusion_routing_route_management.c
 *  \brief  Direct Diffusion based routing approach: Route Management Source Code File
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

/** \brief Function to forward received packet towards the destination (Direct Diffusion)
 *  \fn void route_forward_data_packet_to_destination(call_t *to, call_t *from, packet_t *packet)
 *  \param to is a pointer to the called class
 *  \param from is a pointer to the class who invoked
 *  \param packet is a pointer to the received packet
 **/
void route_forward_packet_to_destination(call_t *to, call_t *from, packet_t *packet) {


  /* encapsulate packet with layer's header and send to lower layers*/
  if ( forward_to_lower_layers(to, from, packet, DATA_PACKET) == UNSUCCESSFUL){
    goto route_forward_packet_error;
  }

  return;

  route_forward_packet_error:

  //route_direct_diffusion_show(to);
  return;
}


/** \brief Function to update the local routing table according to received packets
 *  \fn  int route_update_from_rreq(call_t *to, call_t *from, packet_t *packet)
 *  \param to is a pointer to the called class
 *  \param from is a pointer to the class who invoked
 *  \param packet is a pointer to the received packet
 *  \return SUCCESSFUL if success, UNSUCCESSFUL otherwise
 **/
int route_update_from_packet(call_t *to, call_t *from, packet_t *packet) {

  /* get node private data*/
  struct _direct_diffusion_routing_node_private *nodedata = get_node_private_data(to);

  /* extract the network and rrep headers */
  struct _direct_diffusion_routing_packet_header *header = (struct _direct_diffusion_routing_packet_header *) packet_retrieve_field_value_ptr(packet, "_direct_diffusion_routing_packet_header");

  /* Current object is the src, therefore, nothing to update*/
  if (to->object == header->origin){
    return UNSUCCESSFUL;
  }

  struct _direct_diffusion_routing_route *route;

  int hops = header->ttl_max - header->ttl;

  /* get the sensibility of the radio for the mac layer(call_t from) which has pushed up the current packet*/
  nodedata->mindBm = get_sensitivity_from_radio(to, from) + 5;

  /* check if a nexthop to the dst already exist and update related information */
  list_init_traverse(nodedata->routing_table);
  while((route = (struct _direct_diffusion_routing_route *) list_traverse(nodedata->routing_table)) != NULL) {

    /* the dst is present in routing table */
    if (route->dst == header->origin){

      /* update if it has more recent information and it is closer through such node*/
      if (header->seq >= route->seq  && (packet->RSSI < nodedata->mindBm) && (route->hop_to_dst > (hops)) ) {
        route->nexthop_id = header->src;
        route->hop_to_dst = hops;
        route->time = get_time();
        route->seq = header->seq;
        return SUCCESSFUL;
      }

      /*route is up-to-date, therefore, nothing more to be done*/
      return UNSUCCESSFUL;

    }
  }

  /* in case there is no route on the routing table, update it by inserting the route*/
  route = (struct _direct_diffusion_routing_route *) malloc(sizeof(struct _direct_diffusion_routing_route));
  route->dst = header->origin;
  route->nexthop_id = header->src;
  route->nexthop_lqe = 1.0;
  route->time = get_time();
  route->seq = header->seq;
  route->hop_to_dst = hops;
  list_insert(nodedata->routing_table, (void *) route);

  return SUCCESSFUL;
}

/** \brief Function to get the sensitivity from the radio below lower class
 *  \fn     double get_sensitivity_from_radio(call_t *actual, call_t *lower)
 *  \param actual is a pointer to the current class
 *  \param lower is a pointer to the lower class who invoked
 *  \return sensitivity of the radio below lower class
 **/
double get_sensitivity_from_radio(call_t *actual, call_t *lower){

  /* get the radio binding below the mac layer which sent the packet */
  array_t *down = get_class_bindings_down(lower);

  call_t radio_layer = {down->elts[0], actual->object};

  /* return the sensibility of the radio */
  return transceiver_get_sensibility(&radio_layer, actual);

}

/** \brief Function to compute the nexthop towards a given destination (AODV)
 *  \fn   struct _direct_diffusion_routing_route* route_get_nexthop_to_destination(call_t *to, call_t *from, int dst)
 *  \param to is a pointer to the called class
 *  \param from is a pointer to the class who invoked
 *  \param dst is the ID of the destination
 *  \return NULL if failure, otherwise the destination information of nexthop node
 **/
struct _direct_diffusion_routing_route* route_get_nexthop_to_destination(call_t *to, call_t *from, int dst) {

    /* get class and node private data*/
    struct _direct_diffusion_routing_node_private *nodedata = get_node_private_data(to);
    struct _direct_diffusion_routing_route *route = NULL;

    /* Check for the nexthop towards a particular destination */
    list_init_traverse(nodedata->routing_table);
    while((route = (struct _direct_diffusion_routing_route *) list_traverse(nodedata->routing_table)) != NULL) {
      if (route->dst == dst) {
        return route;
      }
    }

    return NULL;
}

/** \brief Function to list the routing table on the standard output (AODV)
 *  \fn     void route_direct_diffusion_show(call_t *to, call_t *from) 
 *  \param to is a pointer to the called class
 *  \param from is a pointer to the class who invoked
 **/
void route_direct_diffusion_show(call_t *to, call_t *from) {

  /* get class and node private data*/
  struct _direct_diffusion_routing_node_private *nodedata = get_node_private_data(to);
  struct _direct_diffusion_routing_route *route = NULL;

  list_init_traverse(nodedata->routing_table);
  while((route = (struct _direct_diffusion_routing_route *) list_traverse(nodedata->routing_table)) != NULL) {
    fprintf(stderr,"   => Route to dst=%d is nexthop=%d with hop_nbr=%d \n", route->dst, route->nexthop_id, route->hop_to_dst);
  }

}



/** \brief Function to update the global class stats informations (global path establishment time, etc.)
 *  \fn     void route_update_global_stats(call_t *to, call_t *from)
 *  \param to is a pointer to the called class
 *  \param from is a pointer to the class who invoked
 *  \param delay is the path establishment time
 **/
void route_update_global_stats(call_t *to, call_t *from, double path_delay) {

  /* get class private data*/
  struct _direct_diffusion_routing_class_private *classdata = get_class_private_data(to);

  classdata->global_tx_control_packet = classdata->current_tx_control_packet;
  classdata->global_rx_control_packet = classdata->current_rx_control_packet;
  classdata->global_tx_control_packet_bytes = classdata->current_tx_control_packet_bytes;
  classdata->global_rx_control_packet_bytes = classdata->current_rx_control_packet_bytes ;

  if (path_delay > classdata->global_establishment_time) {
    classdata->global_establishment_time = path_delay;
  }

}


