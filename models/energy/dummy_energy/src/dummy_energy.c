/**
 *  \file   dummy_energy.c
 *  \brief  Dummy energy model
 *  \author Luiz Henrique Suraty Filho
 *  \date   2016
 **/

#include <kernel/include/definitions/types.h>
#include <kernel/include/definitions/class.h>
#include <kernel/include/definitions/node.h>
#include <models/models_dbg.h>


/* ************************************************** */
/* ************************************************** */
model_t model =  {
    "Dummy energy model",
    "Luiz Henrique Suraty Filho",
    "0.1",
    MODELTYPE_ENERGY
};


/* ************************************************** */
/* ************************************************** */
struct nodedata {
};


/* ************************************************** */
/* ************************************************** */
int init(call_t *to, void *params) {
  DBG_ENERGY("model dummy_energy.c: initializing class %s\n",
	     get_class_by_id(to->class)->name);
  return 0;
}

int destroy(call_t *to) {
  return 0;
}

int bootstrap(call_t *to, void *params) {
  return 0;
}


/* ************************************************** */
/* ************************************************** */
int bind(call_t *to, void *params) {
  DBG_ENERGY("model dummy_energy.c: binding class %s with node %d\n",
	     get_class_by_id(to->class)->name, get_node_by_id(to->object)->id);
  return 0;
}

int unbind(call_t *to) {
  return 0;
}

/* ************************************************** */
/* ************************************************** */

void recharge(call_t *to, double energy) {
    return;
}

void consume(call_t *to, double current, uint64_t duration) {
    return;
}

double energy_consumed(call_t *to) {
    return 0.0;
}

double energy_recharged(call_t *to) {
  return 0.0;
}

double energy_remaining(call_t *to) {
    return 1.0;
}

double energy_status(call_t *to) {
return 1.0;
}

double get_supply_voltage(call_t *to){
  return 3.0;
}

/* ************************************************** */
/* ************************************************** */
energy_methods_t methods = {recharge,
    consume,
    energy_consumed,
    energy_recharged,
    energy_remaining,
    energy_status,
    get_supply_voltage};

