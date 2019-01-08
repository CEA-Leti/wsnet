/**
 *  \file   modulator_model.h
 *  \brief  Modulator Model Abstract Class definition
 *  \author Arturo Guizar
 *  \date   February 2018
 *  \version 1.0
 **/

#include <iostream>
#include <kernel/include/definitions/types.h>
#include <kernel/include/definitions/types/signal/signal.h>
#include <kernel/include/definitions/models/modulator/modulator_model.h>

ModulatorModelUid ModulatorModel::uid_counter_ = 0;

ModulatorModel::ModulatorModel() : uid_(uid_counter_) {
  ++uid_counter_;
}

ModulatorModel::~ModulatorModel() {}

void ModulatorModel::ApplyModulation(std::shared_ptr<Signal> tracked_signal) {
  ApplyModulationImpl(tracked_signal);
  return;
}

void ModulatorModel::Print() const {
  PrintImpl();
}

ModulatorModelUid ModulatorModel::GetUID() const {
  return uid_;
}

void ModulatorModel::PrintImpl() const {
  std::cout << "No Print Method implemented" << std::endl;
}
