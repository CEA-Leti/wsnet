/**
 *  \file   aodv_routing_route_management.c
 *  \brief  AODV based routing approach: Route Management Source Code File
 *  \author Luiz Henrique Suraty Filho (CEA-LETI)
 *  \date   May 2013
 *  \version 1.0
**/
#include <kernel/include/modelutils.h>
#include "aodv_routing_types.h"
#include "buffer_management.h"
#include "aodv_routing_print_logs.h"
#include "aodv_routing_route_management.h"
#include "aodv_routing_functions.h"


/** \brief Function to forward received packet towards the destination (AODV)
 *  \fn void route_forward_data_packet_to_destination(call_t *to, call_t *from, packet_t *packet)
 *  \param to is a pointer to the called class
 *  \param from is a pointer to the class who invoked
 *  \param packet is a pointer to the received packet
 **/
void route_forward_packet_to_destination(call_t *to, call_t *from, packet_t *packet) {

    //struct _aodv_routing_node_private *nodedata = get_node_private_data(to);

    /* extract the network and rrep headers */
    //struct _aodv_routing_packet_header *header = (struct _aodv_routing_packet_header *) packet_retrieve_field_value_ptr(packet, "_aodv_routing_packet_header");
    
     struct _aodv_routing_packet_fixed_header *fixed_header = (struct _aodv_routing_packet_fixed_header *) packet_retrieve_field_value_ptr(packet, "_aodv_routing_packet_fixed_header");

    /* set destination of packet to the final destination*/
    packet->destination.id = fixed_header->final_dst;

	if (fixed_header->packet_type != DATA_PACKET)
	{
		packet->frame_control_type=1;
	}
    /* encapsulate packet with layer's header and send to lower layers*/
    if ( forward_to_lower_layers(to, from, packet, fixed_header->packet_type) == UNSUCCESSFUL){
        goto route_forward_packet_error;
    }

    return; 

    route_forward_packet_error:
        packet_dealloc(packet);
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
    struct _aodv_routing_node_private *nodedata = get_node_private_data(to);

    /* extract the network and rrep headers */
    struct _aodv_routing_packet_header *header = (struct _aodv_routing_packet_header *) packet_retrieve_field_value_ptr(packet, "_aodv_routing_packet_header");
    
    struct _aodv_routing_packet_fixed_header *fixed_header = (struct _aodv_routing_packet_fixed_header *) packet_retrieve_field_value_ptr(packet, "_aodv_routing_packet_fixed_header");

    /* Current object is the src, therefore, nothing to update*/
    if (to->object == fixed_header->origin){
        return UNSUCCESSFUL;
    }

    struct _aodv_routing_route *route;

    int hops = header->ttl_max - header->ttl + fixed_header->hop_to_dst;

    /* get the sensibility of the radio for the mac layer(call_t from) which has pushed up the current packet*/
    nodedata->mindBm = get_sensitivity_from_radio(to, from) + nodedata->mindBm_difference;
  
    
    if (packet->RSSI < nodedata->mindBm){
        return UNSUCCESSFUL;
    }

    /* check if a nexthop to the dst already exist and update related information */
    list_init_traverse(nodedata->routing_table);
    while((route = (struct _aodv_routing_route *) list_traverse(nodedata->routing_table)) != NULL) {

        /* the final dst is present in routing table */    
        if (route->dst == fixed_header->origin){

            /* update if it has more recent information and it is closer through such node*/
		
            if (fixed_header->origin_seq >= route->seq && (route->hop_to_dst > hops)) {
                route->nexthop_id = header->src;
                route->hop_to_dst = hops;
                route->received_time = get_time();
                route->prior_link_life_time =fixed_header->link_life_time;
                route->seq = fixed_header->origin_seq;
                route->packet_type = fixed_header->packet_type;
                route->rssi = packet->RSSI;

                return SUCCESSFUL;
            }
			//add equal for hops in order to update received time
			if (fixed_header->origin_seq > route->seq && (route->hop_to_dst == hops)) {
                route->nexthop_id = header->src;
                route->hop_to_dst = hops;
                route->received_time = get_time();
                route->prior_link_life_time =fixed_header->link_life_time;
                route->seq = fixed_header->origin_seq;
                route->packet_type = fixed_header->packet_type;
                route->rssi = packet->RSSI;

                return SUCCESSFUL;
            }
			
			
            /*route is up-to-date, therefore, nothing more to be done*/
            return UNSUCCESSFUL;
            
        }
    }
    
    /* in case there is no route on the routing table, update it by inserting the route*/
    route = (struct _aodv_routing_route *) malloc(sizeof(struct _aodv_routing_route));
    route->dst = fixed_header->origin;
    route->nexthop_id = header->src;
    route->received_time = get_time();
    route->prior_link_life_time =fixed_header->link_life_time;
    route->packet_type = fixed_header->packet_type;
    route->seq = fixed_header->origin_seq;
    route->hop_to_dst = hops;
    route->rssi = packet->RSSI;
    list_insert(nodedata->routing_table, (void *) route); 
    
    return SUCCESSFUL;
}


