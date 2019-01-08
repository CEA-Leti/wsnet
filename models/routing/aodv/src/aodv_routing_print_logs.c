/**
 *  \file   aodv_routing_print_logs.c
 *  \brief  AODV based routing approach: Print Logs Source Code File
 *  \author Luiz Henrique Suraty Filho (CEA-LETI)
 *  \date   May 2013
 *  \version 1.0
**/

#include <kernel/include/modelutils.h>
#include "aodv_routing_print_logs.h"
#include "aodv_routing_types.h"

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
  
  char *packet_types[] = {"HELLO_PACKET", "DATA_PACKET", "RREQ_PACKET", "RREP_PACKET"};
  
  char *color[] = {KNRM, KRED, KMAG, KGRN};
  
  char *message1[] = {"", "SRC", "SRC", "DST"};
  
  char *message2[] = {"", "DST", "DST", "SRC"};
  
  /* extract packet header */
  struct _aodv_routing_packet_header *header = (struct _aodv_routing_packet_header *) packet_retrieve_field_value_ptr(packet, "_aodv_routing_packet_header");

  struct _aodv_routing_packet_fixed_header *fixed_header = (struct _aodv_routing_packet_fixed_header *) packet_retrieve_field_value_ptr(packet, "_aodv_routing_packet_fixed_header");

  fprintf(stderr,"%s[AODV_%s_TX] Time=%lfs Node %d has sent a %s originated in %s %d (seq = %d) towards %s %d (seq = %d)  via nexthop %d (type = %s, ttl=%d, seq = %d) %s\n", color[packet_type],packet_types[packet_type], get_time()*0.000000001, to->object, packet_types[packet_type],message1[packet_type],fixed_header->origin, fixed_header->origin_seq, message2[packet_type], fixed_header->final_dst, fixed_header->final_dst_seq, header->dst, packet_types[packet_type], header->ttl, header->seq, KNRM);
   
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
    
  char *packet_types[] = {"HELLO_PACKET", "DATA_PACKET", "RREQ_PACKET", "RREP_PACKET"};

  char *color[] = {KNRM, KBLU, KCYN, KYEL};
  
  char *message_final[] = {"", "forwarding to upper layer", "arrived to DST, send RREP", "table updated"};
  
  char *message1[] = {"", "DST", "DST", "SRC"};
  
  char *message2[] = {"", "from SRC", "from SRC", "to reach DST"};

  /* extract packet header */
  struct _aodv_routing_packet_header *header = (struct _aodv_routing_packet_header *) packet_retrieve_field_value_ptr(packet, "_aodv_routing_packet_header");
  struct _aodv_routing_packet_fixed_header *fixed_header = (struct _aodv_routing_packet_fixed_header *) packet_retrieve_field_value_ptr(packet, "_aodv_routing_packet_fixed_header");
  
  int hops = header->ttl_max - header->ttl + fixed_header->hop_to_dst;
  
  fprintf(stderr,"%s[AODV_%s_RX] Time=%lfs %s %d (seq = %d) has received a %s %s %d (seq = %d) (via node %d, ttl=%d, hops=%d ,seq = %d) => %s %s\n", color[packet_type],packet_types[packet_type], get_time()*0.000000001, message1[packet_type], to->object, fixed_header->final_dst_seq, packet_types[packet_type], message2[packet_type], fixed_header->origin, fixed_header->origin_seq, header->src, header->ttl, hops, header->seq, message_final[packet_type], KNRM);
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
    
  char *packet_types[] = {"HELLO_PACKET", "DATA_PACKET", "RREQ_PACKET", "RREP_PACKET"};

  char *color[] = {KNRM, KNRM, KNRM, KNRM};
    
  /* extract packet header */
  struct _aodv_routing_packet_header *header = (struct _aodv_routing_packet_header *) packet_retrieve_field_value_ptr(packet, "_aodv_routing_packet_header");
  struct _aodv_routing_packet_fixed_header *fixed_header = (struct _aodv_routing_packet_fixed_header *) packet_retrieve_field_value_ptr(packet, "_aodv_routing_packet_fixed_header");

  fprintf(stderr,"%s[AODV_%s_FWD] Time=%lfs Node %d has received a %s from SRC %d towards DST %d (via node %d, ttl=%d, seq = %d) => forwarding to next hop %s\n", color[packet_type],packet_types[packet_type], get_time()*0.000000001, to->object, packet_types[packet_type], fixed_header->origin, fixed_header->final_dst,  header->src, header->ttl,  header->seq, KNRM);
}


