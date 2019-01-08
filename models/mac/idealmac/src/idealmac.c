/**
 *  \file   idealmac.c
 *  \brief  IDEAL MAC protocol with a DISK propagation model
 *  \author Elyes Ben Hamida and Guillaume Chelius and Damien Roth
 *  \date   2008
 **/
#include <stdio.h>
#include <kernel/include/modelutils.h>

/* ************************************************** */
/* ************************************************** */

/* Please uncomment the following line to allow the transmission
   of only one data packet at the same time. */

//#define ONE_PACKET_AT_A_TIME


/* ************************************************** */
/* ************************************************** */
#define UNICAST_TYPE	    0
#define BROADCAST_TYPE      1
#define ONE_MS              1000000


/* ************************************************** */
/* ************************************************** */
struct _mac_header {
  int src;
  int dst;
  int type;
};

int _mac_header_size = sizeof(int) + sizeof(int) + sizeof(int);
/* ************************************************** */
/* ************************************************** */
struct classdata {
  double range;        // Communication range (m)
  double bandwidth;    // Data bandwidth (KB/s)
};


/* ************************************************** */
/* ************************************************** */
struct nodedata {
  list_t *buffer;
#ifdef ONE_PACKET_AT_A_TIME
  int scheduler;
#endif
};


/* ************************************************** */
/* ************************************************** */
model_t model =  {
  "IDEAL MAC module",
  "Elyes Ben Hamida and Guillaume Chelius and Damien Roth",
  "0.1",
  MODELTYPE_MAC
};


int set_header(call_t *to, call_t* from, packet_t *packet, destination_t *dst);

/* ************************************************** */
/* ************************************************** */
int init(call_t *to, void *params) {
  struct classdata *classdata = malloc(sizeof(struct classdata));
  param_t *param;

  /* default values */
  classdata->range     = 10;
  classdata->bandwidth = 15; 

  /* get parameters */
  list_init_traverse(params);
  while ((param = (param_t *) list_traverse(params)) != NULL) {
    if (!strcmp(param->key, "range")) {
      if (get_param_double(param->value, &(classdata->range))) {
	goto error;
      }
    }
    if (!strcmp(param->key, "bandwidth")) {
      if (get_param_double(param->value, &(classdata->bandwidth))) {
	goto error;
      }
    }
  }

  // KB/s to B/s
  classdata->bandwidth *= 1024;

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
  struct nodedata *nodedata = (struct nodedata *) malloc(sizeof(struct nodedata));

  nodedata->buffer    = list_create();

#ifdef ONE_PACKET_AT_A_TIME
  nodedata->scheduler = 0;
#endif

  set_node_private_data(to, nodedata);
  return 0;
}

int unbind(call_t *to) {
  struct nodedata *nodedata = get_node_private_data(to);

  packet_t *packet;
  while ((packet = (packet_t *) list_pop(nodedata->buffer)) != NULL)
    packet_dealloc(packet);

  list_destroy(nodedata->buffer);

  free(nodedata);

  return 0;
}


/* ************************************************** */
/* ************************************************** */
int bootstrap(call_t *to) {
  return 0;
}

int ioctl(call_t *to, int option, void *in, void **out) {
  return 0;
}


