/**
 *  \file   lbdd_sensor.c
 *  \brief  LBDD: the Line-Based Data Dissemination protocol (Elyes Ben Hamida and Guillaume Chelius, A Line-Based Data Dissemination protocol for Wireless Sensor Networks with Mobile Sink, ICC 2008)
 *  \author Elyes Ben Hamida and Guillaume Chelius
 *  \date   2008
 **/
#include <kernel/include/modelutils.h>
#include "data_d_common.h"


/* these macros turn ON logging */

#define LOG_APPLICATION_DISSEMINATION
//#define LOG_APPLICATION_DISSEMINATION_ROUTING

#define LOG_APPLICATION_REQUEST
//#define LOG_APPLICATION_REQUEST_ROUTING

#define LOG_APPLICATION_RESPONSE
//#define LOG_APPLICATION_RESPONSE_ROUTING


/* this macro tunes the mode of the packet transmission: BROADCAST or UNICAST */
#define USE_BROADCAST_COMMUNICATIONS

/* this macro allows to check the node state during the next-hop computation */
//#define CHECK_ACTIVE_NODE


/**************************************************************************/
/**************************************************************************/

model_t model =  {
    "LBDD: the Line-Based Data Dissemination protocol",
    "Elyes Ben Hamida",
    "0.1",
    MODELTYPE_APPLICATION
};


/* ************************************************** */
/* ************************************************** */

/* Defining global variables */

#define LBDD_HELLO_TYPE    10
#define LBDD_DATA_TYPE     11
#define LBDD_REQUEST_TYPE  12
#define LBDD_RESPONSE_TYPE 13

#define INLINE_NODE 1
#define SENSOR_NODE 3

#define DIRECTION_NONE  '0'
#define DIRECTION_UP    'U'
#define DIRECTION_DOWN  'D'


/* ************************************************** */
/* ************************************************** */

/* Defining packet types */

struct lbdd_hello_p {
    nodeid_t    src;       /* node id */
    position_t  position;  /* node position */
};

struct lbdd_data_p {
    int n_hop;             /* nexthop ID */
    int source;            /* source ID */
    int metadata;          /* type of the disseminated data */
    int d_seq;             /* sequence number of the data */
    int d_value;           /* value of the disseminated data */
    int hop;               /* for stats: hop number from the source */
    uint64_t time;         /* for stats: data birth time */
};

struct lbdd_request_p {
    int n_hop;              /* nexthop ID */
    int sink;               /* sink ID */
    position_t position;    /* home node position */
    int r_seq;              /* sequence number of the query */
    int metadata;           /* metadata of the requested data */
    int d_seq;              /* sequence number of the requested data */
    char direction;         /* direction of the query */
    int group_id;           /* group id for the query propagation */
};

struct lbdd_response_p {
    int n_hop;
    int sink;
    int source;
    position_t position;
    int r_seq;
    int metadata;
    int d_seq;
    int d_value;
    int hop;               /* for stats: hop number from the source node */
    uint64_t time;         /* for stats: data birth time */
};

/* ************************************************** */
/* ************************************************** */

/* Defining data type for neighbors table */
struct lbdd_neighbor {
    int id;
    position_t position;                         
    uint64_t time;
};

/**************************************************************************/
/**************************************************************************/

/* Node data */
struct nodedata {
    /* Global variables*/
    int type;
    int group_id;

    /* Hello protocol */
    uint64_t h_start;
    uint64_t h_timeout;	
    uint64_t h_period;	
    int      h_nbr;

    /* storage */
    list_t *neighbors;
    int s_seq[MAX_SOURCE];
    int r_seq[MAX_SINK];
    int d_source[MAX_METADATA];
    int d_value[MAX_METADATA];
    int d_seq[MAX_METADATA];
    int d_hop[MAX_METADATA];
    uint64_t d_time[MAX_METADATA];        

};

/* Class data */
struct classdata {
    /* Virtual structure parameters */
    double line_width;
    double group_size;
    int replication;
    /* stats */
    int TX_hello;
    int TX_data;
    int TX_query;
    int TX_response;
    int TX_sensor_adv;
    int TX_sensor_data;
    int TX_home_adv;
};



/**************************************************************************/
/**************************************************************************/
int hello_callback(call_t *to, call_t *from, void *args);
struct lbdd_neighbor *lbdd_next_hop(call_t *to, position_t *position);
struct lbdd_neighbor *lbdd_inside_next_hop(call_t *to, position_t *position);

int lbdd_is_nearest(call_t *to, position_t *position);

void rx_lbdd_hello(call_t *to, packet_t *packet);
void rx_lbdd_data(call_t *to, packet_t *packet);
void rx_lbdd_request(call_t *to, packet_t *packet);
void rx_lbdd_response(call_t *to, packet_t *packet);

void rx_source_adv(call_t *to, packet_t *packet);
void rx_source_data(call_t *to, packet_t *packet);
void rx_sink_adv(call_t *to, packet_t *packet);


void lbdd_stats(call_t *to);

