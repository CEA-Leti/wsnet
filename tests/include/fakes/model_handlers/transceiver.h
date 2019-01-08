/**
 *  \file   transceiver.h
 *  \brief  Hardware transceiver handler declarations.
 *  \author Luiz Henrique Suraty Filho
 *  \date   2007
 **/

#ifndef WSNET_TEST_FAKES_MODEL_HANDLERS_TRANSCEIVER_H_
#define WSNET_TEST_FAKES_MODEL_HANDLERS_TRANSCEIVER_H_

#include <kernel/include/model_handlers/transceiver.h>

#ifdef __cplusplus
extern "C"{
#endif

double __wrap_transceiver_get_cs(call_t *to, call_t *from){
  (void) to;
  (void) from;
  return 0.0;
}

double __wrap_transceiver_get_noise(call_t *to, call_t *from){
  (void) to;
  (void) from;
  return 0.0;
}

double __wrap_transceiver_get_power(call_t *to, call_t *from){
  (void) to;
  (void) from;
  return 0.0;
}


void __wrap_transceiver_set_power(call_t *to, call_t *from, double power){
  (void) to;
  (void) from;
  (void) power;
}

int __wrap_transceiver_get_channel(call_t *to, call_t *from){
  (void) to;
  (void) from;
  return 0;
}


void __wrap_transceiver_set_channel(call_t *to, call_t *from, int channel){
  (void) to;
  (void) from;
  (void) channel;
}

classid_t __wrap_transceiver_get_modulation(call_t *to, call_t *from){
  (void) to;
  (void) from;
  return 0;
}

void __wrap_transceiver_set_modulation(call_t *to, call_t *from, classid_t modulation){
  (void) to;
  (void) from;
  (void) modulation;
}

uint64_t __wrap_transceiver_get_Tb(call_t *to, call_t *from){
  (void) to;
  (void) from;
  return 0;
}

void __wrap_transceiver_cs(call_t *to, call_t *from, packet_t *packet){
  (void) to;
  (void) from;
  (void) packet;
}

void __wrap_transceiver_set_sensibility(call_t *to, call_t *from, double sensibility){
  (void) to;
  (void) from;
  (void) sensibility;
}

double __wrap_transceiver_get_sensibility(call_t *to, call_t *from){
  (void) to;
  (void) from;
  return 0;
}

void __wrap_transceiver_sleep(call_t *to, call_t *from){
  (void) to;
  (void) from;
}

void __wrap_transceiver_wakeup(call_t *to, call_t *from){
  (void) to;
  (void) from;
}

void __wrap_transceiver_switch_rx(call_t *to, call_t *from){
  (void) to;
  (void) from;
}

void __wrap_transceiver_switch_idle(call_t *to, call_t *from){
  (void) to;
  (void) from;
}

int __wrap_transceiver_get_modulation_bit_per_symbol(call_t *to, call_t *from){
  (void) to;
  (void) from;
  return 0;
}


int __wrap_transceiver_get_modulation_type(call_t *to, call_t *from){
  (void) to;
  (void) from;
  return 0;
}

void __wrap_transceiver_set_modulation_type(call_t *to, call_t *from, int modulation_type){
  (void) to;
  (void) from;
  (void) modulation_type;
}

uint64_t __wrap_transceiver_get_Ts(call_t *to, call_t *from){
  (void) to;
  (void) from;
  return 0;
}

void __wrap_transceiver_set_Ts(call_t *to, call_t *from, uint64_t Ts){
  (void) to;
  (void) from;
  (void) Ts;
}

uint64_t __wrap_transceiver_get_freq_low(call_t *to, call_t *from){
  (void) to;
  (void) from;
  return 0;
}

uint64_t __wrap_transceiver_get_freq_high(call_t *to, call_t *from){
  (void) to;
  (void) from;
  return 0;
}

void __wrap_transceiver_set_freq_low(call_t *to, call_t *from, uint64_t freq_low){
  (void) to;
  (void) from;
  (void) freq_low;
}

void __wrap_transceiver_set_freq_high(call_t *to, call_t *from, uint64_t freq_high){
  (void) to;
  (void) from;
  (void) freq_high;
}

#ifdef __cplusplus
}
#endif

#endif // WSNET_TEST_FAKES_MODEL_HANDLERS_TRANSCEIVER_H_
