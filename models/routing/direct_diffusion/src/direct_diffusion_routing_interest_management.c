/**
 *  \file   direct_diffusion_routing_interest_management.c
 *  \brief  Direct Diffusion based routing approach: Direct Diffusion Interest Management Header File
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



/** \brief Generation of an INTEREST packet
 *  \fn int interest_generation(call_t *to, call_t *from)
 *  \param to is a pointer to the called class
 *  \param from is a pointer to the class who invoked
 *  \return SUCCESSFUL if success, UNSUCCESSFUL otherwise
 **/
int interest_generation(call_t *to, call_t *from) {

  struct _direct_diffusion_routing_node_private *nodedata = get_node_private_data(to);

  /* create and insert RREP header into the packet */
  int _direct_diffusion_routing_interest_header_size = sizeof(struct _direct_diffusion_routing_interest_packet_header);
  packet_t *packet = packet_create(to, _direct_diffusion_routing_interest_header_size, _direct_diffusion_routing_interest_header_size*8);
  struct _direct_diffusion_routing_interest_packet_header *interest_header = malloc(_direct_diffusion_routing_interest_header_size);
  field_t *field_direct_diffusion_routing_interest_header = field_create(INT, _direct_diffusion_routing_interest_header_size, interest_header);

  /* set basic packet header */
  interest_header->sink_id = to->object;
  interest_header->seq = nodedata->seq;
  interest_header->data_type = nodedata->interest_data_type;
  interest_header->position.x = get_node_position(to->object)->x;
  interest_header->position.y = get_node_position(to->object)->y;
  interest_header->position.z = get_node_position(to->object)->z;

  /* add field to the packet*/
  packet_add_field(packet, "_direct_diffusion_routing_interest_header", field_direct_diffusion_routing_interest_header);

  /* set final destination of packet*/
  packet->destination.id = BROADCAST_ADDR;

  /* send packet to lower layers*/
  if (forward_to_lower_layers(to, from, packet, INTEREST_PACKET) == UNSUCCESSFUL){
    goto interest_generation_error;
  }

  /* update nbr of transmitted INTEREST packets */
  if (nodedata->interest_nbr > 0) {
    nodedata->interest_nbr--;
  }


  return SUCCESSFUL;

  interest_generation_error:
  return UNSUCCESSFUL;
}

/** \brief Callback function for the periodic generation of RREQ packets in Direct Diffusion (to be used with the scheduler_add_callback function).
 *  \fn int interest_periodic_generation_callback(call_t *to, call_t *from, void *args)
 *  \param to is a pointer to the called class
 *  \param from is a pointer to the class who invoked
 *  \param args is a pointer to the arguments of the function
 *  \return SUCCESSFUL if success, UNSUCCESSFUL otherwise
 **/
int interest_periodic_generation_callback(call_t *to, call_t *from, void *args) {

  struct _direct_diffusion_routing_node_private *nodedata = get_node_private_data(to);

  /* generate INTEREST packet towards selected destination */
  if (interest_generation(to, from) == UNSUCCESSFUL){
    return UNSUCCESSFUL;
  }


  /* schedules next INTEREST generation */
  if (nodedata->interest_nbr > 0 || nodedata->interest_nbr == -1) {
    nodedata->previous_interest_slot_time += nodedata->interest_period;
    scheduler_add_callback(nodedata->previous_interest_slot_time + get_random_double() * nodedata->interest_period, to, from, interest_periodic_generation_callback, NULL);
  }

  return SUCCESSFUL;
}

/** \brief Function to forward INTEREST packets  (Direct Diffusion)
 *  \fn int interest_propagation(call_t *to, call_t *from, void *args)
 *  \param to is a pointer to the called class
 *  \param from is a pointer to the class who invoked
 *  \param args is a pointer to the arguments of the function
 *  \return SUCCESSFUL if success, UNSUCCESSFUL otherwise
 **/
