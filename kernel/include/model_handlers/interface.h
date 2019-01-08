/**
 *  \file   interface.h
 *  \brief  Interface declarations
 *  \author Guillaume Chelius & Elyes Ben Hamida & Loic Lemaitre
 *  \date   2007
 **/
#ifndef __interface_public__
#define __interface_public__

#include <kernel/include/definitions/types.h>

#ifdef __cplusplus
extern "C"{
#endif

/**
 * \brief Return the signal loss induced by an interface circuit.
 * \param to should be {interface id, node id}.
 * \param from is {calling class id, node id} if needed.
 * \return Signal loss in dB.
 **/
double interface_get_loss(call_t *to, call_t *from);


/**
 * \brief Return the interface orientation.
 * \param to should be {interface id, node id}.
 * \param from is {calling class id, node id} if needed.
 * \return Current interface orientation.
 **/
angle_t *interface_get_angle(call_t *to, call_t *from);


/**
 * \brief Set the interface orientation.
 * \param to should be {interface id, node id}.
 * \param from is {calling class id, node id} if needed.
 * \param angle new interface orientation.
 **/
void interface_set_angle(call_t *to, call_t *from, angle_t *angle);


/**
 * \brief Forward a received packet to the interface.
 * \param to should be {interface id, node id}.
 * \param from is {calling class id, node id} if needed.
 * \param packet the received packet.
 **/
void interface_rx(call_t *to, call_t *from, packet_t *packet);


/**
 * \brief Forward a received packet to the interface.
 * \param to should be {interface id, node id}.
 * \param from is {calling class id, node id} if needed.
 * \param packet the received packet.
 **/
void interface_cs(call_t *to, call_t *from, packet_t *packet);


/**
 * \brief Return the tx interface gain towards the destination direction.
 * \param to should be {interface id, node id}.
 * \param from is {calling class id, node id} if needed.
 * \param position the destination position.
 * \return The interface gain in dB.
 **/
double interface_gain_tx(call_t *to, call_t *from, position_t *position);


/**
 * \brief Return the rx interface gain towards the source direction.
 * \param to should be {interface id, node id}.
 * \param from is {calling class id, node id} if needed.
 * \param position the source position.
 * \return The interface gain in dB.
 **/
double interface_gain_rx(call_t *to, call_t *from, position_t *position);


/**
 * \brief Return the medium matching the node interface.
 * \param to should be {interface id, node id}.
 * \param from is {calling class id, node id} if needed.
 * \return The medium id requested.
 **/
mediumid_t interface_get_medium(call_t *to, call_t *from);

/**
 * \brief Return the interface type
 * \param to should be {interface id, node id}.
 * \return The interface type requested.
 **/
interface_type_t interface_get_type(call_t *to);

#ifdef __cplusplus
}
#endif

#endif //__interface_public__
