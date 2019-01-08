/**
 *  \file   node.h
 *  \brief  Fake functions and objects for node
 *  \author Luiz Henrique Suraty Filho
 *  \date   2018
 **/

#ifndef WSNET_TEST_FAKES_DEFINITIONS_NODE_H_
#define WSNET_TEST_FAKES_DEFINITIONS_NODE_H_

#include <kernel/include/definitions/node.h>

class DefinitionsNodeFake {
 public:
  static void *get_node_private_data(call_t *to) {
    (void) to;
    return node_data_;
  }

  static void set_node_private_data(call_t *to, void *data) {
    (void) to;
    node_data_ = data;
  }

  static void *node_data_;
  static node_t *node_info_;
  static int number_nodes_;

};

void *DefinitionsNodeFake::node_data_ = nullptr;
node_t *DefinitionsNodeFake::node_info_ = nullptr;
int DefinitionsNodeFake::number_nodes_ = 0;



/* ************************************************** */
/*                     WRAPPERS                       */
/* ************************************************** */

extern "C"{
void *__wrap_get_node_private_data(call_t *to) {
  return DefinitionsNodeFake::get_node_private_data(to);
}

void __wrap_set_node_private_data(call_t *to, void *data) {
  DefinitionsNodeFake::set_node_private_data(to,data);
}

node_t *__wrap_get_node_by_id(nodeid_t id){
  (void) id;
  return DefinitionsNodeFake::node_info_;
}

position_t *__wrap_get_node_position(nodeid_t node){
  (void) node;
  return &(DefinitionsNodeFake::node_info_->position);
}

void __wrap_nodes_clean(void){
  return;
}

void __wrap_nodes_update_mobility(void){
  return;
}

int __wrap_get_node_count(void){
  return DefinitionsNodeFake::number_nodes_;
}

void __wrap_node_birth(nodeid_t id){
  (void) id;
  DefinitionsNodeFake::number_nodes_++;
  return;
}

int __wrap_is_node_alive(nodeid_t id){
  (void) id;
  return 1;
}

void __wrap_node_kill(nodeid_t id){
  (void) id;
  //DefinitionsNodeFake::node_info_->state = NODE_DEAD;
  return;
}

int __wrap_nodes_bootstrap(void){
  return 0;
}

int __wrap_nodes_create(void){
  return 0;
}

int __wrap_nodes_init(void){
  return 0;
}

int __wrap_nodes_unbind(void){
  return 0;
}

void __wrap_print_node_groups(nodeid_t _nodeid){
  (void) _nodeid;
  return;
}

array_t __wrap_get_node_groups(nodeid_t _nodeid){
  (void) _nodeid;
  return DefinitionsNodeFake::node_info_->groups;
}

int __wrap_add_group_to_node(nodeid_t _nodeid, groupid_t _groupid){
  (void) _nodeid;
  (void) _groupid;
  /*	DefinitionsNodeFake::node_info_->groups.size++;
	if ( (DefinitionsNodeFake::node_info_->groups.elts = (int*) realloc(DefinitionsNodeFake::node_info_->groups.elts, sizeof(int) * (DefinitionsNodeFake::node_info_->groups.size)) ) == NULL ){
	  return -1;
	}
	DefinitionsNodeFake::node_info_->groups.elts[DefinitionsNodeFake::node_info_->groups.size-1] = _groupid;*/
  return 0;
}

int __wrap_get_node_type_by_family(nodeid_t id, int family){
  (void) id;
  (void) family;

  return 0;
}

node_type_t *__wrap_get_node_types(nodeid_t id){
  (void) id;
  return DefinitionsNodeFake::node_info_->types.elts;
}

int	__wrap_get_node_types_size(nodeid_t id){
  (void) id;
  return DefinitionsNodeFake::node_info_->types.size;
}

bool __wrap_check_node_type(nodeid_t id, int type){
  (void) id;
  int i;
  for (i=0;i < DefinitionsNodeFake::node_info_->types.size; i++){
    if (DefinitionsNodeFake::node_info_->types.elts[i].type == type){
      return true;
    }
  }
  return false;
}

array_t __wrap_get_nodesid_by_nodearchid(nodearchid_t _nodearchid){
  (void) _nodearchid;
  array_t nodes_by_arch = {0,NULL};
  return nodes_by_arch;
}

// no need to wrap these functions
//double distance(position_t *position0, position_t *position1);


}
#endif //WSNET_TEST_FAKES_DEFINITIONS_NODE_H_
