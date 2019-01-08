
#include "radio_halfduplex_spectrum.h"


void set_primitive(char* primitive, packet_t *packet){
  //  create and insert DATA header into the packet
  uint *primitive_header = (uint *) malloc(sizeof(uint));
  field_t *field_data_header = field_create(UINT, sizeof(uint), primitive_header);
  packet_add_field(packet, (char*) primitive, field_data_header);
}


// This is temporarily being done with dummy packets for now.
// There should be a proper interface between radio and phy to provide
// the exchange of primitives
void register_on_spectrum(call_t *to){
  array_t *down = get_class_bindings_down(to);
  int i = down->size;
  while (i--) {
    packet_t *packet_primitive = packet_create(to,-1,-1);
    set_primitive((char*) "PHY-SET-REGISTER-RX.request", packet_primitive);
    call_t from0 = {down->elts[i], to->object};
    TX(&from0, to, packet_primitive);
  }
}

void unregister_on_spectrum(call_t *to){
  array_t *down = get_class_bindings_down(to);
  int i = down->size;
  while (i--) {
    packet_t *packet_primitive = packet_create(to,-1,-1);
    set_primitive((char*) "PHY-RESET-REGISTER-RX.request", packet_primitive);
    call_t from0 = {down->elts[i], to->object};
    TX(&from0, to, packet_primitive);
  }
}

void set_node_default_values(halfduplex_radio_transceiver_node_private_t *nodedata){
  nodedata->bandwidth = 100000;
  nodedata->channel = 0;
  nodedata->log_status = 0;
  nodedata->modulation = -1;
  nodedata->freq_center = 868037500;
  nodedata->freq_low  = nodedata->freq_center-nodedata->bandwidth/2;
  nodedata->freq_high = nodedata->freq_center+nodedata->bandwidth/2;;

  nodedata->mindBm = RADIO_CC_SENSITIVITY;
  nodedata->rxdBm = nodedata->mindBm;
  nodedata->noise_factor_dB = 6;
  nodedata->state = RADIO_SLEEP;

  //default values for the energy consumption (Joule / ns)
  nodedata->energy.current_draw_mA[RADIO_SLEEP]  = RADIO_CC_SLEEP;
  nodedata->energy.current_draw_mA[RADIO_IDLE]   = RADIO_CC_IDLE;
  nodedata->energy.current_draw_mA[RADIO_RX]     = RADIO_CC_RX;
  nodedata->energy.current_draw_mA[RADIO_RXING]  = RADIO_CC_RXING;
  nodedata->energy.current_draw_mA[RADIO_TX]     = RADIO_CC_TX;

  int i;
  for(i=0; i<RADIO_NUMBER_STATES; i++){
    nodedata->energy.duration_ns[i]    = 0;
    nodedata->energy.consumption_J[i] = 0;
  }
}
//consumes the energy according to the current state since the last state change
//NB: we could introduce here energy for changing the state of the radio
void radio_energy_update(call_t *to){
  halfduplex_radio_transceiver_node_private_t *nodedata = (halfduplex_radio_transceiver_node_private_t *) get_node_private_data(to);
  uint64_t duration;

  //updates the energy consumption according to the model
  switch (nodedata->state) {
    case RADIO_SLEEP:
    case RADIO_IDLE:
    case RADIO_RX:
    case RADIO_RXING:
    case RADIO_TX:

      // updates the energy consumption before the state changes!
      duration = get_time() - nodedata->state_time;
      nodedata->energy.duration_ns[nodedata->state]       += duration;
      // 1J = 1 watt second = 1 A * V * s
      // Duration is in nanoseconds, we need to transfer it to seconds
      // Current is in mA, we need to convert it to A
      nodedata->energy.consumption_J[nodedata->state]    += TIME_TO_SECONDS(duration) * (nodedata->energy.current_draw_mA[nodedata->state] * 1e-3) * energy_get_supply_voltage(to) ;

      //nodedata->energy.current_draw_mA is on J/ns, we need to go back to mA
      energy_consume(to,nodedata->energy.current_draw_mA[nodedata->state], duration);

      break;

    default:
      fprintf(stderr, "[RADIO] %d -> unknown state %d\n", to->object, nodedata->state);
      exit(5);

      break;
  }
}

void radio_switch_state(call_t *to, halfduplex_radio_states_t state){
  halfduplex_radio_transceiver_node_private_t *nodedata = (halfduplex_radio_transceiver_node_private_t *) get_node_private_data(to);

  if (nodedata->state == state) {
    return;
  }

  if ((state == RADIO_RX) && ( (nodedata->state != RADIO_RXING) && (nodedata->state != RADIO_RX)) ){
    register_on_spectrum(to);
  }

  if ((state == RADIO_IDLE) || (state == RADIO_SLEEP) || (state == RADIO_TX)){
    unregister_on_spectrum(to);
    nodedata->rxdBm = nodedata->mindBm;
  }
  // update energy consumption
  radio_energy_update(to);

  //fprintf(stderr, "  {%lu} [RADIO] Node %d switch state %s -> %s\n", get_time(), to->object, radio_state_names[nodedata->state], radio_state_names[state]);

  // new state
  nodedata->state        = state;
  nodedata->state_time   = get_time();

}

