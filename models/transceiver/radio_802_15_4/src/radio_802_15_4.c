/**
 *  \file   radio_802_15_4.c
 *  \brief  802.15.4 radio model
 *  \author Guillaume Chelius & Mickael Maman & Luiz Henrique Suraty Filho
 *  \date   2007
 **/
/** Zigbee radio : ref 802.15.4-2006
 *   - 868-868.6 MHz
 *   - 300kchips/s
 *   - BPSK
 *   - 20kb/s
 *   - 20ksymbols/s
 *   - binary symbols 
 *   - channel 0 
 *   - sensibility -92dBm
 **/
/** Zigbee radio : ref 802.15.4-2006
 *   - 902-928 MHz
 *   - 600kchips/s
 *   - BPSK
 *   - 40kb/s
 *   - 40ksymbols/s
 *   - binary symbols
 *   - channels 1-10
 *   - sensibility -92dBm
 **/
/** Zigbee radio : ref 802.15.4-2006
 *   - 2400-2483.5 MHz 
 *   - 2000kchips/s 
 *   - O-QPSK 
 *   - 250kb/s 
 *   - 62.5ksymbols/s 
 *   - 16-ary Orthogonal 
 *   - channels 11-26 
 *   - sensibility -85dBm
 **/
#include <kernel/include/modelutils.h>

/* ************************************************** */
/* ************************************************** */
#ifdef BPSK_868

#define SYMBOL_PERIOD   50000    /* cf p28 ref 802.15.4-2006 */
#define MIN_SENSIBILITY -92      /* cf p52 ref 802.15.4-2006 */
#define MYMODULATION    "bpsk"
//ENERGY CC1100 (mA)
//sensitivity of this chip is -103dBm
#define RADIO_CC_IDLE     160*1e-6      //160 uA, (160*1e-6*3/1e9)
#define RADIO_CC_SLEEP    400*1e-6      //400 nA, (400*1e-9*3/1e9)
#define RADIO_CC_TX       31            //31 mA, 10dBm, (31.0*1e-3*3/1e9)
//#define RADIO_CC_TX     16.9          //16.9 mA, 0dBm, (16.9*1e-3*3/1e9)
//#define RADIO_CC_TX     13.5          //13.5 mA, -6dBm, (13.5*1e-3*3/1e9)
#define RADIO_CC_RX       15.1          //15.1 mA, 250 Kbps, above the sensitivity limit, (15.1*1e-3*3/1e9)
#define RADIO_CC_RXING    RADIO_CC_RX


#elif BPSK_902
#define SYMBOL_PERIOD   25000    /* cf p28 ref 802.15.4-2006 */
#define MIN_SENSIBILITY -92      /* cf p49 ref 802.15.4-2006 */
#define MYMODULATION    "bpsk"
//ENERGY CC1100 (mA)
//sensitivity of this chip is -103dBm
#define RADIO_CC_IDLE     160*1e-6      //160 uA, (160*1e-6*3/1e9)
#define RADIO_CC_SLEEP    400*1e-6      //400 nA, (400*1e-9*3/1e9)
#define RADIO_CC_TX       31            //31 mA, 10dBm, (31.0*1e-3*3/1e9)
//#define RADIO_CC_TX     16.9          //16.9 mA, 0dBm, (16.9*1e-3*3/1e9)
//#define RADIO_CC_TX     13.5          //13.5 mA, -6dBm, (13.5*1e-3*3/1e9)
#define RADIO_CC_RX       15.1          //15.1 mA, 250 Kbps, above the sensitivity limit, (15.1*1e-3*3/1e9)
#define RADIO_CC_RXING    RADIO_CC_RX

#elif OQPSK_2400
#define SYMBOL_PERIOD   16000     /* cf p28 ref 802.15.4-2006 */
#define MIN_SENSIBILITY -85      /* cf p52 ref 802.15.4-2006 */
#define MYMODULATION    "oqpsk"
//ENERGY CC2420 (mA)
//sensitivity of this chip is -95dBm
#define RADIO_CC_IDLE     426*1e-3     // 426 uA, 426 * 1e-6 (uA)  * 3V  * 1e-9 (during 1ns) (426*1e-6*3/1e9)
#define RADIO_CC_SLEEP    20*1e-6     // 20 nA, (20*1e-9*3/1e9)
//#define RADIO_CC_TX     17.4        // 17.4 mA, 0dBm, (17.4*1e-3*3/1e9)
//#define RADIO_CC_TX     14          // 14 mA, -5dBm, (14*1e-3*3/1e9)
//#define RADIO_CC_TX     11          // 11 mA, -10dBm, (11*1e-3*3/1e9)
#define RADIO_CC_TX       9.9         // 9.9 mA, -15dBm, (9.9*1e-3*3/1e9)
//#define RADIO_CC_TX     8.5         // 8.5 mA, -25dBm, (8.5*1e-3*3/1e9)
#define RADIO_CC_RX       18.8        // 18.8 mA, (18.8*1e-3*3/1e9)
#define RADIO_CC_RXING    RADIO_CC_RX

