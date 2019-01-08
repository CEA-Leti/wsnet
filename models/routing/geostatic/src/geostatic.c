/**
 *  \file   geostatic.c
 *  \brief  Static geographic routing
 *  \author Romain Kuntz
 *  \date   01/2010
 **/

#include <stdio.h>
#include <kernel/include/modelutils.h>

#define UNUSED __attribute__((unused))

/* ************************************************** */
/* ************************************************** */
model_t model =  {
    "Static geographic routing",
    "Romain Kuntz",
    "0.1",
    MODELTYPE_ROUTING
};

struct neighbor {
    nodeid_t id;
    position_t position;
    uint64_t time;
};

struct nodedata {
    int hop;
    int random_nexthop;
    int random_counter;
    double range;

    list_t *neighbors;
    nodeid_t curr_dst;
    struct neighbor* curr_nexthop;
};

struct routing_header {
    nodeid_t dst;
    nodeid_t src;
    int hop;
};

/* ************************************************** */
/* ************************************************** */
int routing_header_size = sizeof(nodeid_t) + sizeof(nodeid_t) + sizeof(int);
/* header dst, header src, header hop */


/* ************************************************** */
/* ************************************************** */
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
/* Find all the neighbors (i.e. nodes in range) of the current node */
int find_neighbors(call_t *to) {
  struct nodedata *nodedata = get_node_private_data(to);
  struct neighbor *neighbor = NULL;    
  nodeid_t i;
  double dist = 0;
  int nb_neigh = 0;

  /* Parse all the nodes in the simulation: and find the ones 
   * that are in range of that node
   */
  for(i = 0; i < get_node_count(); i++) {
    /* Do not include myself */
    if (i == to->object) {
      continue;
    }

    dist = distance(get_node_position(to->object), get_node_position(i));
    if (dist <= nodedata->range) {
      /* Add the node in the list of neighbors */
      neighbor = (struct neighbor *) malloc(sizeof(struct neighbor));
      neighbor->id = i;
      neighbor->position.x = get_node_position(i)->x;
      neighbor->position.y = get_node_position(i)->y;
      neighbor->position.z = get_node_position(i)->z;
      neighbor->time = get_time();
      //PRINT_ROUTING("Node %d is added to neighbor list of node %d\n",
      //              neighbor->id, c->node);    
      list_insert(nodedata->neighbors, (void *) neighbor);
      nb_neigh++;
    }
  }

  return nb_neigh;
}

/* Get the best next hop for a specific destination */
struct neighbor* get_nexthop(call_t *to, nodeid_t dst) {
    struct nodedata *nodedata = get_node_private_data(to);
    struct neighbor *neighbor = NULL, *n_hop = NULL;
    double dist = distance(get_node_position(to->object), get_node_position(dst));
    double d = dist;

    if (nodedata->curr_dst != dst
        || nodedata->curr_nexthop == NULL || (!is_node_alive(nodedata->curr_nexthop->id))) {
      /* If the destination is different from the last one,
       * or if the current next hop is dead, reinit the
       * random counters (to force the selection of a
       * new next_hop) */
      nodedata->random_counter = nodedata->random_nexthop;
    }

    if (nodedata->random_nexthop == 0) {
      /* We keep the current next hop if
       * - next hop is not randomized
       * - the next hop is is still alive
       * - the destination is the same
       */
      if (nodedata->curr_nexthop != NULL
          && nodedata->curr_dst == dst
          && is_node_alive(nodedata->curr_nexthop->id)) {
        return nodedata->curr_nexthop;
      }

      /* Parse neighbors */
      list_init_traverse(nodedata->neighbors);
      while ((neighbor = (struct neighbor *)
          list_traverse(nodedata->neighbors)) != NULL) {
        /* Choose next hop (the one the nearest from the final dst)
         * and verify if it is still alive */
        if ((d = distance(&(neighbor->position), get_node_position(dst))) < dist
            && is_node_alive(neighbor->id)) {
          dist = d;
          n_hop = neighbor;
        }
      }
    } else if (nodedata->random_counter == nodedata->random_nexthop) {
      list_t *next_hops = list_create();
      int nh = 0;
      double nexthop_dst = 0;

      /* Random geographic routing : we choose randomly among
       * the neighbors that are nearer from the destination
       * than the current node.
       */
      list_init_traverse(nodedata->neighbors);
      while ((neighbor = (struct neighbor *)
          list_traverse(nodedata->neighbors)) != NULL) {
        /* If the neighbor happens to be the final destination,
         * then we just choose it as the next hop */
        if (neighbor->id == dst) {
          n_hop = neighbor;
          goto out;
        }

        /* Store the neighbors that are nearer from the destination
         * and that are still alive */
        nexthop_dst = distance(&(neighbor->position), get_node_position(dst));
        if (nexthop_dst < dist && is_node_alive(neighbor->id)) {
          list_insert(next_hops, (void *) neighbor);
        }
      }
      /* Choose next hop randomly among the list */
      nh = list_getsize(next_hops);
      if (nh > 0) {
        int rnd = get_random_integer_range(1, nh);
        while (rnd--) {
          neighbor = (struct neighbor *)list_pop(next_hops);
        }
        n_hop = neighbor;
      }
      list_destroy(next_hops);
    } else /* nodedata->random_counter != nodedata->random_nexthop */ {
      /* Keep the current next hop */
      n_hop = nodedata->curr_nexthop;
    }

