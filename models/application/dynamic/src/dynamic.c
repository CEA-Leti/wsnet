/**
 *  \file   dynamic.c
 *  \brief  A dynamic (event/time/query-driven) application
 *  \author Romain KUNTZ
 *  \date   01/2010
 **/

#include <stdio.h>
#include <kernel/include/modelutils.h>
#include "dynamic.h"

/* ************************************************** */
/* ************************************************** */
model_t model =  {
  "A dynamic (event/time/query-driven) application",
  "Romain KUNTZ",
  "0.1",
  MODELTYPE_APPLICATION
};

/* ************************************************** */
/* ************************************************** */
struct _app_data {
  /* Starting / stopping time, relative to the birth of node */
  uint64_t start;
  uint64_t stop;
  /* Type of data collection scheme */
  int scheme;
  /* Time-driven configuration items */
  uint64_t tx_period;
  /* Whether starting time is picked-up in [start, start+tx_period] */
  int random_start;
  /* Event-driven configuration item */
  int threshold;
  /* Data sequence number */
  uint16_t sequence;
  /* Data destination information */
  nodeid_t destination;
  /* Packet payload size in Bytes */
  int payload_size;
  /* Timer */
  void *timer_id;
};

/* Data packet payload, only used to store the 
 * sequence number. Real payload size is
 * _app_data.payload_size + sizeof(struct pkt_payload) 
 */
struct pkt_payload {
  uint16_t seq;     // sequence 0 is used to notify a query
};

/* ************************************************** */
/* ************************************************** */
void switch_time_driven(call_t *to, uint64_t start_time, 
                        uint64_t end_time, uint64_t period);
void tx_data(call_t *to);
void tx_query(call_t *to, nodeid_t dst);

/* ************************************************** */
/* ************************************************** */
// Debug macro
#ifdef LOG_APPLICATION
#define DBG(arg...)				\
  do {						\
    fprintf(stderr, "%s: ", __FUNCTION__);	\
    fprintf(stderr, arg);			\
  } while (0)
