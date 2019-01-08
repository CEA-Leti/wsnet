/**
 *  \file   dcf_802_11.c
 *  \brief  MAC 802.11 DCF protocol
 *  \author Guillaume Chelius and Elyes Ben Hamida and Mickael Maman
 *  \date   2007
 **/
#include <stdio.h>

#include <kernel/include/modelutils.h>


/**
 * TODO: 
 *  - ajuster la taille des paquets 
 * - verifier le backoff pour le difs au debut ou direct backoff
 *  - tenter d'avoir differentes longueurs de paquet, gerer le multi-debit ????
 **/
#define SPEED_LIGHT 3000


/* ************************************************** */
/* ************************************************** */
#define STATE_IDLE		1
#define STATE_BACKOFF		2
#define STATE_RTS		3
#define STATE_TIMEOUT		4
#define STATE_CTS		5
#define STATE_CTS_TIMEOUT	6
#define STATE_DATA		7
#define STATE_BROADCAST		8
#define STATE_ACK		9
#define STATE_DONE		10
#define STATE_BROAD_DONE	11


/* ************************************************** */
/* ************************************************** */
#define macMinDIFSPeriod      50000   
#define macMinSIFSPeriod      10000
#define macMinBE              5     /* 32 slots */
#define macMaxBE              10    /* 1024 slots */
#define macMaxCSMARetries     7     /* 7 trials before dropping */
#define aUnitBackoffPeriod    20000
#define EDThresholdMin        -74


/* ************************************************** */
/* ************************************************** */
#define RTS_TYPE			1
#define CTS_TYPE			2
#define DATA_TYPE			3
#define ACK_TYPE			4
#define BROADCAST_TYPE                  5


/* ************************************************** */
/* ************************************************** */
#define DATA_PADDING_SIZE 8

/* ************************************************** */
/* ************************************************** */
/* ************************************************** */
/* ************************************************** */
struct _dcf_802_11_header {
    int src;
    int dst;
    int type;
};

// FCS is included in transceiver

//      ----------------------------    ------
//RTS   | FC  | Duration | RA | TA |     FCS |
//      ----------------------------    ------
//         2       2       6    6         4

struct _dcf_802_11_rts_header {
    uint64_t nav;
    int size;
};

//      -----------------------    ------
//CTS   | FC  | Duration | RA |     FCS |
//      -----------------------    ------
//         2       2       6         4

struct _dcf_802_11_cts_header {
    uint64_t nav;
};

//      ---------------------------------------------------------------------    ----------------
//CTS   | FC  | Duration + id | @1 | @2 | @3 | Seq Control | @4 | QoSControl|     Payload | FCS |
//      ---------------------------------------------------------------------    ----------------
//         2          2          6    6    6        2         6        2          X      4

struct _dcf_802_11_data_header {
    uint64_t nav;
    int size;
    char padding[DATA_PADDING_SIZE];
};

//      -----------------------    ------
//ACK   | FC  | Duration | RA |     FCS |
//      -----------------------    ------
//         2       2       6         4

struct _dcf_802_11_ack_header {};

//According to std IEEE802.11
//RTS global MAC header Size= 16 = _dcf_802_11_header_size + _dcf_802_11_rts_header_size
//CTS global MAC header Size= 10
//DATA global MAC header Size= 32
//ACK global MAC header Size= 10

int _dcf_802_11_header_size = 10;
int _dcf_802_11_rts_header_size = 6;
int _dcf_802_11_cts_header_size = 0;
int _dcf_802_11_data_header_size = 22;
int _dcf_802_11_ack_header_size = 0;

/* ************************************************** */
/* ************************************************** */
struct nodedata {
  uint64_t clock;
  uint64_t clock_pkt;
  int state;
  int state_pending;
  int dst;
  int size;

  uint64_t backoff;
  int backoff_suspended;
  int NB;
  int BE;

  uint64_t nav;
  int rts_threshold;

  list_t *packets;
  packet_t *txbuf;

  int cs;
  int cca;
  double EDThreshold;
  ;
};

struct classdata {
  int maxCSMARetries;
};


