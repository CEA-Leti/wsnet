/**
 *  \file   interface.c
 *  \brief  Interface utilities
 *  \author Guillaume Chelius & Elyes Ben Hamida & Loic Lemaitre
 *  \date   2007
 **/

#include <kernel/include/definitions/types.h>
#include <kernel/include/definitions/class.h>


/* ************************************************** */
/* ************************************************** */
double interface_get_loss(call_t *to, call_t *from)
{
  class_t *class = get_class_by_id(to->class);
  if (class->model->type != MODELTYPE_INTERFACE)
    return 0;
  return class->methods->interface.get_loss(to, from);
}

angle_t *interface_get_angle(call_t *to, call_t *from)
{
  class_t *class = get_class_by_id(to->class);
  if (class->model->type != MODELTYPE_INTERFACE)
    return NULL;
  return class->methods->interface.get_angle(to, from);
}

void interface_set_angle(call_t *to, call_t *from, angle_t *angle)
{
  class_t *class = get_class_by_id(to->class);
  if (class->model->type != MODELTYPE_INTERFACE)
    return;
  class->methods->interface.set_angle(to, from, angle);
}

void interface_cs(call_t *to, call_t *from, packet_t *packet)
{
  class_t *class = get_class_by_id(to->class);
  if (class->model->type != MODELTYPE_INTERFACE)
    return;
  return class->methods->interface.cs(to, from, packet);
}

void interface_rx(call_t *to, call_t *from, packet_t *packet)
{
  class_t *class = get_class_by_id(to->class);
  if (class->model->type != MODELTYPE_INTERFACE)
    return;
  class->methods->interface.rx(to, from, packet);
}

double interface_gain_tx(call_t *to, call_t *from, position_t *pos)
{
  class_t *class = get_class_by_id(to->class);
  if (class->model->type != MODELTYPE_INTERFACE)
    return 0;
  return class->methods->interface.gain_tx(to, from, pos);
}

double interface_gain_rx(call_t *to, call_t *from, position_t *pos)
{
  class_t *class = get_class_by_id(to->class);
  if (class->model->type != MODELTYPE_INTERFACE)
    return 0;
  return class->methods->interface.gain_rx(to, from, pos);
}

mediumid_t interface_get_medium(call_t *to, call_t *from)
{
  class_t *class = get_class_by_id(to->class);
  if (class->model->type != MODELTYPE_INTERFACE)
    return 0;
  return class->methods->interface.get_medium(to, from);
}

interface_type_t interface_get_type(call_t *to)
{
  class_t *class = get_class_by_id(to->class);
  if (class->model->type != MODELTYPE_INTERFACE)
    return 0;
  return class->methods->interface.get_type(to);
}
