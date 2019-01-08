/**
 *  \file   direct_diffusion_routing.c
 *  \brief  Direct Diffusion based routing approach: Main
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


/** \brief A structure to define the simulation module information
 *  \struct model_t
 **/
model_t model =  {
    "Direct Diffusion based routing approach",                  /*!< The simulation module name. */
    "Luiz Henrique Suraty Filho - CEA-LETI",                                               /*!< The author name. */
    "1.0",                                                                                       /*!< The version number. */
    MODELTYPE_ROUTING,                                                          /*!< The module type. */
};


/* ************************************************** */
/* ************************************************** */

/** \brief The init input-interface to read/initialize the GLOBAL class variables (from the XML configuration file) at the simulation startup.
 *  \fn int init(call_t *to, void *params)
 *  \param to is a pointer to the called class
 *  \param params is a pointer to the parameters read from the XML configuration file 
 *  \return SUCCESSFUL if success, UNSUCCESSFUL otherwise
 **/
int init(call_t *to, void *params) {

  /* initialize class private data*/
  struct _direct_diffusion_routing_class_private *classdata = malloc(sizeof(struct _direct_diffusion_routing_class_private));

  /* set the default values for global parameters */
  classdata->global_establishment_time = -1;

  classdata->global_tx_control_packet = 0;
  classdata->global_rx_control_packet = 0;
  classdata->global_tx_control_packet_bytes = 0;
  classdata->global_rx_control_packet_bytes = 0;

  classdata->current_tx_control_packet = 0;
  classdata->current_rx_control_packet = 0;
  classdata->current_tx_control_packet_bytes = 0;
  classdata->current_rx_control_packet_bytes = 0;

  /* Assign the initialized local variables to the node */
  set_class_private_data(to, classdata);

  return SUCCESSFUL;
}

/** \brief The destroy input-interface to deallocate/free the GLOBAL class variables at the end of simulation.
 *  \fn int destroy(call_t *to)
 *  \param to is a pointer to the called class
 *  \return SUCCESSFUL if success, UNSUCCESSFUL otherwise
 **/
int destroy(call_t *to) {

  /* get class private data*/
  struct _direct_diffusion_routing_class_private *classdata = get_class_private_data(to);

  /* destroy/clean the global variables */
  free(classdata);
  return SUCCESSFUL;
}


/** \brief The bind input-interface to read/initialize the LOCAL node variables (from the XML configuration file) at the simulation startup.
 *  \fn int bind(call_t *to, void *params) 
 *  \param to is a pointer to the called class
 *  \param params is a pointer to the parameters read from the XML configuration file 
 *  \return SUCCESSFUL if success, UNSUCCESSFUL otherwise
 **/
int bind(call_t *to, void *params) {

  /* initialize node private data*/
  struct _direct_diffusion_routing_node_private *nodedata = malloc(sizeof(struct _direct_diffusion_routing_node_private));

  /* Set default values for node private data*/
  set_node_default_values(nodedata);

  /* Read parameters from xml file*/
  if (read_nodedata_from_xml_file(nodedata, params)==UNSUCCESSFUL){
    free(nodedata);
    return UNSUCCESSFUL;
  }

  /* Check the validity of the input parameters */
  if ((nodedata->node_type != SENSOR_NODE) && (nodedata->node_type != SINK_NODE) && (nodedata->node_type != ANCHOR_NODE)) {
    nodedata->node_type = SENSOR_NODE;
  }

  /* Assign the initialized local variables to the node */
  set_node_private_data(to, nodedata);

  return SUCCESSFUL;

}


/** \brief The unbind input-interface to deallocate/free the LOCAL node variables at the end of simulation.
 *  \fn int unbind(call_t *to, call_t *from)
 *  \param to is a pointer to the called class
 *  \param from is a pointer to the class who invoked
 *  \return SUCCESSFUL if success, UNSUCCESSFUL otherwise
 **/
int unbind(call_t *to, call_t *from) {

  /* get node private data*/
  struct _direct_diffusion_routing_node_private *nodedata = get_node_private_data(to);

  print_log_stats(to, from);

  free(nodedata);

  return SUCCESSFUL;
}


/** \brief The bootstrap input-interface which is automatically called by the WSNet simulation core at the beginning of the simulation.
 *  \fn int bootstrap(call_t *to, call_t *from)
 *  \param to is a pointer to the called class
 *  \param from is a pointer to the class who invoked
 *  \return SUCCESSFUL if success, UNSUCCESSFUL otherwise
 **/
int bootstrap(call_t *to) {

  /* get class and node private data*/
  struct _direct_diffusion_routing_node_private *nodedata = get_node_private_data(to);

  /* start the periodic RREQ generation */
  if (nodedata->interest_status == STATUS_ON && nodedata->node_type == SINK_NODE && (nodedata->interest_nbr > 0 || nodedata->interest_nbr == -1)) {
    uint64_t interest_slot_time = get_time() + nodedata->interest_start;
    uint64_t interest_tx_time = interest_slot_time + get_random_double() * nodedata->interest_period;
    nodedata->previous_interest_slot_time = interest_slot_time;
    call_t from = {-1,-1};
    scheduler_add_callback(interest_tx_time, to, &from, interest_periodic_generation_callback, NULL);
  }

  return SUCCESSFUL;
}

