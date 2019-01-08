/**
 *  \file   link.h
 *  \brief  Link utilities
 *  \author Luiz Henrique Suraty Filho
 *  \date   2018
 **/

#ifndef WSNET_TEST_FAKES_MODEL_HANDLERS_LINK_H_
#define WSNET_TEST_FAKES_MODEL_HANDLERS_LINK_H_

#include <kernel/include/model_handlers/link.h>


/* ************************************************** */
/* ************************************************** */

#ifdef __cplusplus
extern "C"{
#endif


link_condition_t __wrap_link_get_link_condition(call_t *to_link, call_t *to_interface, call_t *from_interface){
  (void) to_link;
  (void) to_interface;
  (void) from_interface;
  return LINK_CONDITION_LOS;
}

link_type_t __wrap_link_get_link_type(call_t *to_link, call_t *to_interface, call_t *from_interface){
  (void) to_link;
  (void) to_interface;
  (void) from_interface;
  return 0;
}

link_communication_type_t __wrap_link_get_communication_type(call_t *to_link, call_t *to_interface, call_t *from_interface){
  (void) to_link;
  (void) to_interface;
  (void) from_interface;
  return LINK_COMMUNICATION_TYPE_15_4;
}

int __wrap_link_get_complementary_link_condition(call_t *to_link, call_t *to_interface, call_t *from_interface){
  (void) to_link;
  (void) to_interface;
  (void) from_interface;
  return 0;
}

double __wrap_link_get_mutual_orientation(call_t *to_link, call_t *to_interface, call_t *from_interface){
  (void) to_link;
  (void) to_interface;
  (void) from_interface;
  return 0.0;
}

#ifdef __cplusplus
}
#endif

#endif //WSNET_TEST_FAKES_MODEL_HANDLERS_LINK_H_
