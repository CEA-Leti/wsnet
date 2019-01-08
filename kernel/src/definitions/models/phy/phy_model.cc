/**
 *  \file   phy_model.cc
 *  \brief  PhyModel Abstract Class implementation
 *  \author Luiz Henrique Suraty Filho
 *  \date   July 2017
 *  \version 1.0
 **/

#include <iostream>
#include <kernel/include/definitions/types.h>
#include <kernel/include/definitions/types/signal/signal.h>
#include <kernel/include/definitions/types/interval/registered_rx_node.h>
#include <kernel/include/definitions/models/signal_tracker/signal_tracker_model.h>
#include <kernel/include/definitions/models/interference/interference_model.h>
#include <kernel/include/definitions/models/modulator/modulator_model.h>
#include <kernel/include/definitions/models/error/error_model.h>
#include <kernel/include/definitions/models/spectrum/spectrum_model.h>
#include <kernel/include/definitions/models/coding/coding_model.h>
#include <kernel/include/definitions/models/phy/phy_model.h>

PhyModellUid PhyModel::uid_counter_ = 0;

PhyModel::PhyModel() : uid_(uid_counter_) {
  ++uid_counter_;
}

PhyModel::~PhyModel() {

}

void PhyModel::ReceiveSignalFromSpectrum(std::shared_ptr<Signal> signal) {
  ReceiveSignalFromSpectrumImpl(signal);
  return;
}

void PhyModel::ReceivePacketFromUp_Deprecated(call_t *to, call_t *from,packet_t *packet) {
  ReceivePacketFromUp_DeprecatedImpl(to,from,packet);
  return;
}

void PhyModel::SendPacketToUp_Deprecated(std::shared_ptr<Signal> signal) {
  SendPacketToUp_DeprecatedImpl(signal);
  return;
}

void PhyModel::SendSignalToSpectrum(std::shared_ptr<Signal> signal) {
  SendSignalToSpectrumImpl(signal);
  return;
}

void PhyModel::RegisterRxNode(SetOfFrequencyIntervals freq_intervals) {
  RegisterRxNodeImpl(freq_intervals);
  return;
}

void PhyModel::UnregisterRxNode() {
  UnregisterRxNodeRxNodeImpl();
  return;
}

void PhyModel::ReceivedSignalFromSpectrumRxEnd(std::shared_ptr<Signal> signal){
  ReceivedSignalFromSpectrumRxEndImpl(signal);
  return;
}

void PhyModel::Print() const {
  PrintImpl();
  return;
}

PhyModellUid PhyModel::GetUID() const {
  return uid_;
}

void PhyModel::SetInterferenceModel(InterferenceModel * new_interference_model){
  interference_model_ = new_interference_model;
}
void PhyModel::SetModulatorModel(ModulatorModel * new_modulator_model){
  modulator_model_ = new_modulator_model;
}
void PhyModel::SetErrorModel(ErrorModel * new_error_model){
  error_model_ = new_error_model;
}

void PhyModel::SetCodingModel(CodingModel * new_coding_model){
  coding_model_ = new_coding_model;
}

void PhyModel::SetSignalTrackerModel(SignalTrackerModel * new_signal_tracker_model){
  signal_tracker_model_ = new_signal_tracker_model;
}

void PhyModel::SetSpectrum(SpectrumModel * new_spectrum_model){
  spectrum_=new_spectrum_model;
}

void PhyModel::PrintImpl() const {
  std::cout << "No Print Method implemented" << std::endl;
}

