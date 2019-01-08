/**
 *  \file   802_15_4_u_csma_ca.c
 *  \brief  802.15.4 unslotted csma ca protocol
 *  \author Guillaume Chelius and Mickael Maman
 *  \date   2007
 **/
#include <stdio.h>

#include <kernel/include/modelutils.h>


/* ************************************************** */
/* ************************************************** */
#define STATE_IDLE    1
#define STATE_CCA     2
#define STATE_BACKOFF 3
#define STATE_TX      4
#define STATE_TXING   5
#define STATE_TXEND   6
#define STATE_TIMEOUT 7
#define STATE_ACK     8
#define STATE_ACKING  9
#define STATE_ACKED   10

/* ************************************************** */
/* ************************************************** */
#ifdef BPSK_868
#define macMinLIFSPeriod      2000000 /* 40 symbols cf p30 ref 802.15.4-2006 */
#define macMinSIFSPeriod      600000  /* 12 symbols cf p30 ref 802.15.4-2006 */
#define aUnitBackoffPeriod    1000000 /* 20 symbols cf p159 ref 802.15.4-2006 */
#define EDThresholdMin        -82 /* radio->mindBm + 10 cf p66 ref 802.15.4-2006 */
#define EDDuration			  400000   /* 8 symbols cf ref 802.15.4-2006 */
#elif BPSK_902
#define macMinLIFSPeriod      1000000 /* 40 symbols cf p30 ref 802.15.4-2006 */
#define macMinSIFSPeriod      300000  /* 12 symbols cf p30 ref 802.15.4-2006 */
#define aUnitBackoffPeriod    500000  /* 20 symbols cf p159 ref 802.15.4-2006 */
#define EDThresholdMin        -82     /* transceiver->mindBm + 10 cf p66 ref 802.15.4-2006 */
#define EDDuration			  200000   /* 8 symbols cf ref 802.15.4-2006 */
#elif OQPSK_2400
#define macMinLIFSPeriod      640000  /* 40 symbols cf p30 ref 802.15.4-2006 */
#define macMinSIFSPeriod      192000  /* 12 symbols cf p30 ref 802.15.4-2006 */
#define aUnitBackoffPeriod    320000   /* 20 symbols cf p159 ref 802.15.4-2006 */
#define EDThresholdMin        -75     /* transceiver->mindBm + 10 cf p66 ref 802.15.4-2006 */
#define EDDuration			  128000   /* 8 symbols cf ref 802.15.4-2006 */
#endif /* OQPSK_2400O */
#define macMinBE              3 /* cf p164 ref 802.15.4-2006 */
#define macMinBEMin           0 /* cf p164 ref 802.15.4-2006 */
#define macMaxBE              5 /* cf p163 ref 802.15.4-2006 */
#define macMaxBEMin           3 /* cf p163 ref 802.15.4-2006 */
#define macMaxBEMax           8 /* cf p163 ref 802.15.4-2006 */
#define macMaxCSMABackoffs    4 /* cf p163 ref 802.15.4-2006 */
#define macMaxCSMABackoffsMin 0 /* cf p163 ref 802.15.4-2006 */
#define macMaxCSMABackoffsMax 5 /* cf p163 ref 802.15.4-2006 */


//#define BEACON_TYPE			0
#define DATA_TYPE			1
#define ACK_TYPE			2
#define COMMAND_TYPE        3

#define ACK_SIZE 11

/* ************************************************** */
/* ************************************************** */
struct nodedata {
  uint64_t clock;
  int state;

  int MaxCSMABackoffs;
  int MinBE;
  int MaxBE;
  int BE;
  int NB;

  list_t *packets;
  packet_t *txbuf;

  double EDThreshold;
  int cs;
  int cca;
  int cca_beginning;
  
  int ack_activate;
  int MAC_max_retries;
  int MAC_retries;
  int size_last_MPDU;
  int size_last_RX_MPDU; 
  uint64_t bk_clock;
    
