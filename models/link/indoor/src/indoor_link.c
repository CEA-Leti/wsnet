/*
 *  \file   indoor_link.c
 *  \brief  Indoor link model
 *  \author Luiz Henrique Suraty Filho
 *  \date   2016
 */
#include <kernel/include/modelutils.h>
#include <models/models_dbg.h>


// ************************************************** //
// ************************************************** //

// Defining module informations//
model_t model =  {
    "Indoor link model",
    "Luiz Henrique Suraty Filho",
    "0.1",
    MODELTYPE_LINK
};


/* ************************************************** */
/* ************************************************** */
int init(call_t *to, void *params) {
  return 0;
}

int destroy(call_t *to) {
  return 0;
}

int bootstrap(call_t *to, void *params) {
  return 0;
}

int bind(call_t *to, void *params) {
  return 0;
}

int unbind(call_t *to) {
  return 0;
}
/* ************************************************** */
/* ************************************************** */

/** \brief A function that computes the radio link type (LINK_TYPE_BAN_BELT_TO_HAND, LINK_TYPE_BAN_BELT_TO_EAR, LINK_TYPE_BAN_HAND_TO_EAR, LINK_TYPE_BAN_BELT_TO_OFF, LINK_TYPE_BAN_HAND_TO_OFF, LINK_TYPE_BAN_EAR_TO_OFF).
 *  \fn  int get_link_type(call_t *to_link, call_t *to_interface, call_t *from_interface)
 *  \param to_link is the link
 *  \param from_interface is the interface of the transmitter
 *  \param to_interface is the interface of the receiver
 *  \return the radio link type: LINK_TYPE_BAN_BELT_TO_HAND, LINK_TYPE_BAN_BELT_TO_EAR, LINK_TYPE_BAN_HAND_TO_EAR, LINK_TYPE_BAN_BELT_TO_OFF, LINK_TYPE_BAN_HAND_TO_OFF, LINK_TYPE_BAN_EAR_TO_OFF
 **/
link_type_t get_link_type(call_t *to_link, call_t *to_interface, call_t *from_interface) {

  return -1;

}


/** \brief A function that computes the type of the communication (LINK_COMMUNICATION_TYPE_BAN, LINK_COMMUNICATION_TYPE_B2B) according to the environment modeling.
 *  \fn  int get_communication_type(call_t *to_link, call_t *to_interface, call_t *from_interface)
 *  \param to_link is the link
 *  \param from_interface is the interface of the transmitter
 *  \param to_interface is the interface of the receiver
 *  \return the radio link conditions: "LINK_COMMUNICATION_TYPE_B2B, LINK_COMMUNICATION_TYPE_BAN"
 **/
link_communication_type_t get_communication_type(call_t *to_link, call_t *to_interface, call_t *from_interface) {


  return LINK_COMMUNICATION_TYPE_15_4;

}


/** \brief A function that computes the radio link condition (LOS, NLOS or NLOS2) according to the environment modeling.
 *  \fn int get_link_condition(call_t *to_link, call_t *to_interface, call_t *from_interface)
 *  \param to_link is the link
 *  \param from_interface is the interface of the transmitter
 *  \param to_interface is the interface of the receiver
 *  \return the radio link conditions: "LOS, NLOS, NLOS2 or NLOS3"
 **/
link_condition_t get_link_condition(call_t *to_link, call_t *to_interface, call_t *from_interface) {

  call_t to_map = {get_map_classid(), to_link->object};

  map_conditions_t map_condition = map_get_scenario_condition(&to_map, from_interface, from_interface->object, to_interface->object);

  switch (map_condition){

    case MAP_SAME_ROOM:
      return LINK_CONDITION_LOS;
      break;

    case MAP_ADJACENT_ROOM:
      return LINK_CONDITION_NLOS;
      break;

    case MAP_OTHER_ROOM:
      return LINK_CONDITION_NLOS2;
      break;

    case MAP_FAR_AWAY_ROOM:
      return LINK_CONDITION_NLOS3;
      break;

    default:
      return LINK_CONDITION_NLOS3;
      break;
  }

}

double get_mutual_orientation(call_t *to_link, call_t *to_interface, call_t *from_interface) {
  double mutual_orientation;

  // we first get the mobility mutual orientation

  call_t to_mobility_src = {get_mobility_classid_for_node_id(from_interface->object),from_interface->object};
  call_t to_mobility_dst = {get_mobility_classid_for_node_id(to_interface->object),to_interface->object};
  mutual_orientation = mobility_get_mutual_orientation(&to_mobility_src, &to_mobility_dst);

  return mutual_orientation;

}

int get_complementary_link_condition(call_t *to_link, call_t *to_interface, call_t *from_interface) {
  return -1;

}


link_methods_t methods = {get_link_condition,
    get_link_type,
    get_communication_type,
    get_mutual_orientation,
    get_complementary_link_condition};
