/**
 *  \file   modelutils.h
 *  \brief  Fake functions and objects for utility function declarations
 *  \author Luiz Henrique Suraty Filho
 *  \date   2018
 **/
#ifndef WSNET_TEST_MODEL_UTILS_H_
#define WSNET_TEST_MODEL_UTILS_H_

#include <kernel/include/modelutils.h>

/* ************************************************** */
/* ************************************************** */
#ifdef __cplusplus
extern "C"{
#endif

void __wrap_TX(call_t *to, call_t *from, packet_t *packet){
  (void) to;
  (void) from;
  (void) packet;
  return;
}
void __wrap_RX(call_t *to, call_t *from, packet_t *packet){
  (void) to;
  (void) from;
  (void) packet;
  return;
}
int __wrap_IOCTL(call_t *to, int option, void *in, void **out){
  (void) to;
  (void) in;
  (void) option;
  (void) out;
  return SUCCESSFUL;
}
int __wrap_SET_HEADER(call_t *to, call_t *from, packet_t *packet, destination_t *dst){
  (void) to;
  (void) from;
  (void) packet;
  (void) dst;
  return 1;
}

int __wrap_GET_HEADER_SIZE(call_t *to, call_t *from){
  (void) to;
  (void) from;
  return 0;
}
int __wrap_GET_HEADER_REAL_SIZE(call_t *to, call_t *from){
  (void) to;
  (void) from;
  return 0;
}

#ifdef __cplusplus
}
#endif


#endif // WSNET_TEST_MODEL_UTILS_H_
