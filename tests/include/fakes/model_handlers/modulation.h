/**
 *  \file   modulation.h
 *  \brief  Modulation declarations
 *  \author Guillaume Chelius & Elyes Ben Hamida & Christophe Savigny
 *  \date   2009
 **/
#ifndef WSNET_TEST_FAKES_MODEL_HANDLERS_MODULATION_H_
#define WSNET_TEST_FAKES_MODEL_HANDLERS_MODULATION_H_

#include <kernel/include/model_handlers/modulation.h>

#ifdef __cplusplus
extern "C"{
#endif

int __wrap_modulation_bit_per_symbol(classid_t modulation){
  (void) modulation;
  return 0;
}

void __wrap_modulation_set___wrap_modulation_type(classid_t modulation, int modulation_type){
  (void) modulation;
  (void) modulation_type;
}

int __wrap_modulation_get___wrap_modulation_type(classid_t modulation){
  (void) modulation;
  return 0;
}

int __wrap_modulation_init(void){
  return 0;
}

int __wrap_modulation_bootstrap(void){
  return 0;
}

void __wrap_modulation_clean(void){
  return;
}

double do_modulate(call_t *from, classid_t modulation, double rxmW, double noise){
  (void) from;
  (void) modulation;
  (void) rxmW;
  (void) noise;
  return 0.0;
}

double do_modulate_snr(call_t *from, classid_t modulation, double snr){
  (void) from;
  (void) modulation;
  (void) snr;
  return 0.0;
}

void __wrap_modulation_errors(packet_t *packet){
  (void) packet;
  return;
}

#ifdef __cplusplus
}
#endif
#endif //WSNET_TEST_FAKES_MODEL_HANDLERS_MODULATION_H_