void route_clean(call_t *to, call_t *from){
    
    struct _aodv_routing_node_private *nodedata = get_node_private_data(to);

    struct _aodv_routing_route *route;

    uint64_t total_life_time;

    /* Check for the nexthop towards a particular destination */
    list_init_traverse(nodedata->routing_table);
    while((route = (struct _aodv_routing_route *) list_traverse(nodedata->routing_table)) != NULL) {
        total_life_time = get_time() - route->received_time + route->prior_link_life_time;
        if ( total_life_time > nodedata->route_expiration) {
            list_delete(nodedata->routing_table, route);
        }
    }
}

void route_clean_callback(call_t *to, call_t *from, void *args){

    struct _aodv_routing_node_private *nodedata = get_node_private_data(to);

    route_clean(to,from);

    scheduler_add_callback(get_time() + get_random_double() * nodedata->route_clean_period, to, from, (callback_t) route_clean_callback, NULL);

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


int route_get_last_final_dst_seq_number(call_t *to, nodeid_t dst){

    /* get class and node private data*/
    struct _aodv_routing_node_private *nodedata = get_node_private_data(to);
    struct _aodv_routing_route *route = NULL;
    
    /* check the route table*/
    list_init_traverse(nodedata->routing_table);
    while((route = (struct _aodv_routing_route *) list_traverse(nodedata->routing_table)) != NULL) {
        if (route->dst == dst) {
            /*returns the last received seq number*/
            return route->seq;
        }
    }

    /* returns 0, in case it does not know yet*/
    return 0;
    
}
 
 /** \brief Function to compute the nexthop towards a given destination (AODV)
 *  \fn   struct _aodv_routing_route* route_get_nexthop_to_destination(call_t *to, call_t *from, int dst)
 *  \param to is a pointer to the called class
 *  \param from is a pointer to the class who invoked
 *  \param dst is the ID of the destination
 *  \return NULL if failure, otherwise the destination information of nexthop node
 **/
struct _aodv_routing_route* route_get_nexthop_to_destination(call_t *to, call_t *from, int dst) {

    /* get class and node private data*/
    struct _aodv_routing_node_private *nodedata = get_node_private_data(to);
    struct _aodv_routing_route *route = NULL;

    /* Check for the nexthop towards a particular destination */
    list_init_traverse(nodedata->routing_table);
    while((route = (struct _aodv_routing_route *) list_traverse(nodedata->routing_table)) != NULL) {
        if (route->dst == dst) {
            return route;
        }
    }

    return NULL;
}


/** \brief Function to update the global class stats informations (global path establishment time, etc.)
 *  \fn     void route_update_global_stats(call_t *to, call_t *from)
 *  \param to is a pointer to the called class
 *  \param from is a pointer to the class who invoked
 *  \param delay is the path establishment time
 **/
void route_update_global_stats(call_t *to, call_t *from, double path_delay) {

    /* get class private data*/
    struct _aodv_routing_class_private *classdata = get_class_private_data(to);

    classdata->global_tx_control_packet = classdata->current_tx_control_packet;
    classdata->global_rx_control_packet = classdata->current_rx_control_packet;
    classdata->global_tx_control_packet_bytes = classdata->current_tx_control_packet_bytes;
    classdata->global_rx_control_packet_bytes = classdata->current_rx_control_packet_bytes ;
        
    if (path_delay > classdata->global_establishment_time) {
        classdata->global_establishment_time = path_delay;
    }

}
 
 