  int current_seq_num;
  int pending_state;
  int ack_seq_num;
  
};


/* ************************************************** */
/* ************************************************** */
struct _802_15_4_header {
//Frame control 2 Bytes  
  int FC_type; //Bit0-Bit2
  int FC_ack;  //Bit 5
// Sequence Number 1 Byte
  int SeqNum;
// CRC 2 Bytes
  int CRC;
};

struct _802_15_4_data_header {
// Destination and source addresses and PANID 6 Bytes
  nodeid_t src;
  nodeid_t dst;
  int PANID;
};

// cf. Zigbee Pro standard p339,  full MAC header is 11 but should be between 9 and 25.
int _802_15_4_header_size =  5; 
int _802_15_4_data_header_size =  6; 
/* ************************************************** */
/* ************************************************** */
model_t model =  {
  "802.15.4 unslotted csma ca mac module",
  "Guillaume Chelius and Mickael Maman",
  "0.1",
  MODELTYPE_MAC
};

int get_header_size(call_t *to, call_t *from);
int get_header_real_size(call_t *to, call_t *from); 
int set_header(call_t *to, call_t *from, packet_t *packet, destination_t *dst);

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

  nodedata->clock = 0;
  nodedata->state = STATE_IDLE;
  nodedata->packets = list_create();
  nodedata->txbuf = NULL;
  nodedata->cca = 1;
  nodedata->cs = 1;
  //nodedata->EDThreshold = EDThresholdMin;
  nodedata->MaxCSMABackoffs = macMaxCSMABackoffs;
  nodedata->MaxBE = macMaxBE;
  nodedata->MinBE = macMinBE;

  // by default best effort MAC
  nodedata->ack_activate = 0;
  nodedata->MAC_max_retries = 0;
  nodedata->MAC_retries = 0;
  nodedata->current_seq_num = get_random_integer_range(0,100);
  nodedata->pending_state=STATE_IDLE;
  nodedata->ack_seq_num=0;
  
  call_t to0   = {get_class_bindings_down(to)->elts[0], to->object};
  call_t from0 = {to->class, to->object};
  nodedata->EDThreshold = transceiver_get_sensibility(&to0, &from0)+10;
  
  list_init_traverse(params);
  while ((param = (param_t *) list_traverse(params)) != NULL) {
    if (!strcmp(param->key, "cca")) {
      if (get_param_integer(param->value, &(nodedata->cca))) {
	goto error;
      }
    }
    if (!strcmp(param->key, "cs")) {
      if (get_param_integer(param->value, &(nodedata->cs))) {
	goto error;
      }
    }
    if (!strcmp(param->key, "max-csma-backoffs")) {
      if (get_param_integer(param->value, &(nodedata->MaxCSMABackoffs))) {
	goto error;
      }
    }
    if (!strcmp(param->key, "max-backoff-exponent")) {
      if (get_param_integer(param->value, &(nodedata->MaxBE))) {
	goto error;
      }
    }
    if (!strcmp(param->key, "min-backoff-exponent")) {
      if (get_param_integer(param->value, &(nodedata->MinBE))) {
	goto error;
      }
    }
    if (!strcmp(param->key, "cca-threshold")) {
      if (get_param_double(param->value, &(nodedata->EDThreshold))) {
	goto error;
      }
    }
	if (!strcmp(param->key, "acknowledgement")) {
      if (get_param_integer(param->value, &(nodedata->ack_activate))) {
	goto error;
      }
	}
	if (!strcmp(param->key, "MAC_max_retries")) {
      if (get_param_integer(param->value, &(nodedata->MAC_max_retries))) {
	goto error;
      }
	  
    }
	
  }
  if (nodedata->EDThreshold > EDThresholdMin) {
    nodedata->EDThreshold = EDThresholdMin;
  }
  if ((nodedata->MaxCSMABackoffs < macMaxCSMABackoffsMin)
      || (nodedata->MaxCSMABackoffs > macMaxCSMABackoffsMax)) { 
    nodedata->MaxCSMABackoffs = macMaxCSMABackoffs;
  }
  if ((nodedata->MaxBE < macMaxBEMin)
      || (nodedata->MaxBE > macMaxBEMax)) { 
    nodedata->MaxBE = macMaxBE;
  }
  if ((nodedata->MinBE < macMinBEMin)
      || (nodedata->MinBE > nodedata->MaxBE)) { 
    nodedata->MinBE = macMinBE;
  }


  set_node_private_data(to, nodedata);
  return 0;

 error:
  free(nodedata);
  return -1;
}

