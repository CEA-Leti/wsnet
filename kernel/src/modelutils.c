/**
 *  \file   modelutils.c
 *  \brief  Model utilities
 *  \author Guillaume Chelius & Elyes Ben Hamida & Loic Lemaitre
 *  \date   2007
 **/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <kernel/include/definitions/class.h>
#include <kernel/include/definitions/types.h>


/* ************************************************** */
/* ************************************************** */
void TX(call_t *to, call_t *from, packet_t *packet) {
  class_t *class = get_class_by_id(to->class);
  if (class->implem.cxx.implem_type == CXX_IMPLEM){
    int item = *((int *) hashtable_retrieve(class->objects.indexes, (void *) &to->object));
    class->methods->generic_cpp.tx(class->objects.object[item],to, from, packet);
  }
  else {
    class->methods->generic.tx(to, from, packet);
  }
}
void RX(call_t *to, call_t *from, packet_t *packet) {
  class_t *class = get_class_by_id(to->class);
  if (class->implem.cxx.implem_type == CXX_IMPLEM){
    int item = *((int *) hashtable_retrieve(class->objects.indexes, (void *) &to->object));
    class->methods->generic_cpp.rx(class->objects.object[item],to, from, packet);
  }
  else {
    class->methods->generic.rx(to, from, packet);
  }
}

int IOCTL(call_t *to, int option, void *in, void **out) {
  class_t *class = get_class_by_id(to->class);
  return class->ioctl(to, option, in, out);
}

/* edit by Quentin Lampin <quentin.lampin@orange-ftgroup.com> */

int GET_HEADER_SIZE(call_t *to, call_t *from) {
  class_t *class = get_class_by_id(to->class);
  return class->methods->generic.get_header_size(to, from);
}

int SET_HEADER(call_t *to, call_t *from, packet_t *packet, destination_t *dst) {
  class_t *class = get_class_by_id(to->class);
  return class->methods->generic.set_header(to, from, packet, dst);
}


int GET_HEADER_REAL_SIZE(call_t *to, call_t *from) {
  class_t *class = get_class_by_id(to->class);
  return class->methods->generic.get_header_real_size(to, from);
}

/* end of edition */
