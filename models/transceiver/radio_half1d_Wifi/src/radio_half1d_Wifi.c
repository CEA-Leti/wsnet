/**
 *  \file   half1d.c
 *  \brief  Halfduplex 1-d radio model for Wifi
 *  \author Mickael Maman &  Luiz Henrique Suraty Filho
 *  \date   2007
 **/
#include <kernel/include/modelutils.h>

//ENERGY CC3000 (mA)
//sensitivity of this chip is -91dBm
#define RADIO_CC_IDLE     5*1e-3        //5 uA, (5*1e-6*3.6/1e9)
#define RADIO_CC_SLEEP    5*1e-3        // 5uA, (5*1e-6*3.6/1e9)
#define RADIO_CC_TX       260           //260 mA, 18dBm 11Mbps (260*1e-3*3.6/1e9)
#define RADIO_CC_RX       92            //92 mA, (92*1e-3*3.6/1e9)
#define RADIO_CC_RXING    RADIO_CC_RX

/* Enums for The state of the radio.*/
typedef enum{
  RADIO_SLEEP,            // Radio in Sleep State
  RADIO_IDLE,             // Radio in Idle State
  RADIO_RX,               // Radio in RX State
  RADIO_RXING,            // Radio in RXING State
  RADIO_TX,               // Radio in TX State
  RADIO_NUMBER_STATES     // Number of States
} radio_half1d_Wifi_radio_states_t;

/* ************************************************** */
/* ************************************************** */
model_t model =  {
    "Halfduplex radio interface for Wifi",
    "Mickael Maman &  Luiz Henrique Suraty Filho",
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
}radio_half1d_Wifi_radio_energy_t;

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
    radio_half1d_Wifi_radio_states_t       state;
    radio_half1d_Wifi_radio_energy_t    energy;

    /*edit by Rida El Chall*/
    uint64_t freq_low;
    uint64_t freq_high;
    /* end of edition */
};


struct phy_11_header {
    //sync 10 Bytes
    int sync;
    //SFD 2 Byte
    int sfd;
    //PLCP header 4 Byte (PLW (12bits) + PSF (4 bits) + HEC (16 bits))
    int plcp_header;
    //CRC 4 Byte
    int FCS;
};

int phy_11_header_size = 10+2+4+4;


/* ************************************************** */
/* ************************************************** */
void cs_init(call_t *to);
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
      fprintf(stderr, "[RADIO] %d -> unknown state %d\n", to->object, nodedata->state);
      exit(5);

      break;
  }

  //energy_consume(to, duration * RADIO_ENERGY_CONSUMPTION[nodedata->state]);

}

/* ************************************************** */
//                  STATES
/* ************************************************** */

//switch the current mode
void radio_switch_state(call_t *to, radio_half1d_Wifi_radio_states_t state){
  struct nodedata *nodedata = get_node_private_data(to);

  if (nodedata->state == state) {
    return;
  }
  //if a transmision is on-going and we change our radio state, the reception will fail
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
  return 0;
}

int destroy(call_t *to) {
  return 0;
}


