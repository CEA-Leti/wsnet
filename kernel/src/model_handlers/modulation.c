/**
 *  \file   modulation.c
 *  \brief  Modulation management
 *  \author Guillaume Chelius & Elyes Ben Hamida & Loic Lemaitre
 *  \date   2007
 **/

#include <stdlib.h>
#include <stdio.h>
#include <kernel/include/data_structures/list/list.h>
#include <kernel/include/definitions/class.h>
#include "media_rxtx.h"
#include <kernel/include/modelutils.h>

/* ************************************************** */
/* ************************************************** */
list_t *modulation_entities = NULL;


/* ************************************************** */
/* ************************************************** */
int modulation_init(void) {
  return 0;
}

void modulation_clean(void) {
  list_destroy(modulation_entities);
}


/* ************************************************** */
/* ************************************************** */
int modulation_bootstrap(void) { 
  return 0;
}

/* ************************************************** */
/* ************************************************** */
//double do_modulate(classid_t modulation, double rxmW, double noise) {
double do_modulate(call_t *from, classid_t modulation, double rxmW, double noise) {
  class_t *class = get_class_by_id(modulation);
  call_t   to    = {modulation, -1};
  double   snr   = noise ? (rxmW / noise) : MAX_SNR;

  //printf("Do-modulate:  SNR = %f (%f dBm), noise = %.10f mW (%f dBm), rxmW = %.10f (%f dbm)\n", snr, mW2dBm(snr), noise, mW2dBm(noise), rxmW, mW2dBm(rxmW));

  //return class->methods->modulation.modulate(&to, NULL, snr);
  return class->methods->modulation.modulate(&to, from, snr);
}

double do_modulate_snr(call_t *from, classid_t modulation, double snr) {
  class_t *class = get_class_by_id(modulation);
  call_t   to    = {modulation, -1};

  //printf("Do-modulate:  SNR = %f (%f dBm), noise = %.10f mW (%f dBm), rxmW = %.10f (%f dbm)\n", snr, mW2dBm(snr), noise, mW2dBm(noise), rxmW, mW2dBm(rxmW));

  return class->methods->modulation.modulate(&to, from, snr);
}

/* ************************************************** */
/* ************************************************** */

/* edit by Christophe Savigny <christophe.savigny@insa-lyon.fr> */
int modulation_bit_per_symbol(classid_t modulation){
  class_t *class = get_class_by_id(modulation);
  call_t   to    = {modulation, -1};
  return class->methods->modulation.bit_per_symbol(&to, NULL);
}
/* end of edition */

void modulation_set_modulation_type(classid_t modulation, int modulation_type) {
	  class_t *class = get_class_by_id(modulation);
	  call_t   to    = {modulation, -1};
  class->methods->modulation.set_modulation_type(&to, NULL, modulation_type);
}

int modulation_get_modulation_type(classid_t modulation)
{
  class_t *class = get_class_by_id(modulation);
  call_t   to    = {modulation, -1};
  return class->methods->modulation.get_modulation_type(&to, NULL);
}


/* ************************************************** */
/* ************************************************** */
void modulation_errors(packet_t *packet) {
  /* TODO */
}
