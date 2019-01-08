/**
 *  \file   greedy.c
 *  \brief  Greedy geographic routing
 *  \author Elyes Ben Hamida and Guillaume Chelius
 *  \date   2007
 **/
#include <stdio.h>
#include <kernel/include/modelutils.h>


/* ************************************************** */
/* ************************************************** */
model_t model =  {
  "Greedy geographic routing",
  "Elyes Ben Hamida and Guillaume Chelius",
  "0.1",
  MODELTYPE_ROUTING
};


/* ************************************************** */
/* ************************************************** */
#define HELLO_PACKET 0
#define DATA_PACKET  1


/* ************************************************** */
/* ************************************************** */
int routing_header_size =
  sizeof(nodeid_t)   + 
  3 * sizeof(double) +
  sizeof(nodeid_t)   +
  3 * sizeof(double) +
  sizeof(int)        +
  sizeof(int);
/* header dst, header dst_pos, header src, header src_pos, header hop, header type */


struct routing_header {
  nodeid_t dst;
  double dst_pos_x;
  double dst_pos_y;
  double dst_pos_z;
  nodeid_t src;
  double src_pos_x;
  double src_pos_y;
  double src_pos_z;
  int hop;
  int type;
};


struct neighbor {
  int id;
  position_t position;
  uint64_t time;
};

struct nodedata {
  list_t *neighbors;

  uint64_t start;
  uint64_t period;
  uint64_t timeout;
  int hop;       

  /* stats */
  int hello_tx;
  int hello_rx;
  int data_tx;
  int data_rx;
  int data_noroute;
  int data_hop;
};


