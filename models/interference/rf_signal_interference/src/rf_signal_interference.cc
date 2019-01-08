/**
 *  \file   rf_signal_interference.cc
 *  \brief  RFSignalInterferenceModel Concrete Class implementation
 *  \author Luiz Henrique Suraty Filho
 *  \date   July 2018
 *  \version 1.0
 **/

#include <iostream>
#include <limits>
#include <kernel/include/modelutils.h>
#include <kernel/include/definitions/types.h>
#include <kernel/include/definitions/models.h>
#include <kernel/include/definitions/class.h>
#include <kernel/include/model_handlers/cxx_model_handlers.h>
#include <kernel/include/model_handlers/transceiver.h>
#include <kernel/include/definitions/types/signal/rf_signal.h>
#include "rf_signal_interference.h"


/** \brief A structure to define the simulation module information
 *  \struct model_t
 **/
model_t model =  {
    (char*) "RF Signal Interference Model",
    (char*)"Luiz Henrique SURATY FILHO",
    (char*)"0.1",
    MODELTYPE_INTERFERENCE
};

// This ends up being the Factory method.
void *create_object(call_t *to, void *params) {
  (void) to;
  (void) params;
  void *p = TO_C(new RFSignalInterferenceModel);
  return p;
}

void destroy_object(void *object) {
  delete TO_CPP(object,RFSignalInterferenceModel);
}

int bootstrap(call_t *to) {
  (void) to;
  return SUCCESSFUL;
}

RFSignalInterferenceModel::RFSignalInterferenceModel(){
}

RFSignalInterferenceModel::~RFSignalInterferenceModel(){
}

// probably put these functions on the core
inline double get_intersection_duration(const std::shared_ptr<Signal> &A, const std::shared_ptr<Signal> &B){
  return (double) std::min(A->GetEnd(),B->GetEnd()) - (double) std::max(A->GetBegin(),B->GetBegin());
}
inline double get_intersection_bandwidth(const std::shared_ptr<FrequencyIntervalWaveform> &A, const std::shared_ptr<FrequencyIntervalWaveform> &B){
  return (double) std::min(A->GetHighPoint(),B->GetHighPoint()) - (double) std::max(A->GetLowPoint(),B->GetLowPoint());
}

void RFSignalInterferenceModel::ApplyInterferenceImpl(std::shared_ptr<Signal> signal, MapOfSignals interferences){
  // if signal is not a RF Signal, we are not able to decode the signal,
  // thus, nothing to be done
  auto desired_signal = std::dynamic_pointer_cast<RFSignal>(signal);
  if (!desired_signal){
    return;
  }
  auto interference_mW = 0.0;
  auto signal_duration = desired_signal->GetEnd() - desired_signal->GetBegin();
  auto signal_bandwidth_Hz = desired_signal->GetBandwidth();
  auto signal_power_mW = 0.0;

  // for each freq interval of the desired signal
  for (auto selected_signal_freq_interval : desired_signal->GetWaveform().lock()->GetAllFrequencyInterval()){
    signal_power_mW += selected_signal_freq_interval->GetPSDValue() * (selected_signal_freq_interval->GetHighPoint() - selected_signal_freq_interval->GetLowPoint());

    // for each interferer
    for(auto interference : interferences){

      // do nothing if interferer is the same signal as the selected desired_signal
      if (interference.second == signal){
        continue;
      }

      // do nothing if it is not a RF signal
      auto interferer_rf_signal = std::dynamic_pointer_cast<RFSignal>(interference.second);
      if (!interferer_rf_signal){
        continue;
      }

      // do nothing if there is no time intersection
      auto intersection_duration = get_intersection_duration(interference.second, signal);
      if (intersection_duration <=0){
        continue;
      }

      // for each freq interval of the interferer
      for (auto interf_freq_interval : interferer_rf_signal->GetWaveform().lock()->GetAllFrequencyInterval()){
        auto intersection_bandwidth = get_intersection_bandwidth(interf_freq_interval, selected_signal_freq_interval);
        // interference contribution on this portion of the signal
        // No need to divide this by total BW, because PSD is mW/HZ
        interference_mW += intersection_bandwidth * selected_signal_freq_interval->GetPSDValue() * (intersection_duration / signal_duration);
      }
    }
  }
  call_t to_interface = {desired_signal->GetTo_Deprecated().classid, desired_signal->GetTo_Deprecated().object};
  auto noise_mW = noise_get_noise(&to_interface, signal_bandwidth_Hz);

  // the SINR will be an average of all (sum) noise uniformly distributed
  // throughout the signal (whole time duration and frequency length)
  // thus, we need to first get the signal power of the whole signal
  // and then, we calculate the SINR
  desired_signal->SetSINR(signal_power_mW/(interference_mW+noise_mW));

  return;
}

void RFSignalInterferenceModel::PrintImpl() const{
  std::cout << "The RFSignal Interference Model" << std::endl;
  return;
}