#endif /* OQPSK_2400O */


/* Enums for The state of the radio.*/
typedef enum{
  RADIO_SLEEP,            // Radio in Sleep State
  RADIO_IDLE,             // Radio in Idle State
  RADIO_RX,               // Radio in RX State
  RADIO_RXING,            // Radio in RXING State
  RADIO_TX,               // Radio in TX State
  RADIO_NUMBER_STATES     // Number of States
} radio_802_15_4_radio_states_t;


/* Defining macros MIN and MAX */
#define MAXI(x, y) ((x) > (y)?(x):(y))
#define MINI(x, y) ((x) < (y)?(x):(y))

/* Defining macro RANDN (Box-Muller Algorithm) */
#define TWOPI (6.2831853071795864769252867665590057683943387987502)
#define RANDN (sqrt(-2.0*log(get_random_double()))*cos(TWOPI*get_random_double()))


/* ************************************************** */
/* ************************************************** */
model_t model =  {
    "802.15.4 radio interface",
    "Guillaume Chelius, Mickael Maman & Luiz Henrique Suraty Filho",
    "0.1",
    MODELTYPE_TRANSCEIVER
};


/* ************************************************** */
/* ************************************************** */

//Energy
typedef struct{
    uint64_t    duration_ns[RADIO_NUMBER_STATES];
    double      current_draw_mA[RADIO_NUMBER_STATES];
    double      consumption_J[RADIO_NUMBER_STATES];
}radio_802_15_4_energy_t;

typedef struct _radio_802_15_4_classdata{
    shared_statistics_data_t	*shared_statistics; /*!< Defines the address of the shared statistics */
}radio_802_15_4_classdata_t;

struct nodedata {
    uint64_t Ts;
    double power;
    int channel;
    classid_t modulation;
    double mindBm;
    int sleep;
    int tx_busy;
    int rx_busy;
    double rxdBm;

    int  rx_invalid;     //the reception is invalid if the node has changed its radio state *during* the reception (the packet will not be forwarded to the upper layers)
    uint64_t    state_time;
    radio_802_15_4_radio_states_t       state;
    radio_802_15_4_energy_t    energy;

    /* ***** RSS to RSSI computation as in the 802.15.4 standard */
    /* to be read from the xml file */
    double RSSI_min_value;       /* To compute the minimum detectable received power for RSSI computation (in dBm) */
    double RSSI_max_value;       /* To compute the maximum detectable received power for RSSI computation (in dBm) */
    int RSSI_nbr_bits;           /* Number of bits coding the RSSI indication */
    double RSSI_sigma;           /* Uncertainty affecting the RSSI linear response (in db) */
    /* local variables */
    double RSSI_min;             /* Miminal RSSI value (i.e. mindBm (radio sensitivity) + RSSI_min_value) */
    double RSSI_max;             /* Maximal RSSI value (i.e. RSSI_min + RSSI_max_value) */
    double quantization_levels;  /* RSSI quantization levels (i.e. 2^RSSI_nbr_bits levels)*/
    double quantization_step;    /* RSSI quantization step (i.e. (RSSI_max - RSSI_min) / quantization_levels) */

    /* Variable for printing*/
    int    log_status;

    /*edit by Rida El Chall*/
    uint64_t freq_low;
    uint64_t freq_high;
    /* end of edition */

};


struct phy_154_header {
    //preamble 4 Bytes
    int preamble;
    //SFD 1 Byte
    int sfd;
    //Framer Length 1 Byte
    int frame_length;
};

int phy_154_header_size = 4+1+1;

/* ************************************************** */
/* ************************************************** */
void cs_init(call_t *to);
double RSS_to_RSSI(call_t *to, double RSS);
int get_header_size(call_t *to, call_t *from);
int get_header_real_size(call_t *to, call_t *from); 
int set_header(call_t *to, call_t *from, packet_t *packet, destination_t *dst);

