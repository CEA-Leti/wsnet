/**
 *  \file   hello.c
 *  \brief  module for a simple HELLO protocol
 *  \author Elyes Ben Hamida
 *  \date   2009
 **/

#include <stdio.h>
#include <kernel/include/modelutils.h>
#include <inttypes.h>


/* ************************************************** */
/* ************************************************** */
model_t model =  {
  "HELLO protocol module",
  "Elyes Ben Hamida",
  "0.1",
  MODELTYPE_APPLICATION
};


/* ************************************************** */
/* ************************************************** */
struct neighbor {
  int id;
  uint64_t time;
};

/* ************************************************** */
/* ************************************************** */
struct _hello_private {
  uint64_t start;
  uint64_t w;
  uint64_t d;
  int size;
  int graph;
  /* stats */
  double avg_distance;    /* average distance of discovery */
  double avg_connexity;   /* average connexity */
  double avg_neighbors;   /* average number of discovered neighbors */
  double avg_rx;          /* nbr of received hello packets */
  double avg_delivery;    /* average delivery ratio */
  
};

struct _hello_class_private {
  int nbr_nodes;
  int **adj;
  int source;
  /* stats */
  int TX;
  int RX;
};


/* ************************************************** */
/* ************************************************** */
int callmeback(call_t *to, call_t *from, void *args);
int tx(call_t *to, call_t *from,void *args);

void init_adj(call_t *to);
void show_adj(call_t *to);
void save_adj(call_t *to);

int get_neighbors_nbr(call_t *to, int node);
double get_max_neighbor(call_t *to, int node);
double get_min_neighbor(call_t *to, int node);
int is_connex(call_t *to);
int get_nbrconnex(call_t *to);


/* ************************************************** */
/* ************************************************** */
int init(call_t *to, void *params) {
  struct _hello_class_private *classdata = malloc(sizeof(struct _hello_class_private));
  int i = 0, j = 0;

  /* init neighbors table */
  classdata->nbr_nodes = get_node_count();
  classdata->adj       = (int **) malloc(classdata->nbr_nodes * sizeof(int *));
  classdata->source    = -1;
  classdata->TX        = 0;
  classdata->RX        = 0;

  for (i = 0; i < classdata->nbr_nodes; i++) {
    classdata->adj[i] = (int *) malloc(classdata->nbr_nodes * sizeof(int));
  }

  for (i = 0; i < classdata->nbr_nodes; i++) {
    for (j = 0; j < classdata->nbr_nodes; j++) {
      classdata->adj[i][j] = 0;
    }
  }

  set_class_private_data(to, classdata);
  return 0;
}

int destroy(call_t *to) {
  struct _hello_class_private *classdata = get_class_private_data(to);
  int i = 0;
  for (i = 0; i < classdata->nbr_nodes; i++) {
    free(classdata->adj[i]);
  }
  free(classdata->adj);
  free(classdata);
  return 0;
}


