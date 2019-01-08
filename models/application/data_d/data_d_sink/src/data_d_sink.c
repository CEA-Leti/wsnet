/**
 *  \file   data_d_sink.c
 *  \brief  Sink for data dissemination protocols
 *  \author Guillaume Chelius and Elyes Ben Hamida
 *  \date   2007
 **/
#include <kernel/include/modelutils.h>

#include "data_d_common.h"


#define LOG_APPLICATION_SINK
#define LOG_APPLICATION_SINK_REQ

/**************************************************************************/
/**************************************************************************/
#define IDLE_STATE 0
#define ADV_STATE  1


/**************************************************************************/
/**************************************************************************/
struct nodedata {
    int sink;
    uint64_t r_start;
    uint64_t r_period;    
    int r_seq;    

    int state;
    int metadata;    
    int d_seq;  
    int d_value;  

};

struct classdata {
    int sink;

    /* stats */
    int init;
    int d_rx;
    int attempt;
    int success;
    uint64_t log_period;
    FILE *logfile;

    /* delay */
    double last_packet_time;
    int last_packet;
};


/**************************************************************************/
/**************************************************************************/
model_t model =  {
    "Sink for data dissemination protocols",
    "Guillaume Chelius & Elyes Ben Hamida",
    "0.1",
    MODELTYPE_APPLICATION
};


/**************************************************************************/
/**************************************************************************/
int fsm(call_t *to, call_t *from, void * arg);
int class_log(call_t *to, call_t *from, void * arg);


/**************************************************************************/
/**************************************************************************/
int init(call_t *to, void *params) {
    struct classdata *classdata = malloc(sizeof(struct classdata));
    char logfile[100] = "gossip-sink.log";
    char logfile_delay[100] = "";
    param_t *param;

    /* init stats */
    classdata->sink = 0;
    classdata->init = 0;
    classdata->d_rx = 0;
    classdata->attempt = -1;
    classdata->success = 0;
    classdata->log_period = 1000000000;
    classdata->last_packet_time = -1;
    classdata->last_packet = 0;


    /* get params */
    list_init_traverse(params);
    while ((param = (param_t *) list_traverse(params)) != NULL) {
        if (!strcmp(param->key, "logfile")) {
            strcpy(logfile, param->value);
        } 
        if (!strcmp(param->key, "logfile-delay")) {
            strcpy(logfile_delay, param->value);
        } 
        if (!strcmp(param->key, "log-period")) {
            if (get_param_time(param->value, &(classdata->log_period))) {
                goto error;
            }
        }
     }
 
    /* open node activity file */
    if ((classdata->logfile = fopen(logfile, "w")) == NULL) {
        fprintf(stderr, "data_d_sink: can not open file %s !\n", logfile);
        goto error;
    }
    
    set_class_private_data(to, classdata);
    return 0;
    
 error:
    free(classdata);
    return -1;
}

