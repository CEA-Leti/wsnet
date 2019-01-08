/**
 *  \file   pick_first_signal_tracker_model.h
 *  \brief  PickFirstSignalTrackerModel Concrete Class definition
 *  \author Luiz Henrique Suraty Filho
 *  \date   July 2017
 *  \version 1.0
 **/

#include <iostream>
#include <memory>
#include <kernel/include/definitions/types.h>
#include <kernel/include/definitions/models.h>
#include <kernel/include/definitions/models/signal_tracker/signal_tracker_model.h>
#include <kernel/include/definitions/types/signal/rf_signal.h>
#include <kernel/include/model_handlers/cxx_model_handlers.h>
#include <kernel/include/definitions/class.h>
#include <kernel/include/scheduler/scheduler.h>
#include "pick_first_rf_signal_tracker_model.h"

/** \brief A structure to define the simulation module information
 *  \struct model_t
 **/
model_t model =  {
    (char*) "Pick First RF Signal Tracker Model ",
    (char*)"Luiz Henrique SURATY FILHO",
    (char*)"0.1",
    MODELTYPE_SIGNAL_TRACKER
};

// This ends up being the Factory method.
void *create_object(call_t *to, void *params) {
  //std::shared_ptr<DefaultPhyModel> new_default_phy = std::make_shared<DefaultPhyModel>();
  //default_phy_model *p = TO_C(new_default_phy.get());
  if (!params)
    std::cout<<"NULL params for "<<to->classid<<std::endl;
  void *p = TO_C(new PickFirstSignalTrackerModel);
  return p;
}

void destroy_object(void *object) {
  delete TO_CPP(object,PickFirstSignalTrackerModel);
}

int bootstrap(call_t *to) {
  class_t *my_c = get_class_by_id(to->classid);
  if (!my_c)
      std::cout<<"NULL object for "<<to->classid<<std::endl;
  return 0;
}

PickFirstSignalTrackerModel::PickFirstSignalTrackerModel() : freq_center_(0){
}

PickFirstSignalTrackerModel::PickFirstSignalTrackerModel(Frequency freq) : freq_center_(freq){
}

PickFirstSignalTrackerModel::~PickFirstSignalTrackerModel(){

}

std::shared_ptr<Signal> PickFirstSignalTrackerModel::GetSelectedSignalImpl(){
	auto selected_signal = std::shared_ptr<Signal>(current_signal_);
	ResetSelectedSignalImpl();
	return selected_signal;
}

void PickFirstSignalTrackerModel::ResetSelectedSignalImpl(){
  current_signal_.reset();
}

bool PickFirstSignalTrackerModel::VerifySignalIsSelectedImpl(std::shared_ptr<Signal> signal) {
  if (current_signal_.lock() == signal){
    return true;
  }
  else {
    return false;
  }
}

bool PickFirstSignalTrackerModel::ReceiveSignalImpl(std::shared_ptr<Signal> new_signal){
  // If there is already a signal we return, as we select the first arrived
  if (current_signal_.lock() != nullptr){
    return false;
  }
  auto rf_signal = std::dynamic_pointer_cast<RFSignal>(new_signal);
  // if it is a RFSignal
  // there is no need to filter for the bandwidth as this is done by the phy Rx registration
  if (rf_signal){

    // If we received after the RX_BEGIN we return, as we select the first arrived if we were listening at that time (of RX_BEGIN)
    // This is a special case when the node does RegisterOnSpectrum after the beginning of a(ll) signal(s) it would receive if it was on RX at the time of the transmission
    // In this case, the ReceiveSignal would never be called by Spectrum as this node was not Rxing during that time (of RX_BEGIN)
    // However, it should be called when the node registers on the spectrum, to inform the phy of a(ll) signal(s) in the air that started before the node registering
    // for RX on spectrum and are still being transmitted so that PHY can properly calculate the interferences
    auto travel_time = rf_signal->GetEnd() - rf_signal->GetPacket_Deprecated()->clock1;
    if ( get_time() > rf_signal->GetBegin() + travel_time ) {
      return false;
    }
    current_signal_ = new_signal;
    return true;
  }

  return false;
}

void PickFirstSignalTrackerModel::PrintImpl() const {
	std::cout << "The Pick First Signal Tracker Model" << std::endl;
}