int interest_propagation(call_t *to, call_t *from, void *args) {

  struct _direct_diffusion_routing_node_private *nodedata = get_node_private_data(to);

  packet_t *packet = (packet_t *) args;

  /* extract aodv packet header from the interest packet*/
  struct _direct_diffusion_routing_packet_header *header = (struct _direct_diffusion_routing_packet_header *) packet_retrieve_field_value_ptr(packet, "_direct_diffusion_routing_packet_header");

  /* extract RREQ packet header */
  //struct _direct_diffusion_routing_interest_packet_header *interest_header = (struct _direct_diffusion_routing_interest_packet_header *) packet_retrieve_field_value_ptr(packet, "_direct_diffusion_routing_interest_header");

  /* set destination to BROADCAST address*/
  packet->destination.id = BROADCAST_ADDR;

  /* drop the INTEREST packet according to a given probability */
  if (get_random_double_range(0.0, 1.0) > nodedata->interest_propagation_probability) {
    packet_dealloc(packet);
    return UNSUCCESSFUL;
  }

  /* broadcast packet only once, i.e. if there is no entry on packet table*/
  if (packet_table_lookup(to, from, header->origin, header->final_dst, header->packet_type, header->seq) == SUCCESSFUL) {

    packet_dealloc(packet);

    return UNSUCCESSFUL;
  }


  /* send packet to lower layers*/
  if (forward_to_lower_layers(to, from, packet, INTEREST_PACKET) == UNSUCCESSFUL){
    goto interest_propagation_error;
  }

  /* update packet table*/
  packet_table_update(to, from, header->origin, header->final_dst, INTEREST_PACKET, nodedata->seq);

  return SUCCESSFUL;

  interest_propagation_error:

  return UNSUCCESSFUL;
}

/** \brief Function to check if packet has already been sent (Direct Diffusion)
 *  \fn int interest_table_lookup(call_t *to, int src, int dst, int data_type, int seq)
 *  \param to is a pointer to the called class
 *  \param from is a pointer to the class who invoked
 *  \param origin is the origin ID
 *  \param final_dst is the final destination ID
 *  \param packet_type is the type of the packet (RREQ, RREP, DATA)
 *  \param seq is the sequence number of packet
 *  \return SUCCESSFUL if success, UNSUCCESSFUL otherwise
 **/
int packet_table_lookup(call_t *to, call_t *from, int origin, int final_dst, int packet_type, int seq) {
  struct _direct_diffusion_routing_node_private *nodedata = get_node_private_data(to);
  struct _direct_diffusion_routing_packet_table_entry *table_entry;
  list_init_traverse(nodedata->packet_table);
  while((table_entry = (struct _direct_diffusion_routing_packet_table_entry *) list_traverse(nodedata->packet_table)) != NULL) {
    if (table_entry->origin == origin && table_entry->final_dst == final_dst && table_entry->packet_type == packet_type && table_entry->seq >= seq) {

      if (packet_type == INTEREST_PACKET){
        if (table_entry->seq > seq){
          return SUCCESSFUL;
        }
      }
      else{
        if (table_entry->seq >= seq){
          return SUCCESSFUL;
        }
      }

    }
  }
  return UNSUCCESSFUL;
}

/** \brief Function to update the local packet table to avoid the transmission of duplicate packets (Direct Diffusion)
 *  \fn void interest_table_update(call_t *to, int src, int dst, int data_type, int seq)
 *  \param to is a pointer to the called class
 *  \param from is a pointer to the class who invoked
 *  \param origin is the origin ID
 *  \param final_dst is the final destination ID
 *  \param packet_type is the type of the packet (RREQ, RREP, DATA)
 *  \param seq is the sequence number of packet
 *  \return SUCCESSFUL if success, UNSUCCESSFUL otherwise
 **/
void packet_table_update(call_t *to, call_t *from, int origin, int final_dst, int packet_type, int seq) {
  struct _direct_diffusion_routing_node_private *nodedata = get_node_private_data(to);
  struct _direct_diffusion_routing_packet_table_entry *table_entry;
  list_init_traverse(nodedata->packet_table);
  while((table_entry = (struct _direct_diffusion_routing_packet_table_entry *) list_traverse(nodedata->packet_table)) != NULL) {
    if (table_entry->origin == origin && table_entry->final_dst == final_dst && table_entry->packet_type == packet_type && table_entry->seq > seq) {
      table_entry->seq = seq;
      table_entry->time = get_time();
      return;
    }
  }

  struct _direct_diffusion_routing_packet_table_entry *new_table_entry = (struct _direct_diffusion_routing_packet_table_entry *) malloc(sizeof(struct _direct_diffusion_routing_packet_table_entry));
  new_table_entry->final_dst = final_dst;
  new_table_entry->origin = origin;
  new_table_entry->packet_type = packet_type;
  new_table_entry->seq = seq;
  new_table_entry->time = get_time();
  list_insert(nodedata->packet_table, (void *)new_table_entry);
}


