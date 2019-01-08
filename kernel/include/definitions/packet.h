/**
 *  \file   packet.h
 *  \brief  Packet declarations
 *  \author Guillaume Chelius & Elyes Ben Hamida & Quentin Lampin
 *  \date   2007
 **/
#ifndef __packet_public__
#define __packet_public__

#include <kernel/include/definitions/types.h>

#ifdef __cplusplus
extern "C"{
#endif
/** 
 * \brief Deallocate a packet.
 * \param packet the packet to dealloc.
 **/
void packet_dealloc(packet_t *packet);


/** 
 * \brief Duplicate a packet. Data and other information are copied.
 * \param packet the packet to clone.
 * \return The cloned packet.
 **/
packet_t *packet_clone(packet_t *packet);


/* edit by Quentin Lampin <quentin.lampin@orange-ftgroup.com> */

/**
 * \brief Allocate a packet with real size argument.
 * \param c should be {-1, node id}.
 * \param size the data size of the packet (in bytes).
 * \param real_size real size in bits of the packet.
 * \return The newly allocated packet.
 **/

packet_t *packet_create(call_t *to, int size, int real_size);

/* end of edition */


/** 
 * \brief Allocate a packet - DEPRECATED, use packet_create instead.
 * \param c should be {-1, node id}.
 * \param size the data size of the packet (in bytes).
 * \return The newly allocated packet.
 **/
packet_t *packet_alloc(call_t *to, int size);

#ifdef __cplusplus
}
#endif

#endif //__packet_public__


#ifndef __packet__
#define __packet__

#ifdef __cplusplus
extern "C"{
#endif

/* ************************************************** */
/* ************************************************** */
int packet_init(void);
int packet_bootstrap(void);
void packet_clean(void);


/* ************************************************** */
/* ************************************************** */
packet_t *packet_rxclone(packet_t *packet);


void packet_add_field(packet_t *packet, char *name, field_t *field);
field_t *packet_retrieve_field(packet_t *packet, char *name);
void *packet_retrieve_field_value_ptr(packet_t *packet, char* name);

#ifdef __cplusplus
}
#endif

#endif //__packet__