int destroy(call_t *to) {
    struct classdata *classdata = get_class_private_data(to);
    double val;
    long time = (long) (get_time() * 0.000001);

    if (classdata->attempt > 0) {
        val = ((double) classdata->success) / ((double) classdata->attempt);
        fprintf(classdata->logfile, "%ld %f %d %d\n", time, val, classdata->success, classdata->attempt);
    }

    if (classdata->logfile) {
        fclose(classdata->logfile);
    }
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
    nodedata->sink = classdata->sink++;
    nodedata->r_start = 0;
    nodedata->r_period = 1000000000;	
    nodedata->r_seq = 0;	
    nodedata->state = IDLE_STATE;	
    nodedata->metadata = 0;	
    nodedata->d_seq = -1;	

    /* parse config file */
    list_init_traverse(params);
    while ((param = (param_t *) list_traverse(params)) != NULL) {
        if (!strcmp(param->key, "request-start")) {
            if (get_param_time(param->value, &(nodedata->r_start))) {
                goto error;
            }
        }
        if (!strcmp(param->key, "request-period")) {
            if (get_param_time(param->value, &(nodedata->r_period))) {
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
    struct classdata *classdata = get_class_private_data(to);
    uint64_t schedule = get_time() + nodedata->r_start + get_random_double() * nodedata->r_period;
    call_t from = {-1, -1};
  
    /* register log function */
    if (classdata->init == 0) {
      scheduler_add_callback(get_time() + classdata->log_period, to, &from, class_log, NULL);
        classdata->init = 1;
    }

    /* scheduler first request */
    scheduler_add_callback(schedule, to, &from, fsm, NULL);
    return 0;
}


/**************************************************************************/
/**************************************************************************/
int fsm(call_t *to, call_t *from, void * arg) {
    struct nodedata *nodedata = get_node_private_data(to);
    struct classdata *classdata = get_class_private_data(to);
    call_t to0 = {get_class_bindings_down(to)->elts[0], to->object};
    packet_t *packet = packet_create(to, sizeof(struct data_d_header) + sizeof(struct sink_adv_p), -1);
    struct data_d_header *header = malloc(sizeof(struct data_d_header));
    field_t *field_header = field_create(INT, sizeof(struct data_d_header), header);
    packet_add_field(packet, "data_d_header", field_header);
    struct sink_adv_p *adv = malloc(sizeof(struct sink_adv_p));
    field_t *field_adv = field_create(DBLE, sizeof(struct sink_adv_p), adv);
    packet_add_field(packet, "sink_adv_p", field_adv);
    destination_t dst = {BROADCAST_ADDR, {-1, -1, -1}};
    
    /* update interval variables */
    nodedata->state = ADV_STATE;
    nodedata->r_seq++;


    /* forge and send request */
    packet->destination=dst;

    header->type = SINK_ADV_TYPE;
    adv->sink = nodedata->sink;
    adv->r_seq = nodedata->r_seq;
    adv->metadata = nodedata->metadata;
    adv->home = BROADCAST_ADDR;
    adv->d_seq = nodedata->d_seq + 1;

    TX(&to0, to, packet);
    if (classdata->d_rx) {
        classdata->success++;
    }
    classdata->attempt++;
    classdata->d_rx = 0;
    
#ifdef LOG_APPLICATION_SINK_REQ
    printf("%lf SINK %d REQ %d\n", (get_time()*0.000001), to->object, adv->d_seq);
#endif

    /* schedule next request */
    scheduler_add_callback(get_time() + nodedata->r_period, to, from, fsm, NULL);
    return 0;
}


/**************************************************************************/
/**************************************************************************/
void rx_home_adv(call_t *to, packet_t *packet) {
    struct nodedata *nodedata = get_node_private_data(to);
    struct home_adv_p *home = (struct home_adv_p *) packet_retrieve_field_value_ptr(packet, "home_adv_p");
    packet_t *packet0;
    destination_t dst = {home->sensor, {-1, -1, -1}};
    call_t to0 = {get_class_bindings_down(to)->elts[0], to->object};

    /* check sink id */
    if (home->sink != nodedata->sink) {
        /* not for us */
        packet_dealloc(packet);
        return;
    }
    
    /* check adv sequence */
    if (home->r_seq != nodedata->r_seq) {
        /* old request */
        packet_dealloc(packet);
        return;
    }

    /* check state */
    if (nodedata->state != ADV_STATE) {
        /* request already sent */
        packet_dealloc(packet);
        return;
    }

    /* send data packet */
    packet0 = packet_create(to, sizeof(struct data_d_header) + sizeof(struct sink_adv_p), -1);
    struct data_d_header *header = malloc(sizeof(struct data_d_header));
    field_t *field_header = field_create(INT, sizeof(struct data_d_header), header);
    packet_add_field(packet0, "data_d_header", field_header);
    struct sink_adv_p *adv = malloc(sizeof(struct sink_adv_p));
    field_t *field_adv = field_create(DBLE, sizeof(struct sink_adv_p), adv);
    packet_add_field(packet0, "sink_adv_p", field_adv);

    packet0->destination=dst;

    header->type = SINK_ADV_TYPE;
    adv->sink = nodedata->sink;
    adv->r_seq = nodedata->r_seq;
    adv->metadata = nodedata->metadata;
    adv->home = home->sensor;
    adv->d_seq = nodedata->d_seq + 1;

#ifdef LOG_APPLICATION_SINK_REQ
    printf("%lf SINK %d REQ %d\n", (get_time()*0.000001), to->object, adv->d_seq);
#endif

    TX(&to0, to, packet0);
    packet_dealloc(packet);

    /* return to idle state */
    nodedata->state = IDLE_STATE;

    return;
}


/**************************************************************************/
/**************************************************************************/
void rx_sensor_data(call_t *to, packet_t *packet) {
    struct nodedata *nodedata = get_node_private_data(to);
    struct classdata *classdata = get_class_private_data(to);
    struct sensor_data_p *data = (struct sensor_data_p *) packet_retrieve_field_value_ptr(packet, "sensor_data_p");

    /* check meta data */
    if (data->metadata != nodedata->metadata) {
        /* bad metadata */
        packet_dealloc(packet);
        return;
    }

    /* check sink id */
    if (data->sink != nodedata->sink) {
        /* not for us */
        packet_dealloc(packet);
        return;
    }
    
    /* check request sequence */
    if (data->r_seq != nodedata->r_seq) {
        /* old request */
        packet_dealloc(packet);
        return;
    }
   
    /* check data sequence */
    if (data->d_seq <= nodedata->d_seq) {
        /* old data */
        packet_dealloc(packet);
        return;
    }
    
    /* packet accepted */
    classdata->d_rx++;
    nodedata->d_seq = data->d_seq;
    nodedata->d_value = data->d_value;

    if (classdata->last_packet_time == -1) {
      classdata->last_packet_time = (get_time()*0.000001);
      classdata->last_packet = 1;
    }
    else {
      classdata->last_packet ++;
      classdata->last_packet_time = (get_time()*0.000001);
    }


#ifdef LOG_APPLICATION_SINK
    printf("%lf SINK %d RX %d\n",(get_time()*0.000001), to->object, data->d_seq);
#endif

    packet_dealloc(packet);
    return;    
}


/**************************************************************************/
/**************************************************************************/
void rx(call_t *to, call_t *from, packet_t *packet) {
    struct data_d_header *header = (struct data_d_header *) packet_retrieve_field_value_ptr(packet, "data_d_header");

    switch (header->type) {
    case SOURCE_ADV_TYPE:
        /* not for us */
        packet_dealloc(packet);
        break;
    case SENSOR_ADV_TYPE:
        /* not for us */
        packet_dealloc(packet);
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
        /* for us */
        rx_sensor_data(to, packet);
        break;
    case HOME_ADV_TYPE:
        /* for us */
        rx_home_adv(to, packet);
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
int class_log(call_t *to, call_t *from, void * arg) {
    struct classdata *classdata = get_class_private_data(to);
    long time = (long) (get_time() * 0.000001);
    double val;

    scheduler_add_callback(get_time() + classdata->log_period, to, from, class_log, NULL);

    if (classdata->attempt <= 0) {
        return 0;
    }
    val = ((double) classdata->success) / ((double) classdata->attempt);
    fprintf(classdata->logfile, "%ld %f %d %d\n", time, val, classdata->success, classdata->attempt);
    
    classdata->success = 0;
    classdata->attempt = 0;
    return 0;
}


/**************************************************************************/
/**************************************************************************/
application_methods_t methods = {rx};
