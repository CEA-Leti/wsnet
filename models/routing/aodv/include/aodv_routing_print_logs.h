/**
 *  \file   aodv_routing_print_logs.h
 *  \brief  AODV based routing approach: Print Logs Header File
 *  \author Luiz Henrique Suraty Filho (CEA-LETI)
 *  \date   May 2013
 *  \version 1.0
**/

#ifndef  __aodv_routing_print_logs__
#define __aodv_routing_print_logs__

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31;1m"
#define KGRN  "\x1B[32;1m"
#define KYEL  "\x1B[33;1m"
#define KBLU  "\x1B[34;1m"
#define KMAG  "\x1B[35;1m"
#define KCYN  "\x1B[36;1m"
#define KWHT  "\x1B[37;1m"

#define TX_STATUS 0x000F
#define RX_STATUS 0x00F0
#define FWD_STATUS 0x0F00
#define STATS_STATUS 0xF000

/** \brief Print log related when a packet is transmitted.
 *  \fn void print_log_tx(call_t *to, int packet_type, packet_t *packet) 
 *  \param to is a pointer to the called class
 *  \param packet is a pointer to the packet to be forwarded
 *  \param packet_type indicates which type of packet
 *  \return nothing
 **/
void print_log_tx(call_t *to, int packet_type, packet_t *packet);

/** \brief Print log related when a packet is received.
 *  \fn void print_log_rx(call_t *to, int packet_type, packet_t *packet) 
 *  \param to is a pointer to the called class
 *  \param packet is a pointer to the packet to be forwarded
 *  \param packet_type indicates which type of packet
 *  \return nothing
 **/
void print_log_rx(call_t *to, int packet_type, packet_t *packet);

/** \brief Print log related when a packet will be forwarded.
 *  \fn void print_log_fwd(call_t *to, int packet_type, packet_t *packet) 
 *  \param to is a pointer to the called class
 *  \param packet is a pointer to the packet to be forwarded
 *  \param packet_type indicates which type of packet
 *  \return nothing
 **/
void print_log_fwd(call_t *to, int packet_type, packet_t *packet);

/** \brief Print log related when a path to a certain node is already know.
 *  \fn void print_log_known(call_t *to, int packet_type, packet_t *packet) 
 *  \param to is a pointer to the called class
 *  \param packet is a pointer to the packet to be forwarded
 *  \param packet_type indicates which type of packet
 *  \return nothing
 **/
void print_log_known(call_t *to, int packet_type, packet_t *packet);

/** \brief Print log related to the final stats.
 *  \fn void print_log_stats(call_t *to, call_t *from) 
 *  \param to is a pointer to the called class
 *  \param from is a pointer to the class who invoked
 *  \return nothing
 **/
void print_log_stats(call_t *to, call_t *from);

/** \brief Function to list the routing table on the standard output (AODV)
 *  \fn     void print_aodv_route(call_t *to, call_t *from)
 *  \param to is a pointer to the called class
 *  \param from is a pointer to the class who invoked
 **/
 void print_aodv_route(call_t *to, call_t *from);

/** \brief Verify whether the log should be printed or not, according to the log_status parameter on the XML file
 *  \fn int print_log_check(call_t *to, int packet_type, unsigned int tx_status)
 *  \param to is a pointer to the called class
 *  \param packet is a pointer to the packet to be forwarded
 *  \param packet_type indicates which type of packet
 *  \return SUCCESSFUL if success, UNSUCCESSFUL otherwise
 **/
int print_log_check(call_t *to, int packet_type, unsigned int tx_status);

#endif  //__aodv_routing_route_management__