/* ************************************************** */
//                  ENERGY
/* ************************************************** */

//consumes the energy according to the current state since the last state change 
//NB: we could introduce here energy for changing the state of the radio
void radio_energy_update(call_t *to){
  struct nodedata *nodedata = get_node_private_data(to);
  uint64_t duration;

  //updates the energy consumption according to the model
  switch (nodedata->state) {
    case RADIO_SLEEP:
    case RADIO_IDLE:
    case RADIO_RX:
    case RADIO_RXING:
    case RADIO_TX:

      //updates the energy consumption before the state changes!
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
      printf( "[RADIO] %d -> unknown state %d\n", to->object, nodedata->state);
      exit(5);

      break;
  }
}

/* ************************************************** */
//                  STATES
/* ************************************************** */

//switch the current mode
void radio_switch_state(call_t *to, radio_802_15_4_radio_states_t state){
  struct nodedata *nodedata = get_node_private_data(to);

  //printf( "[RADIO] node %d from state %d to state %d at time %"PRId64" \n", to->object,nodedata->state, state, get_time());

  if (nodedata->state == state) {
    return;
  }
  //if a transmission is on-going and we change our radio state, the reception will fail
  // possible also to check if nodedata->state==RADIO_RXING
  if (nodedata->rx_busy != -1)
    nodedata->rx_invalid = 1;

  //we should not change our state if we are currently receiving / transmitting something
  // if (nodedata->rx_busy != -1 || nodedata->tx_busy != -1){
  // printf("[RADIO] Be careful: node %d has non busy rx (%d) or/and tx (%d) flags. That's ok only if the events are synchronous, else you have probably a bug\n",  to->object, nodedata->rx_busy, nodedata->tx_busy);
  // }

  //update energy consumption
  radio_energy_update(to);

  //new state
  nodedata->state        = state;
  nodedata->state_time   = get_time();

  //sleeping mode update
  if (nodedata->state == RADIO_SLEEP)
    nodedata->sleep = 1;
  else
    nodedata->sleep = 0;

}


/* ************************************************** */
/* ************************************************** */
int init(call_t *to, void *params) {

  radio_802_15_4_classdata_t *classdata = malloc(sizeof(radio_802_15_4_classdata_t));

  set_class_private_data(to, classdata);
  return 0;
}

int destroy(call_t *to) {

  free(get_class_private_data(to));
  return 0;
}