/** \brief Print log related when a path to a certain node is already know.
 *  \fn void print_log_known(call_t *to, int packet_type, packet_t *packet) 
 *  \param to is a pointer to the called class
 *  \param packet is a pointer to the packet to be forwarded
 *  \param packet_type indicates which type of packet
 *  \return nothing
 **/
void print_log_known(call_t *to, int packet_type, packet_t *packet){
    
  if ( print_log_check(to, packet_type, RX_STATUS) == UNSUCCESSFUL ){
    return;
  }
    
  char *packet_types[] = {"HELLO_PACKET", "DATA_PACKET", "RREQ_PACKET", "RREP_PACKET"};

  char *color[] = {KNRM, KBLU, KCYN, KYEL};
    
  /* extract packet header */
  struct _aodv_routing_packet_header *header = (struct _aodv_routing_packet_header *) packet_retrieve_field_value_ptr(packet, "_aodv_routing_packet_header");  struct _aodv_routing_packet_fixed_header *fixed_header = (struct _aodv_routing_packet_fixed_header *) packet_retrieve_field_value_ptr(packet, "_aodv_routing_packet_fixed_header");
  
  fprintf(stderr,"%s[AODV_%s_RX] Time=%lfs Node %d has received a %s from SRC %d towards DST %d (via node %d, ttl=%d, seq = %d) => path already known, send a RREP_PACKET %s\n", color[packet_type],packet_types[packet_type], get_time()*0.000000001, to->object, packet_types[packet_type], fixed_header->origin, fixed_header->final_dst,  header->src, header->ttl,  header->seq, KNRM);

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
    struct _aodv_routing_node_private *nodedata = get_node_private_data(to);

    fprintf(stderr,"[AODV_STATS] Node %d -> Path establishment = %f -> Packets dropped by buffer = %d -> DATA_PACKET_TX = %d || RREQ_PACKET_TX = %d || RREP_PACKET_TX = %d \n", to->object, nodedata->path_establishment_delay, nodedata->packets_dropped_buffer, nodedata->tx_nbr[1], nodedata->tx_nbr[2], nodedata->tx_nbr[3]);
    print_aodv_route(to,from);

}


 /** \brief Function to list the routing table on the standard output (AODV)
 *  \fn     void print_aodv_route(call_t *to, call_t *from)
 *  \param to is a pointer to the called class
 *  \param from is a pointer to the class who invoked
 **/
void print_aodv_route(call_t *to, call_t *from) {
    
    char *packet_types[] = {"HELLO_PACKET", "DATA_PACKET", "RREQ_PACKET", "RREP_PACKET"};
    
    char *color[] = {KNRM, KRED, KGRN, KYEL};
      
    /* get class and node private data*/
    struct _aodv_routing_node_private *nodedata = get_node_private_data(to);
    struct _aodv_routing_route *route = NULL;
    
    list_init_traverse(nodedata->routing_table);
    while((route = (struct _aodv_routing_route *) list_traverse(nodedata->routing_table)) != NULL) {
        fprintf(stderr,"%s   => Route to Dst = %2d  is nexthop = %2d with hop_nbr = %2d RSSI = %f update_time = %lf by a %s seq = %3d prior_life_time = %lf %s \n", color[route->packet_type], route->dst, route->nexthop_id, route->hop_to_dst, route->rssi, route->received_time*0.000000001, packet_types[route->packet_type], route->seq , route->prior_link_life_time*0.000000001 , KNRM);
    }

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
    struct _aodv_routing_node_private *nodedata = get_node_private_data(to);
  
    unsigned int status = 0x0000;
    
    int i;
    
    if (packet_type == 3){
        packet_type = 4;
    }
    
    for (i=0;i<4;i++){
        status = status | (packet_type << i*4);
    }
    
    /* verify whether flags for this type of packet and log are set */
    if (nodedata->log_status & status & tx_status){
        return SUCCESSFUL;
    }
    else{
        return UNSUCCESSFUL;
    }
    
}
