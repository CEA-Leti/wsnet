/**
 *  \file   energy.h
 *  \brief  Energy utilities
 *  \author Luiz Henrique Suraty Filho
 *  \date   2016
 **/

#ifndef WSNET_TEST_FAKES_MODEL_HANDLERS_ENERGY_H_
#define WSNET_TEST_FAKES_MODEL_HANDLERS_ENERGY_H_

#include <kernel/include/model_handlers/energy.h>

/* ************************************************** */
/* ************************************************** */
#ifdef __cplusplus
extern "C"{
#endif

void __wrap_energy_recharge(call_t *to, double energy){
  (void) to;
  (void) energy;
}


void __wrap_energy_consume(call_t *to, double current, uint64_t duration) {
  (void) to;
  (void) current;
  (void) duration;
}

double __wrap_energy_check_energy_recharged(call_t *to) {
  (void) to;
  return 0.0;
}

double __wrap_energy_check_energy_consumed(call_t *to) {
  (void) to;
  return 0.0;
}

double __wrap_energy_check_energy_remaining(call_t *to) {
  (void) to;
  return 0.0;
}

double __wrap_energy_check_energy_status(call_t *to) {
  (void) to;
  return 1.0;
}

double __wrap_energy_get_supply_voltage(call_t *to) {
  (void) to;
  return 3.0;
}

#ifdef __cplusplus
}
#endif

#endif //WSNET_TEST_FAKES_MODEL_HANDLERS_ENERGY_H_
