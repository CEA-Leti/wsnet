/**
 *  \file   radio_halfduplex_spectrum.c
 *  \brief  Halfduplex Radio Model to be used with Spectrum
 *          This is a first example of the implementation of a transceiver to be used
 *          with Spectrum Models.
 *
 *          Reminder : This is NOT the final transceiver model to be used in WSNETv4. It
 *          is only a temporary modified version of the current transceiver (WSNETv3) adapted
 *          to be used with the current Spectrum Model (WSNETv3).
 *
 *          A new transceiver model structure and abstract class is expected for WSNETv4.
 *
 *  \author Luiz Henrique Suraty Filho
 *  \date   2017
 **/

#include "radio_halfduplex_spectrum.h"

char* radio_state_names[] = {
    (char*) "SLEEP",
    (char*) "IDLE",
    (char*) "RX",
    (char*) "RXING",
    (char*) "TX",
};

/* ************************************************** */
/* ************************************************** */
model_t model =  {
    "HALFDUPLEX radio interface",
    "Luiz Henrique Suraty Filho",
    "0.1",
    MODELTYPE_TRANSCEIVER
};

/* ************************************************** */
/* ************************************************** */

int init(call_t *to, void *params) {
  param_t *param;
  int i;
  int nbr_nodes = get_node_count();
  halfduplex_radio_transceiver_class_private_t *classdata = (halfduplex_radio_transceiver_class_private_t*) malloc(sizeof(halfduplex_radio_transceiver_class_private_t));


  classdata->log_status = 0;

  list_init_traverse((list_t *)params);
  while ((param = (param_t *) list_traverse((list_t *) params)) != NULL){
    if (!strcmp(param->key,"log_status_class")) {
      if (get_param_integer(param->value, &(classdata->log_status))) {
        free(classdata);
        return UNSUCCESSFUL;
      }
    }
  }

  if (classdata->log_status){
    classdata->accumulated_received_power = (double**) malloc(sizeof(double*)*nbr_nodes);
    classdata->nbr_received_packet = (uint**) malloc(sizeof(uint*)*nbr_nodes);

    for (i=0;i<nbr_nodes;i++){
      classdata->accumulated_received_power[i] = (double*) malloc(sizeof(double)*nbr_nodes);
      classdata->nbr_received_packet[i] = (uint*) malloc(sizeof(uint)*nbr_nodes);
    }

    for (i=0;i<nbr_nodes;i++){
      for (int j=0;j<nbr_nodes;j++){
        classdata->accumulated_received_power[i][j]=0.0;
        classdata->nbr_received_packet[i][j]=0;
      }
    }

  }
  classdata->sensitivity = 0.0;

  // Assign the GLOBAL class variables
  set_class_private_data(to, classdata);

  return SUCCESSFUL;
}

int destroy(call_t *to) {
  int nbr_nodes = get_node_count();
  halfduplex_radio_transceiver_class_private_t *classdata = (halfduplex_radio_transceiver_class_private_t*) get_class_private_data(to);
  double average_rxdBm = 0;
  if (classdata->log_status){
    for (int i=0;i<nbr_nodes;i++){
      for (int j=0;j<nbr_nodes;j++){
        if (classdata->nbr_received_packet[i][j]>0){
          average_rxdBm =classdata->accumulated_received_power[i][j]/classdata->nbr_received_packet[i][j];
          printf("[RADIO_HALFDUPLEX] Node %d received from %d with average_dBm=%f (sensitivity=%f) average_margin_dBm=%f\n", j, i, average_rxdBm, classdata->sensitivity, average_rxdBm-classdata->sensitivity);
        }
      }
    }

    for (int i=0;i<nbr_nodes;i++){
      free(classdata->accumulated_received_power[i]);
      free(classdata->nbr_received_packet[i]);
    }
    free(classdata->accumulated_received_power);
    free(classdata->nbr_received_packet);

  }

  free(classdata);

  return SUCCESSFUL;
}

/* ************************************************** */
/* ************************************************** */
int bind(call_t *to, void *params) {
  halfduplex_radio_transceiver_node_private_t *nodedata = malloc(sizeof(halfduplex_radio_transceiver_node_private_t));
  halfduplex_radio_transceiver_class_private_t *classdata = (halfduplex_radio_transceiver_class_private_t*) get_class_private_data(to);

  set_node_default_values(nodedata);

  if (read_nodedata_from_xml_file(nodedata,params) == UNSUCCESSFUL){
    free(nodedata);
    return UNSUCCESSFUL;
  }

  classdata->sensitivity = nodedata->mindBm;

  set_node_private_data(to, (void*) nodedata);

  return SUCCESSFUL;
}

