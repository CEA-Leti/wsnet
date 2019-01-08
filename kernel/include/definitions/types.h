/**
 *  \file   types.h
 *  \brief  Type declarations
 *  \author Guillaume Chelius & Elyes Ben Hamida & Quentin Lampin
 *  \date   2007
 **/
#ifndef WSNET_KERNEL_DEFINITIONS_TYPES_H_
#define WSNET_KERNEL_DEFINITIONS_TYPES_H_

#ifdef __cplusplus

#ifdef __APPLE__
#include <sys/types.h>
#endif
#include <sys/types.h>
#include <stdint.h>
#include <memory>
using ErrorModelUid = uint;
using PhyModellUid = uint;
using SignallUid = uint;
using WaveformUid = uint;
using SpectrumUid = uint;
using IntervalUid = uint;
using CodingModelUid = uint;
using InterferenceModelUid = uint;
using SignalTrackerModelUid = uint;
using ModulatorModelUid = uint;
using EventUid = uid_t;

using IntervalBoundary = uint64_t;
using Frequency = IntervalBoundary;
using PSDValue = double;

using RegisterMode = uint;



using SpreadingFactor = uint;
using Time = uint64_t;
#else //__cplusplus

#include <stdlib.h>
#include <inttypes.h>
#include <math.h>
#include <stdbool.h>

typedef struct _shared_ptr_fake {
    void *ptr_1;
    void *ptr_2;
}shared_ptr_fake_t;

#endif //__cplusplus
#include <math.h>
#include <kernel/include/definitions/field.h>
#include <kernel/include/data_structures/hashtable/hashtable.h>
#include <kernel/include/data_structures/list/list.h>
#include <kernel/include/data_structures/circular_array/circ_array.h>
#include <kernel/include/data_structures/sliding_window/sliding_window.h>
/** \typedef link_type_t
 *  \brief Link type identifier
 **/
typedef int link_type_t;

/** \typedef link_conditions_t
 *  \brief An enum with all available link conditions
 **/
typedef enum{
  LINK_CONDITION_LOS,         // Line of Sight
  LINK_CONDITION_NLOS,        // Non-Line of Sight
  LINK_CONDITION_NLOS2,       // Non-Line of Sight 2
  LINK_CONDITION_NLOS3        // Non-Line of Sight 3
} link_condition_t;

/** \typedef link_communication_type_t
 *  \brief An enum with all available link communication type
 **/
typedef enum{
  LINK_COMMUNICATION_TYPE_15_4,   // Communication type of normal nodes running on IEEE 802.15.4
  LINK_COMMUNICATION_TYPE_WIFI,   // Communication type of normal nodes running on IEEE 802.11
  LINK_COMMUNICATION_TYPE_BAN,    // Communication type of nodes running inside a Body Area Network (IEEE 802.15.4 or IEEE 802.15.6)
  LINK_COMMUNICATION_TYPE_B2B     // Communication type of Body to Body Network (IEEE 802.15.4 or IEEE 802.15.6)
} link_communication_type_t;

/** \typedef interface_type_t
 *  \brief An enum with all available interface types
 **/
typedef enum{
  INTERFACE_TYPE_ANTENNA_TOP_LOADED,
  INTERFACE_TYPE_ANTENNA_ALVA,
  INTERFACE_TYPE_ANTENNA_MONOPOLE,
  INTERFACE_TYPE_ANTENNA_OMNIDIRECTIONAL,
  INTERFACE_TYPE_INFRARED_TRANSMITTER,
  INTERFACE_TYPE_VISIBLE_LIGHT_TRANSMITTER
} interface_type_t;

/** \typedef interface_type_t
 *  \brief An enum with all available map scenario conditions
 **/
typedef enum{
  MAP_SAME_ROOM,          // Nodes are in SAME_ROOM
  MAP_ADJACENT_ROOM,      // Nodes are in ADJACENT_ROOM
  MAP_OTHER_ROOM,         // Nodes are in OTHER_ROOM
  MAP_FAR_AWAY_ROOM       // Nodes are in FAR_AWAY_ROOM
} map_conditions_t;

/** \typedef array_t
 * \brief An array of integers containing its size.
 **/
/** \struct _array 
 * \brief An array of integers containing its size. Should use type array_t.
 **/
typedef struct _array {
  int size;  /**< array size **/
  int *elts; /**< array elements **/
} array_t;


/** \typedef classid_t 
 * \brief A class identifier. 
 **/
typedef int classid_t;

/** \typedef mediumid_t 
 * \brief A medium identifier. 
 **/
typedef int mediumid_t;

/** \typedef environmentid_t 
 * \brief A environment identifier. 
 **/
typedef int environmentid_t;

/** \typedef nodearchid_t 
 * \brief A node architecture identifier. 
 **/
typedef int nodearchid_t;

/** \typedef nodeid_t 
 * \brief A node identifier. 
 **/
typedef int nodeid_t;