int unbind(call_t *to) {
  struct nodedata *nodedata = get_node_private_data(to);
  packet_t *packet;
  while ((packet = (packet_t *) list_pop(nodedata->packets)) != NULL) {
    packet_dealloc(packet);
  }
  list_destroy(nodedata->packets);    
  if (nodedata->txbuf) {
    packet_dealloc(nodedata->txbuf);
  }
  free(nodedata);
  return 0;
}


/* ************************************************** */
/* ************************************************** */
int bootstrap(call_t *to) {
  call_t to0   = {get_class_bindings_down(to)->elts[0], to->object};
  call_t from0 = {to->class, to->object};

  //wake up the transceiver
  transceiver_wakeup(&to0, &from0);
  //start to listen the channel
  transceiver_switch_rx(&to0, &from0);

  return 0;
}

int ioctl(call_t *to, int option, void *in, void **out) {
	if (option ==100)
		*((int *) *(out) ) = 1;
  return 0;
}



/* ************************************************** */
/* ************************************************** */
int check_channel_busy(call_t *to) {
  struct nodedata *nodedata = get_node_private_data(to);
  call_t to0   = {get_class_bindings_down(to)->elts[0], to->object};
  call_t from0 = {to->class, to->object};

  if (nodedata->cs && (transceiver_get_cs(&to0, &from0) >= nodedata->EDThreshold)) {
    return 1;
  }
  if (nodedata->cca && (transceiver_get_noise(&to0, &from0) >= nodedata->EDThreshold)) {
    return 1;
  }
  return 0;
}

