/**
 *  \file   models.h
 *  \brief  Models declarations
 *  \author Guillaume Chelius & Elyes Ben Hamida & Quentin Lampin & Loic Lemaitre
 *  \date   2007
 **/

#ifndef __models__
#define  __models__

#include <kernel/include/definitions/types.h>

typedef enum{
  MODELTYPE_UNKNOW,
  MODELTYPE_SHADOWING,
  MODELTYPE_FADING,
  MODELTYPE_INTERFERENCES,
  MODELTYPE_NOISE,
  MODELTYPE_MODULATIONS,
  MODELTYPE_PHYSICAL,
  MODELTYPE_MOBILITY,
  MODELTYPE_INTERFACE,
  MODELTYPE_TRANSCEIVER,
  MODELTYPE_MAC,
  MODELTYPE_ROUTING,
  MODELTYPE_APPLICATION,
  MODELTYPE_PHY,
  MODELTYPE_ERROR,
  MODELTYPE_CODING,
  MODELTYPE_INTERFERENCE,
  MODELTYPE_MODULATOR,
  MODELTYPE_SIGNAL_TRACKER,
  MODELTYPE_ENERGY,
  MODELTYPE_MONITOR,
  MODELTYPE_MAP,
  MODELTYPE_LINK,
  MODELTYPE_GLOBAL_MAP,
  MODELTYPE_SENSOR,
  MODELTYPE_SPECTRUM,
  MODELTYPE_PATHLOSS,
  MODELTYPE_INTERMODULATION
} model_type_t;


/** \typedef model_t
 * \brief Information about a model.
 **/
/** \struct _model
 * \brief Information about a model. Should use type model_t.
 **/
typedef struct _model { /* Model description          */
  char           *oneline;      /* one line description       */
  char           *author;       /* author                     */
  char           *version;      /* version                    */
  model_type_t   type;          /* model type                 */
} model_t;


/**
 * \typedef generic_methods_t
 * \brief Methods that should be implemented by a generic model.
 **/
/**
 * \struct _generic_methods
 * \brief Methods that should be implemented by a generic model. Should use type generic_methods_t.
 **/
typedef struct _generic_methods {
  void (*rx)                   (call_t *to, call_t *from, packet_t *packet);
  void (*tx)                   (call_t *to, call_t *from, packet_t *packet);
  int  (*set_header)           (call_t *to, call_t *from, packet_t *packet, void *dst);
  int  (*get_header_size)      (call_t *to, call_t *from);
  /* edit by Quentin Lampin <quentin.lampin@orange-ftgroup.com> */
  int  (*get_header_real_size) (call_t *to, call_t *from);
  /* end of edition */

  /*edit by Luiz Henrique Suraty Filho */
  int (*push_down)             (call_t *to, call_t *from, packet_t *packet, int packet_type);
  /*end of edition*/
} generic_methods_t;

/**
 * \typedef generic_methods_t
 * \brief Methods that should be implemented by a generic model.
 **/
/**
 * \struct _generic_cpp_methods
 * \brief Methods that should be implemented by a generic model. Should use type generic_methods_t.
 **/
typedef struct _generic_cpp_methods {
  void (*tx)            (void *object, call_t *to, call_t *from, packet_t *packet);
  void (*rx)            (void *object, call_t *to, call_t *from, packet_t *packet);
} generic_cpp_methods_t;

/* Added by Luiz Henrique Suraty Filho*/
/**
 * \struct _generic_methods
 * \brief Methods that should be implemented by a generic model. Should use type generic_methods_t.
 **/
typedef struct _shared_methods {
  void * (*get_shared_data)   (nodeid_t id);
  void * (*get_shared_statistics)   (nodeid_t id);
} shared_methods_t;
/*end of edition*/

/**
 * \typedef mobility_methods_t
 * \brief Methods that should be implemented by a mobility model.
 **/
/**
 * \struct _mobility_methods
 * \brief Methods that should be implemented by a mobility model. Should use type mobility_methods_t.
 **/
typedef struct _mobility_methods {
  void (*update_position) (call_t *to, call_t *from);
  double (*get_speed) (call_t *to);
  angle_t (*get_angle) (call_t *to);
} mobility_methods_t;