int read_nodedata_from_xml_file(halfduplex_radio_transceiver_node_private_t *nodedata, void *params){
  param_t *param;

  //default values
  nodedata->symbol_time = (uint64_t) 1 * (uint64_t) SECONDS/ (uint64_t) RADIO_CC_DATA_RATE;
  nodedata->channel = 0;
  nodedata->power = 14;
  nodedata->modulation = -1;
  nodedata->mindBm = RADIO_CC_SENSITIVITY;

  nodedata->freq_low  = 868000000;
  nodedata->freq_high = 868010000;

  //energy for all the modes (BE CAREFUL: RADIO_TX is the largest mode value)
  int i;
  for(i=0; i<RADIO_NUMBER_STATES; i++){
    nodedata->energy.duration_ns[i]    = 0;
    nodedata->energy.consumption_J[i] = 0;
  }
  //default values for the energy consumption (Joule / ns)
  nodedata->energy.current_draw_mA[RADIO_SLEEP]  = RADIO_CC_SLEEP;
  nodedata->energy.current_draw_mA[RADIO_IDLE]   = RADIO_CC_IDLE;
  nodedata->energy.current_draw_mA[RADIO_RX]     = RADIO_CC_RX;
  nodedata->energy.current_draw_mA[RADIO_RXING]  = RADIO_CC_RXING;
  nodedata->energy.current_draw_mA[RADIO_TX]     = RADIO_CC_TX;



  // get parameters
  list_init_traverse(params);
  while ((param = (param_t *) list_traverse(params)) != NULL) {
    if (!strcmp(param->key, "sensibility")) {
      if (get_param_double(param->value, &(nodedata->mindBm))) {
        return UNSUCCESSFUL;
      }
    }
    if (!strcmp(param->key, "T_s")) {
      if (get_param_time(param->value, &(nodedata->symbol_time))) {
        return UNSUCCESSFUL;
      }
    }
    if (!strcmp(param->key, "channel")) {
      if (get_param_integer(param->value, &(nodedata->channel))) {
        return UNSUCCESSFUL;
      }
    }
    if (!strcmp(param->key, "current_draw_sleep")) {
      if (get_param_double(param->value, &(nodedata->energy.current_draw_mA[RADIO_SLEEP]))) {
        return UNSUCCESSFUL;
      }
    }
    if (!strcmp(param->key, "current_draw_idle")) {
      if (get_param_double(param->value, &(nodedata->energy.current_draw_mA[RADIO_IDLE]))) {
        return UNSUCCESSFUL;
      }
    }
    if (!strcmp(param->key, "current_draw_rx")) {
      if (get_param_double(param->value, &(nodedata->energy.current_draw_mA[RADIO_RX]))) {
        return UNSUCCESSFUL;
      }
    }
    if (!strcmp(param->key, "current_draw_tx")) {
      if (get_param_double(param->value, &(nodedata->energy.current_draw_mA[RADIO_TX]))) {
        return UNSUCCESSFUL;
      }
    }
    if (!strcmp(param->key, "dBm")) {
      if (get_param_double(param->value, &(nodedata->power))) {
        return UNSUCCESSFUL;
      }
    }
    if (!strcmp(param->key, "modulation")) {
      if (get_param_class(param->value, &(nodedata->modulation))) {
        return UNSUCCESSFUL;
      }
    }
    if (!strcmp(param->key, "freq_low")) {
      if (get_param_uint64_integer(param->value, &(nodedata->freq_low))) {
        return UNSUCCESSFUL;
      }
    }
    if (!strcmp(param->key, "freq_high")) {
      if (get_param_uint64_integer(param->value, &(nodedata->freq_high))) {
        return UNSUCCESSFUL;
      }
    }
    if (!strcmp(param->key, "log_status")) {
      if (get_param_integer(param->value, &(nodedata->log_status))) {
        return UNSUCCESSFUL;
      }
    }
  }

  //update RXING as RX
  nodedata->energy.current_draw_mA[RADIO_RXING]  = nodedata->energy.current_draw_mA[RADIO_RX];

  return SUCCESSFUL;
}


void send_packet_down(call_t *to, packet_t *packet){
  array_t *down = get_class_bindings_down(to);
  int i = down->size;

  // Send the packet trough the media (antenna)
  while (i--) {
    packet_t *packet_down;
    packet_down = packet_clone(packet);
    call_t from0 = {down->elts[i], to->object};
    TX(&from0, to, packet_down);
  }
}

void send_packet_up(call_t *to, packet_t *packet){
  array_t *up = get_class_bindings_up(to);
  int i = up->size;
  // forward to upper layers
  while (i--) {
    call_t to_up = {up->elts[i], to->object};
    packet_t *packet_up;

    packet_up = packet_clone(packet);
    RX(&to_up, to, packet_up);
  }
}