int state_machine(call_t *to, call_t *from, void *args) { 
  struct nodedata *nodedata = get_node_private_data(to);
  call_t to0   = {get_class_bindings_down(to)->elts[0], to->object};
  call_t from0 = {to->class, to->object};

  uint64_t timeout;
  uint64_t ack_timeout;
  uint64_t backoff;
  packet_t *packet;	
  packet_t *ack_packet;
  
  if (nodedata->clock != get_time()) {
    return 0;
  }

  switch (nodedata->state) {
		
  case STATE_IDLE:
    if (nodedata->txbuf == NULL) {
      nodedata->txbuf = (packet_t *) list_pop_FIFO(nodedata->packets);
	  nodedata->MAC_retries=0;
      if (nodedata->txbuf == NULL) {
	return 0;
      }
    }
    
    if (nodedata->MaxCSMABackoffs != 0) {
      nodedata->state = STATE_BACKOFF;
      nodedata->BE = nodedata->MinBE;
      nodedata->NB = 0;
	  // FIXED HOW TO CALCULATE BACKOFF: IT SHALL BE INTEGER BETWEEN 0 AND 2^(BE-1)
	  //not backoff = get_random_double() * (pow(2, nodedata->BE) - 1) * aUnitBackoffPeriod;	
	  backoff = get_random_integer_range(0,(pow(2, nodedata->BE) - 1)) * aUnitBackoffPeriod;
	 
      nodedata->clock = get_time() + backoff;  
      scheduler_add_callback(nodedata->clock, to, from, state_machine, NULL);
      return 0;	
    } else {
      nodedata->state = STATE_TX;
      nodedata->clock = get_time();  
      scheduler_add_callback(nodedata->clock, to, from, state_machine, NULL);
      return 0;
    }		
    break;
	
  case STATE_BACKOFF:
	
	 nodedata->cca_beginning=check_channel_busy(to);
	 nodedata->state = STATE_CCA;
     nodedata->clock = get_time()+EDDuration;  
     scheduler_add_callback(nodedata->clock, to, from, state_machine, NULL);
	return 0;
    break;
	
  case STATE_CCA:
    if (check_channel_busy(to)||nodedata->cca_beginning) { 
      if ((++nodedata->BE) > nodedata->MaxBE) {
	nodedata->BE = nodedata->MaxBE;
      } 
      if (++nodedata->NB >= nodedata->MaxCSMABackoffs) {
		
		if (++nodedata->MAC_retries > nodedata->MAC_max_retries) {
			packet_dealloc(nodedata->txbuf);            
			nodedata->txbuf = NULL;
			}
		nodedata->state = STATE_IDLE;
		nodedata->clock = get_time();
		state_machine(to, from, NULL);
		return 0;
      }
	  nodedata->state = STATE_BACKOFF;
      //backoff = get_random_double() * (pow(2, nodedata->BE) - 1) * aUnitBackoffPeriod;
	  backoff = get_random_integer_range(0,(pow(2, nodedata->BE) - 1)) * aUnitBackoffPeriod;
      nodedata->clock = get_time() + backoff;
      scheduler_add_callback(nodedata->clock, to, from, state_machine, NULL);
      return 0;
    }
        
    nodedata->state = STATE_TX;
    nodedata->clock = get_time();  
    state_machine(to, from, NULL);
    return 0;
     break;
	 
  case STATE_TX:
   // packet = nodedata->txbuf;
   // nodedata->txbuf = NULL;
	
	packet = packet_clone(nodedata->txbuf);
    
//packet size = NTW+MAC headers. Need to add PHY header size duration in timeout
// TODO is it necessary to update for several PHY layers?
 field_t *PHY_field_header = packet_retrieve_field(packet,"PHY_header");
 if (PHY_field_header == NULL) {
    call_t to0 = {get_class_bindings_down(to)->elts[0], to->object};
    timeout = (packet->size + GET_HEADER_SIZE(&to0,to)) * 8 * transceiver_get_Tb(&to0, &from0); 
	
	nodedata->size_last_MPDU = packet->size-get_header_size(to,from);
	}
  else {
    timeout = packet->size * 8 * transceiver_get_Tb(&to0, &from0);
	
	call_t to0 = {get_class_bindings_down(to)->elts[0], to->object};
	nodedata->size_last_MPDU = packet->size - get_header_size(to,from)- GET_HEADER_SIZE(&to0,to);
	
   }

    PRINT_MAC("[154_UCSMA/CA] node %d send a Data packet %d: packet_size=%d   \n", to->object, packet->id, packet->size);

    TX(&to0, &from0, packet); 

    nodedata->state = STATE_TXING;
    nodedata->clock = get_time() + timeout;
    scheduler_add_callback(nodedata->clock, to, from, state_machine, NULL);
    return 0;
    break;
	
  case STATE_TXING:
    nodedata->state = STATE_TXEND;

    //switch back in RX MODE
    transceiver_switch_rx(&to0, &from0);

    nodedata->clock = get_time();
    state_machine(to, from, NULL);
    return 0;

  case STATE_TXEND:

    if (nodedata->ack_activate)
	{
   field_t *field_header = packet_retrieve_field(nodedata->txbuf,"_802_15_4_header");
   struct _802_15_4_header *header;
   header = (struct _802_15_4_header *) field_getValue(field_header);
	if (header->FC_ack)
	{
	
	//prepare ACK timeout
	nodedata->state = STATE_TIMEOUT;
	call_t to0 = {get_class_bindings_down(to)->elts[0], to->object};
	
	if (nodedata->size_last_MPDU > 18)
	{
	ack_timeout= macMinSIFSPeriod + ACK_SIZE*8* transceiver_get_Tb(&to0, &from0)+ macMinLIFSPeriod;
    }
	else
	{
	ack_timeout= macMinSIFSPeriod + ACK_SIZE*8* transceiver_get_Tb(&to0, &from0)+ macMinSIFSPeriod;
	}
	nodedata->clock = get_time() + ack_timeout;
    scheduler_add_callback(nodedata->clock, to, from, state_machine, NULL);
	
	
	return 0;
	}
	else
	{
	//end of transmission because no ack required
    nodedata->state = STATE_IDLE;
	packet_dealloc(nodedata->txbuf);            
	nodedata->txbuf = NULL;
	
	// Wait SIFS if short frame or LIFS if long frame (> 18 Bytes) before being IDLE 
	if (nodedata->size_last_MPDU > 18)
	{
	nodedata->clock = get_time()+ macMinLIFSPeriod;
    }
	else
	{
	nodedata->clock = get_time()+ macMinSIFSPeriod;
	}
	
    scheduler_add_callback(nodedata->clock, to, from, state_machine, NULL);

    return 0;
	
	}
	}
	else{
	
	//end of transmission
    nodedata->state = STATE_IDLE;
	packet_dealloc(nodedata->txbuf);            
	nodedata->txbuf = NULL;
	
	// Wait SIFS if short frame or LIFS if long frame (> 18 Bytes) before being IDLE 
	if (nodedata->size_last_MPDU > 18)
	{
	nodedata->clock = get_time()+ macMinLIFSPeriod;
    }
	else
	{
	nodedata->clock = get_time()+ macMinSIFSPeriod;
	}
	
    scheduler_add_callback(nodedata->clock, to, from, state_machine, NULL);

    return 0;
	
	}
    break;
	
  case STATE_TIMEOUT:
	//transmission failed

	PRINT_MAC("[154_UCSMA/CA] node %d ACK Timeout   \n", to->object);
	
	
	nodedata->state = STATE_IDLE;
	if (++nodedata->MAC_retries > nodedata->MAC_max_retries) {
			packet_dealloc(nodedata->txbuf);            
			nodedata->txbuf = NULL;
			}
	nodedata->clock = get_time();
    scheduler_add_callback(nodedata->clock, to, from, state_machine, NULL);
	
	return 0;
    break;
	
  case STATE_ACK:
	//prepare to send ACK
	ack_packet=packet_create(to,_802_15_4_header_size,-1);
	
	struct _802_15_4_header *ack_header;
	ack_header = malloc(sizeof(struct _802_15_4_header));
    field_t *field_802_15_4_header = field_create(INT, sizeof(struct _802_15_4_header), ack_header);
    packet_add_field(ack_packet, "_802_15_4_header", field_802_15_4_header);
	ack_header->FC_type= ACK_TYPE;
	ack_header->SeqNum = nodedata->ack_seq_num;

	PRINT_MAC("[154_UCSMA/CA] node %d sends back ACK packet %d : packet_size=%d   \n", to->object, ack_packet->id, ack_packet->size);
	
	
	TX(&to0, &from0,ack_packet);

	nodedata->state = STATE_ACKING;
	nodedata->clock = get_time()+ ACK_SIZE*8* transceiver_get_Tb(&to0, &from0);
    scheduler_add_callback(nodedata->clock, to, from, state_machine, NULL);
	
	return 0;
    break;
	
 case STATE_ACKING:
    nodedata->state = STATE_ACKED;
    //switch back in RX MODE
    transceiver_switch_rx(&to0, &from0);
    nodedata->clock = get_time();
    state_machine(to, from, NULL);
	return 0;
    break;
	
 case STATE_ACKED:
	// go back to STATE IDLE OR STATE_BACKOFF
    nodedata->state = nodedata->pending_state;
	
	if (nodedata->size_last_RX_MPDU > 18)
	{
	nodedata->clock = get_time()+ macMinLIFSPeriod;
	}
	else
	{
    nodedata->clock = get_time()+ macMinSIFSPeriod;
	}
	
	if (nodedata->clock > nodedata->bk_clock)
	{
	// backoff expired during the ACK procedure
	// forcing state idle to launch a new backoff
		nodedata->state =STATE_IDLE;
	}
	
    scheduler_add_callback(nodedata->clock, to, from, state_machine, NULL);
	return 0;
    break;	
	
  default:
    break;
  }

  return 0;
}
     

