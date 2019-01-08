/**
 *  \file   signal_tracker_model.h
 *  \brief  Signal Tracker Model Abstract Class definition
 *  \author Luiz Henrique Suraty Filho
 *  \date   July 2017
 *  \version 1.0
 **/

#include <iostream>
#include <kernel/include/definitions/types.h>
#include <kernel/include/definitions/types/signal/signal.h>
#include <kernel/include/definitions/models/signal_tracker/signal_tracker_model.h>

SignalTrackerModelUid SignalTrackerModel::uid_counter_ = 0;

SignalTrackerModel::SignalTrackerModel() : uid_(uid_counter_) {
  ++uid_counter_;
}

SignalTrackerModel::~SignalTrackerModel() {}

std::shared_ptr<Signal> SignalTrackerModel::GetSelectedSignal() {
  return GetSelectedSignalImpl();
}

bool SignalTrackerModel::VerifySignalIsSelected(std::shared_ptr<Signal> signal) {
  return VerifySignalIsSelectedImpl(signal);
}

bool SignalTrackerModel::ReceiveSignal(std::shared_ptr<Signal> signal) {
  return ReceiveSignalImpl(signal);
}

void SignalTrackerModel::ResetSelectedSignal(){
  ResetSelectedSignalImpl();
}

void SignalTrackerModel::Print() const {
  PrintImpl();
}

SignalTrackerModelUid SignalTrackerModel::GetUID() const {
  return uid_;
}

void SignalTrackerModel::PrintImpl() const {
  std::cout << "No Print Method implemented" << std::endl;
}

