/**
 *  \file   aodv_routing_functions.c
 *  \brief  AODV based routing approach: Auxiliary Functions Source Code File
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

/* ************************************************** */
/* ************************************************** */

/** \brief Set default values of the node.
 *  \fn void set_node_default_values(struct _aodv_routing_node_private *nodedata)
 *  \param nodedata is a pointer to the node data
 *  \return nothing
**/
void set_node_default_values(struct _aodv_routing_node_private *nodedata){
    int i = 0;

    /* set the default values for global parameters */
    nodedata->node_type = SENSOR_NODE;
    nodedata->routing_table = list_create();
    nodedata->packet_table = list_create();
    nodedata->path_establishment_delay = -1;
    nodedata->sink_id = -1;
    nodedata->seq = get_random_integer_range(0,200);

    for (i=0; i<5; i++) {
        nodedata->rx_nbr[i] = 0;
        nodedata->tx_nbr[i] = 0;
    }
   
    /* set the default values for the AODV protocol */
    nodedata->rreq_status = STATUS_OFF;        
    nodedata->rreq_nbr = -1;                                         
    nodedata->rreq_start = 0;                                 
    nodedata->rreq_period = 10000000000ull; /* 10s */
    nodedata->rreq_propagation_probability = 1.0;
    nodedata->rreq_propagation_backoff = 1000000000ull; /* 1s */
    nodedata->rrep_propagation_backoff = 200000000ull; /* 200ms */
    nodedata->first_rreq_startup_time = 0;
    nodedata->rreq_seq = 0;
    nodedata->ttl = 10; //255;
    nodedata->rreq_data_type = -1;

    nodedata->mindBm = -90;
    nodedata->mindBm_difference = 0;

    nodedata->route_clean_status = STATUS_OFF;
    nodedata->route_clean_period = 10000000000ull; /* 10s */
	nodedata->route_expiration= 2*nodedata->route_clean_period; /* 20s */
    nodedata->log_status = 0x0000;
    nodedata->packets_dropped_buffer=0;

	nodedata->rrep_optimization=0;
	
    return;
}


/** \brief Read parameters from the xml file.
 *  \fn int read_xml_file(struct _aodv_routing_node_private *nodedata, void *params)
 *  \param nodedata is a pointer to the node data
 *  \param params is a pointer to the parameters read from the XML configuration file
 *  \return SUCCESSFUL if success, UNSUCCESSFUL otherwise
**/
int read_nodedata_from_xml_file(struct _aodv_routing_node_private *nodedata, void *params){

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

    /* reading the parameter related to the node type from the xml file */
    if (!strcmp(param->key, "sink_id")) {
        if (get_param_integer(param->value, &(nodedata->sink_id))) {
        return UNSUCCESSFUL;
        }
    }

    /* reading the parameter related to the RREQ protocol from the xml file */
    if (!strcmp(param->key, "rreq_status")) {
        if (get_param_integer(param->value, &(nodedata->rreq_status))) {
        return UNSUCCESSFUL;
        }
    }
    if (!strcmp(param->key, "rreq_nbr")) {
        if (get_param_integer(param->value, &(nodedata->rreq_nbr))) {
        return UNSUCCESSFUL;
        }
    }
    if (!strcmp(param->key, "rreq_start")) {
        if (get_param_time(param->value, &(nodedata->rreq_start))) {
        return UNSUCCESSFUL;
        }
    }
    if (!strcmp(param->key, "rreq_period")) {
        if (get_param_time(param->value, &(nodedata->rreq_period))) {
        return UNSUCCESSFUL;
        }
    }

    /* reading the parameter related to the route clean from the xml file */
    if (!strcmp(param->key, "route_clean_status")) {
        if (get_param_integer(param->value, &(nodedata->route_clean_status))) {
            return UNSUCCESSFUL;
        }
    }
    if (!strcmp(param->key, "route_clean_period")) {
        if (get_param_time(param->value, &(nodedata->route_clean_period))) {
            return UNSUCCESSFUL;
        }
    }
    if (!strcmp(param->key, "rrep_optimization")) {
        if (get_param_integer(param->value, &(nodedata->rrep_optimization))) {
            return UNSUCCESSFUL;
        }
    }

    /* reading the parameter related to the node type from the xml file */
    if (!strcmp(param->key, "log_status")) {
        nodedata->log_status = strtoul(param->value,NULL,16);
    }

    if (!strcmp(param->key, "mindBm_difference")) {
        if (get_param_double(param->value, &(nodedata->mindBm_difference))) {
            return UNSUCCESSFUL;
        }
    }

    }

	nodedata->route_expiration= 2*nodedata->route_clean_period;
	
    return SUCCESSFUL;
}