#else
#define DBG(...)
#endif /* LOG_APPLICATION */

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
  struct _app_data *nodedata = malloc(sizeof(struct _app_data));
  param_t *param;
    
  /* Default values */
  nodedata->start = 0;
  nodedata->stop = 0; 
  nodedata->scheme = APP_TIME_DRV;
  nodedata->random_start = 0;    
  nodedata->tx_period = 0;
  nodedata->threshold = 0;
  nodedata->sequence = 1;
  nodedata->payload_size = 0;
  nodedata->destination = BROADCAST_ADDR;
  nodedata->timer_id = NULL;

  /* Reading the values from the xml config file */
  list_init_traverse(params);
  while ((param = (param_t *) list_traverse(params)) != NULL) {
    /* Time at which the node will start sending data 
     * (this time is relative to the node birth) 
     */
    if (!strcmp(param->key, "start")) {
      if (get_param_time(param->value, &(nodedata->start))) {
	goto error;
      }
    }
    /* Time at which the node will stop sending data (relative 
     * to the node birth). 0 means it never stops until the end 
     * of the simulation
     */
    if (!strcmp(param->key, "stop")) {
      if (get_param_time(param->value, &(nodedata->stop))) {
	goto error;
      }
    }
    /* Data collection scheme. 0 is time-driven, 1 is
     * event-driven, and 2 is query-driven */
    if (!strcmp(param->key, "scheme")) {
      if (get_param_integer(param->value, &(nodedata->scheme))) {
	goto error;
      }
      if (nodedata->scheme != APP_TIME_DRV
	  && nodedata->scheme != APP_EVENT_DRV
	  && nodedata->scheme != APP_QUERY_DRV) {
	nodedata->scheme = APP_TIME_DRV;
	DBG("%d - Unknown collection scheme, using "
	    "the time-driven one\n", to->object);
      }
    }
    /* Data sending period for the time-driven scheme. 0 means 
     * that no data will ever be sent 
     */
    if (!strcmp(param->key, "tx_period")) {
      if (get_param_time(param->value, &(nodedata->tx_period))) {
	goto error;
      }
    }
    /* Whether the start time must be picked up randomly between
     * the start time and (start time + sending period) in the 
     * time-driven scheme. This avoids synchronized sending 
     * among the nodes 
     */
    if (!strcmp(param->key, "random_start")) {
      if (get_param_integer(param->value, &(nodedata->random_start))) {
	goto error;
      }
    }
    /* Threshold vale for the event-driven scheme */
    if (!strcmp(param->key, "threshold")) {
      if (get_param_integer(param->value, &(nodedata->threshold))) {
	goto error;
      }
    }        
    /* Destination (node id) to which the data must be sent */
    if (!strcmp(param->key, "dst")) {
      if (get_param_integer(param->value, &(nodedata->destination))) {
	goto error;
      }
    }
    /* Data payload size (in Bytes) */
    if (!strcmp(param->key, "payload")) {
      if (get_param_integer(param->value, &(nodedata->payload_size))) {
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
  struct _app_data *nodedata = get_node_private_data(to);

  /* Destroy timer */
  if (nodedata->timer_id != NULL) {
    destroy_timer(nodedata->timer_id);
    nodedata->timer_id = NULL; 
  }

  free(nodedata);

  return 0;
}

/* ************************************************** */
/* ************************************************** */
int bootstrap(call_t *to) {
  struct _app_data *nodedata = get_node_private_data(to);

  /* Schedule a timer if node is configured in time-driven mode */
  if (nodedata->scheme == APP_TIME_DRV) {
    switch_time_driven(to, nodedata->start, 
		       nodedata->stop, nodedata->tx_period);
  }

  return 0;
}

/* ************************************************** */
/* ************************************************** */
/* Function used by switch_time_driven() */
int test_stop_timer(call_t *to, void *timer_id) {
  struct _app_data *nodedata = get_node_private_data(to);

  /* Test whether sending packets should stop or not */
  if (nodedata->stop > 0 && get_time() >= nodedata->stop) {
    /* Stop */
    return 1;
  }

  return 0;
}

/* Function used by switch_time_driven() */
int send_time_drv_msg(call_t *to, void *args) {
  struct _app_data *nodedata = get_node_private_data(to);

  /* This function may be called while the node already 
   * switched to another data collection scheme, so we 
   * test if the node is still using a time-driven scheme 
   */
  if (nodedata->scheme == APP_TIME_DRV) {
    tx_data(to);
  }
  return 0;
}

/* Function used to switch to the time-driven data collection 
 * scheme.
 */
void switch_time_driven(call_t *to, uint64_t start_time,
                        uint64_t stop_time, uint64_t period) {
  struct _app_data *nodedata = get_node_private_data(to);   

  if (nodedata->scheme == APP_TIME_DRV
      && nodedata->tx_period == period
      && nodedata->timer_id != NULL) {
    DBG("Node %d: nothing to do (no changes in "
	"mode nor period)\n", to->object);
  } else {
    /* Destroy the previous timer if it exists */
    if (nodedata->timer_id != NULL) {
      destroy_timer(nodedata->timer_id);
      nodedata->timer_id = NULL;
    }

    nodedata->scheme = APP_TIME_DRV;
    nodedata->tx_period = period;
    nodedata->stop = stop_time;

    if (nodedata->random_start && nodedata->tx_period > 0) {
      /* Randomize the start time between start_time and 
       * start_time + nodedata->tx_period
       */
      nodedata->start = get_random_time_range(start_time, 
					      start_time + nodedata->tx_period);
      DBG("%d - Random start = %"PRId64"ns\n", 
	  to->object, nodedata->start);
    }

    /* Create a new timer */
    if ((nodedata->stop <= 0 || nodedata->stop > nodedata->start)
	&& nodedata->tx_period > 0) {
      if (nodedata->stop > 0) {
	/* Stop time is relative to the node birth */
	nodedata->stop = get_time() + nodedata->stop;
      }

      /* Periodic timer for send_msg() */
      nodedata->timer_id = create_timer(to, 
					send_time_drv_msg, 
					test_stop_timer, 
					periodic_trigger, 
					&(nodedata->tx_period));
            
      /* Start timer */
      /* The next event is scheduled automatically at
       * get_time() + nodedata->start, so there is 
       * no need to add get_time() by ourself here 
       */
      start_timer(nodedata->timer_id, nodedata->start);
    }
  }

  return;
}

/* Function used to switch to the event-driven data collection 
 * scheme.
 */
void switch_event_driven(call_t *to, int threshold) {
  struct _app_data *nodedata = get_node_private_data(to);   
 
  /* Destroy the ongoing timer */    
  if (nodedata->scheme == APP_TIME_DRV 
      && nodedata->timer_id != NULL) {
    destroy_timer(nodedata->timer_id);
    nodedata->timer_id = NULL;
  }

  nodedata->scheme = APP_EVENT_DRV;
  nodedata->threshold = threshold;

  return;
}

/* Function used to switch to the query-driven data collection 
 * scheme.
 */
void switch_query_driven(call_t *to) {
  struct _app_data *nodedata = get_node_private_data(to);   
    
  /* Destroy the ongoing timer */    
  if (nodedata->scheme == APP_TIME_DRV 
      && nodedata->timer_id != NULL) {
    destroy_timer(nodedata->timer_id);
    nodedata->timer_id = NULL;
  }

  nodedata->scheme = APP_QUERY_DRV;

  return;
}

void handle_event(call_t *to, int event_value) {
  struct _app_data *nodedata = get_node_private_data(to);    

  if (nodedata->scheme != APP_EVENT_DRV) {
    DBG("Node %d is not in event-driven mode (now in %s mode)\n",
	to->object, (nodedata->scheme == APP_TIME_DRV) ? 
	"time-driven":"query-driven");
  } else if (nodedata->threshold > event_value) {
    DBG("Node %d is in event-driven mode but threshold "
	"not exceeded (%d < %d)\n", to->object, 
	event_value, nodedata->threshold);
  } else {
    DBG("Node %d is in event-driven mode and threshold "
	"exceeded (%d >= %d)\n", to->object, 
	event_value, nodedata->threshold);
    /* Threshold is exceeded, report data */
    tx_data(to);
  }

  return;
}

/* IOCTL function to control the application behaviour */
int ioctl(call_t *to, int option, void *in, void **out) {
  struct _app_data *nodedata = get_node_private_data(to);    
  int new_value = *((int *) in);
  uint64_t period;

  switch (option) {
  case APP_TIME_DRV:
    // Translate period from msec to nsec
    period = new_value*1000000;
    DBG("Node %d switching to time-driven mode with "
	"period %"PRId64"ns\n", to->object, period);
    // Switch to time-driven
    switch_time_driven(to, 0 /* start_time is now */, 
		       0 /* end_time is now */, period);
    break;

  case APP_EVENT_DRV:
    DBG("Node %d switching to event-driven mode with "
	"threshold %d\n", to->object, new_value);
    // Switch to event-driven
    switch_event_driven(to, new_value);
    break;

  case APP_QUERY_DRV:
    DBG("Node %d switching to query-driven mode\n", to->object);
    // Switch to time-driven
    switch_query_driven(to);
    break;

  case APP_PAYLOAD_SIZE:
    DBG("Node %d changing payload size from %d to %d Bytes\n",
	to->object, nodedata->payload_size, new_value);
    nodedata->payload_size = new_value;
    break;

  case ENV_EVENT:
    DBG("Node %d receives an event from the environment with "
	"value = %d\n", to->object, new_value);
    /* Handle events from the environment */
    handle_event(to, new_value);
    break;

  case QUERY_MSG:
    DBG("Node %d will send a query to node %d\n",
	to->object, new_value);
    tx_query(to, new_value);
    break;

  case CHANGE_APP_DST:
    DBG("Destination for node %d is now %d\n",
	to->object, new_value);
    nodedata->destination = new_value;
    break;

  default:
    DBG("Unknown IOCTL message\n");
    break;
  }

  return 0;
}

/* ************************************************** */
/* ************************************************** */
void tx_data(call_t *to) {
  struct _app_data *nodedata = get_node_private_data(to);
  array_t *down = get_class_bindings_down(to);
  call_t to0 = {down->elts[0], to->object};

  /* Create a data packet */
  int real_size = 8*(sizeof(struct pkt_payload)
		     + nodedata->payload_size);
  packet_t *packet = packet_create(to, sizeof(struct pkt_payload), real_size);
  if (packet == NULL) {
    return;
  }

  /* Set the lower layer header */
  destination_t destination = {nodedata->destination, {-1, -1, -1}};
  packet->destination=destination; 

  /* Set the payload */
  struct pkt_payload *payload = malloc(sizeof(struct pkt_payload));
  field_t *field_payload = field_create(INT, sizeof(struct pkt_payload), payload);
  packet_add_field(packet, "pkt_payload", field_payload);
  payload->seq = nodedata->sequence++;

  /* Send the packet to the lower layer */
  DBG("Time %"PRId64" - Node %d sent a msg (seq %d, %d bits)\n",
      get_time(), to->object, payload->seq, real_size);
  TX(&to0, to, packet);
}

void tx_query(call_t *to, nodeid_t dst) {
  array_t *down = get_class_bindings_down(to);
  call_t to0 = {down->elts[0], to->object};

  /* Create a query packet */
  packet_t *packet = packet_create(to, sizeof(struct pkt_payload),-1);
  if (packet == NULL) {
    return;
  }

  /* Set the lower layer header */
  destination_t destination = {dst, {-1, -1, -1}};
  packet->destination=destination; 
 
  /* Set the payload */
  struct pkt_payload *payload = malloc(sizeof(struct pkt_payload));
  field_t *field_payload = field_create(INT, sizeof(struct pkt_payload), payload);
  packet_add_field(packet, "pkt_payload", field_payload);
  payload->seq = 0;   // Query type

  /* Send the packet to the lower layer */
  DBG("Time %"PRId64" - Node %d sent a query to node %d\n",
      get_time(), to->object, dst);
  TX(&to0, to, packet);
}

/* ************************************************** */
/* ************************************************** */
void rx(call_t *to, call_t *from, packet_t *packet) {
  struct _app_data *nodedata = get_node_private_data(to);
  struct pkt_payload *payload = (struct pkt_payload *) packet_retrieve_field_value_ptr(packet, "pkt_payload");
    
  if (payload->seq == 0) {
    DBG("Time %"PRId64" - Node %d received a QUERY...\n",
	get_time(), to->object);

    if (nodedata->scheme == APP_QUERY_DRV) {
      /* A query was received, send data */
      tx_data(to);
    } else {
      DBG("... but node %d is not in query-driven mode "
	  "(now in %s mode)\n", to->object, 
	  (nodedata->scheme == APP_TIME_DRV) ? 
	  "time-driven":"event-driven");
    }
  } else {
    DBG("Time %"PRId64" - Node %d received a msg (seq %d, %d bits)\n",
	get_time(), to->object, payload->seq, packet->real_size);
  }

  packet_dealloc(packet);
}

/* ************************************************** */
/* ************************************************** */
application_methods_t methods = {rx};

