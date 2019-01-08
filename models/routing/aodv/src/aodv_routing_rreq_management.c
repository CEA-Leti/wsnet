 /**
 *  \file   aodv_routing_rreq_management.c
 *  \brief  AODV based routing approach: AODV RREQ/RREP Management Source Code File
 *  \author Luiz Henrique Suraty Filho (CEA-LETI)
 *  \date   May 2013
 *  \version 1.0
**/

#include <kernel/include/modelutils.h>
#include "aodv_routing_types.h"
#include "buffer_management.h"
#include "aodv_routing_print_logs.h"
#include "aodv_routing_route_management.h"
#include "aodv_routing_rreq_management.h"
#include "aodv_routing_functions.h"

//10 seconds
#define TABLE_ENTRY_EXPIRATION 10000000000

/** \brief Generation of a RREQ packet
 *  \fn int rreq_generation(call_t *to, call_t *from, int final_dst)
 *  \param to is a pointer to the called class
 *  \param from is a pointer to the class who invoked
 *  \param final_dst is a the final destination where your RREP packet will be sent
 *  \return SUCCESSFUL if success, UNSUCCESSFUL otherwise
 **/
int rreq_generation(call_t *to, call_t *from, int final_dst) {

    struct _aodv_routing_node_private *nodedata = get_node_private_data(to);
    
    /* create the RREP packet ( size is still 0 at this point ) */
    packet_t *packet = packet_create(to, 0, 0);
    
    /* life time of the link is 0, once you are creating the packet now */
    uint64_t link_life_time = 0;

    /* Number of hops to destination from this node*/
    int hops_to_dst = 0;

    /* Set the fixed part of the packet */
    set_aodv_routing_packet_fixed_header(to, from, packet, to->object, final_dst, ++nodedata->seq , hops_to_dst, link_life_time, RREQ_PACKET);

    /* set final destination of packet to broadcast*/
    packet->destination.id = BROADCAST_ADDR;
	packet->frame_control_type=1;
	
    /* send packet to lower layers*/
    if (forward_to_lower_layers(to, from, packet, RREQ_PACKET) == UNSUCCESSFUL){
        goto rreq_generation_error;
    }

    /* if it is the first generated RREQ message => update the RREQ startup time variable */
    if (nodedata->first_rreq_startup_time == 0) {
        nodedata->first_rreq_startup_time = get_time();
    }
    
    return SUCCESSFUL;

    rreq_generation_error:
        packet_dealloc(packet);
        return UNSUCCESSFUL;
}

/** \brief Callback function for the periodic generation of RREQ packets in AODV (to be used with the scheduler_add_callback function).
 *  \fn int rreq_periodic_generation_callback(call_t *to, call_t *from, void *args)
 *  \param to is a pointer to the called class
 *  \param from is a pointer to the class who invoked
 *  \param args is a pointer to the arguments of the function
 *  \return SUCCESSFUL if success, UNSUCCESSFUL otherwise
 **/