/**
 * \typedef monitor_methods_t
 * \brief Methods that should be implemented by a monitor model.
 **/
/**
 * \struct _monitor_methods
 * \brief Methods that should be implemented by a monitor model. Should use type monitor_methods_t.
 **/
typedef struct _monitor_methods {
  void (*fire) (call_t *to, call_t *from, void *data);
} monitor_methods_t;


/**
 * \typedef transceiver_methods_t
 * \brief Methods that should be implemented by a transceiver model.
 **/
/**
 * \struct _transceiver_methods
 * \brief Methods that should be implemented by a transceiver model. Should use type transceiver_methods_t.
 **/
typedef struct _transceiver_methods {
  void      (*rx)                            (call_t *to, call_t *from, packet_t *packet);
  void      (*tx)                            (call_t *to, call_t *from, packet_t *packet);
  /* edit by Quentin Lampin <quentin.lampin@orange-ftgroup.com> */
  int       (*set_header)                    (call_t *to, call_t *from, packet_t *packet, destination_t *dst);
  int       (*get_header_size)               (call_t *to, call_t *from);
  int       (*get_header_real_size)          (call_t *to, call_t *from);
  /* end of edition */
  void      (*tx_end)                        (call_t *to, call_t *from, packet_t *packet);
  void      (*cs)                            (call_t *to, call_t *from, packet_t *packet);
  double    (*get_noise)                     (call_t *to, call_t *from);
  double    (*get_cs)                        (call_t *to, call_t *from);
  double    (*get_power)                     (call_t *to, call_t *from);
  void      (*set_power)                     (call_t *to, call_t *from, double power);
  int       (*get_channel)                   (call_t *to, call_t *from);
  void      (*set_channel)                   (call_t *to, call_t *from, int channel);
  classid_t (*get_modulation)                (call_t *to, call_t *from);
  void      (*set_modulation)                (call_t *to, call_t *from, classid_t modulation);
  uint64_t  (*get_Tb)                        (call_t *to, call_t *from);
  uint64_t  (*get_Ts)                        (call_t *to, call_t *from);
  void      (*set_Ts)                        (call_t *to, call_t *from, uint64_t Ts);
  double    (*get_sensibility)               (call_t *to, call_t *from);
  void      (*set_sensibility)               (call_t *to, call_t *from, double sensibility);
  void      (*sleep)                         (call_t *to, call_t *from);
  void      (*wakeup)                        (call_t *to, call_t *from);
  void      (*switch_rx)                     (call_t *to, call_t *from);
  void      (*switch_idle)                   (call_t *to, call_t *from);
  int       (*get_modulation_bit_per_symbol) (call_t *to, call_t *from);
  int       (*get_modulation_type) 			 (call_t *to, call_t *from);
  void      (*set_modulation_type) 			 (call_t *to, call_t *from, int modulation_type);
  /* edit by Rida El Chall */
  uint64_t  (*get_freq_low)					 (call_t *to, call_t *from);
  uint64_t  (*get_freq_high)				 (call_t *to, call_t *from);
  void      (*set_freq_low)					 (call_t *to, call_t *from, uint64_t freq_low);
  void      (*set_freq_high)				 (call_t *to, call_t *from, uint64_t freq_high);
  /* end of edition */

} transceiver_methods_t;


/**
 * \typedef energy_methods_t
 * \brief Methods that should be implemented by an energy model.
 **/
/**
 * \struct _energy_methods
 * \brief Methods that should be implemented by an energy model. Should use type energy_methods_t.
 **/
typedef struct _energy_methods {
    void (*recharge) (call_t *to, double energy);
    void (*consume) (call_t *to, double current, uint64_t duration);
    double (*energy_consumed) (call_t *to);
    double (*energy_recharged) (call_t *to);
    double (*energy_remaining) (call_t *to);
    double (*energy_status) (call_t *to);
    double (*get_supply_voltage) (call_t *to);
} energy_methods_t;



/**
 * \typedef interface_methods_t
 * \brief Methods that should be implemented by an interface model (antenna...).
 **/
/**
 * \struct _interface_methods
 * \brief Methods that should be implemented by an interface model. Should use type interface_methods_t.
 **/
