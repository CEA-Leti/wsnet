/**
 *  \file   rf_signal_adjacent_band_phy_model_api.cc
 *  \brief  RFSignalAdjacentBandPhyModel Concrete Class API implementation
 *  \author Luiz Henrique Suraty Filho
 *  \date   July 2018
 *  \version 1.0
 **/

#include <iostream>
#include <kernel/include/modelutils.h>
#include <kernel/include/definitions/types/signal/rf_signal.h>
#include <kernel/include/definitions/types/signal/rf_signal_factory.h>
#include "rf_signal_adjacent_band_phy_model.h"

/** \brief A structure to define the simulation module information
 *  \struct model_t
 **/
model_t model =  {
    (char*) "RF Signal Adjacent Band PhyModel",
    (char*)"Luiz Henrique SURATY FILHO",
    (char*)"0.1",
    MODELTYPE_PHY
};

// This ends up being the Factory method.
void *create_object(call_t *to, void *params) {
  Frequency freq_center = 868075000;
  Frequency bandwidth = 100000;
  Frequency delta_freq_adjacent = 1000;
  auto factor_adjacent = 0.0;
  auto nbr_adjacent_bands = 0;
  int log_status = 0;

  if (!params){
    std::cout<<"NULL params for "<<to->classid<<std::endl;
  }

  param_t *param;
  std::string key_str;

  // Get module parameters from the XML configuration file
  list_init_traverse((list_t*) params);
  while ((param = (param_t *) list_traverse((list_t*) params)) != NULL) {
    key_str = param->key;
    if (key_str=="freq_center") {
      if (get_param_uint64_integer(param->value, &(freq_center))) {
        return NULL;
      }
    }
    if (key_str=="delta_freq_adjacent") {
      if (get_param_uint64_integer(param->value, &(delta_freq_adjacent))) {
        return NULL;
      }
    }
    if (key_str=="bandwidth") {
      if (get_param_uint64_integer(param->value, &(bandwidth))) {
        return NULL;
      }
    }
    if (key_str=="factor_adjacent") {
      if (get_param_double(param->value, &(factor_adjacent))) {
        return NULL;
      }
    }
    if (key_str=="number_adjacent_bands") {
      if (get_param_integer(param->value, &(nbr_adjacent_bands))) {
        return NULL;
      }
    }
    if (key_str=="log_status") {
      if (get_param_integer(param->value, &(log_status))) {
        return NULL;
      }
    }
  }
  void * p = TO_C(new RFSignalAdjacentBandPhyModel(freq_center, factor_adjacent, nbr_adjacent_bands, delta_freq_adjacent, bandwidth, log_status, to->object));
  return p;
}

void destroy_object(void *object) {
  delete TO_CPP(object,RFSignalAdjacentBandPhyModel);
}

int bootstrap(call_t *to) {
	RFSignalAdjacentBandPhyModel *my_object = TO_CPP(get_object(to),RFSignalAdjacentBandPhyModel);
  // set all the models connected to the phy
  my_object->SetSignalTrackerModel(TO_CPP(get_first_object_binding_down_by_type(to,MODELTYPE_SIGNAL_TRACKER),SignalTrackerModel));
  my_object->SetErrorModel(TO_CPP(get_first_object_binding_down_by_type(to,MODELTYPE_ERROR),ErrorModel));
  my_object->SetInterferenceModel(TO_CPP(get_first_object_binding_down_by_type(to,MODELTYPE_INTERFERENCE),InterferenceModel));
  my_object->SetCodingModel(TO_CPP(get_first_object_binding_down_by_type(to,MODELTYPE_CODING),CodingModel));

  // set the spectrum
  my_object->SetSpectrum(TO_CPP(medium_get_spectrum_object(to),SpectrumModel));

	return SUCCESSFUL;
}

void tx(void *object, call_t *to, call_t *from, packet_t *packet){
	TO_CPP(object,RFSignalAdjacentBandPhyModel)->ReceivePacketFromUp_Deprecated(to,from,packet);
}

phy_methods_t methods = {tx};