/* ************************************************** */
/* ************************************************** */
int set_header(call_t *to, call_t *from, packet_t *packet, destination_t *dst) {
  
  struct nodedata *nodedata = get_node_private_data(to);
  field_t *field_header = packet_retrieve_field(packet,"_802_15_4_header");
  struct _802_15_4_header *header;
  if (field_header == NULL) {
   header = malloc(sizeof(struct _802_15_4_header));
   field_t *field_802_15_4_header = field_create(INT, sizeof(struct _802_15_4_header), header);
   packet_add_field(packet, "_802_15_4_header", field_802_15_4_header);
   packet->size += _802_15_4_header_size;
   packet->real_size += _802_15_4_header_size * 8;
  }
  else {
    header = (struct _802_15_4_header *) field_getValue(field_header);
  }

  if (dst->id==BROADCAST_ADDR) {
  //desactivate ack whatever the strategy for BROADCAST packet
  header->FC_ack=0;
  }
  else{
  if (packet->frame_control_type==1)
  {
  header->FC_ack=0;
  }
  else
  {
  header->FC_ack=nodedata->ack_activate; //include ack policy in FC
  }
  
  }
  
  header->FC_type=DATA_TYPE;
  header->SeqNum=nodedata->current_seq_num; // TODO add sequence number
  nodedata->current_seq_num=nodedata->current_seq_num+1;
 
  field_t *field_data_header = packet_retrieve_field(packet,"_802_15_4_data_header");
  struct _802_15_4_data_header *data_header;
  if (field_data_header == NULL) {
   data_header = malloc(sizeof(struct _802_15_4_data_header));
   field_t *field_802_15_4_data_header = field_create(INT, sizeof(struct _802_15_4_data_header), data_header);
   packet_add_field(packet, "_802_15_4_data_header", field_802_15_4_data_header);
   packet->size += _802_15_4_data_header_size;
   packet->real_size += _802_15_4_data_header_size*8;
  }
  else {
    data_header = (struct _802_15_4_data_header *) field_getValue(field_data_header);
  }
  
  data_header->dst = dst->id;
  data_header->src = to->object;
  //data_header->PANID=0; // TODO add real PANID 

 
  return 0;

}

