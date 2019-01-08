/**
 *  \file   energy.c
 *  \brief  Energy utilities
 *  \author Luiz Henrique Suraty Filho
 *  \date   2016
 **/
#include <kernel/include/definitions/types.h>
#include <kernel/include/definitions/class.h>
#include <kernel/include/definitions/node.h>
#include <kernel/include/definitions/nodearch.h>
#include <stdio.h>

static inline class_t *energy_entity(call_t *to, call_t *to0) {
  node_t   *node   = get_node_by_id(to->object);
  nodearch_t *nodearch = get_nodearch_by_id(node->nodearch);
  class_t *class = NULL;

  if (nodearch->energy >= 0) {
    class     = get_class_by_id(nodearch->energy);
    to0->class = class->id;
    to0->object   = to->object;
  }
  return class;
}

void energy_recharge(call_t *to, double energy)
{
  call_t to0;
  class_t *class = energy_entity(to, &to0);

  if(class)
    class->methods->energy.recharge(&to0, energy);
}


void energy_consume(call_t *to, double current, uint64_t duration)
{
  call_t to0;
  class_t *class = energy_entity(to, &to0);

  if(class)
    class->methods->energy.consume(&to0,current, duration);
}

double energy_check_energy_recharged(call_t *to) {
  call_t    to0;
  class_t *class = energy_entity(to, &to0);
  return (class ? class->methods->energy.energy_recharged(&to0) : -1);
}

double energy_check_energy_consumed(call_t *to) {
  call_t    to0;
  class_t *class = energy_entity(to, &to0);
  return (class ? class->methods->energy.energy_consumed(&to0) : -1);
}

double energy_check_energy_remaining(call_t *to) {
  call_t    to0;
  class_t *class = energy_entity(to, &to0);
  return (class ? class->methods->energy.energy_remaining(&to0) : -1);
}

double energy_check_energy_status(call_t *to) {
  call_t    to0;
  class_t *class = energy_entity(to, &to0);
  return (class ? class->methods->energy.energy_status(&to0) : -1);
}

double energy_get_supply_voltage(call_t *to) {
  call_t    to0;
  class_t *class = energy_entity(to, &to0);
  return (class ? class->methods->energy.get_supply_voltage(&to0) : -1);
}