/* ************************************************** */
/* ************************************************** */
int bind(call_t *to, void *params) {
  struct nodedata *nodedata = malloc(sizeof(struct nodedata));
  param_t *param;

  /* default values */
  nodedata->Ts = 4000; // 11Mbps
  nodedata->channel = 0;
  nodedata->power = 0;
  nodedata->modulation = -1;
  nodedata->mindBm = -90;
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
  //default values for the energy consumption (Joule / ns)
  nodedata->energy.current_draw_mA[RADIO_SLEEP]  = RADIO_CC_SLEEP;
  nodedata->energy.current_draw_mA[RADIO_IDLE]   = RADIO_CC_IDLE;
  nodedata->energy.current_draw_mA[RADIO_RX]     = RADIO_CC_RX;
  nodedata->energy.current_draw_mA[RADIO_RXING]  = RADIO_CC_RXING;
  nodedata->energy.current_draw_mA[RADIO_TX]     = RADIO_CC_TX;


  /* get parameters */
  list_init_traverse(params);
  while ((param = (param_t *) list_traverse(params)) != NULL) {
    if (!strcmp(param->key, "sensibility")) {
      if (get_param_double(param->value, &(nodedata->mindBm))) {
        goto error;
      }
    }
    if (!strcmp(param->key, "T_s")) {
      if (get_param_time(param->value, &(nodedata->Ts))) {
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

  printf("[RADIO_HALF1D_WIFI] node %d was in state SLEEP during %lf ms, in IDLE during %lf ms, in RX during %lf ms, in RXING during %lf ms and in TX during %lf ms \n",to->object, nodedata->energy.duration_ns[RADIO_SLEEP]/1e6, nodedata->energy.duration_ns[RADIO_IDLE]/1e6, nodedata->energy.duration_ns[RADIO_RX]/1e6, nodedata->energy.duration_ns[RADIO_RXING]/1e6, nodedata->energy.duration_ns[RADIO_TX]/1e6);

  double sum_consumption=0.0; 
  int i;
  for(i=0; i<RADIO_NUMBER_STATES; i++){
    sum_consumption += nodedata->energy.consumption_J[i];
  }

  printf("[RADIO_HALF1D_WIFI] node %d consumption is %lf \n",to->object , sum_consumption);

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
  if (nodedata->tx_busy != -1) {
    PRINT_REPLAY("radio-tx1 %"PRId64" %d\n", get_time(), to->object);
  }
  if (nodedata->rx_busy != -1) {
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

  /* log tx */
  PRINT_TRANSCEIVER("[RADIO_HALF1D_WIFI] node %d starts to transmit at time %"PRId64" \n",to->object, get_time());

  /* transmit to antenna */
  while (i--) {
    packet_t *packet_down;

    if (i > 0) {
      packet_down = packet_clone(packet);         
    } else {
      packet_down = packet;
    }
    call_t from0 = {down->elts[i], to->object};
    MEDIA_TX(to, &from0, packet_down);
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
    PRINT_TRANSCEIVER("[RADIO_HALF1D_WIFI] node %d ends to transmit at time %"PRId64" \n",to->object, get_time());
    nodedata->tx_busy = -1;
    radio_switch_state(to, RADIO_IDLE);
  }

  return;
}


/* ************************************************** */
/* ************************************************** */
void rx(call_t *to, call_t *from, packet_t *packet) {
  struct nodedata *nodedata = get_node_private_data(to);
  array_t *up = get_class_bindings_up(to);
  int i = up->size;


  /* radio sleep */
  if (nodedata->sleep) {
    packet_dealloc(packet);
    return;
  }

  /* half-duplex */
  if (nodedata->tx_busy != -1) {
    packet_dealloc(packet);
    return;
  }

  /* Radio was receiving */
  if (nodedata->state != RADIO_RXING){
    packet_dealloc(packet);
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
    packet_dealloc(packet);
    return;
  }

  /* check wether the reception has killed us */
  if (!is_node_alive(to->object)) {
    packet_dealloc(packet);
    return;
  }

  /* drop packet depending on the PER */
  if (get_random_double() < packet->PER) {
    //PRINT_TRANSCEIVER("[RADIO_HALF1D] node %d received a packet with bad PER\n" ,to->object);
    packet_dealloc(packet);
    return;
  }    


  if (!nodedata->rx_invalid) 
  {
    PRINT_TRANSCEIVER("[RADIO_HALF1D_WIFI] node %d received at time %"PRId64"\n",to->object, get_time());
    radio_switch_state(to, RADIO_IDLE);

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
  if ((nodedata->state != RADIO_RX) && (nodedata->state != RADIO_RXING)){
    PRINT_TRANSCEIVER("[RADIO_HALF1D_WIFI] node %d make CS outside RX_STATE/RXING \n",to->object);
    return;
  }

  /* check sensibility */
  if (packet->rxdBm < nodedata->mindBm) {
    return;
  }

  /* check channel */
  if (nodedata->channel != packet->channel) {
    return;
  }

  /* check Ts */
  if (nodedata->Ts != (packet->Tb*transceiver_get_modulation_bit_per_symbol(to, from))) {
    return;
  }

  /* check channel */
  if (packet->modulation != nodedata->modulation) {

    return;
  }

  /* capture effect */
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
    PRINT_TRANSCEIVER("[RADIO_HALF1D] node %d make GET_CS outside RX/RXING_STATE \n",to->object);
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
  return phy_11_header_size;
}

int get_header_real_size(call_t *to, call_t *from) {
  return 8*phy_11_header_size;
}

int set_header(call_t *to, call_t *from, packet_t *packet, destination_t *dst) {


  struct phy_11_header *phy_11_header;
  field_t *field_header = packet_retrieve_field(packet,"PHY_header");

  if (field_header == NULL) {
    phy_11_header = malloc(sizeof(struct phy_11_header));
    field_t *field_phy_11_header = field_create(INT, sizeof(struct phy_11_header), phy_11_header);
    packet_add_field(packet, "PHY_header", field_phy_11_header);
    packet->size += get_header_size(to,from);
    packet->real_size += get_header_real_size(to,from);
  }
  else {
    phy_11_header = (struct phy_11_header *) field_getValue(field_header);
  }


  return 0;
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