typedef struct _interface_methods {
  void       (*rx)        (call_t *to, call_t *from, packet_t *packet);
  void       (*tx)        (call_t *to, call_t *from, packet_t *packet);
  void       (*cs)        (call_t *to, call_t *from, packet_t *packet);
  double     (*get_loss)  (call_t *to, call_t *from);
  angle_t  * (*get_angle) (call_t *to, call_t *from);
  void       (*set_angle) (call_t *to, call_t *from, angle_t *angle);
  double     (*gain_tx)   (call_t *to, call_t *from, position_t *pos);
  double     (*gain_rx)   (call_t *to, call_t *from, position_t *pos);
  mediumid_t (*get_medium)(call_t *to, call_t *from);
  interface_type_t		 (*get_type)  (call_t *to);
} interface_methods_t;


/**
 * \typedef fading_methods_t
 * \brief Methods that should be implemented by a fading model.
 **/
/**
 * \struct _fading_methods
 * \brief Methods that should be implemented by a fading model. Should use type fading_methods_t.
 **/
typedef struct _fading_methods {
  double (*fading) (call_t *to_fading, call_t *to_interface, call_t *from_interface, packet_t *packet, double rxdBm);
} fading_methods_t;


/**
 * \typedef shadowing_methods_t
 * \brief Methods that should be implemented by a shadowing model.
 **/
/**
 * \struct _shadowing_methods
 * \brief Methods that should be implemented by a shadowing model. Should use type shadowing_methods_t.
 **/
typedef struct _shadowing_methods {
  double (*shadowing) (call_t *to_shadowing, call_t *to_interface, call_t *from_interface, packet_t *packet, double rxdBm);
} shadowing_methods_t;


/**
 * \typedef pathloss_methods_t
 * \brief Methods that should be implemented by a pathloss model.
 **/
/**
 * \struct _pathloss_methods
 * \brief Methods that should be implemented by a pathloss model. Should use type pathloss_methods_t.
 **/
typedef struct _pathloss_methods {
  double (*pathloss) (call_t *to_pathloss, call_t *to_interface, call_t *from_interface, packet_t *packet, double rxdBm);
} pathloss_methods_t;


/**
 * \typedef noise_methods_t
 * \brief Methods that should be implemented by a noise model.
 **/
/**
 * \struct _noise_methods
 * \brief Methods that should be implemented by a noise model. Should use type noise_methods_t.
 **/
typedef struct _noise_methods {
  double (* noise) (call_t *to, call_t *from, nodeid_t node, int channel);
} noise_methods_t;


/**
 * \typedef interferences_methods_t
 * \brief Methods that should be implemented by a interferences model.
 **/
/**
 * \struct _interferences_methods
 * \brief Methods that should be implemented by a interferences model. Should use type interferences_methods_t.
 **/
typedef struct _interferences_methods {
  double (* interfere) (call_t *to, call_t *from, int channel0, int channel1);
} interferences_methods_t;

/**
 * \typedef intermodulation_methods_t
 * \brief Methods that should be implemented by a intermodulation model.
 **/
/**
 * \struct _intermodulation_methods
 * \brief Methods that should be implemented by a intermodulation model. Should use type intermodulation_methods_t.
 **/
typedef struct _intermodulation_methods {
  double (* intermod) (call_t *to, call_t *from, int channel0, int channel1, int channel2);
} intermodulation_methods_t;




/**
 * \typedef modulation_methods_t
 * \brief Methods that should be implemented by a modulation model.
 **/
/**
 * \struct _modulation_methods
 * \brief Methods that should be implemented by a modulation model. Should use type modulation_methods_t.
 **/
typedef struct _modulation_methods {
  double (* modulate) (call_t *to, call_t *from, double snr);
  int (* bit_per_symbol) (call_t *to, call_t *from);
  int  (*get_modulation_type) (call_t *to, call_t *from);
  void    (*set_modulation_type) (call_t *to, call_t *from, int modulation_type);
} modulation_methods_t;


/**
 * \typedef mac_methods_t
 * \brief Methods that should be implemented by a mac model.
 **/
