/**
 *  \file   aodv_routing_functions.h
 *  \brief  AODV based routing approach: Auxiliary Functions Header File
 *  \author Luiz Henrique Suraty Filho (CEA-LETI)
 *  \date   May 2013
 **/

#ifndef  __aodv_routing_functions__
#define __aodv_routing_functions__

/** \brief Set default values of the node.
 *  \fn void set_node_default_values(struct _aodv_routing_node_private *nodedata)
 *  \param nodedata is a pointer to the node data
 *  \return nothing
**/
void set_node_default_values(struct _aodv_routing_node_private *nodedata);

/** \brief Read parameters from the xml file.
 *  \fn int read_xml_file(struct _aodv_routing_node_private *nodedata, void *params)
 *  \param nodedata is a pointer to the node data
 *  \param params is a pointer to the parameters read from the XML configuration file
 *  \return SUCCESSFUL if success, UNSUCCESSFUL otherwise
**/
int read_nodedata_from_xml_file(struct _aodv_routing_node_private *nodedata, void *params);

/** \brief Process a RREQ packet received.
 *  \fn process_rreq_packet(call_t *to, call_t *from, packet_t *packet)
 *  \param to is a pointer to the called class
 *  \param from is a pointer to the class who invoked
 *  \param packet is a pointer to the packet to be processed
 *  \return nothing
**/
void process_rreq_packet(call_t *to, call_t *from, packet_t *packet);

/** \brief Process a RREP packet received.
 *  \fn void process_rrep_packet(call_t *to, call_t *from, packet_t *packet)
 *  \param to is a pointer to the called class
 *  \param from is a pointer to the class who invoked
 *  \param packet is a pointer to the packet to be processed
 *  \return nothing
**/
void process_rrep_packet(call_t *to, call_t *from, packet_t *packet);


/** \brief Process a HELLO packet received.
 *  \fn void process_hello_packet(call_t *to, call_t *from, packet_t *packet)
 *  \param to is a pointer to the called class
 *  \param from is a pointer to the class who invoked
 *  \param packet is a pointer to the packet to be processed
 *  \return nothing
**/
void process_hello_packet(call_t *to, call_t *from, packet_t *packet);


/** \brief Process a DATA packet received.
 *  \fn void process_data_packet(call_t *to, call_t *from, packet_t *packet)
 *  \param to is a pointer to the called class
 *  \param from is a pointer to the class who invoked
 *  \param packet is a pointer to the packet to be processed
 *  \return nothing
**/
void process_data_packet(call_t *to, call_t *from, packet_t *packet);

/** \brief Forward packets to lower layers
 *  \fn int forward_to_lower_layers(call_t *to, call_t *from, packet_t *packet, int packet_type)
 *  \param to is a pointer to the called class
 *  \param from is a pointer to the class who invoked
 *  \param packet is a pointer to the packet to be forwarded
 *  \param packet_type indicates which type of packet will be forwarded
 *  \return SUCCESSFUL if success, UNSUCCESSFUL otherwise
 **/
int forward_to_lower_layers(call_t *to, call_t *from, packet_t *packet, int packet_type);

/** \brief Encapsulate the packet with layer's header and, in order to do so, verify the existance of a route to the destination
 *  \fn int set_aodv_routing_packet_header(call_t *to, call_t *from, packet_t *packet, int packet_type) 
 *  \param to is a pointer to the called class
 *  \param from is a pointer to the class who invoked
 *  \param packet is a pointer to the packet to be forwarded
 *  \param packet_type indicates which type of packet will be forwarded
 *  \return SUCCESSFUL if success, UNSUCCESSFUL otherwise
 **/
int set_aodv_header(call_t *to, call_t *from, packet_t *packet, int packet_type);

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
int set_aodv_routing_packet_fixed_header(call_t *to, call_t *from, packet_t *packet, int origin, int final_dst, int origin_seq, int hops, uint64_t link_life_time,int packet_type);

#endif  //__aodv_routing_functions__

