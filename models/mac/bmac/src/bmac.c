/**
 *  \file   bmac.c
 *  \brief  B-MAC layer
 *  \author Romain Kuntz
 *  \date   01/2009
 **/

#include <stdio.h>
#include <kernel/include/modelutils.h>
#include "bmac.h"

/* ************************************************** */
/* ************************************************** */
model_t model =  {
    "Implementation of the B-MAC medium access control",
    "Romain Kuntz",
    "0.2",
    MODELTYPE_MAC
};

/* ************************************************** */
/* ************************************************** */
struct _bmac_header {
    nodeid_t dst;
    nodeid_t src;
    uint8_t type;
};

int _bmac_header_size = sizeof(nodeid_t) + sizeof(nodeid_t) + sizeof(uint8_t);
/* ************************************************** */
/* ************************************************** */
struct bmac_nodedata {
    uint64_t clock;
    uint64_t backoff_begin;
    int64_t backoff_remain;
    uint64_t preamble_pkt;
    uint64_t preamble_count;
    int state;
    int state_pending;
    int transceiver_state;
    int prio_queue;
    int pulse_count;
    int ack_wait_count;
    event_t *last_scheduled_event;
    nodeid_t dst_data;
    nodeid_t dst_ack;
    nodeid_t src_preamble;
    nodeid_t src_sync;
    packet_t *tx_pending;
    list_t *packets;

    // B-MAC configurable parameters
    int CCAEnabled;         // Whether CCA is enabled
    int LLAckEnabled;       // Whether LL Ack are required
    int max_retrans;        // Max number of retransmission
    double busy_threshold;  // Busy channel threeshold
    uint64_t initBackoff;   // The Initial Backoff value
    uint64_t congBackoff;   // The Congestion Backoff value
    uint64_t LPL_checkint;  // The Check Interval for LPL
};

/* ************************************************** */
/* ************************************************** */
void switch_transceiver(int on, call_t *to); 
int state_machine(call_t *to, call_t *from, void *args);

/* ************************************************** */
/* ************************************************** */
// Debug macro
#ifdef LOG_MAC
#define DBG(arg...)				\
    do {						\
      fprintf(stderr, "%s: ", __FUNCTION__);	\
      fprintf(stderr, arg);			\
    } while (0)
#else
#define DBG(...)
#endif /* LOG_MAC */

/* ************************************************** */
/* ************************************************** */
// B-MAC header types
#define PREAMBLE_WORD       0xaa    // 170
#define SYNC_WORD           0x33    // 51
#define DATA                0x01    // 1
#define ACK                 0x02    // 2

// B-MAC states
#define STATE_POWER_DOWN       1
#define STATE_PULSE_CHECK      2
#define STATE_IDLE             4
#define STATE_PRETX            8
#define STATE_TX_PREAMBLE      16
#define STATE_TX               32
#define STATE_TXING            64
#define STATE_TX_WAIT_ACK      128
#define STATE_RX_WAIT_SYNC     256
#define STATE_RX_WAIT_DATA     512
#define STATE_TX_ACK           1024
#define STATE_TX_ACK_END       2048

#define ONE_MS                 1000000     // 1ms
#define ONE_SEC                1000000000  // 1s
#define TRANSCEIVER_ON         1
#define TRANSCEIVER_OFF        0

#define PULSE_TIME             450000      // Time to get a transceiver sample (450us)
#define PULSE_INIT_TRANSCEIVER 2500000     // Time to init transceiver (2.5ms)

#define WAIT_ACK_TIMEOUT       100000000   // 100ms
#define WAIT_DAT_TIMEOUT       100000000   // 100ms
#define ACK_DELTA              1000        // Delay between DATA and ACK (1us)

// Time between preamble/sync and data
// For B-MAC, data directly follows the preamble
#define IFS                 0           

/* ************************************************** */
/* ************************************************** */
int get_header_size(call_t *to, call_t *from) {
  return (_bmac_header_size);
}

int get_header_real_size(call_t *to, call_t *from) {
  return (_bmac_header_size*8);
}

int set_header(call_t *to, call_t *from, packet_t *packet, 
    destination_t *dst) {
  field_t *field_header = packet_retrieve_field(packet,"_bmac_header");
  struct _bmac_header *header;

  if (field_header == NULL) {
    header = malloc(sizeof(struct _bmac_header));
    field_t *field_bmac_header = field_create(INT, sizeof(struct _bmac_header), header);
    packet_add_field(packet, "_bmac_header", field_bmac_header);
    packet->size += _bmac_header_size ;
    packet->real_size += 8*_bmac_header_size ;
  }
  else {
    header = (struct _bmac_header *) field_getValue(field_header);
  }

  /* Fill the MAC header */ 
  header->dst = dst->id;
  header->src = to->object;
  header->type = DATA; 

  return 0;
}