int get_header_size(call_t *to, call_t *from) {
  return (_802_15_4_header_size + _802_15_4_data_header_size);
}

int get_header_real_size(call_t *to, call_t *from) {
  return (8 * (_802_15_4_header_size + _802_15_4_data_header_size));
}


/* ************************************************** */
/* ************************************************** */
void tx(call_t *to, call_t *from, packet_t *packet) {
  struct nodedata *nodedata = get_node_private_data(to);

    // encapsulation of MAC header
    destination_t destination = packet->destination;
    if (set_header(to, from, packet, &destination) == -1) {
      packet_dealloc(packet);
      return;
    }

  list_insert(nodedata->packets, (void *) packet);

  if (nodedata->state == STATE_IDLE) {
    nodedata->clock = get_time();  
    state_machine(to, from, NULL);
  } 
}


/* ************************************************** */
/* ************************************************** */
void rx(call_t *to, call_t *from, packet_t *packet) {
  struct nodedata *nodedata = get_node_private_data(to);
  field_t *field_header;
  field_t *field_data_header;
  struct _802_15_4_header *header;
  struct _802_15_4_data_header *data_header;
  array_t *up = get_class_bindings_up(to);
  int i = up->size;
 

  call_t to0   = {get_class_bindings_down(to)->elts[0], to->object};
  call_t from0 = {to->class, to->object};
  //switch back in RX MODE
  transceiver_switch_rx(&to0, &from0);

  field_header = packet_retrieve_field(packet, "_802_15_4_header");
   if (field_header == NULL) {
    fprintf(stderr, "error in 802_15_4_u_csma_ca rx(), field _802_15_4_header not found\n");
    packet_dealloc(packet);
    return;
  }
  header = (struct _802_15_4_header *) field_getValue(field_header); 
  
  switch (header->FC_type)
  {
  
  case DATA_TYPE:
//receive DATA packet	
  field_data_header = packet_retrieve_field(packet, "_802_15_4_data_header");
  if (field_data_header == NULL) {
    fprintf(stderr, "error in 802_15_4_u_csma_ca rx(), field _802_15_4_data_header not found\n");
    packet_dealloc(packet);
    return;
  }
  data_header = (struct _802_15_4_data_header *) field_getValue(field_data_header);
  
  if ( (nodedata->state != STATE_IDLE) && (nodedata->state != STATE_BACKOFF)) {
    packet_dealloc(packet);
    return;
  
  }

  if ((data_header->dst != to->object) && (data_header->dst != BROADCAST_ADDR)) {
    packet_dealloc(packet);
    return;
  }

    PRINT_MAC("[154_UCSMA/CA] node %d received a Data packet %d from %d: packet_size=%d   \n", to->object, packet->id, data_header->src, packet->size);

	
	if ((data_header->dst == to->object)&&(header->FC_ack)) 
	{
	// send ack after SIFS
	nodedata->pending_state=nodedata->state;
	nodedata->bk_clock=nodedata->clock;
	nodedata->ack_seq_num=header->SeqNum;
	
	call_t to0 = {get_class_bindings_down(to)->elts[0], to->object};
	nodedata->size_last_RX_MPDU = packet->size - get_header_size(to,from)- GET_HEADER_SIZE(&to0,to);
	
	nodedata->state = STATE_ACK;
	nodedata->clock = get_time()+ macMinSIFSPeriod;
    scheduler_add_callback(nodedata->clock, to, from, state_machine, NULL);
	
	}
	
  /* forward packet to upper layer */ 
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

  return;
  
  break;
  
  case ACK_TYPE: 
  
  if (nodedata->state != STATE_TIMEOUT) {
  // this ACK was not expected
  packet_dealloc(packet);
  return;
  }
  
  field_t *field_txbuf_header = packet_retrieve_field(nodedata->txbuf,"_802_15_4_header");
  struct _802_15_4_header *txbuf_header;
  txbuf_header = (struct _802_15_4_header *) field_getValue(field_txbuf_header);
  
  	PRINT_MAC("[154_UCSMA/CA] node %d RX ACK \n", to->object);
  
    // check if it is the right ACK 
  if (header->SeqNum == txbuf_header->SeqNum)
  {
  	packet_dealloc(nodedata->txbuf);            
	nodedata->txbuf = NULL;
  	nodedata->state = STATE_IDLE;
	
	// Wait SIFS if short frame or LIFS if long frame (> 18 Bytes) before being IDLE 
	if (nodedata->size_last_MPDU > 18)
	{
	nodedata->clock = get_time()+ macMinLIFSPeriod;
    }
	else
	{
	nodedata->clock = get_time()+ macMinSIFSPeriod;
	}
    scheduler_add_callback(nodedata->clock, to, from, state_machine, NULL);
  }
  packet_dealloc(packet);
  
  return;
  break;
  
  default:
  return;
  break;
}

}

/* ************************************************** */
/* ************************************************** */
mac_methods_t methods = {rx, 
                         tx,
                         set_header, 
                         get_header_size,
                         get_header_real_size};


    
    