/* ************************************************** */
/* ************************************************** */
int bind(call_t *to, void *params) {
  struct _hello_private *nodedata = malloc(sizeof(struct _hello_private));
  param_t *param;
    
  /* default values */
  nodedata->start  = 0;
  nodedata->w      = 1000000000;
  nodedata->d      = 8000000;
  nodedata->size   = 10;
  nodedata->graph  = 0;

  nodedata->avg_distance  = 0;
  nodedata->avg_connexity = 0;
  nodedata->avg_neighbors = 0;
  nodedata->avg_rx        = 0;
  nodedata->avg_delivery  = 0;

  /* get parameters */
  list_init_traverse(params);
  while ((param = (param_t *) list_traverse(params)) != NULL) {

    if (!strcmp(param->key, "start")) {
      if (get_param_time(param->value, &(nodedata->start))) {
	goto error;
      }
    }
    if (!strcmp(param->key, "period")) {
      if (get_param_time(param->value, &(nodedata->w))) {
	goto error;
      }
    }
    if (!strcmp(param->key, "size")) {
      if (get_param_integer(param->value, &(nodedata->size))) {
	goto error;
      }
    }
    if (!strcmp(param->key, "graph")) {
      if (get_param_integer(param->value, &(nodedata->graph))) {
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
  struct _hello_private *nodedata = get_node_private_data(to);

  free(nodedata);
  return 0;
}


/* ************************************************** */
/* ************************************************** */
int bootstrap(call_t *to) {
  struct _hello_class_private *classdata = get_class_private_data(to);

  array_t *down = get_class_bindings_down(to);
  int i = down->size;
  uint64_t start = get_time();
  int n =0, source = 0;
  double dist = 999999, tmp = 0;
  position_t center = {get_topology_area()->x/2, get_topology_area()->y/2, get_topology_area()->z/2};

  /* eventually schedule callback */
  call_t from = {-1, -1};
  scheduler_add_callback(start, to, &from, callmeback, NULL);

  /* elect source node => the node which is the closest to the area center */
  if (to->object == 0) {
    n = get_node_count();
    for (i=0; i<n; i++) {
      tmp = distance(get_node_position((nodeid_t)i), &center);
      if (tmp < dist) {
	dist = tmp;
	source = i;
      }
    }
    classdata->source = source;
    printf("Source election: node %d\n", classdata->source);

  }

  return 0;
}

int ioctl(call_t *to, int option, void *in, void **out) {
  return 0;
}


/* ************************************************** */
/* ************************************************** */
int callmeback(call_t *to, call_t *from, void *args) {
  struct _hello_private *nodedata = get_node_private_data(to);
  struct _hello_class_private *classdata =  get_class_private_data(to);
  double time = 0;

  /* callback for HELLO transmission */
  uint64_t start = get_time() + get_random_double() * (nodedata->w - nodedata->d);
  scheduler_add_callback(start, to, from, tx, NULL);
    
  if (to->object == classdata->source) {
    int neighbors_nbr = get_neighbors_nbr(to, to->object);
    double max_neigh  = get_max_neighbor(to, to->object);
    int connexity = get_nbrconnex(to);
  
    time = get_time() * 0.000000001;

    if (get_time() != 0) {
      fprintf(stdout, "Time=%lfs :  Source_id=%d  Neighbors_nbr=%d  MaxDistance=%lf  Nbr_Connex_components=%d  Delivery_ratio=%.3f%%\n", time, to->object, neighbors_nbr, max_neigh, connexity, ((float)classdata->RX/classdata->TX));
    }

    if (get_time() != 0) {
      nodedata->avg_distance  += max_neigh;
      nodedata->avg_connexity += connexity;
      nodedata->avg_neighbors += neighbors_nbr;
      nodedata->avg_delivery  += ((float)classdata->RX/classdata->TX);
      nodedata->avg_rx ++;
    }

    if (nodedata->graph > 0) {
      save_adj(to);
    }

    classdata->RX       = 0;
    classdata->TX       = 0;

    init_adj(to);
  } 

  scheduler_add_callback(get_time() + nodedata->w, to, from, callmeback, NULL);
  return 0;
}

/* ************************************************** */
/* ************************************************** */
int tx(call_t *to, call_t *from, void *args) {
  struct _hello_private *nodedata = get_node_private_data(to);
  struct _hello_class_private *classdata =  get_class_private_data(to);
  array_t *down = get_class_bindings_down(to);
  int i = down->size;
  destination_t destination = {-1, {-1,-1,-1}};

  while (i--) {
    int *hello_id =  malloc(sizeof(int));
    field_t *field_hello = field_create(INT, sizeof(int), hello_id);
    int hello_header_size = sizeof(int);
    packet_t *packet = packet_create(to, nodedata->size + hello_header_size, -1);
    call_t to0 = {down->elts[i], to->object};

    packet->destination=destination;
    *hello_id = to->object;
    packet_add_field(packet, "_hello_header", field_hello);

   PRINT_APPLICATION("[HELLO_APP] node %d sends a hello packet : packet_size=%d \n", to->object, packet->size); 
    TX(&to0, to, packet);
    classdata->TX++;
  }

  return 0;
}


/* ************************************************** */
/* ************************************************** */
void rx(call_t *to, call_t *from, packet_t *packet) {
  struct _hello_class_private *classdata = get_class_private_data(to);
  int *hello_id = (int *) packet_retrieve_field_value_ptr(packet, "_hello_header");

  if (classdata->adj[to->object][*hello_id] == 0) {
    classdata->adj[to->object][*hello_id] = 1;
  }

  classdata->RX++;

  PRINT_APPLICATION("[HELLO_APP] node %d received HELLO: packet_size=%d  rxdBm=%lf RSSI=%lf \n", to->object, packet->size, packet->rxdBm, packet->RSSI);

  /* dealloc packet */
  packet_dealloc(packet);
}



/* ************************************************** */
/* ************************************************** */
void init_adj(call_t *to) {
  struct _hello_class_private *classdata =  get_class_private_data(to);
  int i = 0, j = 0;
  for (i = 0; i < classdata->nbr_nodes; i++) {
    for (j = 0; j < classdata->nbr_nodes; j++) {
      classdata->adj[i][j] = 0;
    }
  }
}

void save_adj(call_t *to) {
  struct _hello_class_private *classdata =  get_class_private_data(to);
  FILE *file = NULL;
  char file_map[100];
  double time = get_time() * 0.000001;
  int i = 0, j = 0, n = 0;
  double degree = 0;

  /* average node degree */
  for (i = 0; i < classdata->nbr_nodes; i++) {
    n = 0;
    for (j = 0; j < classdata->nbr_nodes; j++) {
      if (i != j && classdata->adj[i][j] > 0) {
	n++;
      }
    }
    degree += n;
  }
    
  degree = (double)(degree/(double)classdata->nbr_nodes);

  /* create file */
  sprintf(file_map, "graph.%.0lf.%d.map", time, (int)floor(degree));
  if ((file = fopen(file_map, "w+")) == NULL) {
    fprintf(stderr, "My monitor: can not open file %s in monitor_event()\n", file_map);
    return;
  }

  for (i = 0; i < classdata->nbr_nodes; i++) {
    fprintf(file, "%d %lf %lf", i, get_node_position(i)->x, get_node_position(i)->y);
    for (j = 0; j < classdata->nbr_nodes; j++) {
      if (classdata->adj[i][j] > 0) {
	fprintf(file, " %d", j);
      }
    }
    fprintf(file, "\n");
  }

  fclose(file);

}

void show_adj(call_t *to) {
  struct _hello_class_private *classdata =  get_class_private_data(to);
  int i = 0, j = 0;
  for (i = 0; i < classdata->nbr_nodes; i++) {
    for (j = 0; j < classdata->nbr_nodes; j++) {
      printf("%d ",classdata->adj[i][j]);
    }
    printf("\n");
  }
}

int get_neighbors_nbr(call_t *to, int node) {
  struct _hello_class_private *classdata =  get_class_private_data(to);
  int i = 0, cpt = 0;

  for (i = 0; i < classdata->nbr_nodes; i++) {
    if (i != node && classdata->adj[node][i] == 1) {
      cpt++;
    }
  }

  return cpt;
}

double get_max_neighbor(call_t *to, int node) {
  struct _hello_class_private *classdata =  get_class_private_data(to);
  int i = 0;
  double dist = 0, tmp = 0;

  for (i = 0; i < classdata->nbr_nodes; i++) {
    if (i != node && classdata->adj[node][i] == 1) {
      tmp = distance(get_node_position(node), get_node_position(i));
      if (tmp > dist) {
	dist = tmp;
      }
    }
  }
  return dist;
}

double get_min_neighbor(call_t *to, int node) {
  struct _hello_class_private *classdata =  get_class_private_data(to);
  int i = 0;
  double dist = 99999, tmp = 0;

  for (i = 0; i < classdata->nbr_nodes; i++) {
    if (i != node && classdata->adj[node][i] == 1) {
      tmp = distance(get_node_position(node), get_node_position(i));
      if (tmp < dist) {
	dist = tmp;
      }
    }
  }
  return dist;
}

int is_connex(call_t *to) {
  struct _hello_class_private *classdata =  get_class_private_data(to);
  int i = 0, j = 0, n = 0, connected = 1;
  uintptr_t  *v  = (uintptr_t *) malloc(classdata->nbr_nodes * sizeof(int));
  //void  *pile =(uintptr_t *) list_create();
  list_t  *pile = list_create();

  for (i=0; i<classdata->nbr_nodes; i++) {
    v[i] = 0;
  }
  
  v[0] = 1;
  list_insert(pile, 0);

  while (list_getsize(pile) != 0) {
    n = (uintptr_t) list_pop(pile);
    v[n] = 1;
    for (j=0; j<n; j++) {
      if (classdata->adj[n][j] == 1 && v[j] == 0) {
	list_insert(pile,(void *)(uintptr_t) j);
	v[j] = 1;
      }
    }
  }

  connected = 1;
  for (i=0; i<classdata->nbr_nodes; i++) {
    if(v[i] == 0) {
      connected = 0;
      break;
    }
  }

  list_destroy(pile);
  free(v);
  return connected;
}


int get_nbredge(call_t *to, int node, int *v) {
  struct _hello_class_private *classdata =  get_class_private_data(to);
  //void *stack =(uintptr_t  * ) list_create();
  list_t *stack = list_create();
  int n = 0, tmp = 0, i = 0;
  
  list_insert(stack,(void *)(uintptr_t) node);

  while(list_getsize(stack)!=0) {
    tmp = (uintptr_t) list_pop(stack);
    v[tmp] = 1;

    for (i=0; i<classdata->nbr_nodes; i++){
      
      if (i != tmp && classdata->adj[tmp][i] != 0) {
	n++;
	if (v[i] == 0) {
	  v[i] = 1;
	  list_insert(stack,(void *)(uintptr_t)i);
	}
      }
    }
  }
  
  return n/2;
}

int get_nbrconnex(call_t *to) {
  struct _hello_class_private *classdata =  get_class_private_data(to);
  int i = 0, n = 0, cpt = 0;

  int *v  = (int *) malloc(classdata->nbr_nodes * sizeof(int));
  
  for (i=0; i<classdata->nbr_nodes; i++) {
    v[i] = 0;
  }

  for (i=0; i<classdata->nbr_nodes; i++) {
    if (v[i] == 0) {
      n = get_nbredge(to, i, v);
      if (n>0) cpt++;
    } 
  }

  free(v);
  return cpt;
}

/* ************************************************** */
/* ************************************************** */
application_methods_t methods = {rx};