/* ************************************************** */
/* ************************************************** */
int tx_delay(call_t *to, call_t *from, void *args) {
  struct classdata *classdata = get_class_private_data(to);
  struct nodedata *nodedata = get_node_private_data(to);
  position_t *local = get_node_position(to->object);
  int i = 0;
  field_t *field_header;
  struct _mac_header *header;
    
  packet_t *packet;
  if ((packet = (packet_t *) list_pop_FIFO(nodedata->buffer)) == NULL)
    return 0;
   
  field_header = packet_retrieve_field(packet, "_mac_header");
  if (field_header == NULL) {
    fprintf(stderr, "error in idealmac tx_delay(), field _mac_header not found\n");
    return -1;
  }
  header = (struct _mac_header *) field_getValue(field_header);

  /* Broadcast packet */
  if (header->type == BROADCAST_TYPE) {
#ifdef LOG_MAC
    fprintf(stdout, "[MAC] node %d wants to broadcast a packet\n", to->object);
#endif
    /* sending the packet to nodes within reach communication */
    for(i = 0; i < get_node_count(); i++) {
	
      if (i != to->object) {
	  
	if (distance(get_node_position(i), local) <= classdata->range) {
	  call_t to0 = {-1, i};
	  array_t *macs = get_mac_classesid(&to0);
	  to0.class = macs->elts[0];
	  packet_t *packet_up = packet_clone(packet);         
	  RX(&to0, to, packet_up);
#ifdef LOG_MAC
	  fprintf(stdout, "[MAC] node %d delivers a packet to node %d\n", to->object, i);
#endif
	}
      }       
    }
    packet_dealloc(packet);
  } 
  /* Unicast packet */
  else if (header->type == UNICAST_TYPE){
      
    if (distance(get_node_position(header->dst), local) <= classdata->range) {
      call_t to0 = {-1, header->dst};
      array_t *macs = get_mac_classesid(&to0);
      to0.class = macs->elts[0];
      RX(&to0, to, packet);
#ifdef LOG_MAC
      fprintf(stdout,"[MAC] node %d delivers a packet to node %d\n", to->object, header->dst);
#endif
    }
    else {
#ifdef LOG_MAC
      fprintf(stdout,"[MAC] node %d: destination node %d is unreachable !!!\n",to->object, header->dst);
#endif
      packet_dealloc(packet);
    }
  }
  else {
#ifdef LOG_MAC
    fprintf(stderr,"[MAC] Unknown packet type (%d) !!!\n", header->type);
#endif
    packet_dealloc(packet);
  }

    
#ifdef ONE_PACKET_AT_A_TIME
  if (list_getsize(nodedata->buffer) == 0) {
    nodedata->scheduler = 0;
  }
  else {
    list_init_traverse(nodedata->buffer);
    if ((packet = (packet_t *) list_traverse(nodedata->buffer)) == NULL)
      return 0;
      
    uint64_t delay = get_time() + (packet->size / classdata->bandwidth) * ONE_MS;
    scheduler_add_callback(delay, to, from, tx_delay, NULL);
  }
#endif

  return 0;
}


/* ************************************************** */
/* ************************************************** */

void tx(call_t *to, call_t *from, packet_t *packet) {
  struct classdata *classdata = get_class_private_data(to);
  struct nodedata *nodedata = get_node_private_data(to);
  call_t to0   = {get_class_bindings_down(to)->elts[0], to->object};
  int phy_header_size=GET_HEADER_SIZE(&to0,to);


    // encapsulation of MAC header
    destination_t destination = packet->destination;
    if (set_header(to, from, packet, &destination) == -1) {
      packet_dealloc(packet);
      return;
    }

  list_insert(nodedata->buffer, (void *) packet);
  int duration = (packet->size + phy_header_size) / classdata->bandwidth * ONE_MS;

PRINT_MAC("[IDEALMAC] node %d transmits an DATA packet %d: packet_size=%d   \n", to->object, packet->id, packet->size);

#ifdef ONE_PACKET_AT_A_TIME
  if (nodedata->scheduler == 0) {
    nodedata->scheduler = 1;
    uint64_t delay = get_time() + duration;
    scheduler_add_callback(delay, to, from, tx_delay, NULL);
  }
#else
  uint64_t delay = get_time() + duration;
  scheduler_add_callback(delay, to, from, tx_delay, NULL);   
#endif
}

/* ************************************************** */
/* ************************************************** */
void rx(call_t *to, call_t *from, packet_t *packet) {
  array_t *up = get_class_bindings_up(to);
  int i = up->size;
  field_t *field_header;
  struct _mac_header *header;

  field_header = packet_retrieve_field(packet, "_mac_header");
  if (field_header == NULL) {
    fprintf(stderr, "error in idealmac rx(), field _mac_header not found\n");
    return;
  }
  header = (struct _mac_header *) field_getValue(field_header);

  if (header->type == UNICAST_TYPE && header->dst != to->object) {
    /* Packet not for us */
    packet_dealloc(packet);
    return;
  }

PRINT_MAC("[IDEALMAC] node %d received an DATA packet %d: packet_size=%d   \n", to->object, packet->id, packet->size);

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
}


/* ************************************************** */
/* ************************************************** */
int set_header(call_t *to, call_t* from, packet_t *packet, destination_t *dst) {
  struct _mac_header *header = malloc(sizeof(struct _mac_header));
  field_t *field_mac_header = field_create(INT, sizeof(struct _mac_header), header);

  packet_add_field(packet, "_mac_header", field_mac_header);

   packet->size += _mac_header_size ;
   packet->real_size += 8*_mac_header_size ;

  if ((header->dst = dst->id) == BROADCAST_ADDR) {
    header->type =BROADCAST_TYPE;
  } else {
    header->type = UNICAST_TYPE;
  }
  header->src = to->object;
    
  return 0;
}

int get_header_size(call_t *to, call_t *from) {
  return (_mac_header_size);
}

int get_header_real_size(call_t *to, call_t *from) {
  return (8*_mac_header_size);
}


/* ************************************************** */
/* ************************************************** */
mac_methods_t methods = {rx, 
                         tx,
                         set_header, 
                         get_header_size,
                         get_header_real_size};


    
    