int ioctl(call_t *to, int option, void *in, void **out) {
  return SUCCESSFUL;
}


/** \brief The setheader input-interface which is automatically called by the WSNet simulation core when a higher-layer simulation module call the SET_HEADER output-interface to determine the nexthop node for a given transmission.
 *  \fn int set_header(call_t *to, call_t *from, packet_t *packet, destination_t *dst)
 *  \param to is a pointer to the called class
 *  \param from is a pointer to the class who invoked
 *  \param packet is a pointer to the created packet
 *  \param dst is a pointer to the structure containing the destination information
 *  \return SUCCESSFUL if success, UNSUCCESSFUL otherwise
 **/
int set_header(call_t *to, call_t *from, packet_t *packet, destination_t *dst) {
  return SUCCESSFUL;
}


/** \brief The getheader_size input-interface which is automatically called by the WSNet simulation core when a higher-layer simulation module call the GET_HEADER_SIZE output-interface to determine the total size of a packet.
 *  \fn int get_header_size(call_t *to, call_t *from)
 *  \param to is a pointer to the called class
 *  \param from is a pointer to the class who invoked
 *  \return the size of all protocol headers (in bytes)
 **/
int get_header_size(call_t *to, call_t *from) {
  return sizeof(struct _direct_diffusion_routing_packet_header);
}


/** \brief The getheader_realsize input-interface which is automatically called by the WSNet simulation core when a higher-layer simulation module call the GET_HEADER_REAL_SIZE output-interface to determine the total real size of a packet.
 *  \fn int get_header_real_size(call_t *to, call_t *from)
 *  \param to is a pointer to the called class
 *  \param from is a pointer to the class who invoked
 *  \return the real size of all protocol headers (in bytes)
 **/
int get_header_real_size(call_t *to, call_t *from) {
  return sizeof(struct _direct_diffusion_routing_packet_header)*8;
}


/** \brief The tx input-interface which is automatically called by the WSNet simulation core when the immediately higher-level simulation module sends a packet.
 *  \fn void tx(call_t *to, call_t *from, packet_t *packet)
 *  \param to is a pointer to the called class
 *  \param from is a pointer to the class who invoked
 *  \param packet is a pointer to the received packet
 **/
void tx(call_t *to, call_t *from, packet_t *packet) {

  /* encapsulate packet with layer's header and send to lower layers*/
  if ( forward_to_lower_layers(to, from, packet, DATA_PACKET) == UNSUCCESSFUL){
    goto tx_error;
  }

  return;


  tx_error:

  return;
}


/** \brief The rx input-interface which is automatically called by the WSNet simulation core at the reception of an incoming data packet from the immediately lower-layer simulation module.
 *  \fn void rx(call_t *to, call_t *from, packet_t *packet)
 *  \param to is a pointer to the called class
 *  \param from is a pointer to the class who invoked
 *  \param packet is a pointer to the received packet
 **/
void rx(call_t *to, call_t *from, packet_t *packet) {

  /* get class and node private data*/
  struct _direct_diffusion_routing_node_private *nodedata = get_node_private_data(to);
  struct _direct_diffusion_routing_class_private *classdata = get_class_private_data(to);

  /* extract the aodv header*/
  struct _direct_diffusion_routing_packet_header *header = (struct _direct_diffusion_routing_packet_header *) packet_retrieve_field_value_ptr(packet, "_direct_diffusion_routing_packet_header");

  /* update local stats */
  nodedata->rx_nbr[header->packet_type]++;

  /* update the global stats */
  classdata->current_rx_control_packet ++;
  classdata->current_rx_control_packet_bytes += (packet->real_size/8);

  /* By default anchor nodes does not process any received packet */
  /* and if ttl is less than 1, packet is dropped*/
  if (nodedata->node_type == ANCHOR_NODE || header->ttl < 1) {
    packet_dealloc(packet);
    return;
  }

  /* process the received packet  */
  switch(header->packet_type) {

    case HELLO_PACKET:

      /* process the HELLO packet*/
      process_hello_packet(to, from, packet);

      break;

    case INTEREST_PACKET:

      /* process the RREP packet*/
      process_interest_packet(to, from, packet);

      break;

    case DATA_PACKET:

      /* process the DATA packet*/
      process_data_packet(to, from, packet);

      break;

    default :
      /* Drop the packet, in case it does not know its type */
      packet_dealloc(packet);

      break;
  }

  return;
}


/* ************************************************** */
/* ************************************************** */
routing_methods_t methods = {rx, 
    tx,
    set_header,
    get_header_size,
    get_header_real_size};
