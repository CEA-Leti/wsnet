/**
 *  \file   class.h
 *  \brief  Fake functions and objects for class
 *  \author Luiz Henrique Suraty Filho
 *  \date   2018
 **/

#ifndef WSNET_TEST_FAKES_DEFINITIONS_CLASS_H_
#define WSNET_TEST_FAKES_DEFINITIONS_CLASS_H_

#include <kernel/include/definitions/class.h>

class DefinitionsClassFake {
 public:
  static void *get_class_private_data(call_t *to) {
    (void) to;
    return class_data_;
  }

  static void set_class_private_data(call_t *to, void *data) {
    (void) to;
    class_data_ = data;
  }

  static class_t *get_class_by_id(classid_t id) {
    (void) id;
    return class_info_;
  }

  static void set_bindings(){
    bindings_ = new array_t;
    bindings_->elts = elts;
  }

  static array_t * get_bindings(){
    if (bindings_==nullptr){
      set_bindings();
    }
    return bindings_;
  }

  static void *class_data_;
  static class_t *class_info_;
  static array_t *bindings_;
  static int elts[1];
  static void *object_;

};


int DefinitionsClassFake::elts[] = {0};
void *DefinitionsClassFake::class_data_ = nullptr;
class_t *DefinitionsClassFake::class_info_ = nullptr;
array_t *DefinitionsClassFake::bindings_ = nullptr;
void *DefinitionsClassFake::object_ = nullptr;


/* ************************************************** */
/*                     WRAPPERS                       */
/* ************************************************** */

extern "C"{

void *__wrap_get_class_private_data(call_t *to) {
  return DefinitionsClassFake::get_class_private_data(to);
}

void __wrap_set_class_private_data(call_t *to, void *data) {
  DefinitionsClassFake::set_class_private_data(to,data);
}

int __wrap_get_class_type(call_t *to) {
  (void) to;
  return MODELTYPE_UNKNOW;
}

int __wrap_get_classid_by_name(char *name){
  (void) name;
  return 0;
}

class_t *__wrap_get_class_by_id(classid_t id){
  return DefinitionsClassFake::get_class_by_id(id);
}

class_t *__wrap_get_class_by_name(char *name){
  (void) name;
  return DefinitionsClassFake::get_class_by_id(0);
}

void __wrap_classes_clean(void){
  return;
}


int __wrap_classes_destroy(void){
  return SUCCESSFUL;
}

classid_t __wrap_get_energy_classid(call_t *to){
  (void) to;
  return 0;
}
classid_t __wrap_get_mobility_classid(call_t *to){
  (void) to;
  return 0;
}
classid_t __wrap_get_mobility_classid_for_node_id(nodeid_t id){
  (void) id;
  return 0;
}
classid_t __wrap_get_map_classid(void){
  return 0;
}

array_t *__wrap_get_class_bindings_up(call_t *to){
  (void) to;
  return DefinitionsClassFake::get_bindings();
}

array_t *__wrap_get_class_bindings_down(call_t *to){
  (void) to;
  return DefinitionsClassFake::get_bindings();
}

array_t *__wrap_get_application_classesid(call_t *to){
  (void) to;
  return DefinitionsClassFake::get_bindings();
}

array_t *__wrap_get_routing_classesid(call_t *to){
  (void) to;
  return DefinitionsClassFake::get_bindings();
}

array_t *__wrap_get_mac_classesid(call_t *to){
  (void) to;
  return DefinitionsClassFake::get_bindings();
}

array_t *__wrap_get_tranceiver_classesid(call_t *to){
  (void) to;
  return DefinitionsClassFake::get_bindings();
}

array_t *__wrap_get_interface_classesid(call_t *to){
  (void) to;
  return DefinitionsClassFake::get_bindings();
}

array_t *__wrap_get_sensor_classesid(call_t *to){
  (void) to;
  return DefinitionsClassFake::get_bindings();
}

array_t *__wrap_get_monitor_classesid(call_t *to){
  (void) to;
  return DefinitionsClassFake::get_bindings();
}

void *__wrap_get_object(call_t *to){
  (void) to;
  return DefinitionsClassFake::object_;
}
void *__wrap_get_first_object_binding_down_by_type(call_t *to, int type){
  (void) to;
  (void) type;
  return DefinitionsClassFake::object_;
}

void *__wrap_get_object_binding_down_by_type_and_name(call_t *to, int type, char * name){
  (void) to;
  (void) type;
  (void) name;
  return DefinitionsClassFake::object_;
}


// no need to wrap these functions
//char *implem_type_to_str(int type);
//unsigned long class_index_hash(void *key);
//int class_index_equal(void *key0, void *key1);

}

#endif //WSNET_TEST_FAKES_DEFINITIONS_CLASS_H_