/* ************************************************** */
/* ************************************************** */
int bind(call_t *to, void *params) {
  struct nodedata *nodedata = malloc(sizeof(struct nodedata));
  param_t *param;

  /* default values */
  nodedata->Ts = SYMBOL_PERIOD;
  nodedata->channel = 0;
  nodedata->power = -15;
  nodedata->mindBm = MIN_SENSIBILITY;
  nodedata->sleep = 0;

  /*edit by Rida El Chall*/
  nodedata->freq_low  = 0;
  nodedata->freq_high = 0;
  /* end of edition */

  //a reception becomes invalid if the node changes its state *during* the reception
  nodedata->rx_invalid    = 0;

  //energy for all the modes (BE CAREFUL: RADIO_TX is the largest mode value)
  int i;
  for(i=0; i<RADIO_NUMBER_STATES; i++){
    nodedata->energy.duration_ns[i]    = 0;
    nodedata->energy.consumption_J[i] = 0;
  }

  // default value for log
  nodedata->log_status = 0;

  //default values for the energy consumption (Joule / ns)
  nodedata->energy.current_draw_mA[RADIO_SLEEP]  = RADIO_CC_SLEEP;
  nodedata->energy.current_draw_mA[RADIO_IDLE]   = RADIO_CC_IDLE;
  nodedata->energy.current_draw_mA[RADIO_RX]     = RADIO_CC_RX;
  nodedata->energy.current_draw_mA[RADIO_RXING]  = RADIO_CC_RXING;
  nodedata->energy.current_draw_mA[RADIO_TX]     = RADIO_CC_TX;


  /* default values from the 802.15.4 standard */
  nodedata->RSSI_min_value = 10;
  nodedata->RSSI_max_value = 40;
  nodedata->RSSI_nbr_bits = 8;
  nodedata->RSSI_sigma = 2;



  /* if (get_param_class(MYMODULATION, &(nodedata->modulation))) {
    goto error;
  }*/

  /* get parameters */
  list_init_traverse(params);

  while ((param = (param_t *) list_traverse(params)) != NULL) {
    if (!strcmp(param->key, "dBm")) {
      if (get_param_double(param->value, &(nodedata->power))) {
        goto error;
      }
    }
    if (!strcmp(param->key, "modulation")) {
      if (get_param_class(param->value, &(nodedata->modulation))) {
        goto error;
      }
    }
    if (!strcmp(param->key, "log_status")) {
      if (get_param_integer(param->value, &(nodedata->log_status))) {
        goto error;
      }
    }
    if (!strcmp(param->key, "sensibility")) {
      if (get_param_double(param->value, &(nodedata->mindBm))) {
        goto error;
      }
    }
    if (!strcmp(param->key, "channel")) {
      if (get_param_integer(param->value, &(nodedata->channel))) {
        goto error;
      }
    }
    if (!strcmp(param->key, "current_draw_sleep")) {
      if (get_param_double(param->value, &(nodedata->energy.current_draw_mA[RADIO_SLEEP]))) {
        goto error;
      }
    }
    if (!strcmp(param->key, "current_draw_idle")) {
      if (get_param_double(param->value, &(nodedata->energy.current_draw_mA[RADIO_IDLE]))) {
        goto error;
      }
    }
    if (!strcmp(param->key, "current_draw_rx")) {
      if (get_param_double(param->value, &(nodedata->energy.current_draw_mA[RADIO_RX]))) {
        goto error;
      }
    }
    if (!strcmp(param->key, "current_draw_tx")) {
      if (get_param_double(param->value, &(nodedata->energy.current_draw_mA[RADIO_TX]))) {
        goto error;
      }
    }
    if (!strcmp(param->key, "RSSI_min_value")) {
      if (get_param_double_range(param->value, &(nodedata->RSSI_min_value), 0, 100)) {
        goto error;
      }
    }
    if (!strcmp(param->key, "RSSI_max_value")) {
      if (get_param_double_range(param->value, &(nodedata->RSSI_max_value), 0, 100)) {
        goto error;
      }
    }
    if (!strcmp(param->key, "RSSI_nbr_bits")) {
      if (get_param_integer(param->value, &(nodedata->RSSI_nbr_bits))) {
        goto error;
      }
    }
    if (!strcmp(param->key, "RSSI_sigma")) {
      if (get_param_double_range(param->value, &(nodedata->RSSI_sigma), 0, 100)) {
        goto error;
      }
    }
    /*edit by Rida El Chall*/
    if (!strcmp(param->key, "freq_low")) {
	  if (get_param_uint64_integer(param->value, &(nodedata->freq_low))) {
		goto error;
	  }
	}
    if (!strcmp(param->key, "freq_high")) {
	  if (get_param_uint64_integer(param->value, &(nodedata->freq_high))) {
		goto error;
	  }
	}
    /* end of edition */
  }
  //update RXING as RX 
  nodedata->energy.current_draw_mA[RADIO_RXING]  = nodedata->energy.current_draw_mA[RADIO_RX];

  if (nodedata->mindBm > MIN_SENSIBILITY) {
    nodedata->mindBm = MIN_SENSIBILITY;        
  }

  /* Pre-computation of RSS to RSSI parameters*/
  nodedata->RSSI_min = nodedata->mindBm + nodedata->RSSI_min_value;
  nodedata->RSSI_max = nodedata->RSSI_min + nodedata->RSSI_max_value;
  nodedata->quantization_levels = pow(2, nodedata->RSSI_nbr_bits);
  nodedata->quantization_step = (nodedata->RSSI_max - nodedata->RSSI_min) / nodedata->quantization_levels;

  /* Get statistics address*/
  /*we need to get the shared statistics here because classes are still being loaded during init*/
  radio_802_15_4_classdata_t *classdata= get_class_private_data(to);
  if (get_class_by_name("shared") != NULL)
    classdata->shared_statistics = (shared_statistics_data_t *)  get_class_by_name("shared")->methods->shared.get_shared_statistics(to->object);
  else
    classdata->shared_statistics = NULL;
  set_node_private_data(to, nodedata);
  return 0;

  error:
  free(nodedata);
  return -1;
}

