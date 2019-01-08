/**
 *  \file   bmac_app_sample.c
 *  \brief  A Test application for BMAC
 *  \author Romain KUNTZ
 *  \date   2009
 **/

#include <stdio.h>
#include <kernel/include/modelutils.h>
#include "bmac.h"

/* ************************************************** */
/* ************************************************** */
model_t model =  {
    "A Test application for BMAC",
    "Romain KUNTZ",
    "0.1",
    MODELTYPE_APPLICATION
};

/* ************************************************** */
/* ************************************************** */
struct _app_data {
  uint64_t start;
  uint64_t stop;
  uint64_t tx_period;
  int random_start;
  int sequence;
  nodeid_t destination;
  position_t position;
  void *timer_id;
};

/* Data Packet Payload */
struct pkt_payload {
  // 6 octets
  uint16_t seq;
  uint16_t dummy1;
  uint16_t dummy2;
};

/* ************************************************** */
/* ************************************************** */
int test_stop_timer(call_t *to, void *timer_id);
int send_msg(call_t *to, call_t *from, void *args); 
int bmac_config(call_t *to);
void tx(call_t *to, call_t *from);

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
    struct _app_data *nodedata = 
                malloc(sizeof(struct _app_data));
    param_t *param;
    
    /* Default values */
    nodedata->start = 0;
    nodedata->stop = 0; 
    nodedata->random_start = 0;    
    nodedata->tx_period = 1000000000;    // 1 sec.
    nodedata->sequence = 0;
    nodedata->destination = BROADCAST_ADDR;

    /* Reading the values from the xml config file */
    list_init_traverse(params);
    while ((param = (param_t *) list_traverse(params)) != NULL) {
        /* Data sending period. 0 means that no data will 
         * ever be sent 
         */
        if (!strcmp(param->key, "tx_period")) {
            if (get_param_time(param->value, &(nodedata->tx_period))) {
                goto error;
            }
        }
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
        /* Whether the start time must be picked up randomly between
         * the start time and (start time + sending period). This 
         * avoids synchronized sending among the nodes 
         */
        if (!strcmp(param->key, "random_start")) {
            if (get_param_integer(param->value, &(nodedata->random_start))) {
                goto error;
            }
        }
        /* Destination (node id) to which the data must be sent */
        if (!strcmp(param->key, "dst")) {
            if (get_param_integer(param->value, &(nodedata->destination))) {
                goto error;
            }
        }
        
    }
   
    if (nodedata->random_start) {
        /* Randomize the start time between nodedata->start and 
         * nodedata->start + nodedata->tx_period
         */
        nodedata->start = get_random_time_range(nodedata->start, 
                nodedata->start + nodedata->tx_period);

        PRINT_APPLICATION("%d - Random start = %"PRId64"us\n", 
                to->object, nodedata->start);
    }

    set_node_private_data(to, nodedata);
    return 0;

 error:
    free(nodedata);
    return -1;
}

int unbind(call_t *to) {
    struct _app_data *nodedata = get_node_private_data(to);
    destroy_timer(nodedata->timer_id);
    free(nodedata);

    return 0;
}

/* ************************************************** */
/* ************************************************** */
int bootstrap(call_t *to) {
    struct _app_data *nodedata = get_node_private_data(to);

    /* Schedule a timer if the node is configured to send data */
    if ((nodedata->stop <= 0 || nodedata->stop > nodedata->start)
         && nodedata->tx_period > 0) {
        /* Start and stop the data transmission relatively 
         * to the node birth
         */
        nodedata->start = get_time() + nodedata->start;
        if (nodedata->stop > 0) {
            nodedata->stop = get_time() + nodedata->stop;
        }

        /* Periodic timer for send_msg() */
        nodedata->timer_id = create_timer(to, send_msg, 
                                test_stop_timer, 
                                periodic_trigger, 
                                &(nodedata->tx_period));

        /* Start timer */
        start_timer(nodedata->timer_id, nodedata->start);
    }

    /* Configure B-MAC from the application layer */
    bmac_config(to);
    return 0;
}

int test_stop_timer(call_t *to, void *timer_id) {
    struct _app_data *nodedata = get_node_private_data(to);

    /* Test whether sending packets should stop or not */
    if (nodedata->stop > 0 && get_time() >= nodedata->stop) {
        /* Stop */
        return 1;
    }

    return 0;
}

int send_msg(call_t *to, call_t *from, void *args) {
    tx(to, from);
    return 0;
}

/* ************************************************** */
/* ************************************************** */
int bmac_config(call_t *to) {
    /* Example code to configure B-MAC from this layer */
    call_t to0;
    uint64_t in = MAC_ENABLED;

    /* Example to enable Link-layer Acknowledgements */
    to0.object = to->object;
    array_t *mac_layer = get_mac_classesid(&to0);
    to0.class = mac_layer->elts[0];
    IOCTL(&to0, MAC_UPDATE_LLACK, &in, 0);

    return 0;
}

/* ************************************************** */
/* ************************************************** */
int ioctl(call_t *to, int option, void *in, void **out) {
    return 0;
}

/* ************************************************** */
/* ************************************************** */
void tx(call_t *to, call_t *from) {
    struct _app_data *nodedata = get_node_private_data(to);
    array_t *down = get_class_bindings_down(to);
    call_t to0   = {down->elts[0], to->object};

    /* Create a data packet */
    packet_t *packet = packet_create(to, sizeof(struct pkt_payload), -1);
    if (packet == NULL) {
        return;
    }

    /* Set the lower layer header */
    destination_t destination = {nodedata->destination, 
                                 {nodedata->position.x, 
                                  nodedata->position.y, 
                                  nodedata->position.z}};
    packet->destination=destination;

    /* Set the payload */
    struct pkt_payload *payload = malloc(sizeof(struct pkt_payload));
    payload->seq = nodedata->sequence++;
    payload->dummy1 = get_random_integer();
    payload->dummy2 = get_random_integer();
    field_t *field_payload = field_create(DBLE, sizeof(struct pkt_payload), payload);
    packet_add_field(packet, "pkt_payload", field_payload);

    /* Send the packet to the lower layer */
    PRINT_APPLICATION("[APP] Time %"PRId64" - Node %d - "
                      "msg tx (seq %d, values %d %d)\n",
                      get_time(), to->object, payload->seq, 
                      payload->dummy1, payload->dummy2);
    TX(&to0, to, packet);
}

/* ************************************************** */
/* ************************************************** */
void rx(call_t *to, call_t *from, packet_t *packet) {
    struct pkt_payload *payload;

    payload = (struct pkt_payload *) packet_retrieve_field_value_ptr(packet, "pkt_payload");

    PRINT_APPLICATION("[APP] Time %"PRId64" - Node %d - "
                      "msg rx (seq %d, values %d %d)\n",
                      get_time(), to->object, payload->seq, 
                      payload->dummy1, payload->dummy2);

    packet_dealloc(packet);
}

/* ************************************************** */
/* ************************************************** */
application_methods_t methods = {rx};

