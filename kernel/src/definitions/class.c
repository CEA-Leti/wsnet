/**
 *  \file   class.c
 *  \brief  Class module
 *  \author Guillaume Chelius & Loic Lemaitre
 *  \date   2007
 */

#include <string.h>
#include <stdio.h>

#include "class.h"
#include "node.h"
#include "nodearch.h"


class_array_t classes = {0, NULL};


/* ************************************************** */
/* ************************************************** */

unsigned long class_index_hash(void *key) {
  return (unsigned long) *((int*) key);
}

int class_index_equal(void *key0, void *key1) {
  return (*((int*) key0) == *((int*) key1)) ;
}


classid_t get_classid_by_name(char *name) {
  int i = classes.size;
  while (i--)
    if (!strcmp(name, (classes.elts + i)->name))
      break;
  return i;
}

class_t *get_class_by_id(classid_t id) {
  return classes.elts + id;
}

class_t *get_class_by_name(char *name) {
  classid_t id = get_classid_by_name(name);
  return (id < 0) ? NULL : get_class_by_id(id);
}

class_t *get_class_by_type(model_type_t type) {
  int id = classes.size;
  while (id--)
    if ((classes.elts + id)->model->type == type)
      break;
  return (id < 0) ? NULL : get_class_by_id(id);
}

void clean_cxx_class(class_t *class){
  if (!g_module_close (class->implem.cxx.library.module)) {
    if(class->implem.cxx.library.file)
      g_warning ("%s: %s", class->implem.cxx.library.file, g_module_error ());
  }

  if(class->implem.cxx.library.name){
    free(class->implem.cxx.library.name);
  }

  if(class->implem.cxx.library.file){
    free(class->implem.cxx.library.file);
  }

  int *index;

  while ((index = (int *) list_traverse(class->objects.saved_indexes)) != NULL) {
    free(index);
  }
  hashtable_destroy(class->objects.indexes);
  list_destroy(class->objects.saved_indexes);

  free(class->objects.object);
}

void clean_c_class(class_t *class){
  if (!g_module_close (class->implem.c.library.module)) {
    if(class->implem.c.library.file)
      g_warning ("%s: %s", class->implem.c.library.file, g_module_error ());
  }

  if(class->implem.c.library.name){
    free(class->implem.c.library.name);
  }

  if(class->implem.c.library.file){
    free(class->implem.c.library.file);
  }
}
/* ************************************************** */
/* ************************************************** */
void classes_clean(void) {
  int i;

  if (classes.elts && classes.size) {
    for (i = 0; i < classes.size; i++) {
      class_t *class = get_class_by_id(i);

      if (class->implem.cxx.implem_type == CXX_IMPLEM){
        clean_cxx_class(class);
      }

      if (class->implem.c.implem_type == C_IMPLEM){
        clean_c_class(class);
      }

      if (class->name) {
        free(class->name);
      }

      if (class->nodearchs.elts) {
        free(class->nodearchs.elts);
      }

      if (class->mediums.elts) {
        free(class->mediums.elts);
      }

      if (class->environments.elts) {
        free(class->environments.elts);
      }
    }
    free(classes.elts);
  }
}


/* ************************************************** */
/* ************************************************** */
int classes_destroy(void)
{
  int i = classes.size;
  int j;

  while (i--)
  {
    class_t *class = get_class_by_id(i);
    call_t to = {class->id, -1};
    if (class->destroy)
    {
      if(class->destroy(&to))
        return -1;
    }
    if (class->destroy_object){
      for (j=0; j<class->objects.size; j++){
        class->destroy_object(class->objects.object[j]);
      }
    }

  }

  return 0;
}

/* ************************************************** */
/* ************************************************** */
void *get_class_private_data(call_t *to) {
  class_t *class = get_class_by_id(to->class);
  return class->private;
}

void set_class_private_data(call_t *to, void *data) {
  class_t *class = get_class_by_id(to->class);
  class->private = data;
}


/* ************************************************** */
/* ************************************************** */
int get_class_type(call_t *to) {
  class_t *class = get_class_by_id(to->class);
  return class->model->type;
}