int unbind(call_t *to) {

  struct nodedata *nodedata = get_node_private_data(to);

  //update energy consumption
  radio_energy_update(to);

  if (nodedata->log_status){
    printf("[RADIO_154] node %d was in state SLEEP during %lf ms, in IDLE during %lf ms, in RX during %lf ms, in RXING during %lf ms and in TX during %lf ms \n",to->object, nodedata->energy.duration_ns[RADIO_SLEEP]/1e6, nodedata->energy.duration_ns[RADIO_IDLE]/1e6, nodedata->energy.duration_ns[RADIO_RX]/1e6, nodedata->energy.duration_ns[RADIO_RXING]/1e6, nodedata->energy.duration_ns[RADIO_TX]/1e6);

    double sum_consumption=0.0;
    int i;
    for(i=0; i<RADIO_NUMBER_STATES; i++){
      sum_consumption += nodedata->energy.consumption_J[i];
    }

    printf( "[RADIO_154] node %d consumption=%lf \n",to->object , sum_consumption);
  }
  free(get_node_private_data(to));
  return 0;
}


/* ************************************************** */
/* ************************************************** */
int bootstrap(call_t *to) {
  struct nodedata *nodedata = get_node_private_data(to);
  nodedata->tx_busy = -1;
  nodedata->rx_busy = -1;
  nodedata->rxdBm = MIN_DBM;

  nodedata->sleep = 1;
  //initial mode
  nodedata->state         = RADIO_SLEEP;
  nodedata->state_time    = get_time();

  return 0;
}

int ioctl(call_t *to, int option, void *in, void **out) {
  return 0;
}


/* ************************************************** */
/* ************************************************** */
void cs_init(call_t *to) {
  struct nodedata *nodedata = get_node_private_data(to);
  /* log */
  if (nodedata->tx_busy != -1)  //TXING
  {
    PRINT_REPLAY("radio-tx1 %"PRId64" %d\n", get_time(), to->object);
  }
  if (nodedata->rx_busy != -1) //RXING
  {
    PRINT_REPLAY("radio-rx1 %"PRId64" %d\n", get_time(), to->object);
  }
  /* init cs */
  nodedata->tx_busy = -1;
  nodedata->rx_busy = -1;
  nodedata->rxdBm = MIN_DBM;
  radio_switch_state(to, RADIO_IDLE);
}


/* ************************************************** */
/* ************************************************** */
void tx(call_t *to, call_t *from, packet_t *packet) {
  struct nodedata *nodedata = get_node_private_data(to);
  array_t *down = get_class_bindings_down(to);
  int i = down->size;

  /* radio sleep */
  if (nodedata->sleep) {
    printf("[RADIO_154] node %d is on sleep mode (time= %"PRId64") and cannot TX\n", to->object, get_time());
    packet_dealloc(packet);
    return;
  }

  // encapsulation of PHY header
  destination_t destination = packet->destination;
  if (set_header(to, from, packet, &destination) == -1) {
    packet_dealloc(packet);
    return;
  }

  /* radio activity */
  cs_init(to);
  nodedata->tx_busy = packet->id;
  radio_switch_state(to, RADIO_TX);

  packet->channel = nodedata->channel;

  /* log tx */
  PRINT_TRANSCEIVER("{%"PRId64"} [RADIO_154] node %d starts to transmit (pckt_id=%d) (pckt_dst=%d) (pckt_channel=%d)\n", get_time(),to->object, packet->id, packet->destination.id, packet->channel);

  /* transmit to antenna */
  while (i--) {
    packet_t *packet_down;

    if (i > 0) {
      packet_down = packet_clone(packet);         
    } else {
      packet_down = packet;
    }
    call_t to0 = {down->elts[i], to->object};
    TX(&to0, to, packet_down);
  }

  return;
}

void tx_end(call_t *to, call_t *from, packet_t *packet) {
  struct nodedata *nodedata = get_node_private_data(to);

  /* consume energy */
  //energy_consume(to, packet->duration, nodedata->power);

  /* check wether the reception has killed us */
  if (!is_node_alive(to->object)) {
    return;
  }

  /* log tx */
  if (nodedata->tx_busy == packet->id) {
    PRINT_TRANSCEIVER("{%"PRId64"} [RADIO_154] node %d ends to transmit\n",get_time(),to->object);
    nodedata->tx_busy = -1;
    radio_switch_state(to, RADIO_IDLE);
  }
  else{
    printf("[RADIO_154] node %d problem, we have two packets to transmit at the same time! (pckt_id=%d) (tx_busy=%d) ( %"PRId64")\n", to->object, packet->id, nodedata->tx_busy, get_time());
    exit(3);
  }

  return;
}