int set_mac_header(call_t *to, call_t *from, packet_t *packet, 
    destination_t *dst, uint8_t type) {
  struct _bmac_header *header;
  field_t *field_header;

  /* Fill the MAC header and rewrite the header type */
  set_header(to, from, packet, dst);

  if (type!=DATA) // no need to update the size
  {
    packet->size -= _bmac_header_size ;
    packet->real_size -= 8*_bmac_header_size ;
  }

  field_header = packet_retrieve_field(packet,"_bmac_header");
  header = (struct _bmac_header *) field_getValue(field_header);
  header->type = type;

  return 0;
}

/* ************************************************** */
/* ************************************************** */
int init(call_t *to, void *params) {
  return 0;
}

int destroy(call_t *to) {
  return 0;
}

int bootstrap(call_t *to) {
  struct bmac_nodedata *nodedata = get_node_private_data(to);

  nodedata->state = STATE_IDLE;
  switch_transceiver(TRANSCEIVER_ON, to);
  call_t from0 = {-1, -1};
  state_machine(to, &from0, NULL);

  return 0;
}

int bind(call_t *to, void *params) {
  call_t from = {-1, -1};
  struct bmac_nodedata *nodedata = malloc(sizeof(struct bmac_nodedata));
  call_t to0   = {get_class_bindings_down(to)->elts[0], to->object};
  call_t from0 = {to->class, to->object};
  param_t *param;   

  int phy_header_size=GET_HEADER_SIZE(&to0,to);

  /* Initialization */
  nodedata->clock = 0;
  nodedata->backoff_begin = 0;
  nodedata->backoff_remain = 0;
  nodedata->state = STATE_IDLE;
  nodedata->state_pending = -1;
  nodedata->packets = list_create();
  nodedata->tx_pending = NULL;
  nodedata->dst_data = 0;
  nodedata->dst_ack = 0;
  nodedata->src_preamble = -1;
  nodedata->src_sync = -1;
  nodedata->pulse_count = 1;
  nodedata->last_scheduled_event = NULL;
  nodedata->preamble_count = 0;
  nodedata->ack_wait_count = 1;
  nodedata->transceiver_state = TRANSCEIVER_OFF;
  nodedata->max_retrans = 2;
  nodedata->preamble_pkt = (get_header_size(to, &from)+phy_header_size)* 8 * transceiver_get_Tb(&to0, &from0);

  /* BMAC default parameters */
  nodedata->CCAEnabled = 1;
  nodedata->busy_threshold = -74; 
  nodedata->LLAckEnabled = 1;
  nodedata->initBackoff = get_random_time_range(ONE_MS, 10*ONE_MS);
  nodedata->congBackoff = get_random_time_range(ONE_MS, 10*ONE_MS);
  nodedata->LPL_checkint = MAC_LPL_MODE_4;

  /* BMAC parameters from the configuration file */
  list_init_traverse(params);
  while ((param = (param_t *) list_traverse(params)) != NULL) {
    /* Whether CCA is enabled */
    if (!strcmp(param->key, "cca")) {
      if (get_param_integer(param->value, 
          &(nodedata->CCAEnabled))) {
        goto error;
      }
    }
    /* Value of the busy channel threshold */
    if (!strcmp(param->key, "busy-threshold")) {
      if (get_param_double(param->value, 
          &(nodedata->busy_threshold))) {
        goto error;
      }
    }
    /* Whether LL ACK are enabled */      
    if (!strcmp(param->key, "ack")) {
      if (get_param_integer(param->value, 
          &(nodedata->LLAckEnabled))) {
        goto error;
      }
    }
    /* Value of the initial backoff */
    if (!strcmp(param->key, "init-back")) {
      if (get_param_time(param->value, 
          &(nodedata->initBackoff))) {
        goto error;
      }
    }
    /* Value of the congestion backoff */
    if (!strcmp(param->key, "cong-back")) {
      if (get_param_time(param->value, 
          &(nodedata->congBackoff))) {
        goto error;
      }
    }
    /* Value of LPL check interval */
    if (!strcmp(param->key, "lpl-check")) {
      if (get_param_time(param->value, 
          &(nodedata->LPL_checkint))) {
        goto error;
      }
    }
    /* Number of retransmission */
    if (!strcmp(param->key, "max-retrans")) {
      if (get_param_integer(param->value, 
          &(nodedata->max_retrans))) {
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
  struct bmac_nodedata *nodedata = get_node_private_data(to);
  packet_t *packet;

  if (nodedata->tx_pending != NULL) {
    packet_dealloc(nodedata->tx_pending);
    nodedata->tx_pending = NULL;
  }

  /* Free the pending packets in the FIFO */
  while ((packet = (packet_t *) 
      list_pop(nodedata->packets)) != NULL) {
    packet_dealloc(packet);
  }
  list_destroy(nodedata->packets);   

  free(nodedata);
  return 0;
}

/* IOCTL function that can be called by other layers 
 * in order to configure some of the B-MAC parameters.
 * See bmac.h
 */
int ioctl(call_t *to, int option, void *in, void **out) {
  struct bmac_nodedata *nodedata = get_node_private_data(to);
  uint64_t new_value = *((uint64_t *) in);

  switch (option) {
    case MAC_UPDATE_CCA:
      DBG("CCAEnabled being modified from %d to %"PRId64"\n",
          nodedata->CCAEnabled, new_value);
      nodedata->CCAEnabled = (int) new_value;
      break;

    case MAC_UPDATE_LLACK:
      DBG("LLAckEnabled being modified from %d to %"PRId64"\n",
          nodedata->LLAckEnabled, new_value);
      nodedata->LLAckEnabled = (int) new_value;
      break;

    case MAC_UPDATE_INIT_BACKOFF:
      DBG("initBackoff being modified from %"PRId64" to %"PRId64"\n",
          nodedata->initBackoff, new_value);
      nodedata->initBackoff = new_value;
      break;

    case MAC_UPDATE_CONG_BACKOFF:
      DBG("congBackoff being modified from %"PRId64" to %"PRId64"\n",
          nodedata->congBackoff, new_value);
      nodedata->congBackoff = new_value;
      break;

    case MAC_UPDATE_LPL_CHECKINT:
      DBG("LPL_checkint being modified from %"PRId64" to %"PRId64"\n",
          nodedata->LPL_checkint, new_value);
      nodedata->LPL_checkint = new_value;
      break;

    default:
      DBG("Unknown IOCTL message\n");
      break;
  }

  set_node_private_data(to, nodedata);
  return 0;    
}

/* ************************************************** */
/* ************************************************** */
void bmac_add_callback(uint64_t clock, call_t *to, call_t *from,
    callback_t callback) {
  struct bmac_nodedata *nodedata = get_node_private_data(to);

  /* Only one event at a time can be scheduled by the 
   * state machine, so we remove the last scheduled 
   * event if it exists and has not been executed yet 
   */
  if (nodedata->last_scheduled_event != NULL
      && nodedata->last_scheduled_event->clock_ > get_time()) {
    scheduler_delete_callback(nodedata->last_scheduled_event);
  }

  /* Schedule the new event */
  nodedata->last_scheduled_event = 
      scheduler_add_callback(clock, to, from, callback, NULL);
  return;
}

/* This function must be called whenever the node leaves 
 * the state STATE_PRETX
 */
void backoff_stop(call_t *to) {
  struct bmac_nodedata *nodedata = get_node_private_data(to);

  /* Decrement the current backoff */
  if (nodedata->backoff_remain > 0) {
    nodedata->backoff_remain -= 
        (get_time() - nodedata->backoff_begin);
  }

  return;
}

/* This function must be called whenever the node goes 
 * back to the state STATE_PRETX
 */ 
uint64_t backoff_restart(call_t *to) {
  struct bmac_nodedata *nodedata = get_node_private_data(to);

  if (nodedata->backoff_remain <= 0) {
    nodedata->backoff_remain = 0;
  } else {
    /* Set the new backoff starting time */
    nodedata->backoff_begin = get_time();
  }

  /* Return to the state STATE_PRETX */
  nodedata->state = STATE_PRETX;
  return get_time() + nodedata->backoff_remain;
}

int check_channel_busy(call_t *to) {
  struct bmac_nodedata *nodedata = get_node_private_data(to);
  call_t to0   = {get_class_bindings_down(to)->elts[0], to->object};
  call_t from0 = {to->class, to->object};

  if (nodedata->transceiver_state == TRANSCEIVER_OFF) {
    DBG("%d - BUG - checking channel while transceiver is OFF\n", 
        to->object);
  }

  if (transceiver_get_cs(&to0, &from0) >= nodedata->busy_threshold 
      || transceiver_get_noise(&to0, &from0) >= nodedata->busy_threshold) {
    return 1;
  }

  return 0;
}

void switch_transceiver(int on, call_t *to) {
  struct bmac_nodedata *nodedata = get_node_private_data(to);
  call_t to0   = {get_class_bindings_down(to)->elts[0], to->object};
  call_t from0 = {to->class, to->object};

  /* Nothing to do if the state remains the same */
  /*if (nodedata->transceiver_state == on) {
    return;
  }*/

  if (on == TRANSCEIVER_ON) {
    transceiver_wakeup(&to0, &from0);
    transceiver_switch_rx(&to0, &from0);
    nodedata->transceiver_state = TRANSCEIVER_ON;
  } else {
    transceiver_sleep(&to0, &from0);  
    nodedata->transceiver_state = TRANSCEIVER_OFF;
  }
}

/* B-MAC state machine */
int state_machine(call_t *to, call_t *from, void *args) { 
  struct bmac_nodedata *nodedata = get_node_private_data(to);
  call_t to0   = {get_class_bindings_down(to)->elts[0], to->object};
  call_t from0 = {to->class, to->object};

  uint64_t timeout, backoff;
  packet_t *packet;
  int packet_size;
  struct _bmac_header *header; 
  destination_t destination;

  int phy_header_size=GET_HEADER_SIZE(&to0,to);

  /* current event is going to be deleted, so stop to point at it */
  nodedata->last_scheduled_event = NULL;

  /* Drop unscheduled events */
  if (nodedata->clock != get_time()) {
    return 0;
  }

  switch (nodedata->state) {
    int busy_channel;            

    case STATE_IDLE:
      if (nodedata->tx_pending == NULL) {
        /* Check if a packet is available in the qeue */
        nodedata->tx_pending =
            (packet_t *) list_pop_FIFO(nodedata->packets);

        if (nodedata->tx_pending == NULL) {
          if (nodedata->LPL_checkint != MAC_LPL_MODE_0) {
            /* If no packet in queue, go to
             * STATE_POWER_DOWN, turn off the
             * transceiver and wake-up later
             */
            nodedata->state = STATE_POWER_DOWN;
            switch_transceiver(TRANSCEIVER_OFF, to);
            nodedata->clock = get_time()
	        + nodedata->LPL_checkint
	        - PULSE_INIT_TRANSCEIVER;
            bmac_add_callback(nodedata->clock, to, from,
                state_machine);
            break;
          } else {
            /* In MAC_LPL_MODE_0, the transceiver is always on,
             * we stay in STATE_IDLE
             */
            break;
          }
        }
      }

      /* Use an initial backoff before transmitting
       * Note: this is different from 802.11 which does not
       * use any backoff if the channel is clear after DIFS
       */
      if (nodedata->CCAEnabled) {
        /* Go to STATE_PRETX */
        backoff = nodedata->initBackoff;
        nodedata->backoff_begin = get_time();
        nodedata->backoff_remain = backoff;
        nodedata->state = STATE_PRETX;
      } else {
        /* CCA is disabled, backoff is set to 0 */
        backoff = 0;
        nodedata->backoff_begin = get_time();
        nodedata->backoff_remain = 0;

        if (nodedata->LPL_checkint == MAC_LPL_MODE_0) {
          /* Go directly to STATE_TX */
          nodedata->state = STATE_TX;
        } else {
          /* Go to STATE_TX_PREAMBLE */
          nodedata->state = STATE_TX_PREAMBLE;
        }
      }

      nodedata->clock = get_time() + backoff;
      bmac_add_callback(nodedata->clock, to, from, state_machine);
      break;

    case STATE_PRETX:
      if (nodedata->tx_pending == NULL) {
        nodedata->backoff_remain = 0;
        nodedata->backoff_begin = 0;
        nodedata->state = STATE_IDLE;
        nodedata->clock = get_time();
        bmac_add_callback(nodedata->clock, to, from, state_machine);
        break;
      }

      /* Decrement the current backoff */
      if (nodedata->backoff_remain > 0) {
        nodedata->backoff_remain -=
            (get_time() - nodedata->backoff_begin);
      }

      if (nodedata->backoff_remain > 0) {
        nodedata->clock = get_time() + nodedata->backoff_remain;
        nodedata->state = STATE_PRETX;
        bmac_add_callback(nodedata->clock, to, from, state_machine);
        break;
      }

      /* Backoff is 0, going to STATE_PULSE_CHECK
       * in order to perform the CCA check
       */
      nodedata->state = STATE_PULSE_CHECK;
      nodedata->state_pending = STATE_PRETX;
      nodedata->pulse_count = 1;
      nodedata->clock = get_time();
      bmac_add_callback(nodedata->clock, to, from, state_machine);
      break;

    case STATE_POWER_DOWN:
      /* Going to STATE_PULSE_CHECK, turn on the transceiver */
      switch_transceiver(TRANSCEIVER_ON, to);
      nodedata->state = STATE_PULSE_CHECK;
      nodedata->pulse_count = 1;
      nodedata->clock = get_time() + PULSE_INIT_TRANSCEIVER;
      bmac_add_callback(nodedata->clock, to, from, state_machine);
      break;

    case STATE_PULSE_CHECK:
      /* Looking for outliers (i.e. channel energy below
       * threshold, which means channel is clear)
       */

      DBG("%d is on pulse check\n", to->object);
      busy_channel = check_channel_busy(to);

      if (!busy_channel) {
        /* Outlier found, i.e. channel is clear */
        if (nodedata->state_pending == STATE_PRETX) {
          DBG("%d channel is clear\n", to->object);
          nodedata->backoff_begin = get_time();
          nodedata->backoff_remain = 0;

          if (nodedata->LPL_checkint == MAC_LPL_MODE_0) {
            /* Go directly to STATE_TX */
            DBG("%d we can send data (STATE_TX)\n", to->object);
            nodedata->state = STATE_TX;
          } else {
            /* Go to STATE_TX_PREAMBLE */
            DBG("%d we will send preamble (STATE_TX_PREAMBLE)\n", to->object);
            nodedata->state = STATE_TX_PREAMBLE;
          }

          nodedata->clock = get_time();
          nodedata->state_pending = -1;
        } else {
          /* Back to STATE_PRETX */
          DBG("%d we will go back to pretx (STATE_PRETX)\n", to->object);
          nodedata->clock = backoff_restart(to);
        }

        bmac_add_callback(nodedata->clock, to, from, state_machine);
      } else if (nodedata->pulse_count < 5) {
        /* Channel is busy, but we sample the channel 5
         * times
         */
        nodedata->pulse_count++;
        nodedata->clock = get_time() + PULSE_TIME;
        bmac_add_callback(nodedata->clock, to, from, state_machine);
      } else {
        /* No outliers found after 5 trials, i.e
         * channel is busy
         */
        if (nodedata->state_pending == STATE_PRETX) {
          /* Configure a congestion backoff.
           * Note: this is different from 802.11 which
           * uses congestion backoff only if it does
           * not receive an ack (e.g. in case of
           * collision)
           */
          backoff = nodedata->congBackoff;
          nodedata->backoff_begin = get_time();
          nodedata->backoff_remain = backoff;
          nodedata->clock = get_time() + backoff;
          nodedata->state_pending = -1;
        } else {
          nodedata->clock = get_time();
        }

        nodedata->state = STATE_PRETX;
        bmac_add_callback(nodedata->clock, to, from, state_machine);
      }
      break;

    case STATE_TX_PREAMBLE:
      packet_size = get_header_size(to, from);
      packet = packet_create(to, packet_size, -1);

      if (packet == NULL) {
        DBG("%d - STATE_TX_PREAMBLE: BUG (packet is NULL)\n",
            to->object);
        break;
      }

      destination.id = BROADCAST_ADDR;

      /* WSNet only notifies the destination of a packet
       * once it has been completely transmitted, so we
       * divide the preamble into small packets sent
       * consecutively on the medium.
       */
      if (nodedata->preamble_count <= nodedata->LPL_checkint) {
        /* Init the preamble header */
        set_mac_header(to, from, packet, &destination, PREAMBLE_WORD);
        nodedata->state = STATE_TX_PREAMBLE;

        /* Adjust clock to the end of transmission
         * and update the preamble counter */
        timeout = nodedata->preamble_pkt;
        nodedata->preamble_count += timeout;

        //PRINT_MAC("[BMAC] node %d send a PREAMBLE %d: \n", to->object, packet->id);

      } else {
        /* All the preamble has been sent, reset the
         * preamble counter and send a SYNC message
         */
        nodedata->preamble_count = 0;
        set_mac_header(to, from, packet, &destination, SYNC_WORD);
        nodedata->state = STATE_TX;

        /* Adjust clock to the end of transmission */
        timeout = (packet_size + phy_header_size)* 8 /* conversion Bytes -> bits */
            * transceiver_get_Tb(&to0, &from0)
        + IFS;

        DBG("[BMAC] node %d send a SYNC %d: \n", to->object, packet->id);
      }

      /* Send the preamble or SYNC packet */
      TX(&to0, &from0, packet);

      nodedata->clock = get_time() + timeout;
      bmac_add_callback(nodedata->clock, to, from, state_machine);
      break;

    case STATE_TX:
      if (nodedata->tx_pending == NULL) {
        DBG("%d - STATE_TX: BUG (Trying to send NULL packet)\n",
            to->object);
        break;
      }

      packet = packet_clone(nodedata->tx_pending);
      field_t *field_header = packet_retrieve_field(packet,"_bmac_header");
      if (field_header == NULL) {
        DBG("error: header bmac not found\n");
        break;
      }
      header = (struct _bmac_header *) field_getValue(field_header);
      header->type = DATA;


      field_t *PHY_field_header = packet_retrieve_field(packet,"PHY_header");
      if (PHY_field_header == NULL) {
        timeout = ((packet->size + phy_header_size)* 8 * transceiver_get_Tb(&to0, &from0));
      }
      else {
        timeout = (packet->size* 8 * transceiver_get_Tb(&to0, &from0));
      }
      PRINT_MAC("[BMAC] node %d send a DATA %d: \n", to->object, packet->id);

      /* Send the Data packet */
      TX(&to0, &from0, packet);

      nodedata->state = STATE_TXING;
      nodedata->dst_data = header->dst;

      /* Adjust clock to the end of transmission */
      nodedata->clock = get_time() + timeout;
      bmac_add_callback(nodedata->clock, to, from, state_machine);

      DBG("%d - STATE_TX: TX at %"PRId64" until %"PRId64"\n",to->object, get_time(), nodedata->clock);
      break;

    case STATE_TXING:
      /* End of transmission of a Data packet.
       * Wait for an ACK if the packet is not
       * broadcasted and if LL Ack are enabled
       */
      if (nodedata->LLAckEnabled
          && nodedata->dst_data != BROADCAST_ADDR) {
        /* Switch to STATE_TX_WAIT_ACK */
        nodedata->state = STATE_TX_WAIT_ACK;
        nodedata->clock = get_time() + WAIT_ACK_TIMEOUT;
        switch_transceiver(TRANSCEIVER_ON, to);
        bmac_add_callback(nodedata->clock, to, from, state_machine);
      } else {
        /* Dealloc the buffered packet */
        packet_dealloc(nodedata->tx_pending);
        nodedata->tx_pending = NULL;
        nodedata->backoff_remain = 0;
        nodedata->backoff_begin = 0;

        /* Switch to STATE_IDLE */
        nodedata->state = STATE_IDLE;
        nodedata->clock = get_time();
        bmac_add_callback(nodedata->clock, to, from, state_machine);
      }
      break;

    case STATE_TX_WAIT_ACK:
      if (nodedata->ack_wait_count >= nodedata->max_retrans) {
        /* We reached the retransmission limit */
        DBG("%d - STATE_TX_WAIT_ACK: No ack received after %d "
            "trials\n", to->object, nodedata->max_retrans);

        /* Reinit the retransmission counter */
        nodedata->ack_wait_count = 1;

        /* Dealloc the buffered packet */
        packet_dealloc(nodedata->tx_pending);
        nodedata->tx_pending = NULL;
        nodedata->backoff_remain = 0;
        nodedata->backoff_begin = 0;
      } else {
        /* Retransmit packet */
        DBG("%d - STATE_TX_WAIT_ACK: No ack received "
            "(trial %d), retransmit packet\n",
            to->object, nodedata->ack_wait_count);
        nodedata->ack_wait_count++;
      }

      /* Back to STATE_IDLE */
      nodedata->state = STATE_IDLE;
      nodedata->clock = get_time();
      bmac_add_callback(nodedata->clock, to, from, state_machine);
      break;

    case STATE_TX_ACK:
      /* Send an ACK */
      packet_size = get_header_size(to, from);
      packet = packet_create(to, packet_size, -1);

      if (packet == NULL) {
        DBG("%d - STATE_TX_ACK: BUG (packet is NULL)\n",
            to->object);
        break;
      }

      destination.id = nodedata->dst_ack;
      //memset(packet->data, 0, packet_size);

      /* Init the ACK header */
      set_mac_header(to, from, packet, &destination, ACK);
      nodedata->dst_ack = 0;

      PRINT_MAC("[BMAC] node %d send an ACK %d to %d: \n", to->object, packet->id, destination.id);

      /* Send the ACK */
      TX(&to0, &from0, packet);

      /* Adjust clock to the end of transmission */
      timeout = (packet_size + phy_header_size)* 8 /* conversion Bytes -> bits */
          * transceiver_get_Tb(&to0, &from0);
      nodedata->clock = get_time() + timeout;

      /* Go to STATE_TX_ACK_END */
      nodedata->state = STATE_TX_ACK_END;
      bmac_add_callback(nodedata->clock, to, from, state_machine);

      DBG("%d - STATE_TX_ACK: TX at %"PRId64" until "
          "%"PRId64"\n", to->object, get_time(), nodedata->clock);
      break;

    case STATE_TX_ACK_END:
      /* Back to STATE_PRETX */
      nodedata->clock = backoff_restart(to);
      switch_transceiver(TRANSCEIVER_ON, to);
      bmac_add_callback(nodedata->clock, to, from, state_machine);
      break;

    case STATE_RX_WAIT_SYNC:
      DBG("%d - %"PRId64" - Waiting for SYNC_WORD "
          "(from node %d) for too long, back to STATE_PRETX\n",
          to->object, get_time(), nodedata->src_preamble);
      nodedata->src_preamble = -1;

      /* Back to STATE_PRETX */
      nodedata->clock = backoff_restart(to);
      bmac_add_callback(nodedata->clock, to, from, state_machine);
      break;

    case STATE_RX_WAIT_DATA:
      DBG("%d - %"PRId64" - Waiting for DATA for too long "
          "(from node %d), back to STATE_PRETX\n",
          to->object, get_time(), nodedata->src_sync);
      nodedata->src_sync = -1;

      /* Back to STATE_PRETX */
      nodedata->clock = backoff_restart(to);
      bmac_add_callback(nodedata->clock, to, from, state_machine);
      break;

    default:
      DBG("%d - STATE BUG (Should never reach this state)\n",
          to->object);
      break;
  }

  if (args) {
    free((int *) args);
  }

  return 0;
}

/* ************************************************** */
/* ************************************************** */
void tx(call_t *to, call_t *from, packet_t *packet) {
  struct bmac_nodedata *nodedata = get_node_private_data(to);

  // encapsulation of MAC header
  destination_t destination=packet->destination;
  if (set_header(to, from, packet, &destination) == -1) {
    packet_dealloc(packet);
    return;
  }

  /* Add the packet in the FIFO */
  list_insert(nodedata->packets, (void *) packet);

  if (nodedata->state == STATE_POWER_DOWN) {
    nodedata->state = STATE_IDLE;
    switch_transceiver(TRANSCEIVER_ON, to);
  } 

  if (nodedata->state == STATE_IDLE) {
    nodedata->clock = get_time();
    bmac_add_callback(nodedata->clock, to, from, state_machine);
  }
}

/* ************************************************** */
/* ************************************************** */
void rx(call_t *to, call_t *from, packet_t *packet) {
  struct _bmac_header *header;
  array_t *up = get_class_bindings_up(to);
  struct bmac_nodedata *nodedata = get_node_private_data(to);    
  field_t *field_header = packet_retrieve_field(packet,"_bmac_header");

  if (field_header == NULL) {
    DBG("error: header bmac not found\n");
    return;
  }
  header = (struct _bmac_header *) field_getValue(field_header);

  /* Accept packet only if destined to myself or 
   * if it is a broadcast packet
   */
  if ((header->dst != to->object) 
      && (header->dst != BROADCAST_ADDR)) {
    /* Go back to STATE_PRETX if was waiting for DATA */
    if (header->type == DATA && 
        nodedata->state == STATE_RX_WAIT_DATA) {
      nodedata->clock = backoff_restart(to);
      bmac_add_callback(nodedata->clock, to, from, state_machine);   
    }

    packet_dealloc(packet);
    goto out;
  }

  switch (header->type) {
    case PREAMBLE_WORD:
      /* Drop preamble if we are not ready to receive it */
      if (nodedata->state != STATE_IDLE
          && nodedata->state != STATE_PRETX
          && nodedata->state != STATE_PULSE_CHECK) {
        DBG("%d - Dropping preamble packet, "
             "node is in state %d\n",
             to->object, nodedata->state);
        switch_transceiver(TRANSCEIVER_ON, to);
      } else {
              DBG("%d - Rx PREAMBLE_WORD\n", to->object);

              DBG("[BMAC] node %d receives a PREAMBLE %d: packet_size=%d   \n", to->object, packet->id, packet->size);

        /* Wait for the SYNC */
        if (nodedata->LPL_checkint != MAC_LPL_MODE_0) {
          backoff_stop(to);
          nodedata->src_preamble = header->src;
          nodedata->state = STATE_RX_WAIT_SYNC;
          nodedata->clock = get_time()
	      + nodedata->LPL_checkint
	      + 10*ONE_MS;
          switch_transceiver(TRANSCEIVER_ON, to);
          bmac_add_callback(nodedata->clock, to, from, state_machine);
        } else {
          /* If the node uses MAC_LPL_MODE_0, we don't
           * need to do anything */
        }
      }

      /* The preamble packet does not need to be
       * distributed to the upper layers
       */
      packet_dealloc(packet);
      break;

    case SYNC_WORD:
      //    DBG("%d - Rx SYNC_WORD\n", to->object);

      DBG("[BMAC] node %d receives a SYNC_WORD \n", to->object);

      /* Drop packet if we are not ready to receive it */
      if (nodedata->state != STATE_RX_WAIT_SYNC) {
        DBG("%d - Dropping SYNC_WORD, not ready "
            "to receive it\n", to->object);
        switch_transceiver(TRANSCEIVER_ON, to);
        packet_dealloc(packet);
        break;
      }

      nodedata->src_sync = header->src;
      nodedata->state = STATE_RX_WAIT_DATA;
      nodedata->clock = get_time() + WAIT_DAT_TIMEOUT;
      switch_transceiver(TRANSCEIVER_ON, to);
      bmac_add_callback(nodedata->clock, to, from, state_machine);

      /* The preamble packet does not need to be
       * distributed to the upper layers
       */
      packet_dealloc(packet);
      break;

    case DATA:
      //    DBG("%d - Rx DATA from node %d\n", to->object, header->src);

      DBG("[BMAC] node %d receives a DATA %d from %d: packet_size=%d  \n", to->object, packet->id, header->src,  packet->size);

      if (nodedata->state == STATE_RX_WAIT_DATA
          || nodedata->state == STATE_PULSE_CHECK
          || nodedata->LPL_checkint == MAC_LPL_MODE_0) {
        /* Send ACK if LL ACK are enabled and the
         * received packet is an unicast one
         */
        if (nodedata->LLAckEnabled
            && header->dst != BROADCAST_ADDR) {
          nodedata->dst_ack = header->src;
          /* Send the ACK after a small delta: it
           * avoids capture effect problems between
           * the ack and the data at the receiving node
           */
          nodedata->state = STATE_TX_ACK;
          nodedata->clock = get_time() + ACK_DELTA;
        } else {
          /* Go back to STATE_PRETX */
          nodedata->clock = backoff_restart(to);
        }

        /* Drop packet if no upper layer available */
        if (up == NULL) {
          packet_dealloc(packet);
        } else {
          int i = up->size;

          /* Distribute the packet to each class
           * located one layer above
           */
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

        bmac_add_callback(nodedata->clock, to, from, state_machine);
      } else {
        packet_dealloc(packet);
      }
      break;

    case ACK:
      if (nodedata->state == STATE_TX_WAIT_ACK) {
        /* Check if we are waiting for an ack from this node */
        if (nodedata->dst_data == header->src) {
          //	DBG("%d - Rx ACK from %d, going to "
          //	    "STATE_IDLE\n", to->object, header->src);

          DBG("[BMAC] node %d receives an ACK %d from %d: packet_size=%d  \n", to->object, packet->id, header->src,  packet->size);

          nodedata->dst_data = 0;
          nodedata->state = STATE_IDLE;
          nodedata->clock = get_time();

          /* Dealloc the buffered Data packet */
          packet_dealloc(nodedata->tx_pending);
          nodedata->backoff_remain = 0;
          nodedata->backoff_begin = 0;
          nodedata->tx_pending = NULL;

          /* Reinit retransmission counter */
          nodedata->ack_wait_count = 1;

          bmac_add_callback(nodedata->clock, to, from,
              state_machine);
        } else {
          DBG("%d - Undesired ACK (from %d but waiting "
              "from %d)\n", to->object, header->src,
              nodedata->dst_data);
        }
      }

      packet_dealloc(packet);
      break;

    default:
      DBG("%d - Rx BUG (Unknown packet type)\n", to->object);
      packet_dealloc(packet);
      break;
  }

  out:
  return;
}

/* ************************************************** */
/* ************************************************** */
mac_methods_t methods = {rx, 
    tx,
    set_header,
    get_header_size,
    get_header_real_size};