/* ************************************************** */
/* ************************************************** */
model_t model =  {
  "802.11 DCF mac module",
  "Guillaume Chelius and Elyes Ben Hamida and Mickael Maman",
  "0.1",
  MODELTYPE_MAC
};

/* ************************************************** */
/* ************************************************** */
int set_header(call_t *to, call_t *from, packet_t *packet, destination_t *dst) {



  /**********************/
  /* _dcf_802_11_header */
  /**********************/
  field_t *field_header = packet_retrieve_field(packet,"_dcf_802_11_header");
  struct _dcf_802_11_header *header;
  if (field_header == NULL) {
   header = malloc(sizeof(struct _dcf_802_11_header));
   field_t *field_dcf_802_11_header = field_create(INT, sizeof(struct _dcf_802_11_header), header);
   packet_add_field(packet, "_dcf_802_11_header", field_dcf_802_11_header);
   packet->size += _dcf_802_11_header_size ;
   packet->real_size += 8*_dcf_802_11_header_size ;
  }
  else {
    header = (struct _dcf_802_11_header *) field_getValue(field_header);
  }


  /***************************/
  /* _dcf_802_11_data_header */
  /***************************/
  field_t *field_data_header = packet_retrieve_field(packet,"_dcf_802_11_data_header");
  struct _dcf_802_11_data_header *data_header;
  if (field_data_header == NULL) {
   data_header = malloc(sizeof(struct _dcf_802_11_data_header));
   field_t *field_dcf_802_11_data_header = field_create(INT, sizeof(struct _dcf_802_11_data_header), data_header);
   packet_add_field(packet, "_dcf_802_11_data_header", field_dcf_802_11_data_header);
   packet->size += _dcf_802_11_data_header_size ;
   packet->real_size += 8*_dcf_802_11_data_header_size ;
  }
  else {
    data_header = (struct _dcf_802_11_data_header *) field_getValue(field_data_header);
  }


    if ((header->dst = dst->id) == BROADCAST_ADDR) {
        header->type =BROADCAST_TYPE;
    } else {
        header->type = DATA_TYPE;
    }
    header->src = to->object;
    data_header->size = packet->size;

    return 0;


//   /**************************/
//   /* _dcf_802_11_rts_header */
//   /**************************/
//   *_dcf_802_11_rts_header_nav = 27511LL;
//   *_dcf_802_11_rts_header_size = 8*8;
//   strcpy(_dcf_802_11_rts_header_padding, "abcdefg");
//   /**************************/
//   /* _dcf_802_11_cts_header */
//   /**************************/
//   *_dcf_802_11_cts_header_nav = 27511LL;
//   strcpy(_dcf_802_11_cts_header_padding, "abcde");
//   /***************************/
//   /* _dcf_802_11_data_header */
//   /***************************/
//   *_dcf_802_11_data_header_nav = 27511LL;
//   strcpy(_dcf_802_11_data_header_padding, "abcdefghijklmnopqrstu");
//   /**************************/
//   /* _dcf_802_11_ack_header */
//   /**************************/
//   strcpy(_dcf_802_11_ack_header_padding, "abcdefghijklm");

}



