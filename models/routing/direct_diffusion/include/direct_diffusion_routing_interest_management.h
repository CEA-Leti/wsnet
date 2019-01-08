 /**
 *  \file   direct_diffusion_routing_interest_management.h
 *  \brief  Direct Diffusion based routing approach: Direct Diffusion Interest Management Header File
 *  \author Luiz Henrique Suraty Filho (CEA-LETI)
 *  \date   May 2013
 *  \version 1.0
**/

#ifndef  __direct_diffusion_routing_interest_management__
#define __direct_diffusion_routing_interest_management__

/** \brief Generation of an INTEREST packet
 *  \fn int interest_generation(call_t *to, call_t *from)
 *  \param to is a pointer to the called class
 *  \param from is a pointer to the class who invoked
 *  \return SUCCESSFUL if success, UNSUCCESSFUL otherwise
 **/
int interest_generation(call_t *to, call_t *from);


/** \brief Callback function for the periodic generation of RREQ packets in Direct Diffusion (to be used with the scheduler_add_callback function).
 *  \fn int interest_periodic_generation_callback(call_t *to, call_t *from, void *args)
 *  \param to is a pointer to the called class
 *  \param from is a pointer to the class who invoked
 *  \param args is a pointer to the arguments of the function
 *  \return SUCCESSFUL if success, UNSUCCESSFUL otherwise
 **/
int interest_periodic_generation_callback(call_t *to, call_t *from, void *args);

 /** \brief Function to forward INTEREST packets  (Direct Diffusion)
 *  \fn int interest_propagation(call_t *to, call_t *from, void *args)
 *  \param to is a pointer to the called class
 *  \param from is a pointer to the class who invoked
 *  \param args is a pointer to the arguments of the function
 *  \return SUCCESSFUL if success, UNSUCCESSFUL otherwise
**/
int interest_propagation(call_t *to, call_t *from, void *args);

/** \brief Function to check if packet has already been sent (Direct Diffusion)
 *  \fn int interest_table_lookup(call_t *to, int src, int dst, int data_type, int seq)
 *  \param to is a pointer to the called class
 *  \param from is a pointer to the class who invoked
 *  \param origin is the origin ID
 *  \param final_dst is the final destination ID
 *  \param packet_type is the type of the packet (RREQ, RREP, DATA)
 *  \param seq is the sequence number of packet
 *  \return SUCCESSFUL if success, UNSUCCESSFUL otherwise
**/
int packet_table_lookup(call_t *to, call_t *from, int origin, int final_dst, int packet_type, int seq);

 /** \brief Function to update the local packet table to avoid the transmission of duplicate packets (Direct Diffusion)
  *  \fn void interest_table_update(call_t *to, int src, int dst, int data_type, int seq)
 *  \param to is a pointer to the called class
 *  \param from is a pointer to the class who invoked
 *  \param origin is the origin ID
 *  \param final_dst is the final destination ID
 *  \param packet_type is the type of the packet (RREQ, RREP, DATA)
 *  \param seq is the sequence number of packet
 *  \return SUCCESSFUL if success, UNSUCCESSFUL otherwise
**/
void packet_table_update(call_t *to, call_t *from, int origin, int final_dst, int packet_type, int seq);

#endif  //__direct_diffusion_routing_interest_management__