/* ************************************************** */
/* ************************************************** */
void rx(call_t *to, call_t *from, packet_t *packet) {
  struct nodedata *nodedata = get_node_private_data(to);
  array_t *up = get_class_bindings_up(to);
  int i = up->size;

  int result = 0;
  void ** ptr_result = (void **) malloc(sizeof(void*));
  *ptr_result = ((void *) &result);

  /*For statistics purpose, we update the number of TX from source according to the PER*/
  /*Doing in this way, we can have a more realistic number of packets transmitted by the source that could potentially be received by the destination*/
  /*We can avoid considering a packet as transmitted when nodes are separated by a very long distance*/
  radio_802_15_4_classdata_t *classdata= get_class_private_data(to);

  if (up->size>0){
    while (i--){
      call_t to_up = {up->elts[i], to->object};
      IOCTL(&to_up,100,(void *) packet,ptr_result);
    }
  }

  free(ptr_result);

  if ((result == 1) && (classdata->shared_statistics!= NULL)){
    classdata->shared_statistics->nbr_tx[to->object][packet->node] += packet->LQI;
    if ( (packet->LQI > 0.05) && (classdata->shared_statistics->init_delay[to->object][packet->node] == (uint64_t) 0))
      classdata->shared_statistics->init_delay[to->object][packet->node] = get_time();
    PRINT_TRANSCEIVER("{%"PRId64"} [RADIO_154] Node %d has a packet from %d (packet_id = %d)  with PER = %.20lf (LQI=%.20lf)(rxdbm = %f) (nbr_tx = %f)\n",get_time(), to->object, packet->node, packet->id, packet->PER, packet->LQI, packet->rxdBm, classdata->shared_statistics->nbr_tx[to->object][packet->node]);
  }

  /* radio sleep */
  if (nodedata->sleep) {
    PRINT_TRANSCEIVER("{%"PRId64"} [RADIO_154] Node %d is sleeping -> packet dealloc\n",get_time(), to->object );

    packet_dealloc(packet);
    return;
  }

  /* half-duplex */
  if (nodedata->tx_busy != -1) {
    PRINT_TRANSCEIVER("{%"PRId64"} [RADIO_154] Node %d is on tx_busy -> packet dealloc\n",get_time(),to->object);
    packet_dealloc(packet);
    return;
  }

  /* Radio was receiving */
  if (nodedata->state != RADIO_RXING){
    PRINT_TRANSCEIVER("{%"PRId64"} [RADIO_154] Node %d is not on RXING (state=%d) -> packet dealloc\n",get_time(), to->object , nodedata->state);

    if ( nodedata->rx_invalid == 1)
      nodedata->rx_invalid = 0;

    packet_dealloc(packet);

    if (nodedata->rx_busy != -1)
      nodedata->rx_busy = -1;

    //printf("the node %d is not in receive mode\n", to->object);
    return;
  }

  /* handle carrier sense */
  if (nodedata->rx_busy == packet->id) {
    nodedata->rx_busy = -1;
    nodedata->rxdBm   = MIN_DBM;

    // End of RXING	
    radio_switch_state(to, RADIO_RX);

    /* log rx */
    PRINT_REPLAY("radio-rxing1 %"PRId64" %d\n", get_time(), to->object);
    /* consume energy */
    //energy_consume(to, packet->duration);
  } else {
    PRINT_TRANSCEIVER("{%"PRId64"} [RADIO_154] Node %d is rx_busy =%d is not same packet_id =%d (PER=%lf) (rxdBm=%lf)-> packet dealloc\n",get_time(), to->object , nodedata->rx_busy,packet->id,packet->PER, packet->rxdBm);
    packet_dealloc(packet);
    return;
  }

  /* check wether the reception has killed us */
  if (!is_node_alive(to->object)) {
    PRINT_TRANSCEIVER("{%"PRId64"} [RADIO_154] Node %d is not alive -> packet dealloc\n",get_time(), to->object );
    packet_dealloc(packet);
    return;
  }

  /* drop packet depending on the FER */
  if (get_random_double() < packet->PER) {
    PRINT_TRANSCEIVER("{%"PRId64"} [RADIO_154] Node %d -> packet dropped due to PER\n",get_time(),to->object);
    packet_dealloc(packet);
    return;
  }
  /* compute the RSSI from the rx received power (or RSS) */
  packet->RSSI = RSS_to_RSSI(to, packet->rxdBm);

  if (!nodedata->rx_invalid) 
  {

    PRINT_TRANSCEIVER("{%"PRId64"} [RADIO_154] Node %d has a packet from %d (packet_id = %d)  with PER = %.20lf (LQI=%.20lf)(rxdbm = %f)\n",get_time(),to->object, packet->node, packet->id, packet->PER, packet->LQI, packet->rxdBm);
    radio_switch_state(to, RADIO_IDLE);
    i = up->size;
    /* forward to upper layers */
    while (i--) {
      call_t to_up = {up->elts[i], to->object};
      packet_t *packet_up;

      if (i > 0) {
        packet_up = packet_clone(packet);         
      } else {
        packet_up = packet;
      }
      RX(&to_up, to, packet_up);
    }

  }
  else
  {
    PRINT_TRANSCEIVER("{%"PRId64"} [RADIO] Node %d rx invalid \n",get_time(), to->object);
  }

  //the on-going reception has finished -> the next one may be ok
  nodedata->rx_invalid = 0;


  return;
}