int rreq_periodic_generation_callback(call_t *to, call_t *from, void *args) {

    struct _aodv_routing_node_private *nodedata = get_node_private_data(to);
       
    struct _aodv_routing_route *route;

    /* broadcast a RREQ packet only if destination ID is known and a route to this destination does not exist */
    if (nodedata->sink_id != -1 && (route = route_get_nexthop_to_destination(to, from, nodedata->sink_id)) == NULL) {

        /* generate RREQ packet towards selected destination */
        rreq_generation(to, from, nodedata->sink_id);

    }
    else {
        return UNSUCCESSFUL;        
    }

    /* update nbr of transmitted RREQ packets */
    if (nodedata->rreq_nbr > 0) {
        nodedata->rreq_nbr--;
    }

    /* schedules next RREQ generation */
    if (nodedata->rreq_nbr > 0 || nodedata->rreq_nbr == -1) {
        nodedata->previous_rreq_slot_time += nodedata->rreq_period;
        scheduler_add_callback(nodedata->previous_rreq_slot_time + get_random_double() * nodedata->rreq_period, to, from, rreq_periodic_generation_callback, NULL);
    }

    return SUCCESSFUL;
}

 /** \brief Function to forward RREQ  packet towards the destination (AODV)
 *  \fn int rreq_propagation(call_t *to, call_t *from, void *args)
 *  \param to is a pointer to the called class
 *  \param from is a pointer to the class who invoked
 *  \param args is a pointer to the arguments of the function
 *  \return SUCCESSFUL if success, UNSUCCESSFUL otherwise
**/
int rreq_propagation(call_t *to, call_t *from, packet_t *packet) {

    struct _aodv_routing_node_private *nodedata = get_node_private_data(to);
        
    /* extract aodv packet header from the rreq packet*/
    //struct _aodv_routing_packet_header *header = (struct _aodv_routing_packet_header *) packet_retrieve_field_value_ptr(packet, "_aodv_routing_packet_header");

    /* extract packet fixed header */
    //struct _aodv_routing_packet_fixed_header *fixed_header = (struct _aodv_routing_packet_fixed_header *) packet_retrieve_field_value_ptr(packet, "_aodv_routing_packet_fixed_header");

    /* drop the SRC node RREQ packet according to a given probability */
    if (get_random_double_range(0.0, 1.0) > nodedata->rreq_propagation_probability) {
        packet_dealloc(packet);   
        return UNSUCCESSFUL;
    }

    /* set destination to BROADCAST address*/
    packet->destination.id = BROADCAST_ADDR;
    packet->frame_control_type=1;
	
    /* send packet to lower layers*/
    if (forward_to_lower_layers(to, from, packet, RREQ_PACKET) == UNSUCCESSFUL){
        goto rreq_propagation_error;
    }

    return SUCCESSFUL;

    rreq_propagation_error:
        packet_dealloc(packet);
        return UNSUCCESSFUL;
}


 /** \brief Function for the transmission of RREP packet to the source node (AODV) => to be done by the sink node
 *  \fn int rrep_transmission(call_t *to, call_t *from, packet_t *rreq_packet)
 *  \param to is a pointer to the called class
 *  \param from is a pointer to the class who invoked
 *  \param rreq_packet is a pointer to the received RREQ packet
 *  \return SUCCESSFUL if success, UNSUCCESSFUL otherwise
**/
int rrep_transmission(call_t *to, call_t *from, packet_t *rreq_packet) {

    struct _aodv_routing_node_private *nodedata = get_node_private_data(to);
    //struct _aodv_routing_class_private *classdata = get_class_private_data(to);

    /* extract aodv packet header from the rreq packet*/
    //struct _aodv_routing_packet_header *received_header = (struct _aodv_routing_packet_header *) packet_retrieve_field_value_ptr(rreq_packet, "_aodv_routing_packet_header");

    /* extract packet fixed header */
    struct _aodv_routing_packet_fixed_header *received_fixed_header = (struct _aodv_routing_packet_fixed_header *) packet_retrieve_field_value_ptr(rreq_packet, "_aodv_routing_packet_fixed_header");

    /* create the RREP packet ( size is still 0 at this point ) */
    packet_t *packet = packet_create(to, 0, 0);

    /* life time of the link is 0, once you are creating the packet now */
    uint64_t link_life_time = 0;

    /* Number of hops to destination from this node*/
    int hops_to_dst = 0;

    /* Set the fixed part of the packet */
    set_aodv_routing_packet_fixed_header(to, from, packet, received_fixed_header->final_dst, received_fixed_header->origin, ++nodedata->seq , hops_to_dst , link_life_time, RREP_PACKET);
    
    
    /* update packet destination to the SRC node of the received header (RREQ packet)*/
    packet->destination.id = received_fixed_header->origin;
    packet->frame_control_type=1;
		
    /* send packet to lower layers*/
    if (forward_to_lower_layers(to, from, packet, RREP_PACKET) == UNSUCCESSFUL){
        goto rrep_transmission_error;
    }

    /* dealloc RREQ packet */
    packet_dealloc(rreq_packet);

    return SUCCESSFUL;
    
    rrep_transmission_error:
        packet_dealloc(packet);
        
        /* dealloc RREQ packet */
        packet_dealloc(rreq_packet);

        return UNSUCCESSFUL;
}


 /** \brief Function for the transmission of RREP packet to the source node (AODV) => to be done by intermediate sensor node
 *  \fn int rrep_transmission_from_relay(call_t *to, call_t *from, void *args) 
 *  \param to is a pointer to the called class
 *  \param from is a pointer to the class who invoked
 *  \param args is a pointer to the function argument
 *  \return SUCCESSFUL if success, UNSUCCESSFUL otherwise
**/
int rrep_transmission_from_relay(call_t *to, call_t *from, packet_t *rreq_packet) {

    //struct _aodv_routing_node_private *nodedata = get_node_private_data(to);
  
    /* extract aodv packet header from the rreq packet*/
    //struct _aodv_routing_packet_header *received_header = (struct _aodv_routing_packet_header *) packet_retrieve_field_value_ptr(rreq_packet, "_aodv_routing_packet_header");

    /* extract packet fixed header */
    struct _aodv_routing_packet_fixed_header *received_fixed_header = (struct _aodv_routing_packet_fixed_header *) packet_retrieve_field_value_ptr(rreq_packet, "_aodv_routing_packet_fixed_header");
    
    /*to verify how many hops from relay to destination*/
    struct _aodv_routing_route *route = route_get_nexthop_to_destination(to, from, received_fixed_header->final_dst);

    if (route==NULL){
      return UNSUCCESSFUL;
    }
    
    /* create the RREP packet ( size is still 0 at this point ) */
    packet_t *packet = packet_create(to, 0, 0);
  
    /* calculate link life time, which is equal to (t_transmission - t_reception) + prior_life_time */
    uint64_t link_life_time = (get_time() - route->received_time) + route->prior_link_life_time;

    /* Set the fixed part of the packet */
    set_aodv_routing_packet_fixed_header(to, from, packet, received_fixed_header->final_dst, received_fixed_header->origin, route->seq , route->hop_to_dst, link_life_time, RREP_PACKET);
    
    /* update packet destination to the SRC node of the received header (RREQ packet)*/
    packet->destination.id = received_fixed_header->origin;
	packet->frame_control_type=1;
    
    /* send packet to lower layers*/
    if (forward_to_lower_layers(to, from, packet, RREP_PACKET) == UNSUCCESSFUL){
      packet_dealloc(packet);
      packet_dealloc(rreq_packet);
      return UNSUCCESSFUL;
    }

    /* dealloc RREQ packet */
    packet_dealloc(rreq_packet);

    return SUCCESSFUL;

}


 /** \brief Function to check if packet has already been sent (AODV)
 *  \fn int packet_table_lookup(call_t *to, call_t *from, int origin, int final_dst, int packet_type, int seq)
 *  \param to is a pointer to the called class
 *  \param from is a pointer to the class who invoked
 *  \param origin is the origin ID
 *  \param final_dst is the final destination ID
 *  \param packet_type is the type of the packet (RREQ, RREP, DATA)
 *  \param seq is the sequence number of packet
 *  \return SUCCESSFUL if success, UNSUCCESSFUL otherwise
**/
int packet_table_lookup(call_t *to, call_t *from, int origin, int final_dst, int packet_type, int seq) {
    struct _aodv_routing_node_private *nodedata = get_node_private_data(to);
    struct _aodv_routing_packet_table_entry *table_entry;
    list_init_traverse(nodedata->packet_table);
    while((table_entry = (struct _aodv_routing_packet_table_entry *) list_traverse(nodedata->packet_table)) != NULL) {
        if (table_entry->origin == origin && table_entry->final_dst == final_dst && table_entry->packet_type == packet_type) {

            if (packet_type == RREP_PACKET){
                if (table_entry->seq > seq)
                    return SUCCESSFUL;
            }
            else{
                if (table_entry->seq >= seq)
                    return SUCCESSFUL;
            }

        }
    }
    return UNSUCCESSFUL;
}

 /** \brief Function to update the local packet table to avoid the transmission of duplicate packets (AODV)
  *  \fn void packet_table_update(call_t *to, call_t *from, int origin, int final_dst, int packet_type, int seq)
 *  \param to is a pointer to the called class
 *  \param from is a pointer to the class who invoked
 *  \param origin is the origin ID
 *  \param final_dst is the final destination ID
 *  \param packet_type is the type of the packet (RREQ, RREP, DATA)
 *  \param seq is the sequence number of packet
 *  \return nothing
**/
void packet_table_update(call_t *to, call_t *from, int origin, int final_dst, int packet_type, int seq) {
    struct _aodv_routing_node_private *nodedata = get_node_private_data(to);
    struct _aodv_routing_packet_table_entry *table_entry;
    list_init_traverse(nodedata->packet_table);
	
    while((table_entry = (struct _aodv_routing_packet_table_entry *) list_traverse(nodedata->packet_table)) != NULL) {
	
		//update table entry found 
	    if (table_entry->origin == origin && table_entry->final_dst == final_dst && table_entry->packet_type == packet_type && table_entry->seq == seq) {
            //table_entry->seq = seq;
            table_entry->time = get_time();
            return;
        }
		
	//clean old packet in packet table	
	 if ((get_time() - table_entry->time) > TABLE_ENTRY_EXPIRATION)
	 {
	 list_delete(nodedata->packet_table, table_entry);
	 }
		
    }

    struct _aodv_routing_packet_table_entry *new_table_entry = (struct _aodv_routing_packet_table_entry *) malloc(sizeof(struct _aodv_routing_packet_table_entry));
    new_table_entry->final_dst = final_dst;
    new_table_entry->origin = origin;
    new_table_entry->packet_type = packet_type;
    new_table_entry->seq = seq;
    new_table_entry->time = get_time();
    list_insert(nodedata->packet_table, (void *)new_table_entry);
	
	//if (to->object==0) printf("Node %d : Time %lfs : PACKET_TABLE_UPDATE size %d\n",to->object,get_time()*0.000000001, list_getsize(nodedata->packet_table));
	
}


