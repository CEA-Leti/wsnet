/**
 *  \file   dynamic.h
 *  \brief  A set of macros to update the "dynamic" application parameters
 *  \author Romain KUNTZ 
 *  \date   01/2010
 **/

/* Macros to update the "dynamic" application parameters 
 * from other layers using the IOCTL function. 
 * Example to switch to the time-driven data collection 
 * scheme on node 7 with a 10 sec. period:
 *
 *    call_t c0;
 *    int value = 10000;    // 10000 msec period
 *    c0.node = 7;          // Node id 7
 *    array_t *app_layer = get_application_entities(&c0);
 *    c0.entity = app_layer->elts[0];
 *    IOCTL(&c0, APP_TIME_DRV, &value, 0);
 */

// Time-driven data collection scheme.
// The corresponding value is a period in millisecond. 
// The node will switch to a time-driven collection scheme.
#define APP_TIME_DRV     0

// Event-driven data collection scheme.
// The corresponding value is the threshold value. The node 
// will switch to an event-driven collection scheme and will 
// wait for an ENV_EVENT IOCTL event.
#define APP_EVENT_DRV    1

// Query-driven data collection scheme.
// No corresponding values. The node will switch to a query-driven 
// collection scheme and will send packet upon queries.
#define APP_QUERY_DRV    2

// Change the payload size.
// The corresponding value is the payload size in Bytes.
#define APP_PAYLOAD_SIZE 3

// Trigger an event from the environment. The corresponding 
// value is tested against the node's application threshold, and 
// will send a packet if the value is equal or higher than 
// its threshold.
#define ENV_EVENT        4

// Send a query to a destination. 
// The corresponding value is the destination of the query. 
// The destination that will receive the query will reply a 
// data packet.
#define QUERY_MSG        5

// Change the destination. 
// The corresponding value is the new destination for the 
// data packets. 
#define CHANGE_APP_DST   6
