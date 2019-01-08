/**
 *  \file   mobility.h
 *  \brief  Fake Mobility declarations
 *  \author Luiz Henrique Suraty Filho
 *  \date   2018
 **/
#ifndef WSNET_TEST_FAKES_MODEL_HANDLERS_NODE_MOBILITY_H_
#define WSNET_TEST_FAKES_MODEL_HANDLERS_NODE_MOBILITY_H_

#include <kernel/include/model_handlers/node_mobility.h>
#ifdef __cplusplus
extern "C"{
#endif


int  __wrap_mobility_init(void){
  return 0;
}

int  __wrap_mobility_bootstrap(void){
  return 0;
}


void __wrap_mobility_clean(void){
  return;
}


void __wrap_mobility_update(uint64_t clock){
  (void) clock;
  return;
}


void __wrap_mobility_event(uint64_t clock){
  (void) clock;
  return;
}


angle_t __wrap_mobility_get_angle(call_t *to){
  (void) to;
  return {0.0,0.0};
}


double __wrap_mobility_get_speed(call_t *to){
  (void) to;
  return 0.0;
}


double __wrap_mobility_get_mutual_orientation(call_t *mobility_src, call_t *mobility_dst){
  (void) mobility_src;
  (void) mobility_dst;
  return 0.0;
}


#ifdef __cplusplus
}
#endif

#endif //WSNET_TEST_FAKES_MODEL_HANDLERS_NODE_MOBILITY_H_
