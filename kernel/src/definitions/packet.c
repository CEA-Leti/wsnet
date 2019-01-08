/**
 *  \file   packet.c
 *  \brief  Packet management
 *  \author Guillaume Chelius & Elyes Ben Hamida & Quentin Lampin
 *  \date   2007
 **/

#include <stdio.h>
#include <string.h>
#include <kernel/include/data_structures/hashtable/hashtable.h>
#include <kernel/include/options.h>
#include "packet.h"


/* ************************************************** */
/* ************************************************** */
// these variables are initialized but never used.
/*static void *mem_packet = NULL;
#if (SNR_STEP > 0)
static void *mem_snr = NULL;
#endif *///SNR_STEP //
static packetid_t id = 0;


/* ************************************************** */
/* ************************************************** */
int packet_init(void) {

  // these variables are initialized but never used.
    /*if ((mem_packet = malloc(sizeof(packet_t))) == NULL) {
        return -1;
    }
#if (SNR_STEP > 0)
    if ((mem_snr = malloc(sizeof(double) * SNR_STEP)) == NULL) {
        return -1;
    }
#endif //SNR_STEP//
*/
    return 0;
}

int packet_bootstrap(void) {
    return 0;
}

void packet_clean(void) {
}


/* ************************************************** */
/* ************************************************** */

/* edit by Quentin Lampin <quentin.lampin@orange-ftgroup.com> */
packet_t *packet_create(call_t *to, int size, int real_size) {
    packet_t *packet;
 
    packet = (packet_t *) malloc(sizeof(packet_t));
    packet->fields = hashtable_create(hash_string, equal_string, hashtable_field_destroy, hashtable_field_clone);
    packet->noise_mW = NULL;
    packet->ber = NULL;   
    packet->id = id++;
    packet->size = size;
    packet->type = 0;
    packet->channel = -1;
    /*edit by Luiz Henrique Suraty Filho*/
    packet->frame_type = 0;
    /* end of edition */
	packet->frame_control_type = 0;
	
	packet->signal = NULL;

	/* edit by El chall Rida */
	packet->SINR = -1;
	/* end of edition */

    if(real_size > 0){
      packet->real_size = real_size;
    }else{
      packet->real_size = 8 * size;
    }
    return packet;
}

packet_t *packet_alloc(call_t *to, int size) {
    return packet_create(to, size, -1);
}

/* end of edition */


void packet_dealloc(packet_t *packet) {
    if (packet->ber) {
#if (SNR_STEP > 0)
      free(packet->noise_mW);
      free(packet->ber);        
#elif (SNR_STEP < 0)
        free(packet->noise_mW);
        free(packet->ber);
#endif /*SNR_STEP*/
    }
    hashtable_destroy(packet->fields);
    free(packet);
}


/* ************************************************** */
/* ************************************************** */
packet_t *packet_clone(packet_t *packet) {
    packet_t *packet0;

    packet0 = (packet_t *) malloc(sizeof(packet_t));
    memcpy(packet0, packet, sizeof(packet_t));
    packet0->fields = clone_hashtable(packet->fields);
    packet0->noise_mW = NULL;
    packet0->ber = NULL;
    packet0->id = id++;

    return packet0;
}


packet_t *packet_rxclone(packet_t *packet) {
    packet_t *packet0;

    packet0 = (packet_t *) malloc(sizeof(packet_t));
    memcpy(packet0, packet, sizeof(packet_t));
    packet0->fields = clone_hashtable(packet->fields);
#if (SNR_STEP > 0)
    packet0->noise_mW = (double *) malloc(sizeof(double) * SNR_STEP);
    packet0->ber = (double *) malloc(sizeof(double) * SNR_STEP);
#elif (SNR_STEP < 0)
    packet0->noise_mW = (double *) malloc(ceil(packet->real_size/8) * sizeof(double));
    packet0->ber = (double *) malloc(ceil(packet->real_size/8) * sizeof(double));
#endif /*SNR_STEP*/

    return packet0;
}


void packet_add_field(packet_t *packet, char* name, field_t *field){
  hashtable_insert(packet->fields, name, field);

  return;
}


field_t *packet_retrieve_field(packet_t *packet, char* name){
  return hashtable_retrieve(packet->fields, name);
}


void *packet_retrieve_field_value_ptr(packet_t *packet, char* name){
  field_t *field = packet_retrieve_field(packet, name);
  if (field == NULL)
      return NULL;
  else
    return field->value;
}
