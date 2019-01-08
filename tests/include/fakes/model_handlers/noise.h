/**
 *  \file   noise.h
 *  \brief  Noise management declarations
 *  \author Luiz Henrique Suraty Filho
 *  \date   2018
 **/
#ifndef WSNET_TEST_FAKES_MODEL_HANDLERS_NOISE_H_
#define WSNET_TEST_FAKES_MODEL_HANDLERS_NOISE_H_

#include <kernel/include/model_handlers/noise.h>

/* ************************************************** */
/* ************************************************** */
#ifdef __cplusplus
extern "C"{
#endif

int __wrap_noise_init(void){
  return 0;
}
int __wrap_noise_bootstrap(void){
  return 0;
}
void __wrap_noise_clean(void){
  return;
}


void __wrap_noise_packet_cs(call_t *to, packet_t *packet){
  (void) to;
  (void) packet;
}
void __wrap_noise_packet_rx(call_t *to, packet_t *packet){
  (void) to;
  (void) packet;
}

double __wrap_medium_get_noise(call_t *to, int channel){
  (void) to;
  (void) channel;
  return 0.0;
}

double __wrap_noise_get_noise(call_t *to_interface, uint64_t bandwidth_signal_Hz){
  (void) to_interface;
  (void) bandwidth_signal_Hz;
  return 0.0;
}

double __wrap_get_thermal_white_noise(call_t *from, mediumid_t medium){
  (void) from;
  (void) medium;
  return 0.0;
}

#ifdef __cplusplus
}
#endif

#endif //WSNET_TEST_FAKES_MODEL_HANDLERS_NOISE_H_
