/**
 *  \file   modelutils.h
 *  \brief  Utility function declarations
 *  \author Luiz Henrique Suraty Filho & Guillaume Chelius & Elyes Ben Hamida & Quentin Lampin & Loic Lemaitre
 *  \date   2017
 **/
#ifndef WSNET_CORE_MODEL_UTILS_H_
#define WSNET_CORE_MODEL_UTILS_H_

#ifdef __cplusplus
#include <kernel/include/model_handlers/cxx_model_handlers.h>
#include <kernel/include/definitions/types/waveform/waveform.h>
#include <kernel/include/definitions/types/waveform/waveform_factory.h>
#else

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <float.h>
#include <limits.h>
#include <math.h>

#endif //__cplusplus

#include <kernel/include/options.h>
#include <kernel/include/log.h>

#include <kernel/include/data_structures/mem_fs/mem_fs.h>
#include <kernel/include/data_structures/list/list.h>
#include <kernel/include/data_structures/circular_array/circ_array.h>
#include <kernel/include/data_structures/hashtable/hashtable.h>
#include <kernel/include/data_structures/sliding_window/sliding_window.h>

#include <kernel/include/tools/math/rng/rng.h>

#include <kernel/include/definitions/types.h>
#include <kernel/include/definitions/models.h>
#include <kernel/include/definitions/class.h>
#include <kernel/include/definitions/medium.h>
#include <kernel/include/definitions/environment.h>
#include <kernel/include/definitions/nodearch.h>
#include <kernel/include/definitions/node.h>
#include <kernel/include/definitions/grouparch.h>
#include <kernel/include/definitions/group.h>
#include <kernel/include/definitions/packet.h>

#include <kernel/include/configuration_parser/param.h>

#include <kernel/include/scheduler/scheduler.h>

#include <kernel/include/model_handlers/modulation.h>
#include <kernel/include/model_handlers/node_mobility.h>
#include <kernel/include/model_handlers/media_rxtx.h>
#include <kernel/include/model_handlers/transceiver.h>
#include <kernel/include/model_handlers/topology.h>
#include <kernel/include/model_handlers/monitor.h>
#include <kernel/include/model_handlers/energy.h>
#include <kernel/include/model_handlers/interface.h>
#include <kernel/include/model_handlers/noise.h>
#include <kernel/include/model_handlers/map.h>
#include <kernel/include/model_handlers/link.h>

#include <libraries/timer/timer.h>

#include <models/models_dbg.h>

/*TODO: THIS SHOUD GO TO A PROPER FILE (e.g. print_options.h)*/
/* Macro definitions for colors */

#define KNRM  	"\x1B[0m"
#define KRED  	"\x1B[31;1m"
#define KGRN  	"\x1B[32;1m"
#define KYEL 	"\x1B[33;1m"
#define KBLU  	"\x1B[34;1m"
#define KMAG  	"\x1B[35;1m"
#define KCYN  	"\x1B[36;1m"
#define KWHT  	"\x1B[37;1m"
#define KRESET 	"\033[0m"

/*TODO: THIS SHOUD GO TO A PROPER FILE (e.g. statistics.h)*/
/** \brief A structure containing a shared entry
 *  \struct _shared_statistics_data
 **/
typedef struct _shared_statistics_data{
    /* Shared statistics variables at the node */
  double    **nbr_tx;     /*!< Defines the total number of transmitted packets (nbr_x[i=RX][t=TX] from j to i). */
  uint64_t  **init_delay; /*!< Defines the inital delay (nbr_x[i=RX][t=TX] from j to i). */
  uint8_t   *log_status;  /*!< Defines which informations we want the statictics for each node (log_status[i=node_id]).*/

} shared_statistics_data_t;

/*TODO: THIS SHOUD GO TO A PROPER FILE (e.g. time_functions.h)*/
/* Macro definitions for easier time manipulation */
#define MICROSECONDS		1000
#define MILLISECONDS		1000000
#define SECONDS				1000000000

#define TIME_TO_MICROSECONDS(a)	(double) (a)/MICROSECONDS
#define TIME_TO_MILLISECONDS(a)		(double) (a)/MILLISECONDS
#define TIME_TO_SECONDS(a)			(double) (a)/SECONDS

/** \def BROADCAST_ADDR
 * \brief Broadcast address.
 **/
#define BROADCAST_ADDR  -1

/* Macro definitions for managing returns of functions.*/
#define UNSUCCESSFUL   -1
#define SUCCESSFUL      0

#ifdef __cplusplus
extern "C"{
#endif


void TX(call_t *to, call_t *from, packet_t *packet);
void RX(call_t *to, call_t *from, packet_t *packet);
int IOCTL(call_t *to, int option, void *in, void **out);
int SET_HEADER(call_t *to, call_t *from, packet_t *packet, destination_t *dst);
int GET_HEADER_SIZE(call_t *to, call_t *from);

/* edit by Quentin Lampin <quentin.lampin@orange-ftgroup.com> */
int GET_HEADER_REAL_SIZE(call_t *to, call_t *from);
/* end of edition */

#ifdef __cplusplus
}
#endif


#endif // WSNET_CORE_MODEL_UTILS_H_