void cs(call_t *to, call_t *from, packet_t *packet) {
  struct nodedata *nodedata = get_node_private_data(to);

  /* radio sleep */
  if (nodedata->sleep) {
    return;
  }

  /* half-duplex */
  if (nodedata->tx_busy != -1) {
    return;
  }

  /* Radio is ready to CS */
  if ((nodedata->state != RADIO_RX) && (nodedata->state != RADIO_RXING) ){
    PRINT_TRANSCEIVER("{%"PRId64"} [RADIO_154] Node %d make CS outside RX_STATE/RXING \n",get_time(),to->object);
    return;
  }

  /* check sensibility */
  /* add 5 dB on packet filtering*/
  if (packet->rxdBm < (nodedata->mindBm - 5)) {
    return;
  }

  /* check channel */
  if (nodedata->channel != packet->channel) {
    return;
  }

  /* check Tb */
  if (nodedata->Ts != (packet->Tb*transceiver_get_modulation_bit_per_symbol(to, from))) {
    return;
  }

  /* check channel */
  if (packet->modulation != nodedata->modulation) {
    return;
  }

  /* capture effect */
  // TODO: need to modify capture effect. 
  //Today the best packet takes the priority but.
  if (packet->rxdBm > nodedata->rxdBm) {
    nodedata->rxdBm = packet->rxdBm;
    radio_switch_state(to, RADIO_RXING);
    nodedata->rx_busy = packet->id;
    /* log cs */
    PRINT_REPLAY("radio-rxing0 %"PRId64" %d\n", get_time(), to->object);
    return;
  }

  return;
}


/* ************************************************** */
/* ************************************************** */
double get_noise(call_t *to, call_t *from) {
  struct nodedata *nodedata = get_node_private_data(to);
  array_t *down = get_class_bindings_down(to);

  call_t from0 = {down->elts[0], to->object};
  return MEDIA_GET_NOISE(to, &from0, nodedata->channel);
}

double get_cs(call_t *to, call_t *from) {
  struct nodedata *nodedata = get_node_private_data(to);

  if ((nodedata->state == RADIO_RXING)||(nodedata->state == RADIO_RX))
  {
    return nodedata->rxdBm;
  }
  else
  {
    PRINT_TRANSCEIVER("{%"PRId64"} [RADIO_154] Node %d make GET_CS outside RX/RXING_STATE\n",get_time(), to->object);
    return nodedata->rxdBm;
  }
}

double get_power(call_t *to, call_t *from) {
  struct nodedata *nodedata = get_node_private_data(to);
  return nodedata->power;
}

void set_power(call_t *to, call_t *from, double power) {
  struct nodedata *nodedata = get_node_private_data(to);
  nodedata->power = power;
}

int get_channel(call_t *to, call_t *from) {
  struct nodedata *nodedata = get_node_private_data(to);
  return nodedata->channel;
}

void set_channel(call_t *to, call_t *from, int channel) {
  struct nodedata *nodedata = get_node_private_data(to);
  cs_init(to);
  nodedata->channel = channel;
}

classid_t get_modulation(call_t *to, call_t *from) {
  struct nodedata *nodedata = get_node_private_data(to);
  return nodedata->modulation;
}

void set_modulation(call_t *to, call_t *from, classid_t modulation) {
  struct nodedata *nodedata = get_node_private_data(to); 
  cs_init(to);
  nodedata->modulation = modulation;
}


/* edit by Christophe Savigny <christophe.savigny@insa-lyon.fr> */
uint64_t get_Tb(call_t *to, call_t *from) {
  struct nodedata *nodedata = get_node_private_data(to);
  return (nodedata->Ts/modulation_bit_per_symbol(nodedata->modulation));
}

