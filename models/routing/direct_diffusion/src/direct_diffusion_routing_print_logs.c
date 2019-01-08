/**
 *  \file   direct_diffusion_routing_print_logs.c
 *  \brief  DD based routing approach: Print Logs Source Code File
 *  \author Luiz Henrique Suraty Filho (CEA-LETI)
 *  \date   May 2013
 *  \version 1.0
 **/

#include <stdio.h>
#include <kernel/include/modelutils.h>
#include "direct_diffusion_routing_print_logs.h"
#include "direct_diffusion_routing_route_management.h"
#include "direct_diffusion_routing_types.h"


/** \brief Print log related when a packet is transmitted.
 *  \fn void print_log_tx(call_t *to, int packet_type, packet_t *packet) 
 *  \param to is a pointer to the called class
 *  \param packet is a pointer to the packet to be forwarded
 *  \param packet_type indicates which type of packet
 *  \return nothing
 **/
void print_log_tx(call_t *to, int packet_type, packet_t *packet){


  if ( print_log_check(to, packet_type, TX_STATUS) == UNSUCCESSFUL ){
    return;
  }

  char *packet_types[] = {"HELLO_PACKET", "DATA_PACKET", "INTEREST_PACKET"};

  char *color[] = {KNRM, KRED, KMAG};

  char *message1[] = {"", "SRC", "DST"};

  char *message2[] = {"", "DST", "SRC"};

  /* extract packet header */
  struct _direct_diffusion_routing_packet_header *header = (struct _direct_diffusion_routing_packet_header *) packet_retrieve_field_value_ptr(packet, "_direct_diffusion_routing_packet_header");

  fprintf(stderr,"%s[DD_%s_TX] Time=%lfs Node %d has sent a %s originated in %s %d towards %s %d via nexthop %d (type = %s, ttl=%d, seq = %d) %s\n", color[packet_type],packet_types[packet_type], get_time()*0.000000001, to->object, packet_types[packet_type],message1[packet_type],header->origin, message2[packet_type], header->final_dst, header->dst, packet_types[packet_type], header->ttl, header->seq, KNRM);

}

/** \brief Print log related when a packet is received.
 *  \fn void print_log_rx(call_t *to, int packet_type, packet_t *packet) 
 *  \param to is a pointer to the called class
 *  \param packet is a pointer to the packet to be forwarded
 *  \param packet_type indicates which type of packet
 *  \return nothing
 **/
void print_log_rx(call_t *to, int packet_type, packet_t *packet){

  if ( print_log_check(to, packet_type, RX_STATUS) == UNSUCCESSFUL ){
    return;
  }

  char *packet_types[] = {"HELLO_PACKET", "DATA_PACKET", "INTEREST_PACKET"};

  char *color[] = {KNRM, KBLU, KCYN};

  char *message_final[] = {"", "forwarding to upper layer", "forward and try update route table"};

  char *message1[] = {"", "DST", "SRC"};

  char *message2[] = {"", "from SRC", "to reach DST"};

  /* extract packet header */
  struct _direct_diffusion_routing_packet_header *header = (struct _direct_diffusion_routing_packet_header *) packet_retrieve_field_value_ptr(packet, "_direct_diffusion_routing_packet_header");

  int hops = header->ttl_max - header->ttl;

  fprintf(stderr,"%s[DD_%s_RX] Time=%lfs %s %d has received a %s %s %d (via node %d, ttl=%d, hops=%d ,seq = %d) => %s %s\n", color[packet_type],packet_types[packet_type], get_time()*0.000000001, message1[packet_type], to->object, packet_types[packet_type], message2[packet_type], header->origin,  header->src, header->ttl, hops, header->seq, message_final[packet_type], KNRM);
}


/** \brief Print log related when a packet will be forwarded.
 *  \fn void print_log_fwd(call_t *to, int packet_type, packet_t *packet) 
 *  \param to is a pointer to the called class
 *  \param packet is a pointer to the packet to be forwarded
 *  \param packet_type indicates which type of packet
 *  \return nothing
 **/
void print_log_fwd(call_t *to, int packet_type, packet_t *packet){

  if ( print_log_check(to, packet_type, FWD_STATUS) == UNSUCCESSFUL ){
    return;
  }

  char *packet_types[] = {"HELLO_PACKET", "DATA_PACKET", "INTEREST_PACKET"};

  char *color[] = {KYEL, KYEL, KYEL};

  /* extract packet header */
  struct _direct_diffusion_routing_packet_header *header = (struct _direct_diffusion_routing_packet_header *) packet_retrieve_field_value_ptr(packet, "_direct_diffusion_routing_packet_header");

  fprintf(stderr,"%s[DD_%s_FWD] Time=%lfs Node %d has received a %s from SRC %d towards DST %d (via node %d, ttl=%d, seq = %d) => forwarding to next hop %s\n", color[packet_type],packet_types[packet_type], get_time()*0.000000001, to->object, packet_types[packet_type], header->origin, header->final_dst,  header->src, header->ttl,  header->seq, KNRM);
}

/** \brief Print log related to the final stats.
 *  \fn void print_log_stats(call_t *to, call_t *from) 
 *  \param to is a pointer to the called class
 *  \param from is a pointer to the class who invoked
 *  \return nothing
 **/
void print_log_stats(call_t *to, call_t *from){

  if ( print_log_check(to, 15 , STATS_STATUS) == UNSUCCESSFUL ){
    return;
  }

  /* get node private data*/
  struct _direct_diffusion_routing_node_private *nodedata = get_node_private_data(to);

  fprintf(stderr,"[DD_STATS] Node %d -> Path establishment = %f\n", to->object, nodedata->path_establishment_delay);
  route_direct_diffusion_show(to,from);

}

/** \brief Verify whether the log should be printed or not, according to the log_status parameter on the XML file
 *  \fn int print_log_check(call_t *to, int packet_type, unsigned int tx_status)
 *  \param to is a pointer to the called class
 *  \param packet is a pointer to the packet to be forwarded
 *  \param packet_type indicates which type of packet
 *  \return SUCCESSFUL if success, UNSUCCESSFUL otherwise
 **/
int print_log_check(call_t *to, int packet_type, unsigned int tx_status){

  /* get class and node private data*/
  struct _direct_diffusion_routing_node_private *nodedata = get_node_private_data(to);

  unsigned int status = 0x0000;

  int i;

  for (i=0;i<4;i++){
    status = status | (packet_type << i*4);
  }


  if (nodedata->log_status & status & tx_status){
    return SUCCESSFUL;
  }
  else{
    return UNSUCCESSFUL;
  }

}
