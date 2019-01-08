/**
 *  \file   buffer_management.c
 *  \brief  Buffer management for all modules
 *  \author Luiz Henrique Suraty Filho (CEA-LETI)
 *  \date   June 2013
 *  \version 1.0
 **/

#include "buffer_management.h"
#include "aodv_routing_functions.h"


/** \brief Function to create a new buffer queue 
 *  \fn buffer_queue_t * buffer_managemet_create_queue(call_t *to, int max_buffer_size, uint64_t cleaning_period )
 *  \param to is a pointer to the called class
 *  \param max_buffer_size is the maximum number of packet that buffer could have
 *  \param cleaning_period is the period of which the cleaning procedure will happen
 *  \return the created buffer_queue
 **/
buffer_queue_t * buffer_managemet_create_queue(call_t *to, int max_buffer_size, uint64_t cleaning_period ){
    buffer_queue_t *buffer_queue;

    buffer_queue = malloc(sizeof(buffer_queue_t));
    buffer_queue->object = to->object;
    buffer_queue->class = to->class;
    buffer_queue->max_size = max_buffer_size;
    buffer_queue->elts = list_create();
    buffer_queue->cleaning_period = cleaning_period;

    return buffer_queue;
}

/** \brief Function to destroy a buffer queue (to be used on the destroy event)
 *  \fn int buffer_management_destroy_queue(buffer_queue_t *buffer_queue)
 *  \param buffer_queue is the buffer with all entries
 *  \return SUCCESSFUL if success, UNSUCCESSFUL otherwise
 **/
int buffer_management_destroy_queue(buffer_queue_t *buffer_queue){

    //removes possibly the elements
    //buffer_management_empty_queue(buffer_queue);
    
    list_destroy(buffer_queue->elts);

    free(buffer_queue);

    return SUCCESSFUL;
}

/** \brief Function to empty a buffer queue
 *  \fn int buffer_management_empty_queue(buffer_queue_t *buffer_queue)
 *  \param buffer_queue is the buffer with all entries
 *  \return SUCCESSFUL if success, UNSUCCESSFUL otherwise
 **/
int buffer_management_empty_queue(buffer_queue_t *buffer_queue){


    if (buffer_queue == NULL)
        return UNSUCCESSFUL;

    if (buffer_management_get_size_of_queue(buffer_queue) == 0)
        return SUCCESSFUL;
   
    buffer_entry_t *buffer_entry;

    list_init_traverse(buffer_queue->elts);
    while ( (buffer_entry = (buffer_entry_t *) list_traverse(buffer_queue->elts)) != NULL ){
        list_delete(buffer_queue->elts, buffer_entry);
    }

    return SUCCESSFUL;

}

/** \brief Function to get the size of a buffer queue
 *  \fn int buffer_management_get_size_of_queue(buffer_queue_t *buffer_queue)
 *  \param buffer_queue is the buffer with all entries
 *  \return the size of the buffer queue
 **/
int buffer_management_get_size_of_queue(buffer_queue_t *buffer_queue){

    return list_getsize(buffer_queue->elts);

}

/** \brief Function to verify if the buffer queue is empty or not
 *  \fn int buffer_management_is_queue_empty(buffer_queue_t *buffer_queue)
 *  \param buffer_queue is the buffer with all entries
 *  \return SUCCESSFUL if success, UNSUCCESSFUL otherwise
 **/
int buffer_management_is_queue_empty(buffer_queue_t *buffer_queue){
    
    if (buffer_queue == NULL)
        return UNSUCCESSFUL;

    if (list_getsize(buffer_queue->elts) == 0)
        return UNSUCCESSFUL;

    return SUCCESSFUL;
    
}

/** \brief Function to remove an entry from the buffer queue
 *  \fn int buffer_management_remove_from_queue(buffer_queue_t *buffer_queue, buffer_entry_t *buffer_entry)
 *  \param buffer_queue is the buffer with all entries
 *  \param buffer_entry is the entry to be deleted
 *  \return SUCCESSFUL if success, UNSUCCESSFUL otherwise
 **/
int buffer_management_remove_from_queue(buffer_queue_t *buffer_queue, buffer_entry_t *buffer_entry){
    //packet_dealloc(buffer_entry->packet);
    list_delete(buffer_queue->elts, buffer_entry);
    return SUCCESSFUL;
}

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
int buffer_management_insert_packet_on_queue(call_t *to, call_t *from, buffer_queue_t *buffer_queue, packet_t *packet, uint64_t time_expiration, int packet_type){

    /* clean old entries first */
    buffer_management_clean_queue(buffer_queue); 

    /* check if buffer is full */
    if ( buffer_management_get_size_of_queue(buffer_queue) == buffer_queue->max_size )
        return UNSUCCESSFUL;

    buffer_entry_t *new_buffer_entry = malloc(sizeof(buffer_entry_t));

    new_buffer_entry->packet = packet;
    new_buffer_entry->number_retry = 0;
    new_buffer_entry->time_insertion = get_time();
    new_buffer_entry->time_expiration = get_time() + time_expiration;
    new_buffer_entry->packet_type = packet_type;

    list_insert(buffer_queue->elts, new_buffer_entry);

    return SUCCESSFUL;
}

