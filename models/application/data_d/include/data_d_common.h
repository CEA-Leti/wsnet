/**
 *  \file   data_d_common.h
 *  \brief  Data dissemination protocol independant declarations
 *  \author Guillaume Chelius & Elyes Ben Hamida
 *  \date   2007
 **/
#ifndef __data_d_common__
#define __data_d_common__


/* ************************************************** */
/* ************************************************** */
#define MAX_METADATA 16
#define MAX_SINK     16
#define MAX_SOURCE   16


/* ************************************************** */
/* ************************************************** */
#define SOURCE_ADV_TYPE  0
#define SENSOR_ADV_TYPE  1
#define SOURCE_DATA_TYPE 2
#define SINK_ADV_TYPE    3
#define SENSOR_DATA_TYPE 4
#define HOME_ADV_TYPE    5


/* ************************************************** */
/* ************************************************** */
struct data_d_header {
    int type;
};

struct source_adv_p {
    int source; // unique source id (unique among sources, different from node identity)
    int s_seq;  // sequence number for this (adv, source)
};

struct sensor_adv_p {
    nodeid_t sensor; // sensor responding (equal to node identity)
    int source;      // source to which we respond
    int s_seq;       // sequence number of the source request to which we are responding
};

struct source_data_p {
    int source;    // source id producing the data
    int sensor;    // sensor to which the data is sent
    int metadata;  // metadata of data being produced
    int d_seq;     // data sequence number (unique for this metadata among all sources)
    int d_value;   // data value
};

struct sink_adv_p {
    int sink;     // unique sink id (unique among sinks, different from node identity)
    int r_seq;    // sequence number for this (request, sink)
    int metadata; // metadata of data being requested
    int home;     // home node sensor to which we are requesting tge data (-1 if we want to start the election or in a gossip protocol, sensor id otherwise)
    int d_seq;    // data sequence number threshold
};

struct sensor_data_p {
    int type;      // packet type SENSOR_DATA_TYPE
    int metadata;  // metadata of data being sent to sink
    int sink;      // sink that has requested
    int r_seq;     // sink request sequence number
    int source;    // source id that has produced the data
    int d_seq;     // data sequence number (unique for this metadata among all sources)
    int d_value;   // data value
  int hop;
  double delay;
};

struct home_adv_p {
    nodeid_t sensor; // sensor proposing itself as a home node (equal to node identity)
    int sink;        // sink to which we respond
    int r_seq;       // sequence number of the sink request to which we are responding
};


#endif //__data_d_common__
