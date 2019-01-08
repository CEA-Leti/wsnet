/**
 *  \file   ght_sensor.c
 *  \brief  Sensor for ght data dissemination protocols
 *  \author Guillaume Chelius and Elyes Ben Hamida
 *  \date   2007
 **/
#include <kernel/include/modelutils.h>

#include "data_d_common.h"


/* ************************************************** */
/* ************************************************** */
struct ght_node {
    int metadata;
    position_t position;
};

struct ght_neighbor {
	int id;
    position_t position;                         
	uint64_t time;
};


/* ************************************************** */
/* ************************************************** */
#define GHT_HELLO_TYPE    10
#define GHT_DATA_TYPE     11
#define GHT_REQUEST_TYPE  12
#define GHT_RESPONSE_TYPE 13


/* ************************************************** */
/* ************************************************** */
struct ght_hello_p {
    nodeid_t src;
    position_t position;
};

struct ght_data_p {
    int n_hop;
    int source;
    int metadata;
    int d_seq;
    int d_value;
};

struct ght_request_p {
    int n_hop;
    int sink;
    position_t position;
    int r_seq;
    int metadata;
    int d_seq;
};

struct ght_response_p {
    int n_hop;
    int sink;
    int source;
    position_t position;
    int r_seq;
    int metadata;
    int d_seq;
    int d_value;
};


/**************************************************************************/
/**************************************************************************/
struct nodedata {
    /* Hello protocol */
    uint64_t h_start;
    uint64_t h_timeout;	
    uint64_t h_period;	

    /* storage */
    list_t *neighbors;
    int s_seq[MAX_SOURCE];
    int r_seq[MAX_SINK];
    int d_source[MAX_METADATA];
    int d_value[MAX_METADATA];
    int d_seq[MAX_METADATA];
};

struct classdata {
    list_t *ght_nodes;
};


/**************************************************************************/
/**************************************************************************/
model_t model =  {
    "Sensor for ght data dissemination protocol",
    "Elyes Ben Hamida",
    "0.1",
    MODELTYPE_APPLICATION
};


/**************************************************************************/
/**************************************************************************/
int hello_callback(call_t *to, call_t *from, void *args);
void rx_ght_hello(call_t *to, packet_t *packet);
void rx_ght_data(call_t *to, packet_t *packet);
void rx_ght_request(call_t *to, packet_t *packet);
void rx_ght_response(call_t *to, packet_t *packet);
void rx_source_adv(call_t *to, packet_t *packet);
void rx_source_data(call_t *to, packet_t *packet);
void rx_sink_adv(call_t *to, packet_t *packet);

position_t *ght_get_position(call_t *to, int metadata);
struct ght_neighbor *ght_next_hop(call_t *to, position_t *position);


/**************************************************************************/
/**************************************************************************/
int init(call_t *to, void *params) {
    struct classdata *classdata = malloc(sizeof(struct classdata));
    param_t *param;
    char *filepath = NULL; 
    FILE *file;
    char str[128];

    /* init values */
    classdata->ght_nodes = list_create();

    /* get parameters */
    list_init_traverse(params);
    while ((param = (param_t *) list_traverse(params)) != NULL) {
        if (!strcmp(param->key, "hashfile")) {
            filepath = param->value;
        }
    }
  
    /* get default ght node */
    if (filepath == NULL) {
        struct ght_node *ghtnode = (struct ght_node *) malloc(sizeof(struct ght_node));
        ghtnode->metadata = -1;
        ghtnode->position.x = get_random_x_position();
        ghtnode->position.y = get_random_y_position();
        ghtnode->position.z = get_random_z_position();
        list_insert(classdata->ght_nodes, (void *) ghtnode);
    } else {    
        if ((file = fopen(filepath, "r")) == NULL) {
            fprintf(stderr, "ght_sensor: can not open file %s in init()\n", filepath);

            goto error;
        }

        /* parse file */
        fseek(file, 0L, SEEK_SET);
        while (fgets(str, 128, file) != NULL) {
            struct ght_node *ghtnode = (struct ght_node *) malloc(sizeof(struct ght_node));
            if (sscanf(str, "%d %lf %lf %lf\n",  
                       &(ghtnode->metadata), 
                       &(ghtnode->position.x), 
                       &(ghtnode->position.y), 
                       &(ghtnode->position.z) ) != 4) {
                fprintf(stderr, "ght_sensor  unable to read ght position in init()\n");
                free(ghtnode);
                goto error;
            }
            list_insert(classdata->ght_nodes, (void *) ghtnode);
        }
        
        fclose(file);
    }

    set_class_private_data(to, classdata);
    return 0;

 error:
    free(classdata);
    return -1;    
}

