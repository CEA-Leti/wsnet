/*
 *  \file   radio_halfduplex_spectrum.h
 *  \brief  radio_halfduplex_spectrum description - header
 *  \author Luiz-Henrique SURATY-FILHO
 *  \date   2018
 */

#ifndef WSNET_MODELS_TRANSCEIVER_RADIO_HALF_DUPLEX_SPECTRUM_H_
#define WSNET_MODELS_TRANSCEIVER_RADIO_HALF_DUPLEX_SPECTRUM_H_

#include <kernel/include/modelutils.h>

//ENERGY (mA) Based on CC1125
#define RADIO_CC_SENSITIVITY  -110        //sensitivity of this chip is -110dBm
#define RADIO_CC_IDLE         1.3         // 1.3 mA
#define RADIO_CC_SLEEP        0.5*1e-3    // 0.5 uA = 0.5*1e-3 nA
#define RADIO_CC_TX           47        // 47 mA (TX power of 14 dBm)
#define RADIO_CC_RX           17         // 17 mA
#define RADIO_CC_RXING        RADIO_CC_RX
#define RADIO_CC_DATA_RATE    50000       // 50 kbps for GFSK 100kHz


// Enums for The state of the radio.
typedef enum{
  RADIO_SLEEP,            // Radio in Sleep State
  RADIO_IDLE,             // Radio in Idle State
  RADIO_RX,               // Radio in RX State
  RADIO_RXING,            // Radio in RXING State
  RADIO_TX,               // Radio in TX State
  RADIO_NUMBER_STATES     // Number of States
} halfduplex_radio_states_t;

/* ************************************************** */
/* ************************************************** */

//Energy
typedef struct{
  uint64_t    duration_ns[RADIO_NUMBER_STATES];
  double      current_draw_mA[RADIO_NUMBER_STATES];
  double      consumption_J[RADIO_NUMBER_STATES];
}halfduplex_radio_energy_t;

typedef struct nodedata {
  uint64_t symbol_time;
  double power;
  int channel;
  classid_t modulation;
  double mindBm;
  double rxdBm;

  uint64_t  bandwidth;  // bandwidth

  uint64_t    state_time;
  halfduplex_radio_states_t       state;
  halfduplex_radio_energy_t    energy;

  int    log_status;

  double noise_factor_dB;

  uint64_t freq_low;
  uint64_t freq_center;
  uint64_t freq_high;
} halfduplex_radio_transceiver_node_private_t;


typedef struct phy_halfduplex_header {
  //preamble 4 Bytes
  char preamble[4];

  // sync word 4 Bytes
  char sync_word[4];

  //Framer Length 1 Byte
  //char frame_length;

  //Extra 1 Byte
  //char extra;

  // header CRC 2 Byte
  char CRC[2];
} halfduplex_radio_transceiver_header_t;

/** \brief A structure to define the GLOBAL class variables
 *  \struct radio_LoRa_transceiver_class_private_t
 **/
typedef struct _halfduplex_radio_transceiver_class_private {
  uint **nbr_received_packet;
  double **accumulated_received_power;
  double sensitivity;
  int log_status;
}halfduplex_radio_transceiver_class_private_t;

/* ************************************************** */
/* ************************************************** */
int get_header_size(call_t *to, call_t *from);
int get_header_real_size(call_t *to, call_t *from);
int set_header(call_t *to, call_t *from, packet_t *packet, destination_t *dst);

void radio_energy_update(call_t *to);

void radio_switch_state(call_t *to, halfduplex_radio_states_t state);


/** \brief Set default values of the node.
 *  \fn void set_node_default_values(call_t *to, halfduplex_radio_transceiver_node_private_t *nodedata)
 *  \param nodedata is a pointer to the node data
 *  \return nothing
**/
void set_node_default_values(halfduplex_radio_transceiver_node_private_t *nodedata);

/** \brief Read parameters from the xml file.
 *  \fn int read_nodedata_from_xml_file(halfduplex_radio_transceiver_node_private_t *nodedata, void *params)
 *  \param nodedata is a pointer to the node data
 *  \param params is a pointer to the parameters read from the XML configuration file
 *  \return SUCCESSFUL if success, UNSUCCESSFUL otherwise
**/
int read_nodedata_from_xml_file(halfduplex_radio_transceiver_node_private_t *nodedata, void *params);


void send_packet_down(call_t *to, packet_t *packet);

void send_packet_up(call_t *to, packet_t *packet);

#endif  // WSNET_MODELS_TRANSCEIVER_RADIO_HALF_DUPLEX_SPECTRUM_H_