/** \brief Process a RREQ packet received.
 *  \fn process_rreq_packet(call_t *to, call_t *from, packet_t *packet)
 *  \param to is a pointer to the called class
 *  \param from is a pointer to the class who invoked
 *  \param packet is a pointer to the packet to be processed
 *  \return nothing
**/
void process_rreq_packet(call_t *to, call_t *from, packet_t *packet){
    
   
    struct _aodv_routing_node_private *nodedata = get_node_private_data(to);
	   
    /* extract packet header */
    //struct _aodv_routing_packet_header *header = (struct _aodv_routing_packet_header *) packet_retrieve_field_value_ptr(packet, "_aodv_routing_packet_header");
    struct _aodv_routing_packet_fixed_header *fixed_header = (struct _aodv_routing_packet_fixed_header *) packet_retrieve_field_value_ptr(packet, "_aodv_routing_packet_fixed_header");
    
    /* check if packet have been already treated, i.e., there is an entry in the packet table*/
    if ((packet_table_lookup(to, from, fixed_header->origin, fixed_header->final_dst, fixed_header->packet_type, fixed_header->origin_seq)) == SUCCESSFUL 
            || (packet_table_lookup(to, from, fixed_header->final_dst, fixed_header->origin, RREP_PACKET, fixed_header->final_dst_seq)) ==SUCCESSFUL){

        /*discard RREQ packet*/
        packet_dealloc(packet);

        return;

    }

    /* update routing table according to received RREQ */
	if (route_update_from_packet(to, from, packet) == UNSUCCESSFUL){
	    /*discard RREQ packet*/
        packet_dealloc(packet);
		return;
	}


    /* update RREQ table to avoid propagating a same RREQ */
    packet_table_update(to, from, fixed_header->origin, fixed_header->final_dst, fixed_header->packet_type, fixed_header->origin_seq);
    
    /* check if RREQ arrived to destination */
    if (fixed_header->final_dst == to->object) {
        
        /* print logs */
        print_log_rx(to,RREQ_PACKET, packet);
            
        /* send a RREP to the SRC and discard RREQ packet*/
        rrep_transmission(to, from, packet);
        
        return;
    }
   
 if (nodedata->rrep_optimization)
   {
    struct _aodv_routing_route* route = NULL;
    /* send a RREP if a path to the DST is already known */
    if ( (route = route_get_nexthop_to_destination(to, from, fixed_header->final_dst)) != NULL) {

        /* if table has fresher information than requested */
        if (route->seq >= fixed_header->final_dst_seq){

            /* print log*/
            print_log_known(to,RREQ_PACKET,packet);

            /* send a RREP to the SRC */
            rrep_transmission_from_relay(to,from,packet);

            return;
        }

    }
    
	}
	
    /* In case the DST is unknown, or the path is not fresh, propagates the RREQ */
    /* print logs */
    print_log_fwd(to, RREQ_PACKET, packet);
    
    /* propagate the RREQ */
    /* schedules it in order to give more diversity, i.e. to avoid same path always or the problem of closest node always be the first answering */
    rreq_propagation(to, from, packet);

    return;
     
}