int unbind(call_t *to) {

  halfduplex_radio_transceiver_node_private_t *nodedata = (halfduplex_radio_transceiver_node_private_t *) get_node_private_data(to);

  // update energy consumption
  radio_energy_update(to);

  uint64_t total_duration = 0;
  double total_energy = 0;
  for (int state=0;state<RADIO_NUMBER_STATES;state++){
    total_duration+=nodedata->energy.duration_ns[state];
    total_energy+=nodedata->energy.consumption_J[state];
  }

  if (nodedata->log_status){
    printf("\n[RADIO_HALFDUPLEX] Node %d was ",to->object);
    for (int state=0;state<RADIO_NUMBER_STATES;state++){
      printf("in state %s during %lf ms (%s_time_ratio=%f, %s_energy_ratio=%f), ",radio_state_names[state], (nodedata->energy.duration_ns[state]/1e6),radio_state_names[state], (double) nodedata->energy.duration_ns[state]/total_duration,radio_state_names[state],(double) (nodedata->energy.consumption_J[state]/total_energy) );
    }
    printf("\n[RADIO_HALFDUPLEX] Node %d duty_cycle=%f%%\n", to->object, (double) nodedata->energy.duration_ns[RADIO_TX] * 100.0/ (double) scheduler_get_end());
    printf("\n");
  }

  free(nodedata);
  nodedata = NULL;
  set_node_private_data(to, (void*) nodedata);
  return SUCCESSFUL;
}


/* ************************************************** */
/* ************************************************** */
int bootstrap(call_t *to) {
  // initial mode
  radio_switch_state(to, RADIO_RX);

  return SUCCESSFUL;
}

int ioctl(call_t *to, int option, void *in, void **out) {
  (void) to;
  (void) option;
  (void) in;
  (void) out;
  return SUCCESSFUL;
}


/* ************************************************** */
/* ************************************************** */
void tx(call_t *to, call_t *from, packet_t *packet) {
  (void) from;
  halfduplex_radio_transceiver_node_private_t *nodedata = (halfduplex_radio_transceiver_node_private_t *) get_node_private_data(to);

  // radio sleep or rxing or txing already
  if ((nodedata->state == RADIO_SLEEP) || (nodedata->state == RADIO_RXING) || (nodedata->state == RADIO_TX)){
    packet_dealloc(packet);
    return;
  }
  // Set the PHY header
  destination_t destination = packet->destination;
  if (set_header(to, from, packet, &destination) == -1) {
    packet_dealloc(packet);
    return;
  }
  // radio activity
  radio_switch_state(to, RADIO_TX);

  // Set the packet channel
  packet->channel = nodedata->channel;

  // Send the packet trough the media (antenna)
  send_packet_down(to,packet);

}

void tx_end(call_t *to, call_t *from, packet_t *packet) {
  (void) from;
  (void) packet;
  // check wether the reception has killed us
  if (!is_node_alive(to->object)) {
    return;
  }

  radio_switch_state(to, RADIO_RX);

  return;
}


/* ************************************************** */
/* ************************************************** */
void rx(call_t *to, call_t *from, packet_t *packet) {
  (void) from;
  halfduplex_radio_transceiver_class_private_t *classdata = (halfduplex_radio_transceiver_class_private_t*) get_class_private_data(to);
  halfduplex_radio_transceiver_node_private_t *nodedata = (halfduplex_radio_transceiver_node_private_t *) get_node_private_data(to);

  // radio sleep, half-duplex, or Radio was not receiving
  if ((nodedata->state == RADIO_SLEEP) || (nodedata->state == RADIO_TX) || (nodedata->state != RADIO_RXING)){
    packet_dealloc(packet);
    return;
  }

  if ( (classdata->log_status)){
    classdata->accumulated_received_power[packet->node][to->object] += packet->rxdBm;
    classdata->nbr_received_packet[packet->node][to->object]++;
  }
  // End of RXING and return to RX
  radio_switch_state(to, RADIO_RX);

  // check wether the reception has killed us
  if (!is_node_alive(to->object)) {
    packet_dealloc(packet);
    return;
  }

  // drop packet depending on the PER
  if (get_random_double() < packet->PER) {
    packet_dealloc(packet);
    return;
  }

  // send the packet up
  send_packet_up(to, packet);

  // It is important to dealloc the packet you received to avoid memory leaks
  packet_dealloc(packet);

  return;
}

