/**
 *  \file   spectrum_model.cc
 *  \brief  SpectrumModel Abstract Class implementation
 *  \author Luiz Henrique Suraty Filho
 *  \date   July 2017
 *  \version 1.0
 **/


#include <memory>
#include <list>
#include <map>
#include <kernel/include/definitions/types.h>
#include <kernel/include/data_structures/interval_tree/interval_tree.h>
#include <kernel/include/data_structures/range_tree/range_tree.h>
#include <kernel/include/definitions/types/interval/registered_rx_node.h>
#include <kernel/include/definitions/types/signal/signal.h>
#include <kernel/include/definitions/types/interval/frequency_interval.h>
#include <kernel/include/definitions/models/spectrum/spectrum_model.h>

SpectrumUid SpectrumModel::uid_counter_ = 0;

SpectrumModel::SpectrumModel(): uid_(uid_counter_) {
  ++uid_counter_;
}

SpectrumModel::~SpectrumModel() {

}

std::vector<std::shared_ptr<Signal>> SpectrumModel::RegisterRXNode(std::weak_ptr<RegisteredRxNode> rx_node) {
  return RegisterRxNodeImpl(rx_node);
}

void SpectrumModel::UnregisterRXNode(std::weak_ptr<RegisteredRxNode> rx_node) {
  UnregisterRxNodeImpl(rx_node);
}

void SpectrumModel::AddSignalTx(std::shared_ptr<Signal> signal) {
  SignalAddTxImpl(signal);
}

void SpectrumModel::SignalRxBegin(std::shared_ptr<Signal> signal){
  SignalRxBeginImpl(signal);
}

void SpectrumModel::SignalTxEnd(std::weak_ptr<Signal> signal) {
  SignalTxEndImpl(signal);
}

void SpectrumModel::SearchRxNodesForSignal(std::weak_ptr<Signal> signal){
  SearchRxNodesForSignalImpl(signal);
}

std::vector<std::shared_ptr<Signal>> SpectrumModel::SearchSignalsForRxNode(std::weak_ptr<RegisteredRxNode> rx_node) {
  return SearchSignalsForRxNodeImpl(rx_node);
}

void SpectrumModel::SignalRxEnd(std::shared_ptr<Signal> signal) {
  SignalRxEndImpl(signal);
}

SpectrumUid SpectrumModel::GetUID() const {
  return uid_;
}

