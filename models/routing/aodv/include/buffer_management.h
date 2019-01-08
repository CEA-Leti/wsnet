/**
 *  \file   buffer_management.h
 *  \brief  Buffer management for all modules - Header File
 *  \author Luiz Henrique Suraty Filho (CEA-LETI)
 *  \date   June 2013
 *  \version 1.0
 **/

#pragma once

/* ************************************************** */
/* ************************************************** */

#include <kernel/include/modelutils.h>
#include <stdio.h>
#include "aodv_routing_types.h"

/** \brief Function to create a new buffer queue 
 *  \fn buffer_queue_t * buffer_managemet_create_queue(call_t *to, int max_buffer_size, uint64_t cleaning_period )
 *  \param to is a pointer to the called class
 *  \param max_buffer_size is the maximum number of packet that buffer could have
 *  \param cleaning_period is the period of which the cleaning procedure will happen
 *  \return the created buffer_queue
 **/
buffer_queue_t * buffer_managemet_create_queue(call_t *to, int max_buffer_size, uint64_t cleaning_period);

/** \brief Function to destroy a buffer queue (to be used on the destroy event)
 *  \fn int buffer_management_destroy_queue(buffer_queue_t *buffer_queue)
 *  \param buffer_queue is the buffer with all entries
 *  \return SUCCESSFUL if success, UNSUCCESSFUL otherwise
 **/
int buffer_management_destroy_queue(buffer_queue_t *buffer_queue);

/** \brief Function to empty a buffer queue
 *  \fn int buffer_management_empty_queue(buffer_queue_t *buffer_queue)
 *  \param buffer_queue is the buffer with all entries
 *  \return SUCCESSFUL if success, UNSUCCESSFUL otherwise
 **/
int buffer_management_empty_queue(buffer_queue_t *buffer_queue);

/** \brief Function to get the size of a buffer queue
 *  \fn int buffer_management_get_size_of_queue(buffer_queue_t *buffer_queue)
 *  \param buffer_queue is the buffer with all entries
 *  \return the size of the buffer queue
 **/
int buffer_management_get_size_of_queue(buffer_queue_t *buffer_queue);

/** \brief Function to verify if the buffer queue is empty or not
 *  \fn int buffer_management_is_queue_empty(buffer_queue_t *buffer_queue)
 *  \param buffer_queue is the buffer with all entries
 *  \return SUCCESSFUL if success, UNSUCCESSFUL otherwise
 **/
int buffer_management_is_queue_empty(buffer_queue_t *buffer_queue);

/** \brief Function to remove an entry from the buffer queue
 *  \fn int buffer_management_remove_from_queue(buffer_queue_t *buffer_queue, buffer_entry_t *buffer_entry)
 *  \param buffer_queue is the buffer with all entries
 *  \param buffer_entry is the entry to be deleted
 *  \return SUCCESSFUL if success, UNSUCCESSFUL otherwise
 **/
int buffer_management_remove_from_queue(buffer_queue_t *buffer_queue, buffer_entry_t *buffer_entry);

/** \brief Function to insert an entry on the buffer queue
 *  \fn int buffer_management_insert_packet_on_queue(buffer_queue_t *buffer_queue, packet_t *packet, uint64_t time_expiration, int packet_type)
 *  \param to is a pointer to the called class
 *  \param from is a pointer to the class who invoked
 *  \param buffer_queue is the buffer with all entries
 *  \param packet is a pointer to the received packet
 *  \param time_expiration is the expiration time expected for the packet
 *  \param packet_type is the packet type
 *  \return SUCCESSFUL if success, UNSUCCESSFUL otherwise
 **/
int buffer_management_insert_packet_on_queue(call_t *to, call_t *from, buffer_queue_t *buffer_queue, packet_t *packet, uint64_t time_expiration, int packet_type);

/** \brief Function to pop an entry from the buffer queue
 *  \fn buffer_entry_t * buffer_management_pop_FIFO_queue(buffer_queue_t *buffer_queue)
 *  \param buffer_queue is the buffer with all entries
 *  \return the popped entry
 **/
buffer_entry_t * buffer_management_pop_FIFO_queue(buffer_queue_t *buffer_queue);

/** \brief Function called periodically for the cleaning of old elements on queue
 *  \fn void buffer_management_clean_queue_callback(call_t *to, call_t *from, void *args)
 *  \param to is a pointer to the called class
 *  \param from is a pointer to the class who invoked 
 *  \param args is a pointer to the arguments of the function
 **/
void buffer_management_clean_queue_callback(call_t *to, call_t *from, void *args);

/** \brief Function to clean the old entries from the buffer queue
 *  \fn void buffer_management_clean_queue(buffer_queue_t *buffer_queue)
 *  \param buffer_queue is the buffer with all entries
 **/
void buffer_management_clean_queue(buffer_queue_t *buffer_queue);

/** \brief Function to print all entries in the buffer queue
 *  \fn void buffer_management_print_queue(buffer_queue_t *buffer_queue)
 *  \param buffer_queue is the buffer with all entries
 **/
void buffer_management_print_queue(buffer_queue_t *buffer_queue);

/** \brief Function to trigger the try of pushing data to lower layer. (Used when a new route is discovered)
 *  \fn void buffer_management_trigger(call_t *to, call_t *from, buffer_queue_t *buffer_queue)
 *  \param to is a pointer to the called class
 *  \param from is a pointer to the class who invoked 
 *  \param buffer_queue is the buffer with all entries
 **/
void buffer_management_trigger(call_t *to, call_t *from, buffer_queue_t *buffer_queue);

/** \brief Function to trigger the try of pushing data (to specific destination) to lower layer. (Used when a new route to a dst is discovered)
 *  \fn void buffer_management_trigger_to_dst(call_t *to, call_t *from, buffer_queue_t *buffer_queue, int dst)
 *  \param to is a pointer to the called class
 *  \param from is a pointer to the class who invoked 
 *  \param buffer_queue is the buffer with all entries
 *  \param dst is the destination which now is available
 **/
void buffer_management_trigger_to_dst(call_t *to, call_t *from, buffer_queue_t *buffer_queue, int dst);

