/*
 *  \file   dummy_link.c
 *  \brief  dummy link example
 *  \author Mickael Maman
 *  \date   2016
 */
#include <kernel/include/modelutils.h>
#include <models/models_dbg.h>


// ************************************************** //
// ************************************************** //

// Defining module informations//
model_t model =  {
    "dummy link example",
    "Mickael Maman",
    "0.1",
    MODELTYPE_LINK
};


/* ************************************************** */
/* ************************************************** */
int init(call_t *to, void *params) {
  (void) to;
  (void) params;

  return 0;
}

int destroy(call_t *to) {
  (void) to;

  return 0;
}

int bootstrap(call_t *to, void *params) {
  (void) to;
  (void) params;

  return 0;
}

int bind(call_t *to, void *params) {
  (void) to;
  (void) params;

  return 0;
}

int unbind(call_t *to) {
  (void) to;

  return 0;
}
/* ************************************************** */
/* ************************************************** */
link_type_t get_link_type(call_t *to_link, call_t *to_interface, call_t *from_interface) {
  (void) to_link;
  (void) to_interface;
  (void) from_interface;

  return LINK_TYPE_BAN_OFF_TO_OFF;
}

link_communication_type_t get_communication_type(call_t *to_link, call_t *to_interface, call_t *from_interface) {
  (void) to_link;
  (void) to_interface;
  (void) from_interface;

  return 0;
}


link_condition_t get_link_condition(call_t *to_link, call_t *to_interface, call_t *from_interface) {
  (void) to_link;
  (void) to_interface;
  (void) from_interface;

  return LINK_CONDITION_LOS;
}

double get_mutual_orientation(call_t *to_link, call_t *to_interface, call_t *from_interface) {
  (void) to_link;
  (void) to_interface;
  (void) from_interface;

  return 0.0;
}

int get_complementary_link_condition(call_t *to_link, call_t *to_interface, call_t *from_interface) {
  (void) to_link;
  (void) to_interface;
  (void) from_interface;

  return 1;
}


link_methods_t methods = {get_link_condition,
    get_link_type,
    get_communication_type,
    get_mutual_orientation,
    get_complementary_link_condition};