/** \typedef grouparchid_t
 * \brief A node group identifier.
 **/
typedef int grouparchid_t;

/** \typedef groupid_t
 * \brief A group identifier.
 **/
typedef int groupid_t;

/** \typedef packetid_t 
 * \brief A packet identifier. 
 **/
typedef int packetid_t;

/** \typedef objectid_t 
 * \brief A node, or medium, or environment identifier. 
 **/
typedef int objectid_t;


/** \typedef call_t
 * \brief A parameter that identifies who we are calling or who has called us. Kind of a self pointer.
 **/
/** \struct _call
 * \brief A parameter that identifies who we are calling or who has called us. Kind of a self pointer. Should use type call_t.
 **/
#ifdef __cplusplus
// The var name "class" cannot exist on the c++ world.
typedef struct _call {
  classid_t     classid;  /**< the called class id **/
  objectid_t    object; /**< the called object id (node, medium, or environment) **/
} call_t;
#else //__cplusplus
typedef struct _call {
  classid_t     class;  /**< the called class id **/
  objectid_t    object; /**< the called object id (node, medium, or environment) **/
} call_t;

#endif //__cplusplus

/** \typedef position_t
 * \brief A position in the 3D space. 
 **/
/** \struct _position
 * \brief A position in the 3D space. Should use type position_t.
 **/
typedef struct _position {
  double x; /**< x position **/
  double y; /**< y position **/ 
  double z; /**< z position **/ 
} position_t;



/** \typedef destination_t
 * \brief A packet destination. May be a node address or a geographical position.
 **/
/** \struct _destination
 * \brief A packet destination. May be a node address or a geographical position. Should use type destination_t.
 **/
typedef struct _destination {
    nodeid_t id;         /**< the destination node id **/
    position_t position; /**< the destination position **/
} destination_t;


/** \typedef packet_t
 * \brief A radio packet.
 **/
/** \struct _packet
 * \brief A radio packet. Should use type packet_t.
 **/
typedef struct _packet {
  packetid_t id;         /**< the packet id **/
  int size;              /**< size of the packet data **/
  int real_size;         /**< real size of the packet data **/
  int type;              /**< type of the packet (for multistandard nodes) **/

  uint64_t clock0;       /**< packet rx start **/
  uint64_t clock1;       /**< packet rx end **/
  uint64_t duration;     /**< packet tx/rx duration **/

  nodeid_t node;         /**< node that has created the packet **/
  classid_t interface;   /**< interface that has emitted the packet **/

  double txdBm;          /**< tx power in dBm **/
  int channel;          
  classid_t modulation;  /**< modulation class **/
  uint64_t Tb;           /**< radio bandwidth: time to send a bit **/
  destination_t destination; /**< destination for encapsulation in lower layer **/

  /* edit by Luiz Henrique Suraty Filho*/
  int frame_type;    /**<* type of access that is desired by layer (0 - S-CSMA, 1 - GTS, 2 - S-ALOHA)*/
  /* end of edition */

   /* edit by Mickael Maman*/
  int frame_control_type;    /**<* type of frame shared between NTW and MAC layers (0 Normal, 1 control without ACK) */
  /* end of edition */ 
  
  double PER;            /**< packet error rate **/
  double rxdBm;          /**< rx power in dBm **/
  double rxmW;           /**< rx power in mW **/
  double *noise_mW;      /**< packet noise in mW **/
  double *ber;           /**< packet ber **/
  double RSSI;           /**< RSSI indicator in dBm **/

  /* edit by Luiz Henrique Suraty Filho*/
  double LQI;    /**<* LQI indicator (value between 0(Bad Link quality) and 1(Good Link Quality)) */
  /* end of edition */

  /*edit Rida el chall */
  double SINR;          /* Define the wideband SINR Value of the packet in linear */
  /*end of edition */

  hashtable_t *fields;   /**< packet data **/

  // pointer to the signal that contain the packet.
  // we are using this as temporary measure to have backward compatibility
  void *signal;
} packet_t;


/** \typedef angle_t
 * \brief An angle in the 3D space.
 **/
/** \struct _angle
 * \brief An angle in the 3D space. Should use type angle_t.
 **/
typedef struct _angle {
  double xy; /**< angle on the xy plane **/ 
  double z;  /**< angle between the xy plane and the z axis**/ 
} angle_t;


/** \typedef param_t
 * \brief A parameter for the "init" and "setnode" class functions.
 **/
/** \struct _param
 * \brief A parameter for the "init" and "setnode" class functions. Should use type param_t.
 **/
typedef struct _param {
  char *key;   /**< the parameter key **/
  char *value; /**< the parameter value **/
} param_t;



/** \typedef callback_t 
 * \brief Prototype of a callback function. 
 **/
typedef int (* callback_t) (call_t *to, call_t *from, void *arg);


#endif //WSNET_KERNEL_DEFINITIONS_TYPES_H_