/**
 * \struct _mac_methods
 * \brief Methods that should be implemented by a mac model. Should use type mac_methods_t.
 **/
typedef struct _mac_methods {
  void (*rx) (call_t *to, call_t *from, packet_t *packet);
  void (*tx) (call_t *to, call_t *from, packet_t *packet);
  int (*set_header) (call_t *to, call_t *from, packet_t *packet, destination_t *dst);
  int (*get_header_size) (call_t *to, call_t *from);
  /* edit by Quentin Lampin <quentin.lampin@orange-ftgroup.com> */
  int (*get_header_real_size)(call_t *to, call_t *from);
  /* end of edition */
} mac_methods_t;


/**
 * \typedef routing_methods_t
 * \brief Methods that should be implemented by a routing model.
 **/
/**
 * \struct _routing_methods
 * \brief Methods that should be implemented by a routing model. Should use type routing_methods_t.
 **/
typedef struct _routing_methods {
  void (*rx) (call_t *to, call_t *from, packet_t *packet);
  void (*tx) (call_t *to, call_t *from, packet_t *packet);
  int (*set_header) (call_t *to, call_t *from, packet_t *packet, destination_t *dst);
  int (*get_header_size) (call_t *to, call_t *from);
  /* edit by Quentin Lampin <quentin.lampin@orange-ftgroup.com> */
  int (*get_header_real_size)(call_t *to, call_t *from);
  /* end of edition */
} routing_methods_t;


/**
 * \typedef application_methods_t
 * \brief Methods that should be implemented by an application model.
 **/
/**
 * \struct _application_methods
 * \brief Methods that should be implemented by an application model. Should use type application_methods_t.
 **/
typedef struct _application_methods {
  void (*rx) (call_t *to, call_t *from, packet_t *packet);
} application_methods_t;


/**
 * \typedef global_map_methods_t
 * \brief Methods that should be implemented by a global map model.
 **/
/**
 * \struct _global_map_methods
 * \brief Methods that should be implemented by a global map model. Should use type global_map_methods_t.
 **/
typedef struct _global_map_methods {
  position_t * (*get_global_area) (call_t *to, call_t *from);
} global_map_methods_t;


/**
 * \typedef map_methods_t
 * \brief Methods that should be implemented by a map model.
 **/
/**
 * \struct _map_methods
 * \brief Methods that should be implemented by a map model. Should use type map_methods_t.
 **/
typedef struct _map_methods {
    map_conditions_t (*get_scenario_condition) (call_t *to, call_t *from, nodeid_t src, nodeid_t dst);
    int (*get_nbr_walls) (call_t *to, call_t *from, nodeid_t src, nodeid_t dst);
} map_methods_t;

/**
 * \typedef map_methods_t
 * \brief Methods that should be implemented by a map model.
 **/
/**
 * \struct _map_methods
 * \brief Methods that should be implemented by a map model. Should use type map_methods_t.
 **/
typedef struct _link_methods {
  link_condition_t (*get_link_condition) (call_t *to_link, call_t *to_interface, call_t *from_interface);
	link_type_t (*get_link_type) (call_t *to_link, call_t *to_interface, call_t *from_interface);
	link_communication_type_t (*get_communication_type) (call_t *to_link, call_t *to_interface, call_t *from_interface);
	double (*get_mutual_orientation) (call_t *to_link, call_t *to_interface, call_t *from_interface);
	int (*get_complementary_link_condition) (call_t *to_link, call_t *to_interface, call_t *from_interface);
} link_methods_t;

/**
 * \typedef phy_methods_t
 * \brief Methods that should be implemented by a PhyModel.
 **/
/**
 * \struct _phy_methods
 * \brief Methods that should be implemented by a PhyModel. Should use type phy_methods_t.
 **/
typedef struct _phy_methods {
     void (*tx) (void *created_object, call_t *to, call_t *from, packet_t *packet);
} phy_methods_t;


#ifdef __cplusplus
extern "C"{
#endif


/**
 * \brief Give model type name from type number.
 * \param type the type number.
 * \return Return a char* on the model type string.
 **/
char* model_type_to_str(int type);


#ifdef __cplusplus
}
#endif

#endif // __models__
