/**
 *  \file   medium.h
 *  \brief  medium declarations
 *  \author Luiz Henrique Suraty Filho
 *  \date   2018
 **/
#ifndef WSNET_TEST_FAKES_DEFINITIONS_MEDIUM_H_
#define WSNET_TEST_FAKES_DEFINITIONS_MEDIUM_H_

#include <kernel/include/definitions/medium.h>

class DefinitionsMediumFake {
 public:
  static medium_t *get_medium_by_id(classid_t id) {
    (void) id;
    return &medium_info_;
  }

  static medium_t *get_medium_by_name(char *name){
    (void) name;
    return &medium_info_;
  }
  static medium_t medium_info_;
  static constexpr array_t    classes_ = {0,nullptr};
};


medium_t DefinitionsMediumFake::medium_info_ = {0,(char*)"fake_medium",0.0,0.0,DefinitionsMediumFake::classes_,0,0,0,0,0,0,0,DefinitionsMediumFake::classes_,DefinitionsMediumFake::classes_,DefinitionsMediumFake::classes_,nullptr};

/* ************************************************** */
/*                     WRAPPERS                       */
/* ************************************************** */

extern "C"{

int __wrap_get_mediumid_by_name(char *name){
  (void) name;
  return 0;
}
medium_t *__wrap_get_medium_by_name(char *name){
  return DefinitionsMediumFake::get_medium_by_name(name);
}
medium_t *__wrap_get_medium_by_id(mediumid_t id){
  return DefinitionsMediumFake::get_medium_by_id(id);
}

int __wrap_mediums_bootstrap(void){
  return 0;
}
void __wrap_mediums_clean(void){
  return;
}
int __wrap_mediums_unbind(void){
  return 0;
}

double __wrap_medium_get_pathloss(call_t *to_interface, call_t *from_interface, packet_t *packet, double rxdBm){
  (void) to_interface;
  (void) from_interface;
  (void) packet;
  (void) rxdBm;
  return 0.0;
}

double __wrap_medium_get_fading(call_t *to_interface, call_t *from_interface, packet_t *packet, double rxdBm){
  (void) to_interface;
  (void) from_interface;
  (void) packet;
  (void) rxdBm;
  return 0.0;
}

double __wrap_medium_get_shadowing(call_t *to_interface, call_t *from_interface, packet_t *packet, double rxdBm){
  (void) to_interface;
  (void) from_interface;
  (void) packet;
  (void) rxdBm;
  return 0.0;
}

int __wrap_medium_get_radio_link_condition(call_t *to_interface, call_t *from_interface){
  (void) to_interface;
  (void) from_interface;

  return 0;
}

void *__wrap_medium_get_spectrum_object(call_t *interface){
  (void) interface;
  return nullptr;
}

}

#endif // WSNET_TEST_FAKES_DEFINITIONS_MEDIUM_H_