    out:
    nodedata->random_counter--;
    if (nodedata->random_counter <= 0) {
      nodedata->random_counter = nodedata->random_nexthop;
    }

    /* Save the current next hop and destination */
    nodedata->curr_nexthop = n_hop;
    nodedata->curr_dst = dst;
    return n_hop;
}

/* ************************************************** */
/* ************************************************** */
int bind(call_t *to, void *params) {
  struct nodedata *nodedata = malloc(sizeof(struct nodedata));
  param_t *param;

  /* Find all the neighbors for this node */
  nodedata->neighbors = list_create();    
  nodedata->curr_dst = -1;
  nodedata->curr_nexthop = NULL;

  /* default values */
  nodedata->hop = 32;
  nodedata->range = 1;
  nodedata->random_nexthop = 0;

  /* Get parameters from config file */
  list_init_traverse((list_t *) params);
  while ((param = (param_t *) list_traverse(params)) != NULL) {
    /* Hop-limit */
    if (!strcmp(param->key, "hop")) {
      if (get_param_integer(param->value, &(nodedata->hop))) {
        goto error;
      }
    }
    /* Range in which neighbors are selected */
    if (!strcmp(param->key, "range")) {
      if (get_param_double(param->value, &(nodedata->range))) {
        goto error;
      }
    }
    /* Randomize the choice of the next hop. 0 means never (always 
     * take the nearest one from the destination), and a value >= 1 
     * randomizes the next hop every "value" time
     */
    if (!strcmp(param->key, "random")) {
      if (get_param_integer(param->value, &(nodedata->random_nexthop))) {
        goto error;
      }
    }
  }
  nodedata->random_counter = nodedata->random_nexthop;
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
  int nb_neigh = 0;

  /* Find all the node's neighbors (i.e. the one in range) */
  nb_neigh = find_neighbors(to);
  printf("Node %d has %d neighbors\n", to->object, nb_neigh);

  return 0;
}

int ioctl(call_t *to, int option, void *in, void **out) {
  return 0;
}

/* ************************************************** */
/* ************************************************** */
int set_header(call_t *to, call_t *from, packet_t *packet, destination_t *dst) {
  struct nodedata *nodedata = get_node_private_data(to);

  destination_t destination;

  /* No route available */
  if (dst->id == BROADCAST_ADDR) {
    destination.id = BROADCAST_ADDR;
  } else {
    struct neighbor *n_hop = get_nexthop(to, dst->id);
    if(n_hop !=NULL) {
      destination.id = n_hop->id;
    }
    else {
      return -1;
    }
  }
  destination.position.x = -1;
  destination.position.y = -1;
  destination.position.z = -1;

  packet->destination=destination;


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
  routing_header->src = to->object;
  routing_header->hop = nodedata->hop;

  return 0;
}

int get_header_size(call_t *to, call_t *from) {

  return routing_header_size;
}

int get_header_real_size(call_t *to, call_t *from) {

  return 8*routing_header_size;
}

/* ************************************************** */
/* ************************************************** */
void tx(call_t *to, call_t *from, packet_t *packet) {
  call_t to0 = {get_class_bindings_down(to)->elts[0], to->object};

  // encapsulation of routing header
  destination_t destination = packet->destination;
  if (set_header(to, from, packet, &destination) == -1) {
    packet_dealloc(packet);
    return;
  }

  TX(&to0, to, packet);
}

/* ************************************************** */
/* ************************************************** */
void forward(call_t *to, call_t *from, packet_t *packet) {  
  call_t to0 = {get_class_bindings_down(to)->elts[0], to->object};

  struct routing_header *routing_header = (struct routing_header *) packet_retrieve_field_value_ptr(packet, "routing_header");

  struct neighbor *n_hop = get_nexthop(to, routing_header->dst);
  destination_t destination;    

  /* No route available */    
  if (n_hop == NULL) {
    packet_dealloc(packet);        
    return;
  }

  /* Update hop count */
  (routing_header->hop)--;

  /* Hop count reached */
  if (routing_header->hop == 0) {
    packet_dealloc(packet);
    return;
  }

  /* Set MAC header */
  destination.id = n_hop->id;
  destination.position.x = -1;
  destination.position.y = -1;
  destination.position.z = -1;

  packet->destination = destination;

  /* Forwarding packet */
  PRINT_ROUTING("forward: Node %d forwards a packet "
      "(from %d to %d, hop limit %d)\n",
      to->object, routing_header->src, routing_header->dst,
      routing_header->hop);
  TX(&to0, to, packet);
}


/* ************************************************** */
/* ************************************************** */
void rx(call_t *to, call_t *from, packet_t *packet) {
  array_t *up = get_class_bindings_up(to);
  int i = up->size;
  struct routing_header *routing_header = (struct routing_header *) packet_retrieve_field_value_ptr(packet, "routing_header");

  /* Forward packet if node is not the recipient */
  if ((routing_header->dst != BROADCAST_ADDR) && (routing_header->dst != to->object) ) {
    forward(to, from, packet);
    return;
  }

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

/* ************************************************** */
/* ************************************************** */
routing_methods_t methods = {rx, 
    tx,
    set_header,
    get_header_size,
    get_header_real_size};