/* ************************************************** */
/* ************************************************** */
int init(call_t *to, void *params) {
  struct classdata *classdata = malloc(sizeof(struct classdata));
  param_t *param;

  /* default values */
  classdata->maxCSMARetries = macMaxCSMARetries;

  /* get parameters */
  list_init_traverse(params);
  while ((param = (param_t *) list_traverse(params)) != NULL) {
    if (!strcmp(param->key, "retry")) {
      if (get_param_integer(param->value, &(classdata->maxCSMARetries))) {
	goto error;
      }
    }
  }

  set_class_private_data(to, classdata);
  return 0;

 error:
  free(classdata);
  return -1;
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
  nodedata->rts_threshold = 500;
  nodedata->clock = 0;
  nodedata->clock_pkt = 0;
  nodedata->state = STATE_IDLE;
  nodedata->state_pending = STATE_IDLE;
  nodedata->nav = 0;
  nodedata->cca = 1;
  nodedata->cs = 1;
  nodedata->EDThreshold = EDThresholdMin;

  /* Init packets buffer */
  nodedata->packets = list_create();
  nodedata->txbuf = NULL;

  /* get params */
  list_init_traverse(params);
  while ((param = (param_t *) list_traverse(params)) != NULL) {
    if (!strcmp(param->key, "cca-threshold")) {
      if (get_param_double(param->value, &(nodedata->EDThreshold))) {
	goto error;
      }
    }
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
    if (!strcmp(param->key, "rts-threshold")) {
      if (get_param_integer(param->value, &(nodedata->rts_threshold))) {
	goto error;
      }
    }
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


int switch_rx_after_tx(call_t *to, call_t *from, void *args) {
  struct nodedata *nodedata = get_node_private_data(to);  
  call_t to0   = {get_class_bindings_down(to)->elts[0], to->object};
  call_t from0 = {to->class, to->object};

  /* Drop unscheduled events */
  if (nodedata->clock_pkt != get_time()) {
    return 0;
  }

  //switch back in RX MODE
  transceiver_switch_rx(&to0, &from0);

  return 0;
}

int dcf_802_11_state_machine(call_t *to, call_t *from, void *args) { 
  struct nodedata *nodedata = get_node_private_data(to);
  struct classdata *classdata = get_class_private_data(to);
  packet_t *packet;	    
  struct _dcf_802_11_header *header;
  struct _dcf_802_11_data_header *data_header;
  uint64_t timeout;
  uint64_t timeout_pkt;
  call_t to0   = {get_class_bindings_down(to)->elts[0], to->object};
  call_t from0 = {to->class, to->object};

  int phy_header_size=GET_HEADER_SIZE(&to0,to);
        
  /* Drop unscheduled events */
  if (nodedata->clock != get_time()) {
    return 0;
  }
    
  /* State machine */
  switch (nodedata->state) {
		
  case STATE_IDLE:
    /* Next packet to send */
    if (nodedata->txbuf == NULL) {
      nodedata->txbuf = (packet_t *) list_pop_FIFO(nodedata->packets);
      if (nodedata->txbuf == NULL) {
	return 0;
      }
    }

    /* Initial backoff */
    nodedata->state = STATE_BACKOFF;
    nodedata->state_pending = STATE_BACKOFF;				        
    nodedata->BE = macMinBE - 1;
    nodedata->NB = 0;
    nodedata->backoff = macMinDIFSPeriod;
    nodedata->backoff_suspended = 0;
			
    /* Backoff */
    nodedata->clock = get_time();  
    dcf_802_11_state_machine(to, from, NULL);
    return 0;			
        
  case STATE_BACKOFF:
    /* If the backoff is over, set to 0 */
    if ((nodedata->backoff > 0) && (nodedata->backoff < aUnitBackoffPeriod)) {
      nodedata->backoff = 0;
      nodedata->clock = get_time();
      dcf_802_11_state_machine(to, from, NULL);
      return 0;
    }
    /* Backoff */
    if (nodedata->backoff > 0) {
      if ((get_time() < nodedata->nav) || (check_channel_busy(to))) { 
	if (nodedata->backoff_suspended == 0) {
	  /* Suspend backoff and add difs */
	  nodedata->backoff_suspended = 1;
	  nodedata->backoff = nodedata->backoff + macMinDIFSPeriod;
	}			
      } else {
	/* Decrease backoff */
	nodedata->backoff_suspended = 0;
	nodedata->backoff = nodedata->backoff - aUnitBackoffPeriod;
      }
            
      /* Set next event to backoff */
      nodedata->clock = get_time() + aUnitBackoffPeriod;
      scheduler_add_callback(nodedata->clock, to, from, dcf_802_11_state_machine, NULL);
      return 0;
    }
        


    /* Broadcast or unicast */
    field_t *field_header = packet_retrieve_field(nodedata->txbuf,"_dcf_802_11_header");

    header = (struct _dcf_802_11_header *) field_getValue(field_header);


    if (header->dst  == BROADCAST_ADDR) {
      nodedata->state = STATE_BROADCAST;

    } else {
      /* RTS or Data */
      if (nodedata->txbuf->size < nodedata->rts_threshold) {
	nodedata->state = STATE_DATA;
      } else {
	nodedata->state = STATE_RTS;
      }
    }
    nodedata->state_pending = STATE_IDLE;
    nodedata->dst = header->dst;
 


    /* Next state */
    nodedata->clock = get_time();  
    dcf_802_11_state_machine(to, from, NULL);
    return 0;
        
        
  case STATE_RTS:
    
        /* Build RTS */
       packet = packet_create(to, _dcf_802_11_header_size + _dcf_802_11_rts_header_size, -1);

       struct _dcf_802_11_header *header;
       header = malloc(sizeof(struct _dcf_802_11_header));
       field_t *field_dcf_802_11_header = field_create(INT, sizeof(struct _dcf_802_11_header), header);
       packet_add_field(packet, "_dcf_802_11_header", field_dcf_802_11_header);

       header->dst = nodedata->dst;
       header->src = to->object;
       header->type = RTS_TYPE; 

       struct _dcf_802_11_rts_header *rts_header;
       rts_header = malloc(sizeof(struct _dcf_802_11_rts_header));
       field_t *field_dcf_802_11_rts_header = field_create(INT, sizeof(struct _dcf_802_11_rts_header), rts_header);
       packet_add_field(packet, "_dcf_802_11_rts_header", field_dcf_802_11_rts_header);

	rts_header->size = nodedata->txbuf->size;

        rts_header->nav = macMinSIFSPeriod 
	+ (_dcf_802_11_header_size + _dcf_802_11_cts_header_size + phy_header_size) * transceiver_get_Tb(&to0, &from0) * 8 
	+ macMinSIFSPeriod 
	+ (nodedata->txbuf->size + phy_header_size) * 8 * transceiver_get_Tb(&to0, &from0) 
	+ macMinSIFSPeriod 
	+ (_dcf_802_11_header_size + _dcf_802_11_ack_header_size + phy_header_size) * 8 * transceiver_get_Tb(&to0, &from0);	
			

	timeout_pkt = (_dcf_802_11_header_size + _dcf_802_11_rts_header_size + phy_header_size) * 8 * transceiver_get_Tb(&to0, &from0);

 	timeout = (_dcf_802_11_header_size + _dcf_802_11_rts_header_size + phy_header_size) * 8 * transceiver_get_Tb(&to0, &from0) 
	+ macMinSIFSPeriod 
	+ (_dcf_802_11_header_size + _dcf_802_11_cts_header_size + phy_header_size) * 8 * transceiver_get_Tb(&to0, &from0) 
	+ SPEED_LIGHT; 

 PRINT_MAC("[DCF] node %d send a RTS packet %d: packet_size=%d   \n", to->object, packet->id, packet->size);

    /* Send RTS */
    TX(&to0, &from0, packet); 

    // switch RX after transmission
    nodedata->clock_pkt = get_time() + timeout_pkt;
    scheduler_add_callback(nodedata->clock_pkt, to, from, switch_rx_after_tx, NULL);

    /* Wait for timeout or CTS */
    nodedata->state = STATE_TIMEOUT;
    nodedata->clock = get_time() + timeout;
    scheduler_add_callback(nodedata->clock, to, from, dcf_802_11_state_machine, NULL);		

    return 0;
        
        
  case STATE_TIMEOUT:

PRINT_MAC("[DCF] node %d Timeout \n", to->object);
			
    if ((++nodedata->NB) >= classdata->maxCSMARetries) {
      /* Transmit retry limit reached */
      packet_dealloc(nodedata->txbuf);            
      nodedata->txbuf = NULL;
      /* Return to idle */
      nodedata->state = STATE_IDLE;
      nodedata->clock = get_time();
      dcf_802_11_state_machine(to, from, NULL);
      return 0;
    }
			
    /* Update backoff */
    if ((++nodedata->BE) > macMaxBE) {
      nodedata->BE = macMaxBE;
    }
    nodedata->backoff = get_random_double() 
      * (pow(2, nodedata->BE) - 1) 
      * aUnitBackoffPeriod 
      + macMinDIFSPeriod;
    nodedata->backoff_suspended = 0;
    nodedata->state = STATE_BACKOFF;
    nodedata->state_pending = STATE_BACKOFF;				
        
    /* Backoff */
    nodedata->clock = get_time();  
    dcf_802_11_state_machine(to, from, NULL);
    return 0;
			
        
  case STATE_CTS:
 

  /* Build CTS */
  packet = packet_create(to, _dcf_802_11_header_size + _dcf_802_11_cts_header_size, -1);

       struct _dcf_802_11_header *header2;
       header2 = malloc(sizeof(struct _dcf_802_11_header));
       field_t *field_dcf_802_11_header2 = field_create(INT, sizeof(struct _dcf_802_11_header), header2);
       packet_add_field(packet, "_dcf_802_11_header", field_dcf_802_11_header2);

       header2->dst = nodedata->dst;
       header2->src = to->object;
       header2->type = CTS_TYPE; 

       struct _dcf_802_11_cts_header *cts_header;
       cts_header = malloc(sizeof(struct _dcf_802_11_cts_header));
       field_t *field_dcf_802_11_cts_header = field_create(INT, sizeof(struct _dcf_802_11_cts_header), cts_header);
       packet_add_field(packet, "_dcf_802_11_cts_header", field_dcf_802_11_cts_header);


        cts_header->nav = macMinSIFSPeriod 
	+ (nodedata->size + phy_header_size) * 8 * transceiver_get_Tb(&to0, &from0) 
	+ macMinSIFSPeriod 
	+ (_dcf_802_11_header_size + _dcf_802_11_ack_header_size + phy_header_size) * 8 * transceiver_get_Tb(&to0, &from0);
			

	timeout_pkt = (_dcf_802_11_header_size + _dcf_802_11_cts_header_size + phy_header_size) * 8 * transceiver_get_Tb(&to0, &from0) ;

 	timeout = (_dcf_802_11_header_size + _dcf_802_11_cts_header_size + phy_header_size) * 8 * transceiver_get_Tb(&to0, &from0) 
	+ macMinSIFSPeriod 
        + (nodedata->size + phy_header_size) * 8 * transceiver_get_Tb(&to0, &from0)
	+ SPEED_LIGHT; 

PRINT_MAC("[DCF] node %d send a CTS packet %d: packet_size=%d   \n", to->object, packet->id, packet->size);

     /* Send CTS */
     TX(&to0, &from0, packet); 

    // switch RX after transmission
    nodedata->clock_pkt = get_time() + timeout_pkt;
    scheduler_add_callback(nodedata->clock_pkt, to, from, switch_rx_after_tx, NULL);
        
    /* Wait for timeout or DATA */
    nodedata->state = STATE_CTS_TIMEOUT;
    nodedata->clock = get_time() + timeout;
    scheduler_add_callback(nodedata->clock, to, from, dcf_802_11_state_machine, NULL);
    return 0;
        
        
  case STATE_CTS_TIMEOUT:
    /* Return to pending state */
    nodedata->state = nodedata->state_pending;
    if (nodedata->state != STATE_IDLE)
      {
	field_t *field_header = packet_retrieve_field(nodedata->txbuf,"_dcf_802_11_header");
	header = (struct _dcf_802_11_header *) field_getValue(field_header);
 	nodedata->dst = header->dst;
      }
    nodedata->clock = get_time();
    dcf_802_11_state_machine(to, from, NULL);
    return 0;
			
        
  case STATE_DATA:
    /* Build data packet */	
    packet = packet_clone(nodedata->txbuf);
    
    field_t *field_data_header = packet_retrieve_field(packet,"_dcf_802_11_data_header");
    data_header = (struct _dcf_802_11_data_header *) field_getValue(field_data_header);
    
    data_header->nav = macMinSIFSPeriod 
	+ (_dcf_802_11_header_size + _dcf_802_11_ack_header_size + phy_header_size) * 8 * transceiver_get_Tb(&to0, &from0);
        
    timeout_pkt = (packet->size + phy_header_size) * 8 * transceiver_get_Tb(&to0, &from0);

    timeout = (packet->size + phy_header_size) * 8 * transceiver_get_Tb(&to0, &from0) 
	+ macMinSIFSPeriod 
	+ (_dcf_802_11_header_size + _dcf_802_11_ack_header_size + phy_header_size) * 8 * transceiver_get_Tb(&to0, &from0)
	+ SPEED_LIGHT;


PRINT_MAC("[DCF] node %d send a Data packet %d: packet_size=%d   \n", to->object, packet->id, packet->size);
        
    /* Send data */
    TX(&to0, &from0, packet);
        
     // switch RX after transmission
    nodedata->clock_pkt = get_time() + timeout_pkt;
    scheduler_add_callback(nodedata->clock_pkt, to, from, switch_rx_after_tx, NULL);


    /* Wait for timeout or ACK */
    nodedata->state = STATE_TIMEOUT;
    nodedata->clock = get_time() + timeout;
    scheduler_add_callback(nodedata->clock, to, from, dcf_802_11_state_machine, NULL);
    return 0;
        
  case STATE_BROADCAST:
   
  /* Build data packet */	
    packet = packet_clone(nodedata->txbuf);
    timeout = (packet->size + phy_header_size) * 8 * transceiver_get_Tb(&to0, &from0) + macMinSIFSPeriod;

    timeout_pkt = (packet->size + phy_header_size) * 8 * transceiver_get_Tb(&to0, &from0);

PRINT_MAC("[DCF] node %d send a BROADCAST packet %d: packet_size=%d   \n", to->object, packet->id, packet->size);

    /* Send data */
    TX(&to0, &from0, packet);
        
     // switch RX after transmission
    nodedata->clock_pkt = get_time() + timeout_pkt;
    scheduler_add_callback(nodedata->clock_pkt, to, from, switch_rx_after_tx, NULL);

    /* Wait for timeout or ACK */
    nodedata->state = STATE_BROAD_DONE;
    nodedata->clock = get_time() + timeout;
    scheduler_add_callback(nodedata->clock, to, from, dcf_802_11_state_machine, NULL);
    return 0;
        
  case STATE_ACK:
    /* Build ack packet */	
    
	packet = packet_create(to, _dcf_802_11_header_size + _dcf_802_11_ack_header_size, -1);

       struct _dcf_802_11_header *header4;
       header4 = malloc(sizeof(struct _dcf_802_11_header));
       field_t *field_dcf_802_11_header4 = field_create(INT, sizeof(struct _dcf_802_11_header), header4);
       packet_add_field(packet, "_dcf_802_11_header", field_dcf_802_11_header4);
       header4->type = ACK_TYPE; 
       header4->src = to->object;
       header4->dst = nodedata->dst;

       struct _dcf_802_11_ack_header *ack_header;
       ack_header = malloc(sizeof(struct _dcf_802_11_ack_header));
       field_t *field_dcf_802_11_ack_header = field_create(INT, sizeof(struct _dcf_802_11_ack_header), ack_header);
       packet_add_field(packet, "_dcf_802_11_ack_header", field_dcf_802_11_ack_header);


    timeout =  (packet->size + phy_header_size) * 8 * transceiver_get_Tb(&to0, &from0) 
    + macMinSIFSPeriod;

    timeout_pkt =  (packet->size + phy_header_size) * 8 * transceiver_get_Tb(&to0, &from0); 

        
PRINT_MAC("[DCF] node %d send an ACK packet %d: packet_size=%d   \n", to->object, packet->id, packet->size);

    /* Send ack */
    TX(&to0, &from0, packet);

     // switch RX after transmission
    nodedata->clock_pkt = get_time() + timeout_pkt;
    scheduler_add_callback(nodedata->clock_pkt, to, from, switch_rx_after_tx, NULL);
        
    /* Wait for end of transmission */
    nodedata->state = STATE_DONE;
    nodedata->clock = get_time() + timeout;
    scheduler_add_callback(nodedata->clock, to, from, dcf_802_11_state_machine, NULL);
    return 0;
        
  case STATE_DONE:
    /* Return to pending state */
    nodedata->state = nodedata->state_pending;
    if (nodedata->state != STATE_IDLE)
      {	  
	
	field_t *field_header = packet_retrieve_field(nodedata->txbuf,"_dcf_802_11_header");
	header = (struct _dcf_802_11_header *) field_getValue(field_header);
	nodedata->dst =header->dst;
      }
    nodedata->clock = get_time();
    dcf_802_11_state_machine(to, from,NULL);
    return 0;
        
        
  case STATE_BROAD_DONE:
    /* Destroy txbuf */
    packet_dealloc(nodedata->txbuf);
    nodedata->txbuf = NULL;
			
    /* Back to idle state*/
    nodedata->state = STATE_IDLE;
    nodedata->clock = get_time();			
    dcf_802_11_state_machine(to, from,NULL);
    return 0;

		
  default:
    return 0;
  }

  return 0;
}


/* ************************************************** */
/* ************************************************** */
void tx(call_t *to, call_t *from, packet_t *packet) {
  struct nodedata *nodedata = get_node_private_data(to);

    // encapsulation of MAC header
    destination_t destination=packet->destination;
    if (set_header(to, from, packet, &destination) == -1) {
      packet_dealloc(packet);
      return;
    }

  list_insert(nodedata->packets, (void*)packet);

  if (nodedata->state == STATE_IDLE) {
    nodedata->clock = get_time();  
    dcf_802_11_state_machine(to, from, NULL);
  } 
}


/* ************************************************** */
/* ************************************************** */
void rx(call_t *to, call_t *from, packet_t *packet) {
  struct nodedata *nodedata = get_node_private_data(to);
  array_t *up = get_class_bindings_up(to);
  int i = up->size;

  call_t to0   = {get_class_bindings_down(to)->elts[0], to->object};
  call_t from0 = {to->class, to->object};
  //switch back in RX MODE
  transceiver_switch_rx(&to0, &from0);

  struct _dcf_802_11_header *header = (struct _dcf_802_11_header *) packet_retrieve_field_value_ptr(packet, "_dcf_802_11_header");

  struct _dcf_802_11_data_header *data_header;
  struct _dcf_802_11_rts_header *rts_header;
  struct _dcf_802_11_cts_header *cts_header;

  switch (header->type)
    {	
    case RTS_TYPE:

 /* Receive RTS*/
    rts_header = (struct _dcf_802_11_rts_header *) packet_retrieve_field_value_ptr(packet, "_dcf_802_11_rts_header");

      if (header->dst != to->object)
	{
	  /* Packet not for us */
          if (nodedata->nav < (get_time() + rts_header->nav)) {
                nodedata->nav = get_time() + rts_header->nav;
            }
            packet_dealloc(packet);
            return;
	}
      if ((nodedata->state != STATE_IDLE) && (nodedata->state != STATE_BACKOFF))
	{
	  /* If not expecting rts, do nothing */
	  packet_dealloc(packet);
	  return;
	}
      /* Record RTS info */

      nodedata->dst = header->src;
      nodedata->size = rts_header->size;
      packet_dealloc(packet);

      /* Send CTS */
      if (nodedata->state == STATE_BACKOFF)
	{
	  nodedata->state_pending = nodedata->state;
	}
      else
	{
	  nodedata->state_pending = STATE_IDLE;
	}
      nodedata->state = STATE_CTS;
      nodedata->clock = get_time() + macMinSIFSPeriod;
      scheduler_add_callback(nodedata->clock, to, from, dcf_802_11_state_machine, NULL);

      break;
	
    case CTS_TYPE:

      /* Receive CTS */
     cts_header = (struct _dcf_802_11_cts_header *) packet_retrieve_field_value_ptr(packet, "_dcf_802_11_cts_header");
      if (header->dst != to->object)
	{
	  /* Packet not for us */
            if (nodedata->nav < (get_time() + cts_header->nav)) {
                nodedata->nav = get_time() + cts_header->nav;
            }
            packet_dealloc(packet);
            return;
	}
        
      if (nodedata->state != STATE_TIMEOUT)
	{
	  /* If not expecting cts, do nothing */
	  packet_dealloc(packet);
	  return;
	}
	
      if (nodedata->dst != header->src)
	{
	  /* Expecting cts, but not from this node */
	  packet_dealloc(packet);
	  return;
	}
	
      /* Record CTS info */
      packet_dealloc(packet);
	
      /* Send DATA */
      nodedata->state = STATE_DATA;
      nodedata->clock = get_time() + macMinSIFSPeriod;
      scheduler_add_callback(nodedata->clock, to, from, dcf_802_11_state_machine, NULL);
      break;
	
    case DATA_TYPE:
 
       data_header = (struct _dcf_802_11_data_header *) packet_retrieve_field_value_ptr(packet, "_dcf_802_11_data_header");

     /* Received DATA */	
      if (header->dst != to->object) {
	/* Packet not for us */
            if (nodedata->nav < (get_time() + data_header->nav)) {
                nodedata->nav = get_time() + data_header->nav;
            }
            packet_dealloc(packet);
            return;
      }

PRINT_MAC("[DCF] node %d receives an DATA packet %d: packet_size=%d   \n", to->object, packet->id, packet->size);

				
      if ((nodedata->state != STATE_IDLE) 
	  && (nodedata->state != STATE_BACKOFF) 
	  && (nodedata->state != STATE_CTS_TIMEOUT)) {
	/* If not expecting data, do nothing */
	packet_dealloc(packet);
	return;
      }
	
      if ((nodedata->state == STATE_CTS_TIMEOUT) && (nodedata->dst != header->src))
	{
	  /* Expecting data, but not from this node */
	  packet_dealloc(packet);
	  return;
	}
							
      /* Send ACK */
      if (nodedata->state == STATE_BACKOFF)
	{
	  nodedata->state_pending = nodedata->state;
	}
      else
	{
	  nodedata->state_pending = STATE_IDLE;
	}
      nodedata->dst = header->src;
      nodedata->state = STATE_ACK;
      nodedata->clock = get_time() + macMinSIFSPeriod;
      scheduler_add_callback(nodedata->clock, to, from, dcf_802_11_state_machine, NULL);

      /* forward to upper layer */
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
      break;
			

    case BROADCAST_TYPE:
      /* Receive RTS*/

PRINT_MAC("[DCF] node %d receives an BROADCAST packet %d: packet_size=%d   \n", to->object, packet->id, packet->size);
              
      if (header->dst != BROADCAST_ADDR) {
	/* Packet not for us */
	packet_dealloc(packet);
	return;
      }
        
      /* forward to upper layer */
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
      break;

    case ACK_TYPE:
      /* Received ACK */

 
      if (header->dst != to->object) {
	/* Packet not for us */
	packet_dealloc(packet);
	return;
      }

PRINT_MAC("[DCF] node %d receives an ACK packet %d: packet_size=%d   \n", to->object, packet->id, packet->size);
        
      if (nodedata->state != STATE_TIMEOUT) {
	/* If not expecting ack, do nothing */
	packet_dealloc(packet);
	return;
      }
			
      if (nodedata->dst != header->src)
	{
	  /* Expecting ack, but not from this node */
	  packet_dealloc(packet);
	  return;
	}
				
      /* Destroy txbuf */
      packet_dealloc(packet);
      packet_dealloc(nodedata->txbuf);
      nodedata->txbuf = NULL;      
			
      /* Back to idle state*/
      nodedata->state = STATE_IDLE;
      nodedata->clock = get_time();			
      dcf_802_11_state_machine(to, from,NULL);

      break;
        
    default:
      packet_dealloc(packet);
      return;        
    }
}

/* ************************************************** */
/* ************************************************** */

int get_header_size(call_t *to, call_t *from) {
  return (_dcf_802_11_header_size + _dcf_802_11_data_header_size);
}

int get_header_real_size(call_t *to, call_t *from) {
  return 8*(_dcf_802_11_header_size + _dcf_802_11_data_header_size);
}


/* ************************************************** */
/* ************************************************** */
mac_methods_t methods = {rx, 
                         tx,
                         set_header, 
                         get_header_size,
                         get_header_real_size};


    
    