/**************************************************************************/
/**************************************************************************/
int init(call_t *to, void *params) {
  struct classdata *classdata = malloc(sizeof(struct classdata));
  param_t *param;

  position_t *area = get_topology_area();

  classdata->line_width  = area->x / 10;
  classdata->group_size  = area->y / 10;
  classdata->replication = 0;

  classdata->TX_hello       = 0;
  classdata->TX_data        = 0;
  classdata->TX_query       = 0;
  classdata->TX_response    = 0;
  classdata->TX_sensor_adv  = 0;
  classdata->TX_sensor_data = 0;
  classdata->TX_home_adv    = 0;

  /* get class parameters */
  list_init_traverse(params);
  while ((param = (param_t *) list_traverse(params)) != NULL) {
    if (!strcmp(param->key, "line-width")) {
      if (get_param_double_range(param->value, &(classdata->line_width), 0, area->x)) {
        fprintf(stderr, "lbdd_sensor: error while reading the line-width parameter (%s) !\n", param->key);
        goto error;
      }
    }
    if (!strcmp(param->key, "group-size")) {
      if (get_param_double_range(param->value, &(classdata->group_size), 0, area->y)) {
        fprintf(stderr, "lbdd_sensor: error while reading the group-size parameter (%s) !\n", param->key);
        goto error;
      }
    }
    if (!strcmp(param->key, "replication")) {
      if (get_param_integer(param->value, &(classdata->replication))) {
        fprintf(stderr, "lbdd_sensor: error while reading the replication parameter (%s) !\n", param->key);
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
  struct classdata *classdata = get_class_private_data(to);

  long int total = classdata->TX_hello + classdata->TX_data + classdata->TX_query + classdata->TX_response + classdata->TX_sensor_adv + classdata->TX_sensor_data + classdata->TX_home_adv;

  double total_virtual = classdata->TX_hello*0.5 + classdata->TX_data + classdata->TX_query + classdata->TX_response + classdata->TX_sensor_adv*0.5 + classdata->TX_sensor_data + classdata->TX_home_adv*0.5;

  long int total_octets = classdata->TX_hello*sizeof(struct lbdd_hello_p) + classdata->TX_data*sizeof(struct lbdd_data_p) + classdata->TX_query*sizeof(struct lbdd_request_p) + classdata->TX_response*sizeof(struct lbdd_response_p) + classdata->TX_sensor_adv*sizeof(struct sensor_adv_p) + classdata->TX_sensor_data*sizeof(struct sensor_data_p) + classdata->TX_home_adv*sizeof(struct home_adv_p);

  printf("[ENERGY] HELLO %d DATA %d QUERY %d RESPONSE %d SENSOR_ADV %d SENSOR_DATA %d HOME_ADV %d TOTAL %ld TOTAL_VIRTUAL %lf TOTAL_OCTETS %ld\n", classdata->TX_hello, classdata->TX_data, classdata->TX_query, classdata->TX_response, classdata->TX_sensor_adv, classdata->TX_sensor_data, classdata->TX_home_adv, total, total_virtual, total_octets);


  free(classdata);
  return 0;
}


/**************************************************************************/
/**************************************************************************/
int bind(call_t *to, void *params) {
  struct nodedata *nodedata     = malloc(sizeof(struct nodedata));
  param_t *param;

  int i = MAX_METADATA;
  int j = MAX_SOURCE;
  int k = MAX_SINK;

  /* default values */
  nodedata->h_start   = 0;
  nodedata->h_period  = 1000000000;
  nodedata->h_timeout = nodedata->h_period * 2.5;
  nodedata->h_nbr     = -1;
  nodedata->neighbors = list_create();

  while (i--) {
    nodedata->d_source[i] = -1;
    nodedata->d_value[i]  = -1;
    nodedata->d_seq[i]    = -1;
    nodedata->d_hop[i]    = -1;
    nodedata->d_time[i]   = -1;
  }
  while (j--) {
    nodedata->s_seq[j] = -1;
  }
  while (k--) {
    nodedata->r_seq[k] = -1;
  }

  /* get parameters */
  list_init_traverse(params);
  while ((param = (param_t *) list_traverse(params)) != NULL) {
    if (!strcmp(param->key, "hello-start")) {
      if (get_param_time(param->value, &(nodedata->h_start))) {
        fprintf(stderr,"[LBDD] Error while reading the hello-start parameter ! (%s)\n",param->value);
        goto error;
      }
    }
    if (!strcmp(param->key, "hello-period")) {
      if (get_param_time(param->value, &(nodedata->h_period))) {
        fprintf(stderr,"[LBDD] Error while reading the hello-period parameter ! (%s)\n",param->value);
        goto error;
      }
    }
    if (!strcmp(param->key, "hello-timeout")) {
      if (get_param_time(param->value, &(nodedata->h_timeout))) {
        fprintf(stderr,"[LBDD] Error while reading the hello-timeout parameter ! (%s)\n",param->value);
        goto error;
      }
    }
    if (!strcmp(param->key, "hello-nbr")) {
      if (get_param_integer(param->value, &(nodedata->h_nbr))) {
        fprintf(stderr,"[LBDD] Error while reading the hello-nbr parameter ! (%s)\n",param->value);
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
  struct lbdd_neighbor *neighbor;

  //lbdd_stats(to);

  while ((neighbor = (struct lbdd_neighbor *) list_pop(nodedata->neighbors)) != NULL) {
    free(neighbor);
  }
  list_destroy(nodedata->neighbors);

  free(nodedata);
  return 0;
}


/**************************************************************************/
/**************************************************************************/
int bootstrap(call_t *to) {
  struct classdata *classdata = get_class_private_data(to);
  struct nodedata *nodedata = get_node_private_data(to);
  position_t *position = get_node_position(to->object);
  position_t *area     = get_topology_area();

  /* determining node type and group id */
  if ( (position->x >= (area->x-classdata->line_width)/2.0) && (position->x <= (area->x+classdata->line_width)/2.0) ) {
    nodedata->type     = INLINE_NODE;
    nodedata->group_id = (int)(position->y/classdata->group_size);
  }
  else {
    nodedata->type     = SENSOR_NODE;
    nodedata->group_id = -1;
  }


  uint64_t schedule = get_time() + nodedata->h_start + get_random_double() * nodedata->h_period;

  /* scheduler first hello */
  if (nodedata->h_nbr == -1 || nodedata->h_nbr > 0) {
    call_t from = {-1, -1};
    scheduler_add_callback(schedule, to, &from, hello_callback, NULL);
  }

  return 0;
}


/* ************************************************** */
/* ************************************************** */

int lbdd_is_nearest(call_t *to, position_t *sink_position) {
  struct nodedata *nodedata = get_node_private_data(to);
  struct lbdd_neighbor *neighbor = NULL;
  double dist = distance(get_node_position(to->object), sink_position);

  list_init_traverse(nodedata->neighbors);    

  while ((neighbor = (struct lbdd_neighbor *) list_traverse(nodedata->neighbors)) != NULL) {
    if ((distance(&(neighbor->position), sink_position) < dist) && (is_node_alive(neighbor->id))) {
      return 0;
    }
  }

  return 1;
}

int is_inside_line(call_t *to, position_t *position) {
  struct classdata *classdata = get_class_private_data(to);
  position_t *area     = get_topology_area();

  if ( (position->x >= (area->x-classdata->line_width)/2.0) && (position->x <= (area->x+classdata->line_width)/2.0)) return 1;
  else return 0;
}

struct lbdd_neighbor *lbdd_inside_next_hop(call_t *to, position_t *position) {
    struct nodedata *nodedata = get_node_private_data(to);
    struct lbdd_neighbor *neighbor = NULL, *n_hop = NULL;
    double dist = distance(get_node_position(to->object), position);
    double d = dist;
    uint64_t clock = get_time();

    /* parse neighbors */
    list_init_traverse(nodedata->neighbors);    

    while ((neighbor = (struct lbdd_neighbor *) list_traverse(nodedata->neighbors)) != NULL) {

#ifdef CHECK_ACTIVE_NODE
      if ( !is_node_alive(neighbor->id) || (is_inside_line(to, &(neighbor->position)) == 0) 
          || ((nodedata->h_timeout > 0) && (clock - neighbor->time) >= nodedata->h_timeout) ) {
        continue;
      }
#else
      if ( (is_inside_line(to, &(neighbor->position)) == 0) ||
          ((nodedata->h_timeout > 0) && (clock - neighbor->time) >= nodedata->h_timeout)) {
        continue;
      }
#endif

      /* choose next hop */
      if ((d = distance(&(neighbor->position), position)) < dist) {
        dist = d;
        n_hop = neighbor;
      }
    }    

    if (n_hop == NULL) return NULL; //lbdd_next_hop(to,position);
    else return n_hop;
}


/* Greedy geographic routing => computing the nexthop */ 
struct lbdd_neighbor *lbdd_next_hop(call_t *to, position_t *position) {
    struct nodedata *nodedata = get_node_private_data(to);
    struct lbdd_neighbor *neighbor = NULL, *n_hop = NULL;
    double dist = distance(get_node_position(to->object), position);
    double d = dist;
    uint64_t clock = get_time();

    /* parse neighbors */
    list_init_traverse(nodedata->neighbors);    

    while ((neighbor = (struct lbdd_neighbor *) list_traverse(nodedata->neighbors)) != NULL) {

#ifdef CHECK_ACTIVE_NODE
      if ( !is_node_alive(neighbor->id) || ((nodedata->h_timeout > 0) && (clock - neighbor->time) >= nodedata->h_timeout) ) {
        continue;
      }
#else
      if ((nodedata->h_timeout > 0) && (clock - neighbor->time) >= nodedata->h_timeout ) {
        continue;
      }
#endif

      /* choose next hop */
      if ((d = distance(&(neighbor->position), position)) < dist) {
        dist = d;
        n_hop = neighbor;
      }
    }    

    return n_hop;
}

int neighbor_timeout(void *data, void *arg) {
  call_t *to = (call_t *) arg;
  struct nodedata *nodedata = get_node_private_data(to);
  struct lbdd_neighbor *neighbor = (struct lbdd_neighbor *) data;

  if ((get_time() - neighbor->time) > nodedata->h_timeout) {
    return 1;
  }

  return 0;
}

/* Periodic exchange of hello packets */
int hello_callback(call_t *to, call_t *from, void *args) {
  struct nodedata *nodedata = get_node_private_data(to);
  struct classdata *classdata = get_class_private_data(to);

  array_t *down = get_class_bindings_down(to);
  call_t to0 = {down->elts[0], to->object};

  destination_t destination = {BROADCAST_ADDR, {-1, -1, -1}};

  packet_t *packet = packet_create(to, sizeof(struct data_d_header) + sizeof(struct lbdd_hello_p), -1);
  struct data_d_header *header = malloc(sizeof(struct data_d_header));
  field_t *field_header = field_create(INT, sizeof(struct data_d_header), header);
  packet_add_field(packet, "data_d_header", field_header);
  struct lbdd_hello_p *hello = malloc(sizeof(struct lbdd_hello_p));
  field_t *field_hello = field_create(DBLE, sizeof(struct lbdd_hello_p), hello);
  packet_add_field(packet, "lbdd_hello_p", field_hello);


  packet->destination=destination;

  position_t *pos = get_node_position(to->object);

  /* set header */
  header->type      = LBDD_HELLO_TYPE;
  hello->src        = to->object;
  hello->position.x = pos->x;
  hello->position.y = pos->y;
  hello->position.z = pos->z;

  PRINT_APPLICATION("[LBDD] node %d sends HELLO packet \n", to->object);

  TX(&to0, to, packet);
  classdata->TX_hello++;

  /* check neighbors timeout */
  if (nodedata->h_timeout > 0) {
    list_selective_delete(nodedata->neighbors, neighbor_timeout, (void *) to);
  }

  /* schedules hello */
  if (nodedata->h_nbr > 0) {
    nodedata->h_nbr --;
  }

  if (nodedata->h_nbr == -1 || nodedata->h_nbr > 0) {
    scheduler_add_callback(get_time() + nodedata->h_period, to, from, hello_callback, NULL);
  }

  return 0;
}


/* ************************************************** */
/* ************************************************** */
void rx(call_t *to, call_t *from, packet_t *packet) {
  struct data_d_header *header = (struct data_d_header *) packet_retrieve_field_value_ptr(packet, "data_d_header");

  switch (header->type) {

    case LBDD_HELLO_TYPE:
      /* for us */
      rx_lbdd_hello(to, packet);
      break;

    case LBDD_DATA_TYPE:
      /* for us */
      rx_lbdd_data(to, packet);
      break;
    case LBDD_REQUEST_TYPE:
      /* for us */
      rx_lbdd_request(to, packet);
      break;
    case LBDD_RESPONSE_TYPE:
      /* for us */
      rx_lbdd_response(to, packet);
      break;

    case SOURCE_ADV_TYPE:
      /* for us */
      rx_source_adv(to, packet);
      break;
    case SOURCE_DATA_TYPE:
      /* for us */
      rx_source_data(to, packet);
      break;
    case SINK_ADV_TYPE:
      /* for us */
      rx_sink_adv(to, packet);
      break;

    default:
      /* not for us */
      //printf("Node %d received an unknown packet type (%d) !!!\n", to->object, header->type);
      packet_dealloc(packet);
      break;
  }

  return;
}


/* ************************************************** */
/* ************************************************** */
void rx_lbdd_hello(call_t *to, packet_t *packet) {
  struct nodedata *nodedata = get_node_private_data(to);
  struct lbdd_hello_p *hello = (struct lbdd_hello_p *) packet_retrieve_field_value_ptr(packet, "lbdd_hello_p");
  struct lbdd_neighbor *neighbor;

  //    PRINT_APPLICATION("[LBDD] node %d received HELLO packet from %d \n", to->object, hello->src);

  /* check for existing neighbors */
  list_init_traverse(nodedata->neighbors);
  while ((neighbor = (struct lbdd_neighbor *) list_traverse(nodedata->neighbors)) != NULL) {
    if (neighbor->id == hello->src) {
      neighbor->position.x = hello->position.x;
      neighbor->position.y = hello->position.y;
      neighbor->position.z = hello->position.z;
      neighbor->time       = get_time();
      packet_dealloc(packet);
      return;
    }
  }

  /* new neighbor */
  neighbor = (struct lbdd_neighbor *) malloc(sizeof(struct lbdd_neighbor));
  neighbor->id         = hello->src;
  neighbor->position.x = hello->position.x;
  neighbor->position.y = hello->position.y;
  neighbor->position.z = hello->position.z;
  neighbor->time       = get_time();
  list_insert(nodedata->neighbors, (void *) neighbor);
  packet_dealloc(packet);
  return;
}


/* ************************************************** */
/* ************************************************** */
void rx_source_adv(call_t *to, packet_t *packet) {
  struct nodedata *nodedata = get_node_private_data(to);
  struct classdata *classdata = get_class_private_data(to);
  struct source_adv_p *source = (struct source_adv_p *) packet_retrieve_field_value_ptr(packet, "source_adv_p");
  packet_t *packet0;
  destination_t dst = {BROADCAST_ADDR, {-1, -1, -1}};
  array_t *down = get_class_bindings_down(to);
  call_t to0 = {down->elts[0], to->object};

  /* check adv sequence */
  if (source->s_seq <= nodedata->s_seq[source->source]) {
    /* old request */
    packet_dealloc(packet);
    return;
  }
  nodedata->s_seq[source->source] = source->s_seq;

  /* reply */
  packet0 = packet_create(to, sizeof(struct data_d_header) + sizeof(struct sensor_adv_p), -1);
  struct data_d_header *header = malloc(sizeof(struct data_d_header));
  field_t *field_header = field_create(INT, sizeof(struct data_d_header), header);
  packet_add_field(packet0, "data_d_header", field_header);
  struct sensor_adv_p *sensor = malloc(sizeof(struct sensor_adv_p));
  field_t *field_sensor = field_create(DBLE, sizeof(struct sensor_adv_p), sensor);
  packet_add_field(packet0, "sensor_adv_p", field_sensor);

  packet0->destination=dst;

  header->type   = SENSOR_ADV_TYPE;
  sensor->sensor = to->object;
  sensor->source = source->source;
  sensor->s_seq  = source->s_seq;

  PRINT_APPLICATION("[LBDD] node %d sends ADV packet \n", to->object);

  TX(&to0, to, packet0);
  classdata->TX_sensor_adv++;

  packet_dealloc(packet);
  return;
}


/**************************************************************************/
/**************************************************************************/

/* received a DATA report from a source node => forwarding towards the rendez-vous area */
void rx_source_data(call_t *to, packet_t *packet) {
  struct nodedata *nodedata = get_node_private_data(to);
  struct classdata *classdata = get_class_private_data(to);
  struct source_data_p *data = (struct source_data_p *) packet_retrieve_field_value_ptr(packet, "source_data_p");
  position_t rdv_position;
  struct lbdd_neighbor *n_hop;

  /* check sensor */
  if (data->sensor != to->object) {
    /* not for us */
    if (nodedata->type == INLINE_NODE && data->d_seq > nodedata->d_seq[data->metadata]) {
      nodedata->d_seq[data->metadata]    = data->d_seq;
      nodedata->d_source[data->metadata] = data->source;
      nodedata->d_value[data->metadata]  = data->d_value;
      nodedata->d_hop[data->metadata]    = 1;
      nodedata->d_time[data->metadata]   = get_time();
    }
    packet_dealloc(packet);
    return;
  }

  /* get the RENDEZ-VOUS AREA position */
  rdv_position.x = (get_topology_area())->x/2;
  rdv_position.y = (get_node_position(to->object))->y;
  rdv_position.z = -1;

  /* get next hop */
  n_hop = lbdd_next_hop(to, &rdv_position);

  /* if I am the storing node */
  if (n_hop == NULL) {

#ifdef LOG_APPLICATION_DISSEMINATION
    if (nodedata->type == INLINE_NODE) printf("[INLINE] %d (%.2lf,%.2lf,%.2lf) : no path towards (%.2lf,%.2lf,%.2lf) => storing data locally (%d:%d:%d:%d)  hop=1  delay=0 ms \n", to->object, (get_node_position(to->object))->x, (get_node_position(to->object))->y, (get_node_position(to->object))->z, rdv_position.x, rdv_position.y, rdv_position.z, data->source, data->d_value, data->d_seq, data->metadata);
    else printf("[Node] %d (%.2lf,%.2lf,%.2lf) : no path towards (%.2lf,%.2lf,%.2lf) => storing data locally (%d:%d:%d:%d)  hop=1  delay=0 ms \n", to->object, (get_node_position(to->object))->x, (get_node_position(to->object))->y, (get_node_position(to->object))->z, rdv_position.x, rdv_position.y, rdv_position.z, data->source, data->d_value, data->d_seq, data->metadata);
#endif
    /* store information */
    if (data->d_seq <= nodedata->d_seq[data->metadata]) {
      /* old data */
      packet_dealloc(packet);
      return;
    }
    nodedata->d_seq[data->metadata]    = data->d_seq;
    nodedata->d_source[data->metadata] = data->source;
    nodedata->d_value[data->metadata]  = data->d_value;
    nodedata->d_hop[data->metadata]    = 1;
    nodedata->d_time[data->metadata]   = get_time();

    packet_dealloc(packet);
    return;
  } else {

#ifdef LOG_APPLICATION_DISSEMINATION_ROUTING
    printf("Node %d : forwarding DATA(%d,%d) to node %d \n", to->object, data->metadata, data->d_seq, n_hop->id);
#endif
    /* create lbdd data packet */

#ifdef USE_BROADCAST_COMMUNICATIONS
    destination_t dst = {BROADCAST_ADDR, {-1, -1, -1}};
#else
    destination_t dst = {n_hop->id, {-1, -1, -1}};
#endif

    packet_t *packet0 = packet_create(to, sizeof(struct data_d_header) + sizeof(struct lbdd_data_p), -1);
    struct data_d_header *header = malloc(sizeof(struct data_d_header));
    field_t *field_header = field_create(INT, sizeof(struct data_d_header), header);
    packet_add_field(packet0, "data_d_header", field_header);
    struct lbdd_data_p *disseminate = malloc(sizeof(struct lbdd_data_p));
    field_t *field_disseminate = field_create(DBLE, sizeof(struct lbdd_data_p), disseminate);
    packet_add_field(packet0, "lbdd_data_p", field_disseminate);

    array_t *down = get_class_bindings_down(to);
    call_t to0 = {down->elts[0], to->object};

    packet0->destination=dst;

    header->type          = LBDD_DATA_TYPE;
    disseminate->n_hop    = n_hop->id;
    disseminate->source   = data->source;
    disseminate->metadata = data->metadata;
    disseminate->d_seq    = data->d_seq;
    disseminate->d_value  = data->d_value;
    disseminate->hop      = 2;
    disseminate->time     = get_time();

    PRINT_APPLICATION("[LBDD] node %d forwards DATA packet %d to node %d \n", to->object, data->d_seq, dst.id);

    TX(&to0, to, packet0);
    classdata->TX_data++;

    packet_dealloc(packet);
    return;
  }

  return;
}


/**************************************************************************/
/**************************************************************************/

/* Received a DATA report from a sensor */
void rx_lbdd_data(call_t *to, packet_t *packet) {
  struct nodedata *nodedata = get_node_private_data(to);
  struct classdata *classdata = get_class_private_data(to);
  struct lbdd_data_p *data = (struct lbdd_data_p *) packet_retrieve_field_value_ptr(packet, "lbdd_data_p");
  position_t rdv_position;
  struct lbdd_neighbor *n_hop;

  /* check sensor */
  if (data->n_hop != to->object) {
    if (nodedata->type == INLINE_NODE && data->d_seq > nodedata->d_seq[data->metadata]) {
      nodedata->d_seq[data->metadata]    = data->d_seq;
      nodedata->d_source[data->metadata] = data->source;
      nodedata->d_value[data->metadata]  = data->d_value;
      nodedata->d_hop[data->metadata]    = data->hop;
      nodedata->d_time[data->metadata]   = data->time;
    }
    /* not for us */
    packet_dealloc(packet);
    return;
  }

  /* get the RENDEZ-VOUS AREA position */
  rdv_position.x = (get_topology_area())->x/2;
  rdv_position.y = (get_node_position(to->object))->y;
  rdv_position.z = -1;

  /* get next hop */
  n_hop = lbdd_next_hop(to, &rdv_position);

  /* if I am the storing node */
  if (n_hop == NULL) {

#ifdef LOG_APPLICATION_DISSEMINATION
    if (nodedata->type == INLINE_NODE) printf("[INLINE] %d (%.2lf,%.2lf,%.2lf) : no path towards (%.2lf,%.2lf,%.2lf) => storing data locally (%d:%d:%d:%d)  hop=%d\n", to->object, (get_node_position(to->object))->x, (get_node_position(to->object))->y, (get_node_position(to->object))->z, rdv_position.x, rdv_position.y, rdv_position.z, data->source, data->d_value, data->d_seq, data->metadata, data->hop);
    else printf("[Node] %d (%.2lf,%.2lf,%.2lf) : no path towards (%.2lf,%.2lf,%.2lf) => storing data locally (%d:%d:%d:%d)  hop=%d\n", to->object, (get_node_position(to->object))->x, (get_node_position(to->object))->y, (get_node_position(to->object))->z, rdv_position.x, rdv_position.y, rdv_position.z, data->source, data->d_value, data->d_seq, data->metadata, data->hop);
#endif
    /* store information */
    if (data->d_seq <= nodedata->d_seq[data->metadata]) {
      /* old data */
      packet_dealloc(packet);
      return;
    }

    nodedata->d_seq[data->metadata]    = data->d_seq;
    nodedata->d_source[data->metadata] = data->source;
    nodedata->d_value[data->metadata]  = data->d_value;
    nodedata->d_hop[data->metadata]    = data->hop;
    nodedata->d_time[data->metadata]   = data->time;

    packet_dealloc(packet);
    return;
  } else {

#ifdef LOG_APPLICATION_DISSEMINATION_ROUTING
    printf("Node %d : forwarding DATA(%d,%d) to node %d \n", to->object, data->metadata, data->d_seq, n_hop->id);
#endif
    /* forward lbdd data packet */

#ifdef USE_BROADCAST_COMMUNICATIONS
    destination_t dst = {BROADCAST_ADDR, {-1, -1, -1}};
#else
    destination_t dst = {n_hop->id, {-1, -1, -1}};
#endif

    array_t *down = get_class_bindings_down(to);
    call_t to0 = {down->elts[0], to->object};

    packet->destination=dst;

    data->n_hop = n_hop->id;
    data->hop++;

    PRINT_APPLICATION("[LBDD] node %d forwards DATA packet %d to node %d \n", to->object, data->d_seq, dst.id);

    TX(&to0, to, packet);
    classdata->TX_data++;

    return;
  }

  return;
}


/* ************************************************** */
/* ************************************************** */

/* Received a request from a sink */
void rx_sink_adv(call_t *to, packet_t *packet) {
  struct nodedata *nodedata = get_node_private_data(to);
  struct classdata *classdata = get_class_private_data(to);
  struct sink_adv_p *sink = (struct sink_adv_p *) packet_retrieve_field_value_ptr(packet, "sink_adv_p");
  array_t *down = get_class_bindings_down(to);
  call_t to0 = {down->elts[0], to->object};

  /* starts home node election */
  if (sink->home == -1) {

    packet_t *packet0;
    destination_t dst = {BROADCAST_ADDR, {-1, -1, -1}};

    /* check request sequence */
    if (sink->r_seq <= nodedata->r_seq[sink->sink]) {
      /* old request */
      packet_dealloc(packet);
      return;
    }
    nodedata->r_seq[sink->sink] = sink->r_seq;

    packet0 = packet_create(to, sizeof(struct data_d_header) + sizeof(struct home_adv_p),-1);
    struct data_d_header *header = malloc(sizeof(struct data_d_header));
    field_t *field_header = field_create(INT, sizeof(struct data_d_header), header);
    packet_add_field(packet0, "data_d_header", field_header);
    struct home_adv_p *adv = malloc(sizeof(struct home_adv_p));
    field_t *field_adv = field_create(DBLE, sizeof(struct home_adv_p), adv);
    packet_add_field(packet0, "home_adv_p", field_adv);

    packet0->destination=dst;

    header->type = HOME_ADV_TYPE;
    adv->sensor  = to->object;
    adv->sink    = sink->sink;
    adv->r_seq   = sink->r_seq;

    PRINT_APPLICATION("[LBDD] node %d sends HOME ADV packet \n", to->object);

    TX(&to0, to, packet0);
    classdata->TX_home_adv++;

    packet_dealloc(packet);
    return;
  } else if (sink->home == to->object) {
    /* sends the request towards the hash location */
    position_t rdv_position;
    struct lbdd_neighbor *n_hop;

    /* get the RENDEZ-VOUS AREA position */
    rdv_position.x = (get_topology_area())->x/2;
    rdv_position.y = (get_node_position(to->object))->y;
    rdv_position.z = -1;

    /* get the nexthop towards the RDV area */
    n_hop = lbdd_next_hop(to, &rdv_position);

    /* if I am the storing node */
    if (n_hop == NULL) {
      packet_t *packet0;
      destination_t dst = {BROADCAST_ADDR, {-1, -1, -1}};

      /* check wether we have the data */
      if (sink->d_seq > nodedata->d_seq[sink->metadata]) {
        packet_dealloc(packet);
        return;
      }

#ifdef LOG_APPLICATION_REQUEST
      printf("Node %d : broadcasting DATA REPLY (%d,%d,%d) to sink %d \n", to->object, nodedata->d_source[sink->metadata], sink->metadata, nodedata->d_seq[sink->metadata], sink->sink);
#endif

      /* reply */
      packet0 = packet_create(to, sizeof(struct data_d_header) + sizeof(struct sensor_data_p), -1);
      struct data_d_header *header = malloc(sizeof(struct data_d_header));
      field_t *field_header = field_create(INT, sizeof(struct data_d_header), header);
      packet_add_field(packet0, "data_d_header", field_header);
      struct sensor_data_p *data = malloc(sizeof(struct sensor_data_p));
      field_t *field_data = field_create(DBLE, sizeof(struct sensor_data_p), data);
      packet_add_field(packet0, "sensor_data_p", field_data);

      packet0->destination=dst;

      header->type   = SENSOR_DATA_TYPE;
      data->metadata = sink->metadata;
      data->sink     = sink->sink;
      data->r_seq    = sink->r_seq;
      data->source   = nodedata->d_source[sink->metadata];
      data->d_seq    = nodedata->d_seq[sink->metadata];
      data->d_value  = nodedata->d_value[sink->metadata];

      PRINT_APPLICATION("[LBDD] node %d broadcasts DATA REPLY packet to sink %d\n", to->object, sink->sink);

      TX(&to0, to, packet0);
      classdata->TX_sensor_data++;

      packet_dealloc(packet);
      return;
    } else {

#ifdef LOG_APPLICATION_REQUEST_ROUTING
      printf("Node %d (%lf,%lf) : forwarding REQUEST(sink=%d,%d,%d) to node %d (%lf,%lf)\n", to->object, get_node_position(to->object)->x, get_node_position(to->object)->y, sink->sink, sink->metadata, sink->r_seq, n_hop->id, get_node_position(n_hop->id)->x, get_node_position(n_hop->id)->y);
#endif

#ifdef USE_BROADCAST_COMMUNICATIONS
      destination_t dst = {BROADCAST_ADDR, {-1, -1, -1}};
#else
      destination_t dst = {n_hop->id, {-1, -1, -1}};
#endif

      packet_t *packet0 = packet_create(to, sizeof(struct data_d_header) + sizeof(struct lbdd_request_p), -1);
      struct data_d_header *header = malloc(sizeof(struct data_d_header));
      field_t *field_header = field_create(INT, sizeof(struct data_d_header), header);
      packet_add_field(packet0, "data_d_header", field_header);
      struct lbdd_request_p *request = malloc(sizeof(struct lbdd_request_p));
      field_t *field_request = field_create(DBLE, sizeof(struct lbdd_request_p), request);
      packet_add_field(packet0, "lbdd_request_p", field_request);
      position_t *pos = get_node_position(to->object);

      /* create request */
      packet0->destination=dst;

      header->type        = LBDD_REQUEST_TYPE;
      request->n_hop      = n_hop->id;
      request->sink       = sink->sink;
      request->r_seq      = sink->r_seq;
      request->metadata   = sink->metadata;
      request->d_seq      = sink->d_seq;
      request->position.x = pos->x;
      request->position.y = pos->y;
      request->position.z = pos->z;
      request->direction  = DIRECTION_NONE;
      request->group_id   = -1;

      PRINT_APPLICATION("[LBDD] node %d forwards REQUEST packet (sink %d) \n", to->object, sink->sink);

      TX(&to0, to, packet0);
      classdata->TX_query++;

      packet_dealloc(packet);
      return;
    }
  } else {
    packet_dealloc(packet);
    return;
  }
}


/**************************************************************************/
/**************************************************************************/

/* received a request from a sensor */
void rx_lbdd_request(call_t *to, packet_t *packet) {
  struct nodedata *nodedata = get_node_private_data(to);
  struct classdata *classdata = get_class_private_data(to);

  struct lbdd_request_p *request = (struct lbdd_request_p *) packet_retrieve_field_value_ptr(packet, "lbdd_request_p");
  position_t rdv_position;
  struct lbdd_neighbor *n_hop;
  array_t *down = get_class_bindings_down(to);
  call_t to0 = {down->elts[0], to->object};

  /* are we the next hop */
  if (request->n_hop != to->object) {
    packet_dealloc(packet);
    return;
  }

  /* ################################## */
  /* we are inside the RENDEZ-VOUS AREA */
  if (nodedata->type == INLINE_NODE) {

    //printf("node %d: INSIDE\n",to->object);

    /* do we have the requested data (or some more recent data) ? */
    if (request->d_seq <= nodedata->d_seq[request->metadata]) {

      /* get next hop to home node */
      struct lbdd_neighbor *p_hop = lbdd_next_hop(to, &(request->position));

      if (p_hop == NULL) {
        packet_t *packet0;
        destination_t dst = {BROADCAST_ADDR, {-1, -1, -1}};

        /* reply */
        packet0 = packet_create(to,sizeof(struct data_d_header) + sizeof(struct sensor_data_p), -1);
        struct data_d_header *header = malloc(sizeof(struct data_d_header));
        field_t *field_header = field_create(INT, sizeof(struct data_d_header), header);
        packet_add_field(packet0, "data_d_header", field_header);
        struct sensor_data_p *data = malloc(sizeof(struct sensor_data_p));
        field_t *field_data = field_create(DBLE, sizeof(struct sensor_data_p), data);
        packet_add_field(packet0, "sensor_data_p", field_data);
        packet0->destination=dst;

        header->type   = SENSOR_DATA_TYPE;
        data->metadata = request->metadata;
        data->sink     = request->sink;
        data->r_seq    = request->r_seq;
        data->source   = nodedata->d_source[request->metadata];
        data->d_seq    = nodedata->d_seq[request->metadata];
        data->d_value  = nodedata->d_value[request->metadata];
        data->delay    = nodedata->d_time[request->metadata];
        data->hop      = nodedata->d_hop[request->metadata];

        TX(&to0, to, packet0);
        classdata->TX_sensor_data++;

        packet_dealloc(packet);
        return;
      }
      else {
        packet_t *packet0;

#ifdef USE_BROADCAST_COMMUNICATIONS
        destination_t dst = {BROADCAST_ADDR, {-1, -1, -1}};
#else
        destination_t dst = {n_hop->id, {-1, -1, -1}};
#endif

        /* reply */
        packet0 = packet_create(to,sizeof(struct data_d_header) + sizeof(struct lbdd_response_p), -1);
        struct data_d_header *header = malloc(sizeof(struct data_d_header));
        field_t *field_header = field_create(INT, sizeof(struct data_d_header), header);
        packet_add_field(packet0, "data_d_header", field_header);
        struct lbdd_response_p *response = malloc(sizeof(struct lbdd_response_p));
        field_t *field_response = field_create(DBLE, sizeof(struct lbdd_response_p), response);
        packet_add_field(packet0, "lbdd_response_p", field_response);
        packet0->destination=dst;

        header->type         = LBDD_RESPONSE_TYPE;
        response->n_hop      = p_hop->id;
        response->metadata   = request->metadata;
        response->sink       = request->sink;
        response->r_seq      = request->r_seq;
        response->source     = nodedata->d_source[request->metadata];
        response->d_seq      = nodedata->d_seq[request->metadata];
        response->d_value    = nodedata->d_value[request->metadata];
        response->time       = nodedata->d_time[request->metadata];
        response->hop        = nodedata->d_hop[request->metadata];
        response->position.x = request->position.x;
        response->position.y = request->position.y;
        response->position.z = request->position.z;

        PRINT_APPLICATION("[LBDD] node %d sends RESPONSE packet \n", to->object);

        TX(&to0, to, packet0);
        classdata->TX_response++;

        packet_dealloc(packet);
        return;
      }

    }

    /* ********************************************** */
    /* propagates query towards UP and DOWN direction */
    else if (request->direction == DIRECTION_NONE) {

      /* is there a nexthop towards the group leader */
      rdv_position.x = (get_topology_area())->x/2;
      rdv_position.y = (get_node_position(to->object))->y;
      rdv_position.z = -1;

      n_hop = lbdd_inside_next_hop(to, &rdv_position);

      /* forwards query towards group leader */
      if (n_hop != NULL) {

#ifdef LOG_APPLICATION_REQUEST
        printf("[INLINE] Node %d : received a request from sink %d (%.2lf,%.2lf) seq=%d => sending request towards group leader \n", to->object, request->sink, request->position.x, request->position.y, request->r_seq);
#endif

#ifdef USE_BROADCAST_COMMUNICATIONS
        destination_t dst = {BROADCAST_ADDR, {-1, -1, -1}};
#else
        destination_t dst = {n_hop->id, {-1, -1, -1}};
#endif
        packet->destination=dst;

#ifdef LOG_APPLICATION_REQUEST
        printf("node %d: forwards towards group leader via node %d \n", to->object, n_hop->id);
#endif

        request->n_hop = n_hop->id;
        TX(&to0, to, packet);
        classdata->TX_query++;

      }
      /* propagates query towards north and south direction */
      else {

        int group = (int)(get_node_position(to->object))->y / classdata->group_size;
        position_t up = {(get_topology_area())->x/2.0, ((group-1)*classdata->group_size)+classdata->group_size/2.0, -1};
        position_t down = {(get_topology_area())->x/2.0, ((group+1)*classdata->group_size)+classdata->group_size/2.0, -1};

#ifdef LOG_APPLICATION_REQUEST
        printf("[INLINE] Node %d (%lf,%lf) group=%d  =>  Propagation   UP:(%lf,%lf)  DOWN:(%lf,%lf)\n",to->object,(get_node_position(to->object))->x,(get_node_position(to->object))->y,group,up.x,up.y,down.x,down.y);
#endif

        /* transmits packet towards UP and DOWN direction */
        if ( (up.y >=0 && up.y <=(get_topology_area())->y) && (down.y >=0 && down.y <=(get_topology_area())->y)) {

          /* forwards packet to up direction */
          n_hop = lbdd_inside_next_hop(to, &up);

          if (n_hop != NULL) {
            request->direction = DIRECTION_UP;
            request->group_id  = (int)(up.y / classdata->group_size);
            request->n_hop     = n_hop->id;

#ifdef USE_BROADCAST_COMMUNICATIONS
            destination_t dst = {BROADCAST_ADDR, {-1, -1, -1}};
#else
            destination_t dst = {n_hop->id, {-1, -1, -1}};
#endif

            packet->destination=dst;
            request->n_hop = n_hop->id;

#ifdef LOG_APPLICATION_REQUEST
            printf("[INLINE] Node %d (%lf,%lf) =>  Propagation UP %d \n",to->object,(get_node_position(to->object))->x,(get_node_position(to->object))->y, n_hop->id);
#endif

            TX(&to0, to, packet);
            classdata->TX_query++;

          }
          else {
#ifdef LOG_APPLICATION_REQUEST_ROUTING
            printf("[INLINE] Node %d (%lf,%lf) group=%d : No path to UP (%lf,%lf)\n",to->object,(get_node_position(to->object))->x,(get_node_position(to->object))->y, group, up.x,up.y);
#endif
            packet_dealloc(packet);
          }

          /* forwards packet to down direction */
          n_hop = lbdd_inside_next_hop(to, &down);

          if (n_hop != NULL) {
            packet_t *packet_down = packet_clone(packet);
            request = (struct lbdd_request_p *) packet_retrieve_field_value_ptr(packet_down, "lbdd_request_p");

            request->direction = DIRECTION_DOWN;
            request->group_id  = (int)(down.y / classdata->group_size);
            request->n_hop     = n_hop->id;

#ifdef USE_BROADCAST_COMMUNICATIONS
            destination_t dst = {BROADCAST_ADDR, {-1, -1, -1}};
#else
            destination_t dst = {n_hop->id, {-1, -1, -1}};
#endif

            packet_down->destination=dst;
            request->n_hop = n_hop->id;

#ifdef LOG_APPLICATION_REQUEST
            printf("[INLINE] Node %d (%lf,%lf) =>  Propagation DOWN %d \n",to->object,(get_node_position(to->object))->x,(get_node_position(to->object))->y, n_hop->id);
#endif

            TX(&to0, to, packet_down);
            classdata->TX_query++;

          }
          else {
#ifdef LOG_APPLICATION_REQUEST_ROUTING
            printf("[INLINE] Node %d (%lf,%lf) group=%d : No path to DOWN (%lf,%lf)\n",to->object,(get_node_position(to->object))->x,(get_node_position(to->object))->y, group,up.x,up.y);
#endif
          }

        }
        /* transmits the packet towards the UP direction */
        else if (up.y >=0 && up.y <=(get_topology_area())->y) {

          n_hop = lbdd_inside_next_hop(to, &up);

          if (n_hop != NULL) {
            request->direction = DIRECTION_UP;
            request->group_id  = (int)(up.y / classdata->group_size);
            request->n_hop     = n_hop->id;

#ifdef USE_BROADCAST_COMMUNICATIONS
            destination_t dst = {BROADCAST_ADDR, {-1, -1, -1}};
#else
            destination_t dst = {n_hop->id, {-1, -1, -1}};
#endif

            packet->destination=dst;
            request->n_hop = n_hop->id;

#ifdef LOG_APPLICATION_REQUEST
            printf("[INLINE] Node %d (%lf,%lf) =>  Propagation UP1 %d \n",to->object,(get_node_position(to->object))->x,(get_node_position(to->object))->y, n_hop->id);
#endif

            TX(&to0, to, packet);
            classdata->TX_query++;

          }
          else {

#ifdef LOG_APPLICATION_REQUEST_ROUTING
            printf("[INLINE] Node %d (%lf,%lf) group=%d : No path to UP1 (%lf,%lf) \n",to->object,(get_node_position(to->object))->x,(get_node_position(to->object))->y, group,up.x,up.y);
#endif
            packet_dealloc(packet);
          }

        }
        /* transmits the packet towards the DOWN direction */
        else if (down.y >=0 && down.y <=(get_topology_area())->y) {

          n_hop = lbdd_inside_next_hop(to, &down);

          if (n_hop != NULL) {
            request->direction = DIRECTION_DOWN;
            request->group_id  = (int)(down.y / classdata->group_size);
            request->n_hop     = n_hop->id;

#ifdef USE_BROADCAST_COMMUNICATIONS
            destination_t dst = {BROADCAST_ADDR, {-1, -1, -1}};
#else
            destination_t dst = {n_hop->id, {-1, -1, -1}};
#endif
            packet->destination=dst;
            request->n_hop = n_hop->id;

#ifdef LOG_APPLICATION_REQUEST
            printf("[INLINE] Node %d (%lf,%lf) => Propagation DOWN1 %d \n",to->object,(get_node_position(to->object))->x,(get_node_position(to->object))->y, n_hop->id);
#endif

            TX(&to0, to, packet);
            classdata->TX_query++;

          }
          else {
#ifdef LOG_APPLICATION_REQUEST_ROUTING
            printf("[INLINE] Node %d (%lf,%lf) group=%d : No path to DOWN1 (%lf,%lf) \n",to->object,(get_node_position(to->object))->x,(get_node_position(to->object))->y, group,down.x,down.y);
#endif
            packet_dealloc(packet);
          }

        }
        else {
          /* should not happen */
          packet_dealloc(packet);
        }

        return;
      }

    }

    /* ************************************* */
    /* propagates query towards UP direction */
    else if (request->direction == DIRECTION_UP) {

#ifdef LOG_APPLICATION_REQUEST
      printf("[INLINE] Node %d (%lf,%lf) group=%d UP \n",to->object,(get_node_position(to->object))->x,(get_node_position(to->object))->y,(int)(get_node_position(to->object)->y/classdata->group_size));
#endif

      position_t pos = {(get_topology_area())->x/2.0, (request->group_id*classdata->group_size)+classdata->group_size/2.0, -1};
      n_hop = lbdd_inside_next_hop(to, &pos);

      /* forwards request towards the group leader */
      if (n_hop != NULL) {

#ifdef USE_BROADCAST_COMMUNICATIONS
        destination_t dst = {BROADCAST_ADDR, {-1, -1, -1}};
#else
        destination_t dst = {n_hop->id, {-1, -1, -1}};
#endif
        packet->destination=dst;
        request->n_hop = n_hop->id;

#ifdef LOG_APPLICATION_REQUEST
        printf("Node %d received packet UP fowarding to leader via %d\n",to->object,n_hop->id);
#endif

        TX(&to0, to, packet);
        classdata->TX_query++;

      }
      /* forwards request towards the upper group leader */
      else if ( (request->group_id-1) >= 0) {
        request->group_id --;
        position_t up = {(get_topology_area())->x/2.0, (request->group_id*classdata->group_size)+classdata->group_size/2.0, -1};
        n_hop = lbdd_inside_next_hop(to, &up);

        if (n_hop != NULL) {
          request->n_hop     = n_hop->id;

#ifdef USE_BROADCAST_COMMUNICATIONS
          destination_t dst = {BROADCAST_ADDR, {-1, -1, -1}};
#else
          destination_t dst = {n_hop->id, {-1, -1, -1}};
#endif

          packet->destination=dst;
          request->n_hop = n_hop->id;

#ifdef LOG_APPLICATION_REQUEST
          printf("Node %d received packet UP fowarding to upper leader via %d\n",to->object,n_hop->id);
#endif

          TX(&to0, to, packet);
          classdata->TX_query++;

        }
        else {
#ifdef LOG_APPLICATION_REQUEST_ROUTING
          printf("[INLINE] Node %d (%lf,%lf) group=%d : No path to UP (%lf,%lf) \n",to->object,(get_node_position(to->object))->x,(get_node_position(to->object))->y,(int)(get_node_position(to->object)->y/classdata->group_size),up.x,up.y);
#endif
          packet_dealloc(packet);
        }

      }
      else {
        packet_dealloc(packet);
      }

      return;
    }
    /* ************************************* */
    /* propagates query towards DOWN direction */
    else if (request->direction == DIRECTION_DOWN) {

#ifdef LOG_APPLICATION_REQUEST
      //printf("[INLINE] Node %d (%lf,%lf) group=%d DOWN \n",to->object,(get_node_position(to->object))->x,(get_node_position(to->object))->y,(int)(get_node_position(to->object)->y/classdata->group_size));
      printf("Node %d (%lf,%lf) received packet DOWN\n",to->object,(get_node_position(to->object))->x,(get_node_position(to->object))->y);
#endif
      position_t pos = {(get_topology_area())->x/2.0, (request->group_id*classdata->group_size)+classdata->group_size/2.0, -1};
      n_hop = lbdd_inside_next_hop(to, &pos);

      /* forwards request towards the group leader */
      if (n_hop != NULL) {

#ifdef USE_BROADCAST_COMMUNICATIONS
        destination_t dst = {BROADCAST_ADDR, {-1, -1, -1}};
#else
        destination_t dst = {n_hop->id, {-1, -1, -1}};
#endif

        packet->destination=dst;
        request->n_hop = n_hop->id;

#ifdef LOG_APPLICATION_REQUEST
        printf("Node %d received packet DOWN fowarding to leader via %d\n",to->object,n_hop->id);
#endif

        TX(&to0, to, packet);
        classdata->TX_query++;

      }
      /* forwards request towards the upper group leader */
      else if ( (request->group_id+1) <= (get_topology_area())->y) {
        request->group_id ++;
        position_t down = {(get_topology_area())->x/2.0, (request->group_id*classdata->group_size)+classdata->group_size/2.0, -1};
        n_hop = lbdd_inside_next_hop(to, &down);

        if (n_hop != NULL) {
          request->n_hop     = n_hop->id;

#ifdef USE_BROADCAST_COMMUNICATIONS
          destination_t dst = {BROADCAST_ADDR, {-1, -1, -1}};
#else
          destination_t dst = {n_hop->id, {-1, -1, -1}};
#endif

          packet->destination=dst;
          request->n_hop = n_hop->id;

#ifdef LOG_APPLICATION_REQUEST
          printf("Node %d received packet DOWN fowarding to down leader via %d\n",to->object,n_hop->id);
#endif

          TX(&to0, to, packet);
          classdata->TX_query++;

        }
        else {
#ifdef LOG_APPLICATION_REQUEST_ROUTING
          printf("[INLINE] Node %d (%lf,%lf) group=%d : No path to DOWN (%lf,%lf) \n",to->object,(get_node_position(to->object))->x,(get_node_position(to->object))->y,(int)(get_node_position(to->object)->y/classdata->group_size),down.x,down.y);
#endif
          packet_dealloc(packet);
        }

      }
      else {
        packet_dealloc(packet);
      }

      return;
    }
  }

  /* ########################################### */
  /* forwards query towards the RENDEZ-VOUS AREA */
  else {
    /* get the RENDEZ-VOUS AREA position */
    rdv_position.x = (get_topology_area())->x/2;
    rdv_position.y = (get_node_position(to->object))->y;
    rdv_position.z = -1;

    n_hop = lbdd_next_hop(to, &rdv_position);

    /* if I am the storing node */
    if (n_hop == NULL) {
      /* get next hop to home node */
      struct lbdd_neighbor *p_hop = lbdd_next_hop(to, &(request->position));

      /* check wether we have the data */
      if (request->d_seq > nodedata->d_seq[request->metadata]) {
        packet_dealloc(packet);
        return;
      }

      /* broadcasts the data to the sink */
      if (p_hop == NULL) {
        packet_t *packet0;
        destination_t dst = {BROADCAST_ADDR, {-1, -1, -1}};

        /* reply */
        packet0 = packet_create(to, sizeof(struct data_d_header) + sizeof(struct sensor_data_p), -1);
        struct data_d_header *header = malloc(sizeof(struct data_d_header));
        field_t *field_header = field_create(INT, sizeof(struct data_d_header), header);
        packet_add_field(packet0, "data_d_header", field_header);
        struct sensor_data_p *data = malloc(sizeof(struct sensor_data_p));
        field_t *field_data = field_create(DBLE, sizeof(struct sensor_data_p), data);
        packet_add_field(packet0, "sensor_data_p", field_data);
        packet0->destination=dst;
        header->type   = SENSOR_DATA_TYPE;
        data->metadata = request->metadata;
        data->sink     = request->sink;
        data->r_seq    = request->r_seq;
        data->source   = nodedata->d_source[request->metadata];
        data->d_seq    = nodedata->d_seq[request->metadata];
        data->d_value  = nodedata->d_value[request->metadata];
        TX(&to0, to, packet0);
        classdata->TX_sensor_data++;

        packet_dealloc(packet);
        return;
      }
      /* sends the response back to the home-node */
      else {
        packet_t *packet0;

#ifdef USE_BROADCAST_COMMUNICATIONS
        destination_t dst = {BROADCAST_ADDR, {-1, -1, -1}};
#else
        destination_t dst = {n_hop->id, {-1, -1, -1}};
#endif

        /* reply */
        packet0 = packet_create(to, sizeof(struct data_d_header) + sizeof(struct lbdd_response_p), -1);
        struct data_d_header *header = malloc(sizeof(struct data_d_header));
        field_t *field_header = field_create(INT, sizeof(struct data_d_header), header);
        packet_add_field(packet0, "data_d_header", field_header);
        struct lbdd_response_p *response = malloc(sizeof(struct lbdd_response_p));
        field_t *field_response = field_create(DBLE, sizeof(struct lbdd_response_p), response);
        packet_add_field(packet0, "lbdd_response_p", field_response);
        packet0->destination=dst;

        header->type         = LBDD_RESPONSE_TYPE;
        response->n_hop      = p_hop->id;
        response->metadata   = request->metadata;
        response->sink       = request->sink;
        response->r_seq      = request->r_seq;
        response->source     = nodedata->d_source[request->metadata];
        response->d_seq      = nodedata->d_seq[request->metadata];
        response->d_value    = nodedata->d_value[request->metadata];
        response->position.x = request->position.x;
        response->position.y = request->position.y;
        response->position.z = request->position.z;

        TX(&to0, to, packet0);
        classdata->TX_response++;

        packet_dealloc(packet);
        return;
      }
    } else {

#ifdef LOG_APPLICATION_REQUEST_ROUTING
      printf("Node %d (%lf,%lf) : forwarding REQUEST(sink=%d,%d,%d) to node %d (RDV %lf,%lf)\n", to->object, (get_node_position(to->object))->x,(get_node_position(to->object))->y, request->sink, request->metadata, request->r_seq, n_hop->id,rdv_position.x,rdv_position.y);
#endif

      /* forward packet */
#ifdef USE_BROADCAST_COMMUNICATIONS
      destination_t dst = {BROADCAST_ADDR, {-1, -1, -1}};
#else
      destination_t dst = {n_hop->id, {-1, -1, -1}};
#endif

      /* forward data */
      packet->destination=dst;
      request->n_hop = n_hop->id;

      TX(&to0, to, packet);
      classdata->TX_query++;
    }
  }

  return;
}


/**************************************************************************/
/**************************************************************************/
void rx_lbdd_response(call_t *to, packet_t *packet) {
  struct classdata *classdata = get_class_private_data(to);
  struct lbdd_response_p *response =
      (struct lbdd_response_p *) packet_retrieve_field_value_ptr(packet, "lbdd_response_p");
  struct lbdd_neighbor *n_hop;
  array_t *down = get_class_bindings_down(to);
  call_t to0 = {down->elts[0], to->object};

  /* are we the next hop */
  if (response->n_hop != to->object) {
    packet_dealloc(packet);
    return;
  }

  /* get next hop */
  n_hop = lbdd_next_hop(to, &(response->position));

  /* if I am the home node */
  if (n_hop == NULL) {

#ifdef LOG_APPLICATION_RESPONSE
    printf("[HOME] node %d : broadcasting received DATA to (sink=%d,%d,%d)\n", to->object, response->sink, response->metadata, response->r_seq);
#endif
    packet_t *packet0;
    destination_t dst = {BROADCAST_ADDR, {-1, -1, -1}};

    /* reply */
    packet0 = packet_create(to, sizeof(struct data_d_header) + sizeof(struct sensor_data_p), -1);
    struct data_d_header *header = malloc(sizeof(struct data_d_header));
    field_t *field_header = field_create(INT, sizeof(struct data_d_header), header);
    packet_add_field(packet0, "data_d_header", field_header);
    struct sensor_data_p *data = malloc(sizeof(struct sensor_data_p));
    field_t *field_data = field_create(DBLE, sizeof(struct sensor_data_p), data);
    packet_add_field(packet0, "sensor_data_p", field_data);
    packet0->destination=dst;

    header->type   = SENSOR_DATA_TYPE;
    data->metadata = response->metadata;
    data->sink     = response->sink;
    data->r_seq    = response->r_seq;
    data->source   = response->source;
    data->d_seq    = response->d_seq;
    data->d_value  = response->d_value;
    data->delay    = (get_time()-response->time)*0.000001;
    data->hop      = response->hop;

    TX(&to0, to, packet0);
    classdata->TX_sensor_data++;

    packet_dealloc(packet);
    return;
  } else {

#ifdef LOG_APPLICATION_RESPONSE_ROUTING
    printf("Node %d : forwarding REPLAY to (sink=%d,%d,%d) via node %d \n", to->object, response->sink, response->metadata, response->r_seq, n_hop->id);
#endif
    /* forward packet */

#ifdef USE_BROADCAST_COMMUNICATIONS
    destination_t dst = {BROADCAST_ADDR, {-1, -1, -1}};
#else
    destination_t dst = {n_hop->id, {-1, -1, -1}};
#endif

    /* forward data */
    packet->destination=dst;
    response->n_hop = n_hop->id;
    response->hop++;

    TX(&to0, to, packet);
    classdata->TX_response++;
  }

  return;
}

/* ************************************************** */
/* ************************************************** */
void lbdd_stats(call_t *to) {
  struct nodedata *nodedata = get_node_private_data(to);
  position_t *position = get_node_position(to->object);

  if (nodedata->type == INLINE_NODE) {
    printf("INLINE node %d (%lf,%lf,%lf)  Group_id=%d   Neighbors=%d\n", to->object, position->x, position->y, position->z, nodedata->group_id, list_getsize(nodedata->neighbors));
  } else {
    printf("SENSOR node %d (%lf,%lf,%lf)  Neighbors=%d\n", to->object, position->x, position->y, position->z, list_getsize(nodedata->neighbors));

  }
}


/* ************************************************** */
/* ************************************************** */
application_methods_t methods = {rx};

