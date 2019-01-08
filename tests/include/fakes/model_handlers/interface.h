/**
 *  \file   interface.h
 *  \brief  Fake Interface declarations
 *  \author Luiz Henrique Suraty Filho
 *  \date   2018
 **/
#ifndef WSNET_TEST_FAKES_MODEL_HANDLERS_INTERFACE_H_
#define WSNET_TEST_FAKES_MODEL_HANDLERS_INTERFACE_H_

#include <kernel/include/model_handlers/interface.h>

#ifdef __cplusplus
extern "C"{
#endif

double __wrap_interface_get_loss(call_t *to, call_t *from){
  (void) to;
  (void) from;
  return 0.0;
}

angle_t *__wrap_interface_get_angle(call_t *to, call_t *from){
  (void) to;
  (void) from;
  return nullptr;
}

void __wrap_interface_set_angle(call_t *to, call_t *from, angle_t *angle){
  (void) to;
  (void) from;
  (void) angle;
  return;
}

void __wrap_interface_rx(call_t *to, call_t *from, packet_t *packet){
  (void) to;
  (void) from;
  (void) packet;
  return;
}

void __wrap_interface_cs(call_t *to, call_t *from, packet_t *packet){
  (void) to;
  (void) from;
  (void) packet;
  return;
}

double __wrap_interface_gain_tx(call_t *to, call_t *from, position_t *position){
  (void) to;
  (void) from;
  (void) position;
  return 0.0;
}

double __wrap_interface_gain_rx(call_t *to, call_t *from, position_t *position){
  (void) to;
  (void) from;
  (void) position;
  return 0.0;
}

mediumid_t __wrap_interface_get_medium(call_t *to, call_t *from){
  (void) to;
  (void) from;
  return 0;
}

interface_type_t __wrap_interface_get_type(call_t *to){
  (void) to;
  return INTERFACE_TYPE_ANTENNA_OMNIDIRECTIONAL;
}

#ifdef __cplusplus
}
#endif

#endif //WSNET_TEST_FAKES_MODEL_HANDLERS_INTERFACE_H_