/** \brief Function to pop an entry from the buffer queue
 *  \fn buffer_entry_t * buffer_management_pop_FIFO_queue(buffer_queue_t *buffer_queue)
 *  \param buffer_queue is the buffer with all entries
 *  \return the popped entry
 **/
buffer_entry_t * buffer_management_pop_FIFO_queue(buffer_queue_t *buffer_queue){

    /* clean old entries first */
    buffer_management_clean_queue(buffer_queue);
    
    if (buffer_management_get_size_of_queue(buffer_queue) == 0)
        return NULL;
    else
        return (buffer_entry_t *) list_pop_FIFO(buffer_queue->elts);
}

/** \brief Function called periodically for the cleaning of old elements on queue
 *  \fn void buffer_management_clean_queue_callback(call_t *to, call_t *from, void *args)
 *  \param to is a pointer to the called class
 *  \param from is a pointer to the class who invoked 
 *  \param args is a pointer to the arguments of the function
 **/
void buffer_management_clean_queue_callback(call_t *to, call_t *from, void *args){
    
    buffer_queue_t *buffer_queue = (buffer_queue_t *) args;

    /* clean old entries */
    buffer_management_clean_queue(buffer_queue);
    
    /* schedule next cleaning */
    if (buffer_queue->cleaning_period>0)
        scheduler_add_callback(get_time() + get_random_double() * buffer_queue->cleaning_period, to, from, (callback_t) buffer_management_clean_queue_callback, (void *)(buffer_queue));
}

/** \brief Function to clean the old entries from the buffer queue
 *  \fn void buffer_management_clean_queue(buffer_queue_t *buffer_queue)
 *  \param buffer_queue is the buffer with all entries
 **/
void buffer_management_clean_queue(buffer_queue_t *buffer_queue){
    buffer_entry_t *buffer_entry;
    
    list_init_traverse(buffer_queue->elts);
    while ( (buffer_entry = (buffer_entry_t *) list_traverse(buffer_queue->elts)) != NULL ){
        if (buffer_entry->time_expiration <= get_time()){
            list_delete(buffer_queue->elts, buffer_entry);
        }
    }

}

/** \brief Function to print all entries in the buffer queue
 *  \fn void buffer_management_print_queue(buffer_queue_t *buffer_queue)
 *  \param buffer_queue is the buffer with all entries
 **/
void buffer_management_print_queue(buffer_queue_t *buffer_queue){
    
    fprintf(stderr,"[BUFFER_MANAGEMENT] Node %d\n", buffer_queue->object);

    buffer_entry_t *buffer_entry;

    list_init_traverse(buffer_queue->elts);
    while ( (buffer_entry = (buffer_entry_t *) list_traverse(buffer_queue->elts)) != NULL ){

        fprintf(stderr," => packet_id = %3d  number_retry = %2d destination = %2d  insertion = %lf  expiration = %lf \n",buffer_entry->packet->id, buffer_entry->number_retry, buffer_entry->packet->destination.id, buffer_entry->time_insertion*0.000000001, buffer_entry->time_expiration*0.000000001);
    }


}

/** \brief Function to trigger the try of pushing data to lower layer. (Used when a new route is discovered)
 *  \fn void buffer_management_trigger(call_t *to, call_t *from, buffer_queue_t *buffer_queue)
 *  \param to is a pointer to the called class
 *  \param from is a pointer to the class who invoked 
 *  \param buffer_queue is the buffer with all entries
 **/
void buffer_management_trigger(call_t *to, call_t *from, buffer_queue_t *buffer_queue){

    buffer_entry_t *buffer_entry;

    /* check which packet can be pushed to the lower layer */
    list_init_traverse(buffer_queue->elts);

    while ( (buffer_entry = (buffer_entry_t *) list_traverse(buffer_queue->elts)) != NULL ){
        /* If forward to the lower layer is successful, remove packet from buffer*/
        if (forward_to_lower_layers(to,from,buffer_entry->packet, buffer_entry->packet_type) == 0){

            buffer_management_remove_from_queue(buffer_queue, buffer_entry);

        }
    }
}

/** \brief Function to trigger the try of pushing data (to specific destination) to lower layer. (Used when a new route to a dst is discovered)
 *  \fn void buffer_management_trigger_to_dst(call_t *to, call_t *from, buffer_queue_t *buffer_queue, int dst)
 *  \param to is a pointer to the called class
 *  \param from is a pointer to the class who invoked 
 *  \param buffer_queue is the buffer with all entries
 *  \param dst is the destination which now is available
 **/
void buffer_management_trigger_to_dst(call_t *to, call_t *from, buffer_queue_t *buffer_queue, int dst){
    buffer_entry_t *buffer_entry;

    int i = buffer_management_get_size_of_queue(buffer_queue);

    /* check which packet can be pushed to the lower layer */
    list_init_traverse(buffer_queue->elts);
    while (i--){
        if ( (buffer_entry = (buffer_entry_t *) list_traverse(buffer_queue->elts)) != NULL ){
            if (buffer_entry->packet->destination.id == dst){
                /* If forward to the lower layer is successful, remove packet from buffer*/
                if (forward_to_lower_layers(to,from,buffer_entry->packet, buffer_entry->packet_type) == 0){

                    buffer_management_remove_from_queue(buffer_queue, buffer_entry);

                }   
            }
        }
    }
}
