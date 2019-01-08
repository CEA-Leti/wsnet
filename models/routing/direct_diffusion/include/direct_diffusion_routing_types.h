/**
 *  \file   _direct_diffusion_routing_types.h
 *  \brief  Direct Diffusion based routing approach: Common Variables and Types
 *  \author Luiz Henrique Suraty Filho (CEA-LETI)
 *  \date   May 2013
 *  \version 1.0
**/

#ifndef  __direct_diffusion_routing_types__
#define __direct_diffusion_routing_types__

/* Macro definitions for nodes types .*/
#define SINK_NODE       0
#define SENSOR_NODE     1
#define ANCHOR_NODE     2

 /* Macro definitions for protocol states.*/
#define STATUS_ON   1
#define STATUS_OFF  0

 /* Macro definitions for packet types.*/
#define HELLO_PACKET    0
#define DATA_PACKET     1
#define INTEREST_PACKET 2

#define UNSUCCESSFUL   -1
#define SUCCESSFUL      0


/* ************************************************** */
/* ************************************************** */
/** \brief A structure containing the GLOBAL node entity variables and parameters
 *  \struct _direct_diffusion_routing_class_private
 **/
struct _direct_diffusion_routing_class_private {
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
 };


/** \brief A structure containing the LOCAL node variables and parameters
 *  \struct _direct_diffusion_routing_node_private
 **/
struct _direct_diffusion_routing_node_private{

    /* Local variables at the node */
    int node_type;                      /*!< Defines the node type. */
    position_t node_position;                       /*!< Defines the node position. */
    void *routing_table;                    /*!< Defines the local node route table. */
    void *packet_table;                                         /*!< Defines the local node RREP packet table. */
    int sink_id;                        /*!< Defines the ID the sink (necessary for Direct Diffusion). */
   
    /* Parameters of the Direct Diffusion protocol  */
    int interest_status;                                   /*!< Defines the status of the interest dissemination protocol. */
    double interest_propagation_probability;     /*!< Defines the probability of the interest dissemination protocol. */
    uint64_t interest_propagation_backoff;       /*!< Defines the backoff of the interest dissemination protocol. */
    int interest_nbr;                                       /*!< Defines the maximal number of interest packets to be transmitted. */
    int seq;                                      /*!< Defines the sequence of the interest packets. */
    uint64_t interest_start;                             /*!< Defines the startup time of the interest dissemination protocol. */
    uint64_t interest_period;                           /*!< Defines the periodicity of the interest dissemination protocol. */
    int ttl;                                         /*!< Defines the TTL for the dissemination of interest packets. */
    int interest_data_type;                              /*!< Defines the requested data types of the interest packets. */
    uint64_t previous_interest_slot_time;

    double mindBm;
    
    /* Variable for printing*/
    unsigned int log_status;

    /* Local variable for stats */
    int rx_nbr[5];                      /*!< Defines the number of received packets. */
    int tx_nbr[5];                      /*!< Defines the number of transmitted packets. */
    int data_packet_size;                   /*!< Defines the size of data packets. */
    double path_establishment_delay;                        /*!< Defines the path establishment delay (ms). */
    uint64_t first_rreq_startup_time;                       /*!< Defines the startup time of the first RREQ transmission. */
};

/** \brief A structure for the route table management (Direct Diffusion)
 *  *  \struct _direct_diffusion_routing_route
 *   **/
struct _direct_diffusion_routing_route{
    int dst;                            /*!< dst node ID */
    int nexthop_id;                     /*!< Nexthop node ID */
    int nexthop_lqe;                        /*!< Nexthop node LQE */
    int hop_to_dst;                     /*!< Number of hop to the dst */
    uint64_t time;                      /*!< Time related to the last route update */
    int seq;                            /*!< Sequence number related to the last route update */
};


/** \brief A structure for the packet table entry (Direct Diffusion)
 *  *  *  \struct _direct_diffusion_routing_packet_table_entry
 *   *   **/
struct _direct_diffusion_routing_packet_table_entry{
    int origin;                                                    /*!<  src node ID */
    int final_dst;                                                    /*!<  dst node ID */
    int packet_type;                                            /*!<  packet type (hello, interest, RREQ, RREP, DATA) */
    int seq;                                                    /*!<  Sequence number of the last RREQ received */
    uint64_t time;                                              /*!<  Time related to the last RREQ update */
};


/* ************************************************** */
/* ************************************************** */

/** \brief A structure defining the Direct Diffusion header
 *  \struct _direct_diffusion_routing_packet_header
**/
struct _direct_diffusion_routing_packet_header{
    nodeid_t src;                       /*!<  current source node ID */
    nodeid_t dst;                       /*!<  current destination node ID */
    nodeid_t origin;                                            /*!<  origin node ID */
    nodeid_t final_dst;                     /*!<  final destination node ID (Sink ID)*/
    int sensor_type;                        /*!<  source node type */
    int packet_type;                        /*!<  packet type (hello, interest, RREQ, RREP, DATA) */
    int seq;                                                    /*!<  Packet sequence number */
    int ttl;                                                    /*!<  Time-To-Live to limit packet propagation */
    int ttl_max;                                                /*!<  Initial Time-To-Live to limit packet propagation */
    int app_id;                             /*!< App ID which called AODV to route its packet */
};

/** \brief A structure defining the Direct Diffusion Interest packet header
 *  \struct _direct_diffusion_routing_interest_packet_header
**/
struct _direct_diffusion_routing_interest_packet_header{
    int seq;                                /* Sequence number of packet*/
    int sink_id;                            /* Sink ID*/
    int data_type;                          /* Requested data type*/
    position_t position;                     /* Position of node */
};

#endif  //__direct_diffusion_routing_types__