void cs(call_t *to, call_t *from, packet_t *packet) {
  (void) from;
  halfduplex_radio_transceiver_node_private_t *nodedata = (halfduplex_radio_transceiver_node_private_t *) get_node_private_data(to);

  // Radio is ready to CS
  if ((nodedata->state != RADIO_RX) && (nodedata->state != RADIO_RXING) ){
    PRINT_TRANSCEIVER("{%"PRId64"} [RADIO_LORA] Node %d make CS outside RX_STATE/RXING \n",get_time(),to->object);
    return;
  }

  nodedata->rxdBm = packet->rxdBm;
  radio_switch_state(to, RADIO_RXING);
  return;
}


/* ************************************************** */
/* ************************************************** */
double get_noise(call_t *to, call_t *from) {
  (void) from;
  halfduplex_radio_transceiver_node_private_t *nodedata = (halfduplex_radio_transceiver_node_private_t *) get_node_private_data(to);
  return nodedata->noise_factor_dB;
}

double get_cs(call_t *to, call_t *from) {
  (void) from;
  halfduplex_radio_transceiver_node_private_t *nodedata = (halfduplex_radio_transceiver_node_private_t *) get_node_private_data(to);
  return nodedata->rxdBm;
}

double get_power(call_t *to, call_t *from) {
  (void) from;
  halfduplex_radio_transceiver_node_private_t *nodedata = (halfduplex_radio_transceiver_node_private_t *) get_node_private_data(to);
  return nodedata->power;
}

void set_power(call_t *to, call_t *from, double power) {
  (void) from;
  halfduplex_radio_transceiver_node_private_t *nodedata = (halfduplex_radio_transceiver_node_private_t *) get_node_private_data(to);
  nodedata->power = power;
}

int get_channel(call_t *to, call_t *from) {
  (void) from;
  halfduplex_radio_transceiver_node_private_t *nodedata = (halfduplex_radio_transceiver_node_private_t *) get_node_private_data(to);
  return nodedata->channel;
}

void set_channel(call_t *to, call_t *from, int channel) {
  (void) from;
  halfduplex_radio_transceiver_node_private_t *nodedata = (halfduplex_radio_transceiver_node_private_t *) get_node_private_data(to);
  nodedata->channel = channel;
}

classid_t get_modulation(call_t *to, call_t *from) {
  (void) from;
  halfduplex_radio_transceiver_node_private_t *nodedata = (halfduplex_radio_transceiver_node_private_t *) get_node_private_data(to);
  return nodedata->modulation;
}

void set_modulation(call_t *to, call_t *from, classid_t modulation) {
  (void) from;
  halfduplex_radio_transceiver_node_private_t *nodedata = (halfduplex_radio_transceiver_node_private_t *) get_node_private_data(to);
  nodedata->modulation = modulation;
}

uint64_t get_Tb(call_t *to, call_t *from) {
  struct nodedata *nodedata = get_node_private_data(to);
  return (nodedata->symbol_time/modulation_bit_per_symbol(nodedata->modulation));
}

void set_Ts(call_t *to, call_t *from, uint64_t Ts) {
  (void) from;
  halfduplex_radio_transceiver_node_private_t *nodedata = (halfduplex_radio_transceiver_node_private_t *) get_node_private_data(to);
  nodedata->symbol_time = Ts;
}

uint64_t get_Ts(call_t *to, call_t *from) {
  (void) from;
  halfduplex_radio_transceiver_node_private_t *nodedata = (halfduplex_radio_transceiver_node_private_t *) get_node_private_data(to);
  return nodedata->symbol_time;
}

double get_sensibility(call_t *to, call_t *from) {
  (void) from;
  halfduplex_radio_transceiver_node_private_t *nodedata = (halfduplex_radio_transceiver_node_private_t *) get_node_private_data(to);
  return nodedata->mindBm;
}

void set_sensibility(call_t *to, call_t *from, double sensibility) {
  (void) from;
  halfduplex_radio_transceiver_node_private_t *nodedata = (halfduplex_radio_transceiver_node_private_t *) get_node_private_data(to);
  nodedata->mindBm = sensibility;
}


