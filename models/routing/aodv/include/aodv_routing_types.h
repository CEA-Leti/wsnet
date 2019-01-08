/**
 *  \file   _aodv_routing_types.h
 *  \brief  AODV based routing approach: Common Variables and Types
 *  \author Luiz Henrique Suraty Filho (CEA-LETI)
 *  \date   May 2013
 *  \version 1.0
**/

#ifndef  __aodv_routing_types__
#define __aodv_routing_types__

/* Macro definitions for nodes types .*/
#define SINK_NODE       0
#define SENSOR_NODE     1

 /* Macro definitions for protocol states.*/
#define STATUS_ON   1
#define STATUS_OFF  0

 /* Macro definitions for packet types.*/
#define HELLO_PACKET    0
#define DATA_PACKET     1
#define RREQ_PACKET     2
#define RREP_PACKET     3

/* ************************************************** */
/* ************************************************** */
/** \brief A structure containing a buffer entry
 *  \struct _buffer_entry
 **/
typedef struct _buffer_entry{

    packet_t        *packet;                    /*!< Defines the packet **/
    int             number_retry;               /*!< Defines the number of retries **/
    int       number_backoffs_tried;    /*!< Defines the number of backoff tried **/
    uint64_t        time_insertion;             /*!< Defines the time of insertion on buffer **/
    uint64_t        time_expiration;            /*!< Defines the expiration time for the packet **/
    int             priority;                   /*!< Defines the priority of the packet **/
    int             packet_type;                /*!< Defines the packet type **/
    int             backoff_periods;            /*!< Defines the backoff_periods*/
    int       up_down;          /*!< Defines which cap is being used (up, down or indifferent) */
    int       acked;            /*!< Defines the packet must be acked */
} buffer_entry_t;

/** \brief A structure containing a buffer queue
 *  \struct _buffer_queue
 **/
typedef struct _buffer_queue{

    classid_t       class;                      /*!< Defines the called class id **/
    objectid_t      object;                     /*!< Defines the called object id (node, medium, or environment) **/
    void            *elts;                      /*!< Defines the buffer elements */
    int             max_size;                   /*!< Defines the maximum allowed size of the buffer */
    uint64_t        cleaning_period;            /*!< Defines the period for the cleaning procedure*/

} buffer_queue_t;


/** \brief A structure containing the GLOBAL node entity variables and parameters
 *  \struct _aodv_routing_class_private
 **/
struct _aodv_routing_class_private {
    /* current values */
    int current_tx_control_packet;                      /*!< Defines the current total number of transmitted packets. */
    int current_rx_control_packet;                      /*!< Defines the current total number of received packets. */
    int current_tx_control_packet_bytes;                        /*!< Defines the current total amount of transmitted packets (bytes). */
    int current_rx_control_packet_bytes;                        /*!< Defines the current total amount of received packets (bytes). */
    /* final values */
    double global_establishment_time;                           /*!< Defines the global path establishment time from all the sensors to the sink (ms). */
    int global_tx_control_packet;                       /*!< Defines the total number of transmitted packets. */
    int global_rx_control_packet;                       /*!< Defines the total number of received packets. */
    int global_tx_control_packet_bytes;                         /*!< Defines the total amount of transmitted packets (bytes). */
    int global_rx_control_packet_bytes;                         /*!< Defines the total amount of received packets (bytes). */
    
    list_t *shared;
 };


/** \brief A structure containing the LOCAL node variables and parameters
 *  \struct _aodv_routing_node_private
 **/
struct _aodv_routing_node_private{

    /* Local variables at the node */
    int node_type;                      /*!< Defines the node type. */
    void *routing_table;                    /*!< Defines the local node route table. */
    void *packet_table;                     /*!< Defines the local node RREP packet table. */
    int sink_id;                        /*!< Defines the ID the sink (necessary for AODV). */
   
    /* Parameters of the AODV protocol  */
    int rreq_status;                        /*!< Defines the status of the RREQ based path discovery protocol. */
    int rreq_nbr;                       /*!< Defines the maximal number of RREQ packets to be transmitted. */
    uint64_t rreq_start;                    /*!< Defines the startup time of the RREQ protocol. */
    uint64_t rreq_period;                   /*!< Defines the periodicity of the RREQ protocol. */
    double rreq_propagation_probability;                    /*!< Defines the probability of the RREQ propagation protocol. */
    uint64_t rreq_propagation_backoff;                          /*!< Defines the backoff of the RREQ propagation protocol. */
    uint64_t rrep_propagation_backoff;                          /*!< Defines the probability of the RREP dissemination protocol. */
    int seq;
    int rreq_seq;                       /*!< Defines the sequence number of RREQ packets. */
    int ttl;                       /*!< Defines the TTL number of RREQ packets. */
    int rreq_data_type;                         /*!< Defines the data types requested by RREQ packets. */
    uint64_t previous_rreq_slot_time;


    double mindBm;
    double    mindBm_difference;
    
    

    int route_clean_status;
    uint64_t route_clean_period;
    uint64_t route_expiration;
    buffer_queue_t *buffer;

	int rrep_optimization;
	
    /* Variable for printing*/
    unsigned int log_status;

    /* Local variable for stats */
    int rx_nbr[5];                      /*!< Defines the number of received packets. */
    int tx_nbr[5];                      /*!< Defines the number of transmitted packets. */
    int data_packet_size;                   /*!< Defines the size of data packets. */
    double path_establishment_delay;                        /*!< Defines the path establishment delay (ms). */
    uint64_t first_rreq_startup_time;                       /*!< Defines the startup time of the first RREQ transmission. */
    int packets_dropped_buffer; /*!< Defines number of packets which were dropped because buffer was full*/

};

/** \brief A structure for the route table management (AODV)
 *  *  \struct _aodv_routing_route
 *   **/
struct _aodv_routing_route{
    int dst;                            /*!< dst node ID */
    int nexthop_id;                     /*!< Nexthop node ID */
    int hop_to_dst;                     /*!< Number of hop to the dst */
    uint64_t received_time;         
    uint64_t prior_link_life_time;
    int seq;                            /*!< Sequence number related to the last route update */
    double rssi;
    int packet_type;
};


/** \brief A structure for the packet table entry (AODV)
 *  *  *  \struct _aodv_routing_packet_table_entry
 *   *   **/
struct _aodv_routing_packet_table_entry{
    int origin;                                                    /*!<  src node ID */
    int final_dst;                                                    /*!<  dst node ID */
    int packet_type;                                            /*!<  packet type (hello, interest, RREQ, RREP, DATA) */
    int seq;                                                    /*!<  Sequence number of the last RREQ received */
    uint64_t time;                      /*!< Time related to the last update */
};


/* ************************************************** */
/* ************************************************** */

/** \brief A structure defining the AODV header
 *  \struct _aodv_routing_packet_header
**/
struct _aodv_routing_packet_header{
    nodeid_t src;                       /*!<  current source node ID */
    nodeid_t dst;                       /*!<  current destination node ID */
    int seq;                                                    /*!<  Packet sequence number */
    int ttl;                                                    /*!<  Time-To-Live to limit packet propagation */
    int ttl_max;                                                /*!<  Initial Time-To-Live to limit packet propagation */
};

/** \brief A structure defining the fixed part of the AODV header
 *  \struct _aodv_routing_packet_fixed_header
**/
struct _aodv_routing_packet_fixed_header {
    nodeid_t origin;
    nodeid_t final_dst;
    int origin_seq;
    int final_dst_seq;
    int packet_type;
    int app_id;
    int hop_to_dst;
    uint64_t link_life_time;
};

#endif  //__aodv_routing_types__