/** \brief Process a RREP packet received.
 *  \fn void process_rrep_packet(call_t *to, call_t *from, packet_t *packet)
 *  \param to is a pointer to the called class
 *  \param from is a pointer to the class who invoked
 *  \param packet is a pointer to the packet to be processed
 *  \return nothing
**/
void process_rrep_packet(call_t *to, call_t *from, packet_t *packet){
    
    struct _aodv_routing_node_private *nodedata = get_node_private_data(to);
    
    /* extract packet header */
    //struct _aodv_routing_packet_header *header = (struct _aodv_routing_packet_header *) packet_retrieve_field_value_ptr(packet, "_aodv_routing_packet_header");
    struct _aodv_routing_packet_fixed_header *fixed_header = (struct _aodv_routing_packet_fixed_header *) packet_retrieve_field_value_ptr(packet, "_aodv_routing_packet_fixed_header");
    


    /* check if packet have been already treated, i.e., there is an entry in packet table*/
    if (packet_table_lookup(to, from, fixed_header->origin, fixed_header->final_dst, fixed_header->packet_type, fixed_header->origin_seq) == SUCCESSFUL){
        /*discard RREQ packet*/
        packet_dealloc(packet);

        return;

    }

    /* update the local sink id */
    //nodedata->sink_id = fixed_header->origin;

    /* update packet table to avoid processing same packet */
    packet_table_update(to, from, fixed_header->origin, fixed_header->final_dst, fixed_header->packet_type, fixed_header->origin_seq);
                
    /* check if RREP packet has arrived to destination */
    if (fixed_header->final_dst == to->object) {
        
        /* check if current packet has less hops than previous or even if there was any previous*/
        if (route_update_from_packet(to, from, packet) == SUCCESSFUL){
            /* print logs only when route is updated*/
            print_log_rx(to,RREP_PACKET, packet);

            /* trigger function to push packet which are on buffer and can be routed to lower layers*/
            buffer_management_trigger_to_dst(to,from,nodedata->buffer, fixed_header->origin);
        }

        /* compute the initial path establishment time */
        if (nodedata->path_establishment_delay == -1) {
            nodedata->path_establishment_delay = (get_time()-nodedata->first_rreq_startup_time) * 0.000001;
            route_update_global_stats(to, from, nodedata->path_establishment_delay);
        }

        /* SHOULD HERE WARN UPPER LAYER THAT RREP IS OK!!! SO SEND DATA PACKETS! Maybe with ioctl*/
                        
        /* Destroy the received packet */
        packet_dealloc(packet);
    }
    /* else forward RREP towards the source node */
    else {
        
		if (route_update_from_packet(to, from, packet) == UNSUCCESSFUL){
        packet_dealloc(packet);
		return;
		}

        /* print log */
        print_log_fwd(to,RREP_PACKET, packet);
        
        /*route packet to final destination*/
        route_forward_packet_to_destination(to, from, packet);
    }
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
   
    // _aodv_routing_node_private *nodedata = get_node_private_data(to);

    /* extract packet header */
    //struct _aodv_routing_packet_header *header = (struct _aodv_routing_packet_header *) packet_retrieve_field_value_ptr(packet, "_aodv_routing_packet_header");
    
     struct _aodv_routing_packet_fixed_header *fixed_header = (struct _aodv_routing_packet_fixed_header *) packet_retrieve_field_value_ptr(packet, "_aodv_routing_packet_fixed_header");

    /* check if packet have been already treated, i.e., there is an entry in packet table*/
    if (packet_table_lookup(to, from, fixed_header->origin, fixed_header->final_dst, fixed_header->packet_type, fixed_header->origin_seq) == SUCCESSFUL){

        /*discard data packet*/
        packet_dealloc(packet);

        return;

    }
    
    /* update routing table according to received packet*/
	if (route_update_from_packet(to, from, packet) == UNSUCCESSFUL){
	    /*discard DATA packet*/
        packet_dealloc(packet);
		return;
	}
        
    /* update packet table to avoid processing same packet */
    packet_table_update(to, from, fixed_header->origin, fixed_header->final_dst, fixed_header->packet_type, fixed_header->origin_seq);

    /* check if data packet has arrived to destination and forwards to upper layers */
    if (fixed_header->final_dst  == to->object) {
        
        /* print logs*/
        print_log_rx(to, DATA_PACKET, packet);
        //array_t *up = get_class_bindings_up(to);
        call_t to0   = {fixed_header->app_id, to->object};
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


/** \brief Encapsulate the packet with layer's header and, in order to do so, verify the existance of a route to the destination
 *  \fn int set_aodv_routing_packet_header(call_t *to, call_t *from, packet_t *packet, int packet_type) 
 *  \param to is a pointer to the called class
 *  \param from is a pointer to the class who invoked
 *  \param packet is a pointer to the packet to be forwarded
 *  \param packet_type indicates which type of packet will be forwarded
 *  \return SUCCESSFUL if success, UNSUCCESSFUL otherwise
 **/
int set_aodv_routing_packet_header(call_t *to, call_t *from, packet_t *packet, int packet_type) {
    struct _aodv_routing_node_private *nodedata = get_node_private_data(to);

    field_t *field = packet_retrieve_field(packet, "_aodv_routing_packet_header");

    /* check if packet header does not exists yet*/
    if ( field == NULL ){
        
        /* create and insert AODV header into the packet */
        int _aodv_routing_packet_header_size = sizeof(struct _aodv_routing_packet_header);
        struct _aodv_routing_packet_header *new_header = malloc(_aodv_routing_packet_header_size);
        field_t *field_aodv_routing_packet_header = field_create(INT, _aodv_routing_packet_header_size, new_header);
        
        _aodv_routing_packet_header_size = 4;
        /* increase header size*/
        packet->size += _aodv_routing_packet_header_size;
        packet->real_size += _aodv_routing_packet_header_size*8;
        
        /* set origin and final destination*/
        new_header->ttl = nodedata->ttl;
        new_header->ttl_max = nodedata->ttl;

        /* add field to the packet*/
        packet_add_field(packet, "_aodv_routing_packet_header", field_aodv_routing_packet_header);

    }

    struct _aodv_routing_packet_header *header = (struct _aodv_routing_packet_header *) packet_retrieve_field_value_ptr(packet, "_aodv_routing_packet_header");
    struct _aodv_routing_packet_fixed_header *fixed_header = (struct _aodv_routing_packet_fixed_header *) packet_retrieve_field_value_ptr(packet, "_aodv_routing_packet_fixed_header");

    /* check for a route to the destination */
    struct _aodv_routing_route *route = NULL;
    if (packet->destination.id != BROADCAST_ADDR) {
        /* if no route, launch discovery procedure(RREQ) and return UNSUCCESSFUL */
        if ( (route = route_get_nexthop_to_destination(to, from, fixed_header->final_dst)) == NULL) {
            rreq_generation(to, from, fixed_header->final_dst);
            return UNSUCCESSFUL;
        }
        header->dst = route->nexthop_id;
    }
    else {
        header->dst = BROADCAST_ADDR;
    }

    /* update packet header values*/
    header->src = to->object;
   // header->sensor_type = nodedata->node_type;
    header->seq = ++(nodedata->seq);
    header->ttl--;

    /* update packet destination for further layer*/
    packet->destination.id  = header->dst;

    return SUCCESSFUL;
}

/** \brief Set the packet with fixed part of the header
 *  \fn int set_aodv_routing_packet_fixed_header(call_t *to, call_t *from, packet_t *packet, int origin, int final_dst, int origin_seq, int hops,int packet_type)
 *  \param to is a pointer to the called class
 *  \param from is a pointer to the class who invoked
 *  \param packet is a pointer to the packet to be forwarded
 *  \param origin indicates the origin of the packet
 *  \param final_dst indicates the final_dst of the packet
 *  \param origin_seq indicates the sequence number of the origin of the packet
 *  \param hops indicates the number of hops (used for RREP packets)
 *  \param packet_type indicates which type of packet will be forwarded
 *  \return SUCCESSFUL if success, UNSUCCESSFUL otherwise
 **/
int set_aodv_routing_packet_fixed_header(call_t *to, call_t *from, packet_t *packet, int origin, int final_dst, int origin_seq, int hops, uint64_t link_life_time, int packet_type){

   // struct _aodv_routing_node_private *nodedata = get_node_private_data(to);

	field_t *fixed_field = packet_retrieve_field(packet, "_aodv_routing_packet_fixed_header");
	struct _aodv_routing_packet_fixed_header *fixed_header;
	
    /* check if packet header does not exists yet*/
    if ( fixed_field == NULL ){
	
	/*  create and insert AODV inner header into the packet */
    int _aodv_routing_packet_fixed_header_size = sizeof(struct _aodv_routing_packet_fixed_header);
    fixed_header = malloc(_aodv_routing_packet_fixed_header_size);
    field_t *field_aodv_routing_packet_fixed_header = field_create(INT, _aodv_routing_packet_fixed_header_size, fixed_header);

    _aodv_routing_packet_fixed_header_size = 4;
    /* increase header size*/
    packet->size += _aodv_routing_packet_fixed_header_size;
    packet->real_size += _aodv_routing_packet_fixed_header_size*8;
	
	/* add field to the packet*/
    packet_add_field(packet, "_aodv_routing_packet_fixed_header", field_aodv_routing_packet_fixed_header);
	}
	else
	{
	//fixed_header = (struct _aodv_routing_packet_fixed_header *) packet_retrieve_field_value_ptr(packet, "_aodv_routing_packet_fixed_header");
	fixed_header = (struct _aodv_routing_packet_fixed_header *) field_getValue(fixed_field);
	}

    /* set origin and final destination*/
    fixed_header->origin = origin;
    fixed_header->final_dst = final_dst;
    fixed_header->origin_seq = origin_seq;

    /* check on the table, the last known seq number for the destination */
    fixed_header->final_dst_seq = route_get_last_final_dst_seq_number(to, final_dst);
    //fixed_header->origin_sensor_type = nodedata->node_type;
    fixed_header->packet_type = packet_type;
    fixed_header->hop_to_dst = hops;
    fixed_header->app_id = from->class;
    fixed_header->link_life_time = link_life_time;



    return SUCCESSFUL;

}

/** \brief Forward packets to lower layers
 *  \fn int forward_to_lower_layers(call_t *to, call_t *from, packet_t *packet, int packet_type)
 *  \param to is a pointer to the called class
 *  \param from is a pointer to the class who invoked
 *  \param packet is a pointer to the packet to be forwarded
 *  \param packet_type indicates which type of packet will be forwarded
 *  \return SUCCESSFUL if success, UNSUCCESSFUL otherwise
 **/
int forward_to_lower_layers(call_t *to, call_t *from, packet_t *packet, int packet_type){

   
    /* get class and node private data*/
    struct _aodv_routing_node_private *nodedata = get_node_private_data(to);
    struct _aodv_routing_class_private *classdata = get_class_private_data(to);

    /* encapsulate packet with layer's header */
    if (set_aodv_routing_packet_header(to, from, packet, packet_type) == UNSUCCESSFUL){

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


