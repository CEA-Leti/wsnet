/**
 *  \file   data_d_source.c
 *  \brief  Source for data dissemination protocols
 *  \author Guillaume Chelius and Elyes Ben Hamida
 *  \date   2007
 **/
#include <kernel/include/modelutils.h>

#include "data_d_common.h"


/**************************************************************************/
/**************************************************************************/
#define IDLE_STATE 0
#define ADV_STATE  1


/**************************************************************************/
/**************************************************************************/
struct nodedata {
    int source;
    uint64_t d_start;
    uint64_t d_period;    
    int d_seq;    
    int metadata;    
    
    int state;
    int s_seq;    

};

struct classdata {
    int source;
};


/**************************************************************************/
/**************************************************************************/
model_t model =  {
    "Source for data dissemination protocols",
    "Guillaume Chelius & Elyes Ben Hamida",
    "0.1",
    MODELTYPE_APPLICATION
};


/**************************************************************************/
/**************************************************************************/
int fsm(call_t *to, call_t *from, void * arg);


/**************************************************************************/
/**************************************************************************/
int init(call_t *to, void *params) {
    struct classdata *classdata = malloc(sizeof(struct classdata));
    /* init source id */
    classdata->source = 0;
    set_class_private_data(to, classdata);
    return 0;
}

int destroy(call_t *to) {
    struct classdata *classdata = get_class_private_data(to);
    free(classdata);
    return 0;
}


/**************************************************************************/
/**************************************************************************/
int bind(call_t *to, void *params) {
    struct nodedata *nodedata = malloc(sizeof(struct nodedata));
    struct classdata *classdata = get_class_private_data(to);
    param_t *param;

    /* default values */
    nodedata->source = classdata->source++;
    nodedata->d_start = 0;
    nodedata->d_period = 1000000000;	
    nodedata->d_seq = 0;	
    nodedata->metadata = 0;	
    nodedata->state = IDLE_STATE;	
    nodedata->s_seq = 0;	

    /* parse config file */
    list_init_traverse(params);
    while ((param = (param_t *) list_traverse(params)) != NULL) {
        if (!strcmp(param->key, "data-start")) {
            if (get_param_time(param->value, &(nodedata->d_start))) {
                goto error;
            }
        }
        if (!strcmp(param->key, "data-period")) {
            if (get_param_time(param->value, &(nodedata->d_period))) {
                goto error;
            }
        }
        if (!strcmp(param->key, "metadata")) {
            if (get_param_integer(param->value, &(nodedata->metadata))) {
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
    free(nodedata);
    return 0;
}


/**************************************************************************/
/**************************************************************************/
int bootstrap(call_t *to) {
    struct nodedata *nodedata = get_node_private_data(to);
    uint64_t schedule = get_time() + nodedata->d_start + get_random_double() * nodedata->d_period;

    /* scheduler first request */
    call_t from = {-1, -1};
    scheduler_add_callback(schedule, to, &from, fsm, NULL);

    return 0;
}


/**************************************************************************/
/**************************************************************************/
int fsm(call_t *to, call_t *from, void * arg) {
    struct nodedata *nodedata = get_node_private_data(to);
    call_t to0 = {get_class_bindings_down(to)->elts[0], to->object};
    packet_t *packet = packet_create(to,sizeof(struct data_d_header) + sizeof(struct source_adv_p), -1);
    struct data_d_header *header = malloc(sizeof(struct data_d_header));
    field_t *field_header = field_create(INT, sizeof(struct data_d_header), header);
    packet_add_field(packet, "data_d_header", field_header);
    struct source_adv_p *adv = malloc(sizeof(struct source_adv_p));
    field_t *field_adv = field_create(DBLE, sizeof(struct source_adv_p), adv);
    packet_add_field(packet, "source_adv_p", field_adv);
    destination_t dst = {BROADCAST_ADDR, {-1, -1, -1}};
    
    /* update interval variables */
    nodedata->state = ADV_STATE;
    nodedata->s_seq++;

    packet->destination=dst;
    header->type = SOURCE_ADV_TYPE;
    adv->source = nodedata->source;
    adv->s_seq = nodedata->s_seq;
    TX(&to0, to, packet);

    /* schedule next request */
    scheduler_add_callback(get_time() + nodedata->d_period, to, from, fsm, NULL);
    return 0;
}


/**************************************************************************/
/**************************************************************************/
void rx_sensor_adv(call_t *to, packet_t *packet) {
    struct nodedata *nodedata = get_node_private_data(to);
    struct sensor_adv_p *adv =
      (struct sensor_adv_p *) packet_retrieve_field_value_ptr(packet, "sensor_adv_p");
    packet_t *packet0;
    destination_t dst = {adv->sensor, {-1, -1, -1}};
    call_t to0 = {get_class_bindings_down(to)->elts[0], to->object};

    /* check source id */
    if (adv->source != nodedata->source) {
        /* not for us */
        packet_dealloc(packet);
        return;
    }
    
    /* check adv sequence */
    if (adv->s_seq != nodedata->s_seq) {
        /* old request */
        packet_dealloc(packet);
        return;
    }

    /* check state */
    if (nodedata->state != ADV_STATE) {
        /* data already sent */
        packet_dealloc(packet);
        return;
    }

    /* send data packet */
    packet0 = packet_create(to, sizeof(struct data_d_header) + sizeof(struct source_data_p), -1);
    struct data_d_header *header = malloc(sizeof(struct data_d_header));
    field_t *field_header = field_create(INT, sizeof(struct data_d_header), header);
    packet_add_field(packet0, "data_d_header", field_header);
    struct source_data_p *data = malloc(sizeof(struct source_data_p));
    field_t *field_data = field_create(DBLE, sizeof(struct source_data_p), data);
    packet_add_field(packet0, "source_data_p", field_data);
    header->type = SOURCE_DATA_TYPE;
    data->source = nodedata->source;
    data->sensor = adv->sensor;
    data->metadata = nodedata->metadata;
    data->d_seq = nodedata->d_seq++;
    data->d_value = nodedata->d_seq;
    packet0->destination=dst;

    TX(&to0, to, packet0);
    packet_dealloc(packet);

    /* return to idle state */
    nodedata->state = IDLE_STATE;

    return;
}


/**************************************************************************/
/**************************************************************************/
void rx(call_t *to, call_t *from, packet_t *packet) {
    struct data_d_header *header = 
      (struct data_d_header *) packet_retrieve_field_value_ptr(packet, "data_d_header");

    switch (header->type) {
    case SOURCE_ADV_TYPE:
        /* not for us */
        packet_dealloc(packet);
        break;
    case SENSOR_ADV_TYPE:
        /* for us */
        rx_sensor_adv(to, packet);
        break;
    case SOURCE_DATA_TYPE:
        /* not for us */
        packet_dealloc(packet);
        break;
    case SINK_ADV_TYPE:
        /* not for us */
        packet_dealloc(packet);
        break;
    case SENSOR_DATA_TYPE:
        /* not for us */
        packet_dealloc(packet);
        break;
    case HOME_ADV_TYPE:
        /* not for us */
        packet_dealloc(packet);
        break;
    default:
        /* not for us */
        packet_dealloc(packet);
        break;
    }

    return;
}


/**************************************************************************/
/**************************************************************************/
application_methods_t methods = {rx};