/* ************************************************** */
/* ************************************************** */
array_t *get_class_bindings_up(call_t *to) {
  class_t    *class    = get_class_by_id(to->class);
  node_t     *node     = get_node_by_id(to->object);
  nodearch_t *nodearch = get_nodearch_by_id(node->nodearch);

  return nodearch->up + class->nodearchs.elts[nodearch->id];
}


array_t *get_class_bindings_down(call_t *to) {
  class_t    *class    = get_class_by_id(to->class);
  node_t     *node     = get_node_by_id(to->object);
  nodearch_t *nodearch = get_nodearch_by_id(node->nodearch);

  return nodearch->down + class->nodearchs.elts[nodearch->id];
}


/* ************************************************** */
/* ************************************************** */
static inline nodearch_t *get_nodearch_for_node(call_t *to) {
  node_t *node = get_node_by_id(to->object);
  return get_nodearch_by_id(node->nodearch);
}


array_t *get_application_classesid(call_t *to) {
  return &get_nodearch_for_node(to)->applications;
}

array_t *get_routing_classesid(call_t *to) {
  return &get_nodearch_for_node(to)->routings;
}

array_t *get_mac_classesid(call_t *to) {
  return &get_nodearch_for_node(to)->macs;
}

array_t *get_tranceiver_classesid(call_t *to) {
  return &get_nodearch_for_node(to)->transceivers;
}

array_t *get_interface_classesid(call_t *to) {
  return &get_nodearch_for_node(to)->interfaces;
}

array_t *get_sensor_classesid(call_t *to) {
  return &get_nodearch_for_node(to)->sensors;
}

array_t *get_monitor_classesid(call_t *to) {
  return &get_nodearch_for_node(to)->monitors;
}

classid_t get_energy_classid(call_t *to) {
  return get_nodearch_for_node(to)->energy;
}

classid_t get_mobility_classid(call_t *to) {
  return get_nodearch_for_node(to)->mobility;
}

classid_t get_mobility_classid_for_node_id(nodeid_t id) {
  node_t     *node     = get_node_by_id(id);
  return get_nodearch_by_id(node->nodearch)->mobility;
}

classid_t get_map_classid(void) {
  return get_class_by_type(MODELTYPE_MAP)->id;
}

void * get_object(call_t *to){
  class_t *class = get_class_by_id(to->class);
  return class->objects.object[to->object];
}


void * get_first_object_binding_down_by_type(call_t *to, int type){
  array_t *classes_down = get_class_bindings_down(to);
  int i;
  class_t *obj_class = NULL;
  for (i=0;i<classes_down->size;i++){
    call_t new_classid = {classes_down->elts[i],to->object};
    if (get_class_type(&new_classid)== type){
      obj_class = get_class_by_id(classes_down->elts[i]);
    }
  }
  if (obj_class==NULL){
    return NULL;
  }
  else {
    int item = *((int *) hashtable_retrieve(obj_class->objects.indexes, (void *) &to->object));
    return obj_class->objects.object[item];
  }
}


void * get_object_binding_down_by_type_and_name(call_t *to, int type, char * name){
  array_t *classes_down = get_class_bindings_down(to);
  int i;
  class_t *obj_class = NULL;
  for (i=0;i<classes_down->size;i++){
    call_t new_classid = {classes_down->elts[i],to->object};
    if (get_class_type(&new_classid)== type){
      obj_class = get_class_by_id(classes_down->elts[i]);
      if (!strcmp(name, obj_class->name)){
        break;
      }
    }
  }
  if (obj_class==NULL){
    return NULL;
  }
  else {
    int item = *((int *) hashtable_retrieve(obj_class->objects.indexes, (void *) &to->object));
    return obj_class->objects.object[item];
  }
}

/* ************************************************** */
/* ************************************************** */
char *implem_type_to_str(int type)
{
  switch(type)
  {
    case CXX_IMPLEM      : return "C++";
    case C_IMPLEM      : return "C";
    case PYTHON_IMPLEM : return "PYTHON";    
    case RUBY_IMPLEM   : return "RUBY";
  }

  return "unknown";
}