void set_Ts(call_t *to, call_t *from, uint64_t Ts) {
  struct nodedata *nodedata = get_node_private_data(to);
  cs_init(to);
  nodedata->Ts = Ts;
}

uint64_t get_Ts(call_t *to, call_t *from) {
  struct nodedata *nodedata = get_node_private_data(to);
  return nodedata->Ts;
}
/* end of edition */

double get_sensibility(call_t *to, call_t *from) {
  struct nodedata *nodedata = get_node_private_data(to);
  return nodedata->mindBm;
}

void set_sensibility(call_t *to, call_t *from, double sensibility) {
  struct nodedata *nodedata = get_node_private_data(to);
  cs_init(to);
  nodedata->mindBm = sensibility;
  if (nodedata->mindBm > MIN_SENSIBILITY) {
    nodedata->mindBm = MIN_SENSIBILITY;        
  }
}


/* ************************************************** */
/* ************************************************** */
static void sleep(call_t *to, call_t *from) {
  struct nodedata *nodedata = get_node_private_data(to);
  cs_init(to);
  nodedata->sleep = 1;
  radio_switch_state(to, RADIO_SLEEP);
}

void wakeup(call_t *to, call_t *from) {
  struct nodedata *nodedata = get_node_private_data(to);
  cs_init(to);
  nodedata->sleep = 0;
  radio_switch_state(to, RADIO_IDLE);
}

void switch_rx(call_t *to, call_t *from) {
  radio_switch_state(to, RADIO_RX);
}

void switch_idle(call_t *to, call_t *from) {
  radio_switch_state(to, RADIO_IDLE);
}

/* ************************************************** */
/* ************************************************** */
/* edit by Christophe Savigny <christophe.savigny@insa-lyon.fr> */
int get_modulation_bit_per_symbol(call_t *to, call_t *from){
  struct nodedata *nodedata = get_node_private_data(to);
  return modulation_bit_per_symbol(nodedata->modulation);
}
/* end of edition */

int get_modulation_type(call_t *to, call_t *from)
{
  struct nodedata *nodedata = get_node_private_data(to);
  return modulation_get_modulation_type(nodedata->modulation);

}

void set_modulation_type(call_t *to, call_t *from, int modulation_type)
{
  struct nodedata *nodedata = get_node_private_data(to);
  modulation_set_modulation_type(nodedata->modulation, modulation_type);

}

/* ************************************************** */
/* ************************************************** */
int get_header_size(call_t *to, call_t *from) {
  return phy_154_header_size;
}

int get_header_real_size(call_t *to, call_t *from) {

  //8*get_header_size
  return (8*phy_154_header_size);
}

int set_header(call_t *to, call_t *from, packet_t *packet, destination_t *dst) {

  struct phy_154_header *phy_154_header;
  field_t *field_header = packet_retrieve_field(packet,"PHY_header");

  if (field_header == NULL) {
    phy_154_header = malloc(sizeof(struct phy_154_header));
    field_t *field_phy_154_header = field_create(INT, sizeof(struct phy_154_header), phy_154_header);
    packet_add_field(packet, "PHY_header", field_phy_154_header);

    packet->size += get_header_size(to,from);
    packet->real_size += get_header_real_size(to,from);

  }
  else {
    phy_154_header = (struct phy_154_header *) field_getValue(field_header);
  }



  return 0;
}

/* ************************************************** */
/* ************************************************** */
double RSS_to_RSSI(call_t *to, double RSS) {
  struct nodedata *nodedata = get_node_private_data(to);

  double ED_quantized_level = round ( ((RSS-nodedata->RSSI_min) + nodedata->RSSI_sigma * RANDN) / nodedata->quantization_step);

  return MINI( MAXI(ED_quantized_level * nodedata->quantization_step + nodedata->RSSI_min, nodedata->RSSI_min), nodedata->RSSI_max);
}


/* ************************************************** */
/* ************************************************** */
uint64_t get_freq_low(call_t *to, call_t *from) {
	struct nodedata *nodedata = get_node_private_data(to);
	return nodedata->freq_low;
}

uint64_t get_freq_high(call_t *to, call_t *from){
	struct nodedata *nodedata = get_node_private_data(to);
	return nodedata->freq_high;
}

void set_freq_low(call_t *to, call_t *from, uint64_t freq_low) {
	struct nodedata *nodedata = get_node_private_data(to);
	nodedata->freq_low = freq_low;
}

void set_freq_high(call_t *to, call_t *from, uint64_t freq_high) {
	struct nodedata *nodedata = get_node_private_data(to);
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
    sleep,
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
