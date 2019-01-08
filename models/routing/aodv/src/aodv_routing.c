/**
 *  \file   aodv_routing.c
 *  \brief  AODV based routing approach: Main
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


 /** \brief A structure to define the simulation module information
 *  \struct model_t
 **/
model_t model =  {
  "AODV based routing approach",                  /*!< The simulation module name. */
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
    struct _aodv_routing_class_private *classdata = malloc(sizeof(struct _aodv_routing_class_private));

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
    struct _aodv_routing_class_private *classdata = get_class_private_data(to);

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
    
    /* initialize class private data*/
    //struct _aodv_routing_class_private *classdata = malloc(sizeof(struct _aodv_routing_class_private));

    /* initialize node private data*/
    struct _aodv_routing_node_private *nodedata = malloc(sizeof(struct _aodv_routing_node_private));
    
    /* Set default values for node private data*/
    set_node_default_values(nodedata);
    
    /* Read parameters from xml file*/
    if (read_nodedata_from_xml_file(nodedata, params)==UNSUCCESSFUL){
        free(nodedata);
        return UNSUCCESSFUL;
    }
    
    /* Check the validity of the input parameters */
    if ((nodedata->node_type != SENSOR_NODE) && (nodedata->node_type != SINK_NODE)) {
        nodedata->node_type = SENSOR_NODE;
    }


    /* Assign the initialized local variables to the node */
    set_node_private_data(to, nodedata);

    /* Create buffer */
    nodedata->buffer = buffer_managemet_create_queue(to, 5, -1);
    
    return SUCCESSFUL;

}


/** \brief The unbind input-interface to deallocate/free the LOCAL node variables at the end of simulation.
 *  \fn int unbind(call_t *to, call_t *from)
 *  \param to is a pointer to the called class
 *  \param from is a pointer to the class who invoked
 *  \return SUCCESSFUL if success, UNSUCCESSFUL otherwise
 **/
int unbind(call_t *to, call_t *from) {

    /* initialize class private data*/
    //struct _aodv_routing_class_private *classdata = malloc(sizeof(struct _aodv_routing_class_private));

    /* get node private data*/
    struct _aodv_routing_node_private *nodedata = get_node_private_data(to);

    print_log_stats(to, from);

    //buffer_management_print_queue(nodedata->buffer);
    
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
    struct _aodv_routing_node_private *nodedata = get_node_private_data(to);

    /* start the periodic RREQ generation */
    if (nodedata->rreq_status == STATUS_ON && (nodedata->rreq_nbr > 0 || nodedata->rreq_nbr == -1)) {
        uint64_t rreq_slot_time = get_time() + nodedata->rreq_start;
        uint64_t rreq_tx_time = rreq_slot_time + get_random_double() * nodedata->rreq_period;
        call_t from = {-1,-1};
        nodedata->previous_rreq_slot_time = rreq_slot_time;
        scheduler_add_callback(rreq_tx_time, to, &from, rreq_periodic_generation_callback, NULL);
    }

    if (nodedata->route_clean_status == STATUS_ON){
        call_t from = {-1,-1};
        scheduler_add_callback( get_time() + get_random_double() * nodedata->route_clean_period , to, &from, (callback_t) route_clean_callback, NULL);
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
    return sizeof(struct _aodv_routing_packet_header);
}


/** \brief The getheader_realsize input-interface which is automatically called by the WSNet simulation core when a higher-layer simulation module call the GET_HEADER_REAL_SIZE output-interface to determine the total real size of a packet.
 *  \fn int get_header_real_size(call_t *to, call_t *from)
 *  \param to is a pointer to the called class
 *  \param from is a pointer to the class who invoked
 *  \return the real size of all protocol headers (in bytes)
 **/
int get_header_real_size(call_t *to, call_t *from) {
    return sizeof(struct _aodv_routing_packet_header)*8;
}


/** \brief The tx input-interface which is automatically called by the WSNet simulation core when the immediately higher-level simulation module sends a packet.
 *  \fn void tx(call_t *to, call_t *from, packet_t *packet)
 *  \param to is a pointer to the called class
 *  \param from is a pointer to the class who invoked
 *  \param packet is a pointer to the received packet
 **/
void tx(call_t *to, call_t *from, packet_t *packet) {

    /* get node private data*/
    struct _aodv_routing_node_private *nodedata = get_node_private_data(to);

    /* Number of hops to destination from this node*/
    int hops_to_dst = 0;

    /* life time of the link is 0, once you are creating the packet now */
    uint64_t link_life_time = 0;

    /* set the fixed part of the AODV header*/
    set_aodv_routing_packet_fixed_header(to, from, packet, to->object, packet->destination.id, ++nodedata->seq, hops_to_dst, link_life_time, DATA_PACKET);

    /* encapsulate packet with layer's header and send to lower layers*/
    if ( forward_to_lower_layers(to, from, packet, DATA_PACKET) == UNSUCCESSFUL){
        goto tx_error;
    }

    return;


    tx_error:
        if (buffer_management_insert_packet_on_queue(to, from, nodedata->buffer, packet, 100000000000,DATA_PACKET) == UNSUCCESSFUL){
            nodedata->packets_dropped_buffer++; 
            packet_dealloc(packet);
        }
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
    struct _aodv_routing_node_private *nodedata = get_node_private_data(to);
    struct _aodv_routing_class_private *classdata = get_class_private_data(to);
    
    /* extract the aodv header*/
    struct _aodv_routing_packet_header *header = (struct _aodv_routing_packet_header *) packet_retrieve_field_value_ptr(packet, "_aodv_routing_packet_header");
    struct _aodv_routing_packet_fixed_header *fixed_header = (struct _aodv_routing_packet_fixed_header *) packet_retrieve_field_value_ptr(packet, "_aodv_routing_packet_fixed_header");

    /* update local stats */
    nodedata->rx_nbr[fixed_header->packet_type]++;

    /* update the global stats */
    classdata->current_rx_control_packet ++;
    classdata->current_rx_control_packet_bytes += (packet->real_size/8);

    /* if ttl is less than 1, packet is dropped*/
    /* or if the packet have been originated on the node*/
    if (header->ttl < 1 || fixed_header->origin == to->object) {
        packet_dealloc(packet);
        return;
    }

    /* process the received packet  */
    switch(fixed_header->packet_type) {

        case HELLO_PACKET:        
            
            /* process the HELLO packet*/
            process_hello_packet(to, from, packet);
            
        break;

        case RREQ_PACKET: 
           
            /* process the RREQ packet*/
            process_rreq_packet(to, from, packet);
            
        break;
                        
        case RREP_PACKET: 
            
            /* process the RREP packet*/
            process_rrep_packet(to, from, packet);

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
