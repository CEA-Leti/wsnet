/**
 *  \file   interference_model.h
 *  \brief  Interference Model Abstract Class definition
 *  \author Luiz Henrique Suraty Filho
 *  \date   July 2017
 *  \version 1.0
 **/

#include <iostream>
#include <kernel/include/definitions/types.h>
#include <kernel/include/definitions/types/signal/signal.h>
#include <kernel/include/definitions/models/interference/interference_model.h>

InterferenceModelUid InterferenceModel::uid_counter_ = 0;

InterferenceModel::InterferenceModel() : uid_(uid_counter_) {
  ++uid_counter_;
}

InterferenceModel::~InterferenceModel() {}

void InterferenceModel::ApplyInterference(std::shared_ptr<Signal> tracked_signal, MapOfSignals interferers) {
  ApplyInterferenceImpl(tracked_signal,interferers);
  return;
}

void InterferenceModel::Print() const {
  PrintImpl();
}

InterferenceModelUid InterferenceModel::GetUID() const {
  return uid_;
}

void InterferenceModel::PrintImpl() const {
  std::cout << "No Print Method implemented" << std::endl;
}