/* ************************************************** */
/* ************************************************** */
void sleep_transceiver(call_t *to, call_t *from) {
  (void) from;
  radio_switch_state(to, RADIO_SLEEP);
}

void wakeup(call_t *to, call_t *from) {
  (void) from;
  radio_switch_state(to, RADIO_IDLE);
}

void switch_rx(call_t *to, call_t *from) {
  (void) from;
  radio_switch_state(to, RADIO_RX);
}

void switch_idle(call_t *to, call_t *from) {
  (void) from;
  radio_switch_state(to, RADIO_IDLE);
}

int get_modulation_bit_per_symbol(call_t *to, call_t *from){
  (void) from;
  halfduplex_radio_transceiver_node_private_t *nodedata = (halfduplex_radio_transceiver_node_private_t *) get_node_private_data(to);
  return modulation_bit_per_symbol(nodedata->modulation);
}

int get_modulation_type(call_t *to, call_t *from) {
  (void) from;
  halfduplex_radio_transceiver_node_private_t *nodedata = (halfduplex_radio_transceiver_node_private_t *) get_node_private_data(to);
  return nodedata->bandwidth;

}

void set_modulation_type(call_t *to, call_t *from, int modulation_type)
{
  (void) from;
  halfduplex_radio_transceiver_node_private_t *nodedata = (halfduplex_radio_transceiver_node_private_t *) get_node_private_data(to);
  modulation_set_modulation_type(nodedata->modulation, modulation_type);
}
int get_header_size(call_t *to, call_t *from) {
  return sizeof(halfduplex_radio_transceiver_header_t);
}

int get_header_real_size(call_t *to, call_t *from) {
  return 8*sizeof(halfduplex_radio_transceiver_header_t);
}

int set_header(call_t *to, call_t *from, packet_t *packet, destination_t *dst) {
  struct phy_halfduplex_header *phy_11_header;
  field_t *field_header = packet_retrieve_field(packet,"PHY_header");

  if (field_header == NULL) {
    phy_11_header = malloc(sizeof(struct phy_halfduplex_header));
    field_t *field_phy_11_header = field_create(INT, sizeof(struct phy_halfduplex_header), phy_11_header);
    packet_add_field(packet, "PHY_header", field_phy_11_header);
    packet->size += get_header_size(to,from);
    packet->real_size += get_header_real_size(to,from);
  }
  else {
    phy_11_header = (struct phy_halfduplex_header *) field_getValue(field_header);
  }
  return 0;
}

uint64_t get_freq_low(call_t *to, call_t *from) {
  (void) from;
  halfduplex_radio_transceiver_node_private_t *nodedata = (halfduplex_radio_transceiver_node_private_t *) get_node_private_data(to);
  return nodedata->freq_low;
}

uint64_t get_freq_high(call_t *to, call_t *from){
  (void) from;
  halfduplex_radio_transceiver_node_private_t *nodedata = (halfduplex_radio_transceiver_node_private_t *) get_node_private_data(to);
  return nodedata->freq_high;
}

void set_freq_low(call_t *to, call_t *from, uint64_t freq_low) {
  (void) from;
  halfduplex_radio_transceiver_node_private_t *nodedata = (halfduplex_radio_transceiver_node_private_t *) get_node_private_data(to);
  nodedata->freq_low = freq_low;
}

void set_freq_high(call_t *to, call_t *from, uint64_t freq_high) {
  (void) from;
  halfduplex_radio_transceiver_node_private_t *nodedata = (halfduplex_radio_transceiver_node_private_t *) get_node_private_data(to);
  nodedata->freq_high = freq_high;
}


/* ************************************************** */
/* ************************************************** */
transceiver_methods_t methods = {rx,
    tx,
    set_header,
    get_header_size,
    get_header_real_size,
    tx_end,
    cs,
    get_noise,
    get_cs,
    get_power,
    set_power,
    get_channel,
    set_channel,
    get_modulation,
    set_modulation,
    get_Tb,
    get_Ts,
    set_Ts,
    get_sensibility,
    set_sensibility,
    sleep_transceiver,
    wakeup,
    switch_rx,
    switch_idle,
    get_modulation_bit_per_symbol,
    get_modulation_type,
    set_modulation_type,
    get_freq_low,
    get_freq_high,
    set_freq_low,
    set_freq_high};
