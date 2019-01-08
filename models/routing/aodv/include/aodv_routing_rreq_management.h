/**
 *  \file   aodv_routing_rreq_management.h
 *  \brief  AODV based routing approach: AODV RREQ/RREP Management Header File
 *  \author Luiz Henrique Suraty Filho (CEA-LETI)
 *  \date   May 2013
 *  \version 1.0
**/

 #ifndef  __aodv_routing_rreq_management__
#define  __aodv_routing_rreq_management__



/** \brief Generation of a RREQ packet
 *  \fn int rreq_generation(call_t *to, call_t *from, int final_dst)
 *  \param to is a pointer to the called class
 *  \param from is a pointer to the class who invoked
 *  \param final_dst is a the final destination where your RREP packet will be sent
 *  \return SUCCESSFUL if success, UNSUCCESSFUL otherwise
 **/
int rreq_generation(call_t *to, call_t *from, int final_dst);

/** \brief Callback function for the periodic generation of RREQ packets in AODV (to be used with the scheduler_add_callback function).
 *  \fn int rreq_periodic_generation_callback(call_t *to, call_t *from, void *args)
 *  \param to is a pointer to the called class
 *  \param from is a pointer to the class who invoked
 *  \param args is a pointer to the arguments of the function
 *  \return SUCCESSFUL if success, UNSUCCESSFUL otherwise
 **/
int rreq_periodic_generation_callback(call_t *to, call_t *from, void *args);

 /** \brief Function to forward RREQ  packet towards the destination (AODV)
 *  \fn int rreq_propagation(call_t *to, call_t *from, void *args)
 *  \param to is a pointer to the called class
 *  \param from is a pointer to the class who invoked
 *  \param args is a pointer to the arguments of the function
 *  \return SUCCESSFUL if success, UNSUCCESSFUL otherwise
**/
int rreq_propagation(call_t *to, call_t *from, packet_t *packet);


 /** \brief Function for the transmission of RREP packet to the source node (AODV) => to be done by the sink node
 *  \fn int rrep_transmission(call_t *to, call_t *from, packet_t *rreq_packet)
 *  \param to is a pointer to the called class
 *  \param from is a pointer to the class who invoked
 *  \param rreq_packet is a pointer to the received RREQ packet
 *  \return SUCCESSFUL if success, UNSUCCESSFUL otherwise
**/
int rrep_transmission(call_t *to, call_t *from, packet_t *rreq_packet);


 /** \brief Function for the transmission of RREP packet to the source node (AODV) => to be done by intermediate sensor node
 *  \fn int rrep_transmission_from_relay(call_t *to, call_t *from, void *args) 
 *  \param to is a pointer to the called class
 *  \param from is a pointer to the class who invoked
 *  \param args is a pointer to the function argument
 *  \return SUCCESSFUL if success, UNSUCCESSFUL otherwise
**/
int rrep_transmission_from_relay(call_t *to, call_t *from, packet_t *rreq_packet);

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
int packet_table_lookup(call_t *to, call_t *from, int src, int dst, int packet_type, int seq);


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
void packet_table_update(call_t *to, call_t *from, int src, int dst, int packet_type, int seq);


#endif  // __aodv_routing_rreq_management__