int destroy(call_t *to) {
    struct classdata *classdata = get_class_private_data(to);
    struct ght_node *ghtnode;
    while ((ghtnode = (struct ght_node *) list_pop(classdata->ght_nodes)) != NULL) {
        free(ghtnode);
    }
    list_destroy(classdata->ght_nodes);
    free(classdata);
    return 0;
}


/**************************************************************************/
/**************************************************************************/
int bind(call_t *to, void *params) {
    struct nodedata *nodedata = malloc(sizeof(struct nodedata));    
    param_t *param;
    int i = MAX_METADATA;
    int j = MAX_SOURCE;
    int k = MAX_SINK;
    
    /* default values */
    nodedata->h_start = 0;
    nodedata->h_period = 1000000000;	
    nodedata->h_timeout = nodedata->h_period * 2;
    nodedata->neighbors = list_create();
    while (i--) {
        nodedata->d_source[i] = -1;
        nodedata->d_value[i] = -1;
        nodedata->d_seq[i] = -1;
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
            if (get_param_time(param->value, &(nodedata->h_start)))
                goto error;
        }
        if (!strcmp(param->key, "hello-period")) {
            if (get_param_time(param->value, &(nodedata->h_period)))
                goto error;
        }
        if (!strcmp(param->key, "hello-timeout")) {
            if (get_param_time(param->value, &(nodedata->h_timeout)))
                goto error;
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
    struct ght_neighbor *neighbor;

    while ((neighbor = (struct ght_neighbor *) list_pop(nodedata->neighbors)) != NULL) {
        free(neighbor);
    }
    list_destroy(nodedata->neighbors);

    free(nodedata);
    return 0;
}


/**************************************************************************/
/**************************************************************************/
int bootstrap(call_t *to) {
    struct nodedata *nodedata = get_node_private_data(to);
    uint64_t schedule = get_time() + nodedata->h_start + get_random_double() * nodedata->h_period;

    /* scheduler first hello */
    call_t from = {-1, -1};
    scheduler_add_callback(schedule, to, &from, hello_callback, NULL);
    return 0;
}


/* ************************************************** */
/* ************************************************** */
struct ght_neighbor *ght_next_hop(call_t *to, position_t *position) {
    struct nodedata *nodedata = get_node_private_data(to);
    struct ght_neighbor *neighbor = NULL, *n_hop = NULL;
    double dist = distance(get_node_position(to->object), position);
    double d = dist;
    uint64_t clock = get_time();

    /* parse neighbors */
    list_init_traverse(nodedata->neighbors);    
    while ((neighbor = (struct ght_neighbor *) list_traverse(nodedata->neighbors)) != NULL) {
        if ((nodedata->h_timeout > 0)
            && (clock - neighbor->time) >= nodedata->h_timeout ) {
            continue;
        }
        
        /* choose next hop */
        if ((d = distance(&(neighbor->position), position)) < dist) {
            dist = d;
            n_hop = neighbor;
        }
    }    
    
    return n_hop;
}


/* ************************************************** */
/* ************************************************** */
position_t *ght_get_position(call_t *to, int metadata) {
    struct classdata *classdata = get_class_private_data(to);
    struct ght_node *ghtnode, *def = NULL, *found = NULL;

    list_init_traverse(classdata->ght_nodes);    
    while ((ghtnode = (struct ght_node *) list_traverse(classdata->ght_nodes)) != NULL) {
        if (ghtnode->metadata == metadata) {
            found = ghtnode;
            break;
        } else if (ghtnode->metadata == -1) {
            def = ghtnode;
        }
    }
    if (found) {
        return &(found->position);
    } else if (def) {
        return &(def->position);
    }
    
    return NULL;
}


/**************************************************************************/
/**************************************************************************/
int neighbor_timeout(void *data, void *arg) {
    call_t *to = (call_t *) arg;
    struct nodedata *nodedata = get_node_private_data(to);
    struct ght_neighbor *neighbor = (struct ght_neighbor *) data;

    if ((get_time() - neighbor->time) > nodedata->h_timeout) {
        return 1;
    }

    return 0;
}

int hello_callback(call_t *to, call_t *from, void *args) {
    struct nodedata *nodedata = get_node_private_data(to);
    call_t to0 = {get_class_bindings_down(to)->elts[0], to->object};
    destination_t destination = {BROADCAST_ADDR, {-1, -1, -1}};
    position_t *pos = get_node_position(to->object);
    packet_t *packet = packet_create(to, sizeof(struct data_d_header) + sizeof(struct ght_hello_p), -1);

    struct data_d_header *header = malloc(sizeof(struct data_d_header));
    field_t *field_header = field_create(INT, sizeof(struct data_d_header), header);
    packet_add_field(packet, "data_d_header", field_header);

    struct ght_hello_p *hello = malloc(sizeof(struct ght_hello_p));
    field_t *field_ght = field_create(DBLE, sizeof(struct ght_hello_p), hello);
    packet_add_field(packet, "ght_hello_p", field_ght);

    packet->destination=destination;

    /* set header */
    header->type = GHT_HELLO_TYPE;
    hello->src = to->object;
    hello->position.x = pos->x;
    hello->position.y = pos->y;
    hello->position.z = pos->z;
    TX(&to0, to, packet);	   
    
    /* check neighbors timeout */
    if (nodedata->h_timeout > 0) {
        list_selective_delete(nodedata->neighbors, neighbor_timeout, (void *) to);
    }
    
    /* schedules hello */
    scheduler_add_callback(get_time() + nodedata->h_period, to, from, hello_callback, NULL);
    return 0;
}

void rx_ght_hello(call_t *to, packet_t *packet) {
    struct nodedata *nodedata = get_node_private_data(to);
    field_t *hello_field = packet_retrieve_field(packet, "ght_hello_p");
    struct ght_neighbor *neighbor;

    if (hello_field == NULL){
      packet_dealloc(packet);
      return;
    }

    struct ght_hello_p *hello = (struct ght_hello_p *) field_getValue(hello_field);

   /* check for existing neighbors */
    list_init_traverse(nodedata->neighbors);      
    while ((neighbor = (struct ght_neighbor *) list_traverse(nodedata->neighbors)) != NULL) {
        if (neighbor->id == hello->src) {
            neighbor->position.x = hello->position.x;
            neighbor->position.y = hello->position.y;
            neighbor->position.z = hello->position.z;
            neighbor->time = get_time();
            packet_dealloc(packet);
            return;
        }
    }  

    /* new neighbor */
    neighbor = (struct ght_neighbor *) malloc(sizeof(struct ght_neighbor));
    neighbor->id = hello->src;
    neighbor->position.x = hello->position.x;
    neighbor->position.y = hello->position.y;
    neighbor->position.z = hello->position.z;
    neighbor->time = get_time();
    list_insert(nodedata->neighbors, (void *) neighbor);
    packet_dealloc(packet);
    return;
}


/* ************************************************** */
/* ************************************************** */
void rx_source_adv(call_t *to, packet_t *packet) {
    struct nodedata *nodedata = get_node_private_data(to);
    struct source_adv_p *source = (struct source_adv_p *) packet_retrieve_field_value_ptr(packet, "source_adv_p");
    packet_t *packet0;
    destination_t dst = {BROADCAST_ADDR, {-1, -1, -1}};
    call_t to0 = {get_class_bindings_down(to)->elts[0], to->object};

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

    header->type = SENSOR_ADV_TYPE;
    sensor->sensor = to->object;
    sensor->source = source->source;
    sensor->s_seq = source->s_seq;
    TX(&to0, to, packet0);
    packet_dealloc(packet);
    return;
}


/**************************************************************************/
/**************************************************************************/
void rx_source_data(call_t *to, packet_t *packet) {
    struct nodedata *nodedata = get_node_private_data(to);
    struct source_data_p *data = (struct source_data_p *) packet_retrieve_field_value_ptr(packet, "source_data_p");
    position_t *ght_position;
    struct ght_neighbor *n_hop;

    /* check sensor */
    if (data->sensor != to->object) {
        /* not for us */
        packet_dealloc(packet);
        return;
    }
    
   /* get ght storing node */
    ght_position = ght_get_position(to, data->metadata);
    /* get next hop */
    n_hop = ght_next_hop(to, ght_position);
    
    /* if I am the storing node */
    if (n_hop == NULL) {
        /* store information */
        if (data->d_seq <= nodedata->d_seq[data->metadata]) {
            /* old data */
            packet_dealloc(packet);
            return;
        }
        nodedata->d_seq[data->metadata] = data->d_seq;
        nodedata->d_source[data->metadata] = data->source;
        nodedata->d_value[data->metadata] = data->d_value;
        packet_dealloc(packet);
        return;
    } else {
        /* create ght data */
        destination_t dst = {BROADCAST_ADDR, {-1, -1, -1}};
        packet_t *packet0 = packet_create(to,sizeof(struct data_d_header) + sizeof(struct ght_data_p), -1);
	struct data_d_header *header = malloc(sizeof(struct data_d_header));
	field_t *field_header = field_create(INT, sizeof(struct data_d_header), header);
	packet_add_field(packet0, "data_d_header", field_header);
	struct ght_data_p *disseminate = malloc(sizeof(struct ght_data_p));
	field_t *field_disseminate = field_create(DBLE, sizeof(struct ght_data_p), disseminate);
	packet_add_field(packet0, "ght_data_p", field_disseminate);
        call_t to0 = {get_class_bindings_down(to)->elts[0], to->object};
        
        /* forward data */
        packet0->destination=dst;

        header->type = GHT_DATA_TYPE;
        disseminate->n_hop = n_hop->id;
        disseminate->source = data->source;
        disseminate->metadata = data->metadata;
        disseminate->d_seq = data->d_seq;
        disseminate->d_value = data->d_value;
        TX(&to0, to, packet0);
        packet_dealloc(packet);
        return;
    }

    return;
}


/**************************************************************************/
/**************************************************************************/
void rx_ght_data(call_t *to, packet_t *packet) {
    struct nodedata *nodedata = get_node_private_data(to);
    struct ght_data_p *data = (struct ght_data_p *) packet_retrieve_field_value_ptr(packet, "ght_data_p");
    position_t *ght_position;
    struct ght_neighbor *n_hop;

    /* are we the next hop*/
    if (data->n_hop != to->object) {
        packet_dealloc(packet);
        return;
    }

    /* get ght storing node */
    ght_position = ght_get_position(to, data->metadata);
    /* get next hop */
    n_hop = ght_next_hop(to, ght_position);
    
    /* if I am the storing node */
    if (n_hop == NULL) {
        /* store information */
        if (data->d_seq <= nodedata->d_seq[data->metadata]) {
            /* old data */
            packet_dealloc(packet);
            return;
        }
        nodedata->d_seq[data->metadata] = data->d_seq;
        nodedata->d_source[data->metadata] = data->source;
        nodedata->d_value[data->metadata] = data->d_value;
        packet_dealloc(packet);
        return;
    } else {
        /* forward ght data */
        destination_t dst = {BROADCAST_ADDR, {-1, -1, -1}};
        call_t to0 = {get_class_bindings_down(to)->elts[0], to->object};
        
        /* forward data */
        packet->destination=dst;

        data->n_hop = n_hop->id;
        TX(&to0, to, packet);
        return;
    }

    return;
}


/* ************************************************** */
/* ************************************************** */
void rx_sink_adv(call_t *to, packet_t *packet) {
    struct nodedata *nodedata = get_node_private_data(to);
    struct sink_adv_p *sink = (struct sink_adv_p *) packet_retrieve_field_value_ptr(packet, "sink_adv_p");
    call_t to0 = {get_class_bindings_down(to)->elts[0], to->object};
            
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

        /* reply */
        packet0 = packet_create(to, sizeof(struct data_d_header) + sizeof(struct home_adv_p), -1);
	struct data_d_header *header = malloc(sizeof(struct data_d_header));
	field_t *field_header = field_create(INT, sizeof(struct data_d_header), header);
	packet_add_field(packet0, "data_d_header", field_header);
	struct home_adv_p *adv = malloc(sizeof(struct home_adv_p));
	field_t *field_adv = field_create(DBLE, sizeof(struct home_adv_p), adv);
	packet_add_field(packet0, "home_adv_p", field_adv);

        packet0->destination=dst;

        header->type = HOME_ADV_TYPE;
        adv->sensor = to->object;
        adv->sink = sink->sink;
        adv->r_seq = sink->r_seq;
        
        TX(&to0, to, packet0);	   
        packet_dealloc(packet);
        return;
    } else if (sink->home == to->object) {
        /* sends the request towards the hash location */
        position_t *ght_position;
        struct ght_neighbor *n_hop;

        /* get ght storing node */
        ght_position = ght_get_position(to, sink->metadata);
        n_hop = ght_next_hop(to, ght_position);
        
        /* if I am the storing node */
        if (n_hop == NULL) {
            packet_t *packet0;
            destination_t dst = {BROADCAST_ADDR, {-1, -1, -1}};
            
            /* check wether we have the data */
            if (sink->d_seq > nodedata->d_seq[sink->metadata]) {
                packet_dealloc(packet);
                return;
            }
            
            /* reply */
            packet0 = packet_create(to,sizeof(struct data_d_header) + sizeof(struct sensor_data_p), -1);
	    struct data_d_header *header = malloc(sizeof(struct data_d_header));
	    field_t *field_header = field_create(INT, sizeof(struct data_d_header), header);
	    packet_add_field(packet0, "data_d_header", field_header);
	    struct sensor_data_p *data = malloc(sizeof(struct sensor_data_p));
	    field_t *field_data = field_create(DBLE, sizeof(struct sensor_data_p), data);
	    packet_add_field(packet0, "sensor_data_p", field_data);
            packet0->destination=dst;

            header->type = SENSOR_DATA_TYPE;
            data->metadata = sink->metadata;
            data->sink = sink->sink;
            data->r_seq = sink->r_seq;
            data->source = nodedata->d_source[sink->metadata];
            data->d_seq = nodedata->d_seq[sink->metadata];
            data->d_value = nodedata->d_value[sink->metadata];
            TX(&to0, to, packet0);
            packet_dealloc(packet);
            return;
        } else {
            destination_t dst = {BROADCAST_ADDR, {-1, -1, -1}};
            packet_t *packet0 = packet_create(to, sizeof(struct data_d_header) + sizeof(struct ght_request_p), -1);
	    struct data_d_header *header = malloc(sizeof(struct data_d_header));
	    field_t *field_header = field_create(INT, sizeof(struct data_d_header), header);
	    packet_add_field(packet0, "data_d_header", field_header);
	    struct ght_request_p *request = malloc(sizeof(struct ght_request_p));
	    field_t *field_request = field_create(DBLE, sizeof(struct ght_request_p), request);
	    packet_add_field(packet0, "ght_request_p", field_request);
            position_t *pos = get_node_position(to->object);

            /* create request */
            packet0->destination=dst;

            header->type = GHT_REQUEST_TYPE;
            request->n_hop = n_hop->id;
            request->sink = sink->sink;
            request->r_seq = sink->r_seq;
            request->metadata = sink->metadata;
            request->d_seq = sink->d_seq;
            request->position.x = pos->x;
            request->position.y = pos->y;
            request->position.z = pos->z;
            TX(&to0, to, packet0);    
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
void rx_ght_request(call_t *to, packet_t *packet) {
    struct nodedata *nodedata = get_node_private_data(to);
    struct ght_request_p *request =
      (struct ght_request_p *) packet_retrieve_field_value_ptr(packet, "ght_request_p");
    position_t *ght_position;
    struct ght_neighbor *n_hop;
    call_t to0 = {get_class_bindings_down(to)->elts[0], to->object};

    /* are we the next hop */
    if (request->n_hop != to->object) {
        packet_dealloc(packet);
        return;
    }
    
    /* get ght storing node and next hop */
    ght_position = ght_get_position(to, request->metadata);
    n_hop = ght_next_hop(to, ght_position);    

    /* if I am the storing node */
    if (n_hop == NULL) {
        /* get next hop to home node */
        struct ght_neighbor *p_hop = ght_next_hop(to, &(request->position));
        
        /* check wether we have the data */
        if (request->d_seq > nodedata->d_seq[request->metadata]) {
            packet_dealloc(packet);
            return;
        }
        
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

            header->type = SENSOR_DATA_TYPE;
            data->metadata = request->metadata;
            data->sink = request->sink;
            data->r_seq = request->r_seq;
            data->source = nodedata->d_source[request->metadata];
            data->d_seq = nodedata->d_seq[request->metadata];
            data->d_value = nodedata->d_value[request->metadata];
            TX(&to0, to, packet0);
            packet_dealloc(packet);
            return;
        } else {
            packet_t *packet0;
            destination_t dst = {BROADCAST_ADDR, {-1, -1, -1}};
            
            /* reply */
            packet0 = packet_create(to,sizeof(struct data_d_header) + sizeof(struct ght_response_p), -1);
	    struct data_d_header *header = malloc(sizeof(struct data_d_header));
	    field_t *field_header = field_create(INT, sizeof(struct data_d_header), header);
	    packet_add_field(packet0, "data_d_header", field_header);
	    struct ght_response_p *response = malloc(sizeof(struct ght_response_p));
	    field_t *field_response = field_create(DBLE, sizeof(struct ght_response_p), response);
	    packet_add_field(packet0, "ght_response_p", field_response);
            packet0->destination=dst;

            header->type = GHT_RESPONSE_TYPE;
            response->n_hop = p_hop->id;
            response->metadata = request->metadata;
            response->sink = request->sink;
            response->r_seq = request->r_seq;
            response->source = nodedata->d_source[request->metadata];
            response->d_seq = nodedata->d_seq[request->metadata];
            response->d_value = nodedata->d_value[request->metadata];
            response->position.x = request->position.x;
            response->position.y = request->position.y;
            response->position.z = request->position.z;
            TX(&to0, to, packet0);
            packet_dealloc(packet);
            return;
        }
    } else {
        /* forward packet */
        destination_t dst = {BROADCAST_ADDR, {-1, -1, -1}};
        
        /* forward data */
        packet->destination=dst;

        request->n_hop = n_hop->id;
        TX(&to0, to, packet);    
    }

    return;
}


/**************************************************************************/
/**************************************************************************/
void rx_ght_response(call_t *to, packet_t *packet) {
    struct ght_response_p *response =
      (struct ght_response_p *) packet_retrieve_field_value_ptr(packet, "ght_response_p");
    struct ght_neighbor *n_hop;
    call_t to0 = {get_class_bindings_down(to)->elts[0], to->object};

    /* are we the next hop */
    if (response->n_hop != to->object) {
        packet_dealloc(packet);
        return;
    }

    /* get next hop */
    n_hop = ght_next_hop(to, &(response->position));
    
    /* if I am the home node */
    if (n_hop == NULL) {
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

        header->type = SENSOR_DATA_TYPE;
        data->metadata = response->metadata;
        data->sink = response->sink;
        data->r_seq = response->r_seq;
        data->source = response->source;
        data->d_seq = response->d_seq;
        data->d_value = response->d_value;
        TX(&to0, to, packet0);
        packet_dealloc(packet);
        return;
    } else {
        /* forward packet */
        destination_t dst = {BROADCAST_ADDR, {-1, -1, -1}};
        
        /* forward data */
        packet->destination=dst;

        response->n_hop = n_hop->id;
        TX(&to0, to, packet);    
    }

    return;
}


/* ************************************************** */
/* ************************************************** */
void rx(call_t *to, call_t *from, packet_t *packet) {
   struct data_d_header *header =
     (struct data_d_header *) packet_retrieve_field_value_ptr(packet, "data_d_header");
    
   switch (header->type) {       
   case GHT_HELLO_TYPE:
        /* for us */
       rx_ght_hello(to, packet);
       break;       
   case GHT_DATA_TYPE:
        /* for us */
       rx_ght_data(to, packet);
       break;       
   case GHT_REQUEST_TYPE:
        /* for us */
       rx_ght_request(to, packet);
       break;       
   case GHT_RESPONSE_TYPE:
        /* for us */
       rx_ght_response(to, packet);
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
       packet_dealloc(packet);
       break;
   }
   
    return;
}



/* ************************************************** */
/* ************************************************** */
application_methods_t methods = {rx};