/* ************************************************** */
/* ************************************************** */
int advert_callback(call_t *to, call_t *from, void *args);
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

  /* default values */
  nodedata->neighbors = list_create();
  nodedata->hello_tx = 0;
  nodedata->hello_rx = 0;
  nodedata->data_tx = 0;
  nodedata->data_rx = 0;
  nodedata->data_noroute = 0;
  nodedata->data_hop = 0;
  nodedata->start = 0;
  nodedata->hop = 32;
  nodedata->period = 1000000000;
  nodedata->timeout = 2500000000ull;
 
  /* get params */
  list_init_traverse(params);
  while ((param = (param_t *) list_traverse(params)) != NULL) {
    if (!strcmp(param->key, "start")) {
      if (get_param_time(param->value, &(nodedata->start))) {
	goto error;
      }
    }
    if (!strcmp(param->key, "period")) {
      if (get_param_time(param->value, &(nodedata->period))) {
	goto error;
      }
    }
    if (!strcmp(param->key, "hop")) {
      if (get_param_integer(param->value, &(nodedata->hop))) {
	goto error;
      }
    }
    if (!strcmp(param->key, "timeout")) {
      if (get_param_time(param->value, &(nodedata->timeout))) {
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
  struct neighbor *neighbor;
  while ((neighbor = (struct neighbor *) list_pop(nodedata->neighbors)) != NULL) {
    free(neighbor);
  }
  list_destroy(nodedata->neighbors);    
  free(nodedata);
  return 0;
}


/* ************************************************** */
/* ************************************************** */
int bootstrap(call_t *to) {
  struct nodedata *nodedata = get_node_private_data(to);

  /*  hello packet */
  if (nodedata->period > 0) {
    call_t from = {-1, -1};
    uint64_t start = get_time() + nodedata->start + get_random_double() * nodedata->period;
    scheduler_add_callback(start, to, &from, advert_callback, NULL);
  }

  return 0;
}

int ioctl(call_t *to, int option, void *in, void **out) {
  return 0;
}


/* ************************************************** */
/* ************************************************** */
struct neighbor* get_nexthop(call_t *to, position_t *dst) {
  struct nodedata *nodedata = get_node_private_data(to);
  struct neighbor *neighbor = NULL, *n_hop = NULL;
  uint64_t clock = get_time();
  double dist = distance(get_node_position(to->object), dst);
  double d = dist;

  /* parse neighbors */
  list_init_traverse(nodedata->neighbors);    
  while ((neighbor = (struct neighbor *) list_traverse(nodedata->neighbors)) != NULL) {        
    if ((nodedata->timeout > 0)
	&& (clock - neighbor->time) >= nodedata->timeout ) {
      continue;
    }
        
    /* choose next hop */
    if ((d = distance(&(neighbor->position), dst)) < dist) {
      dist = d;
      n_hop = neighbor;
    }
  }    

  return n_hop;
}

void add_neighbor(call_t *to, packet_t *packet) {
  struct nodedata *nodedata = get_node_private_data(to);
  struct neighbor *neighbor = NULL;

  /* check wether neighbor already exists */
  list_init_traverse(nodedata->neighbors);

  struct routing_header *routing_header = (struct routing_header *) packet_retrieve_field_value_ptr(packet, "routing_header");
 
  while ((neighbor = (struct neighbor *) list_traverse(nodedata->neighbors)) != NULL) {      
    if (neighbor->id == routing_header->src) {
      neighbor->position.x = routing_header->src_pos_x;
      neighbor->position.y = routing_header->src_pos_y; 
      neighbor->position.z = routing_header->src_pos_z; 
      neighbor->time = get_time();
      return;
    }
  }  

  neighbor = (struct neighbor *) malloc(sizeof(struct neighbor));
  neighbor->id = routing_header->src;
  neighbor->position.x = routing_header->src_pos_x;
  neighbor->position.y = routing_header->src_pos_y; 
  neighbor->position.z = routing_header->src_pos_z; 
  neighbor->time = get_time();
  list_insert(nodedata->neighbors, (void *) neighbor);
  return;
}


/* ************************************************** */
/* ************************************************** */

int get_header_size(call_t *to, call_t *from) {

  return routing_header_size;
}

int get_header_real_size(call_t *to, call_t *from) {

  return (8*routing_header_size);
}




int set_header(call_t *to, call_t *from, packet_t *packet, destination_t *dst) {
  struct nodedata *nodedata = get_node_private_data(to);
  struct neighbor *n_hop = get_nexthop(to, &(dst->position));
  destination_t destination;    
  int next_hop;

  PRINT_ROUTING("[GREEDY] SET HEADER node %d \n", to->object);

  /* if no route, return -1 */
  if (dst->id != BROADCAST_ADDR && n_hop == NULL) {
    nodedata->data_noroute++;
    PRINT_ROUTING("[GREEDY] SET HEADER node %d NO ROUTE \n", to->object);
    return -1;
  }
  else if (dst->id == BROADCAST_ADDR) {
    next_hop = BROADCAST_ADDR;
  }
  else {
    next_hop = n_hop->id;
  }

  /* Set routing header */

  struct routing_header *routing_header;
  field_t *field_header = packet_retrieve_field(packet,"routing_header");

  if (field_header == NULL) {
  routing_header = malloc(sizeof(struct routing_header));
  field_t *field_routing_header = field_create(INT, sizeof(struct routing_header), routing_header);
  packet_add_field(packet, "routing_header", field_routing_header);
  packet->size += get_header_size(to, from);
  packet->real_size += get_header_real_size(to, from);
  }
  else {
    routing_header = (struct routing_header *) field_getValue(field_header);
  }

  routing_header->dst = dst->id;
  routing_header->dst_pos_x = dst->position.x;
  routing_header->dst_pos_y = dst->position.y;
  routing_header->dst_pos_z = dst->position.z;
  routing_header->src = to->object;
  routing_header->src_pos_x = get_node_position(to->object)->x;
  routing_header->src_pos_y = get_node_position(to->object)->y;
  routing_header->src_pos_z = get_node_position(to->object)->z;
  routing_header->hop = nodedata->hop;
  routing_header->type = DATA_PACKET;

  /* Set mac header */
  destination.id = next_hop;
  destination.position.x = -1;
  destination.position.y = -1;
  destination.position.z = -1;

  packet->destination=destination;

 PRINT_ROUTING("[GREEDY] SET HEADER node %d HEADER OK \n", to->object);

  return 0;
}


/* ************************************************** */
/* ************************************************** */
int neighbor_timeout(void *data, void *arg) {
  struct neighbor *neighbor = (struct neighbor *) data;
  call_t *to = (call_t *) arg;
  struct nodedata *nodedata = get_node_private_data(to);
  if ((get_time() - neighbor->time) >= nodedata->timeout) {
    return 1;
  }
  return 0;
}

int advert_callback(call_t *to, call_t *from, void *args) {
  struct nodedata *nodedata = get_node_private_data(to);
  call_t to0   = {get_class_bindings_down(to)->elts[0], to->object};
  packet_t *packet = packet_create(to, routing_header_size, -1);


 /* Set routing header */

  struct routing_header *routing_header;
  routing_header = malloc(sizeof(struct routing_header));
  field_t *field_routing_header = field_create(INT, sizeof(struct routing_header), routing_header);
  packet_add_field(packet, "routing_header", field_routing_header);

  packet->destination.position.x = -1.0;
  packet->destination.position.y = -1.0;
  packet->destination.position.z = -1.0;
  packet->destination.id = BROADCAST_ADDR;
  routing_header->dst = BROADCAST_ADDR;
  routing_header->dst_pos_x = -1.0;
  routing_header->dst_pos_y = -1.0;
  routing_header->dst_pos_z = -1.0;
  routing_header->src = to->object;
  routing_header->src_pos_x = get_node_position(to->object)->x;
  routing_header->src_pos_y = get_node_position(to->object)->y;
  routing_header->src_pos_z = get_node_position(to->object)->z;
  routing_header->hop = 1;
  routing_header->type = HELLO_PACKET;

  PRINT_ROUTING("[GREEDY] ADVERT: node %d sends a HELLO packet : packet_size=%d \n", to->object, packet->size);

  /* send hello */
  TX(&to0, to, packet);
  nodedata->hello_tx++;

  /* check neighbors timeout  */
  list_selective_delete(nodedata->neighbors, neighbor_timeout, (void *) to);

  /* schedules hello */
  scheduler_add_callback(get_time() + nodedata->period, to, from, advert_callback, NULL);

  return 0;
}


/* ************************************************** */
/* ************************************************** */
void tx(call_t *to, call_t* from, packet_t *packet) {
  struct nodedata *nodedata = get_node_private_data(to);
  call_t to0   = {get_class_bindings_down(to)->elts[0], to->object};
  call_t from0 = {to->class, to->object};

// encapsulation of routing header
destination_t destination = packet->destination;
if (set_header(to, from, packet, &destination) == -1) {
    packet_dealloc(packet);
    return;
  }

  nodedata->data_tx++;

  PRINT_ROUTING("[GREEDY] TX: node %d sends a data packet to %d: packet_size=%d \n", to->object, packet->destination.id, packet->size);

  TX(&to0, &from0, packet);
}


/* ************************************************** */
/* ************************************************** */
void forward(call_t *to, call_t *from, packet_t *packet) {  
  struct nodedata *nodedata = get_node_private_data(to);
  call_t to0   = {get_class_bindings_down(to)->elts[0], to->object};
  call_t from0 = {to->class, to->object};
  position_t dst_pos;

  struct routing_header *routing_header = (struct routing_header *) packet_retrieve_field_value_ptr(packet, "routing_header");

  dst_pos.x = routing_header->dst_pos_x;
  dst_pos.y = routing_header->dst_pos_y;
  dst_pos.z = routing_header->dst_pos_z;
  struct neighbor *n_hop = get_nexthop(to, &dst_pos);
  destination_t destination;

  /* delivers packet to application layer */
  if (n_hop == NULL) {
    array_t *up = get_class_bindings_up(to);
    int i = up->size;
  
  PRINT_ROUTING("[GREEDY] FORWARD: node %d has no next hop \n", to->object);
      
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
  }
    
  /* update hop count */
  (routing_header->hop)--;
  if ((routing_header->hop) == 0) {
    nodedata->data_hop++;
    packet_dealloc(packet);
    return;
  }
    

  /* set mac header */
  destination.id = n_hop->id;
  destination.position.x = -1;
  destination.position.y = -1;
  destination.position.z = -1;

  packet->destination = destination;

  PRINT_ROUTING("[GREEDY] FORWARD: node %d sends a data packet to %d \n", to->object, packet->destination.id);

  /* forwarding packet */
  nodedata->data_tx++;
  TX(&to0, &from0, packet);
}


/* ************************************************** */
/* ************************************************** */
void rx(call_t *to, call_t *from, packet_t *packet) {
  struct nodedata *nodedata = get_node_private_data(to);
  array_t *up = get_class_bindings_up(to);
  int i = up->size;

  struct routing_header *routing_header = (struct routing_header *) packet_retrieve_field_value_ptr(packet, "routing_header");

  switch (routing_header->type) {
  case HELLO_PACKET:         
    nodedata->hello_rx++;
    add_neighbor(to, packet);
    packet_dealloc(packet);
    break;

  case DATA_PACKET : 
    nodedata->data_rx++;
  PRINT_ROUTING("[GREEDY] RX0: node %d received a data packet from %d : packet_size=%d \n", to->object, packet->node, packet->size);

    if ( (routing_header->dst != BROADCAST_ADDR) && (routing_header->dst  != to->object) ) {
      
 	PRINT_ROUTING("[GREEDY] RX0: node %d forward a data packet \n", to->object);
	forward(to, from, packet);
      return;
    }
  PRINT_ROUTING("[GREEDY] RX: node %d received a data packet from %d (%d): packet_size=%d \n", to->object, packet->node, routing_header->dst, packet->size);
 
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

  default :
    break;       
  }

  return;
}


/* ************************************************** */
/* ************************************************** */
routing_methods_t methods = {rx, 
                             tx, 
                             set_header, 
                             get_header_size,
                             get_header_real_size};
