/**
 *  \file   multiband_rf_spectrum_model.cc
 *  \brief  MultiBandRFSpectrumModel Class implementation
 *
 *          Current implementation is still a transition between
 *          WSNETv3 to WSNETv4.
 *
 *  \author Luiz Henrique Suraty Filho
 *  \date   July 2017
 *  \version 1.0
 **/


#include <map>
#include <memory>
#include <kernel/include/definitions/models.h>
#include <kernel/include/definitions/class.h>
#include <kernel/include/model_handlers/cxx_model_handlers.h>
#include <kernel/include/definitions/models/spectrum/spectrum_model.h>
#include <kernel/include/definitions/types/signal/rf_signal.h>
#include <kernel/include/definitions/types/waveform/waveform_factory.h>
#include <kernel/include/definitions/types/interval/frequency_interval_waveform.h>
#include <kernel/include/definitions/models/phy/phy_model.h>
#include <kernel/include/definitions/types/signal/signal_factory.h>
#include <kernel/include/data_structures/interval_tree/interval_tree.h>
#include <kernel/include/data_structures/interval_tree/redblack_interval_tree.h>
#include <kernel/include/modelutils.h>
#include "multiband_rf_spectrum_model.h"

uint64_t get_travel_time(auto rf_signal, auto registered_rx_node);

void set_transceiver_to_tx_end(auto rf_signal);


MultiBandRFSpectrumModel::MultiBandRFSpectrumModel(){
  rx_nodes_search_tree_ = std::make_unique<RedBlackIntervalTree>();
  txing_signals_search_tree_ = std::make_unique<RedBlackIntervalTree>();
  range_tree_ = std::make_unique<RangeTree>();
  register_mode_ = 0;
}

MultiBandRFSpectrumModel::MultiBandRFSpectrumModel(std::unique_ptr<IntervalTree> rx_nodes_search_tree,
		std::unique_ptr<IntervalTree> txing_signals_search_tree,
		std::unique_ptr<RangeTree> range_tree,
		RegisterMode register_mode){
	rx_nodes_search_tree_ = std::move(rx_nodes_search_tree);
	txing_signals_search_tree_ = std::move(txing_signals_search_tree);
	range_tree_ = std::move(range_tree);
	register_mode_ = register_mode;
}

std::vector<std::shared_ptr<Signal>> MultiBandRFSpectrumModel::RegisterRxNodeImpl(std::weak_ptr<RegisteredRxNode> rx_node){
	rx_nodes_registered_.insert(std::make_pair (rx_node.lock()->GetNodeID(),rx_node));
	for (auto freq : rx_node.lock()->GetAllFrequencyInterval()){
		rx_nodes_search_tree_->Insert(freq);
	}
	return SearchSignalsForRxNode(rx_node);
}

void MultiBandRFSpectrumModel::UnregisterRxNodeImpl(std::weak_ptr<RegisteredRxNode> rx_node){

	if (rx_nodes_registered_.count(rx_node.lock()->GetNodeID())){
		rx_nodes_registered_.erase(rx_node.lock()->GetNodeID());
		for (auto freq : rx_node.lock()->GetAllFrequencyInterval()){
			rx_nodes_search_tree_->Delete(freq);
		}
	}
}

std::vector<std::shared_ptr<Signal>> MultiBandRFSpectrumModel::SearchSignalsForRxNodeImpl(std::weak_ptr<RegisteredRxNode> rx_node){

	std::vector<std::shared_ptr<Signal>> signals;
	std::map<WaveformUid, std::weak_ptr<Signal>> signals_map;
	std::list<std::weak_ptr<Interval>> query_results;

	for (auto freq : rx_node.lock()->GetAllFrequencyInterval()){
	  auto frequency_intervals = txing_signals_search_tree_->FindAllIntersections(freq);
	  query_results.insert(query_results.end(), frequency_intervals.begin(),frequency_intervals.end());
	}

	// take only one interval_rx_node (even if there are more than one intersections)
	for (auto result : query_results){
	  std::weak_ptr<FrequencyIntervalWaveform> interval_waveform = std::dynamic_pointer_cast<FrequencyIntervalWaveform>(std::shared_ptr<Interval>(result));
	  signals_map.insert(std::make_pair(interval_waveform.lock()->GetWaveform().lock()->GetUID(), interval_waveform.lock()->GetWaveform().lock()->GetSignal()));
	}

	// copy all signals before sending it to the node
	for (auto const &signal : signals_map){
	  // static cast as the filtering for RFSignal was done before
	  auto rf_signal = std::static_pointer_cast<RFSignal>(signal.second.lock()->Clone());
	  auto travel_time = get_travel_time(rf_signal, rx_node.lock());
    rf_signal->SetBegin((Time) rf_signal->GetPacket_Deprecated()->clock0+travel_time);
    rf_signal->SetEnd((Time) rf_signal->GetPacket_Deprecated()->clock1+travel_time);
    rf_signal->SetDestination(rx_node.lock());
    // only rx_end is scheduled as rx_begin already happened
    scheduler_add_rx_signal_end(rf_signal->GetEnd(), this, rf_signal);
	  signals.push_back(rf_signal);
	}

	return signals;

}

void MultiBandRFSpectrumModel::SignalRxEndImpl(std::shared_ptr<Signal> signal) {
  signal->GetDestination()->GetPhyModel()->ReceivedSignalFromSpectrumRxEnd(signal);
}

void MultiBandRFSpectrumModel::SignalRxBeginImpl(std::shared_ptr<Signal> signal) {
	// start the reception
  signal->GetDestination()->GetPhyModel()->ReceiveSignalFromSpectrum(signal);
}

void MultiBandRFSpectrumModel::SearchRxNodesForSignalImpl(std::weak_ptr<Signal> signal){
	std::list<std::weak_ptr<Interval>> query_results;
	std::map<nodeid_t, std::weak_ptr<RegisteredRxNode>> rx_nodes;
	std::weak_ptr<Waveform> waveform = (std::dynamic_pointer_cast<RFSignal>(std::shared_ptr<Signal>(signal))->GetWaveform());

	for (auto freq : waveform.lock()->GetAllFrequencyInterval()){
		auto frequency_intervals = rx_nodes_search_tree_->FindAllIntersections(freq);
		query_results.insert(query_results.end(), frequency_intervals.begin(),frequency_intervals.end());
	}

	// take only one interval_rx_node (even if there are more than one intersections)
	for (auto result : query_results){
		std::weak_ptr<FrequencyIntervalRegisteredRxNode> interval_rx_node = std::dynamic_pointer_cast<FrequencyIntervalRegisteredRxNode>(std::shared_ptr<Interval>(result));
		rx_nodes.insert(std::make_pair(interval_rx_node.lock()->GetRxNode().lock()->GetNodeID(), interval_rx_node.lock()->GetRxNode()));
	}

	// send copies to receiving nodes
	for(auto const &rx_node : rx_nodes){

	  std::shared_ptr<RegisteredRxNode> registered_rx_node(rx_node.second);

		auto rf_signal = std::static_pointer_cast<RFSignal>(signal.lock()->Clone());

		rf_signal->SetDestination(registered_rx_node);

		auto travel_time = get_travel_time(rf_signal, registered_rx_node);

		rf_signal->SetBegin((Time) rf_signal->GetPacket_Deprecated()->clock0+travel_time);
		rf_signal->SetEnd((Time) rf_signal->GetPacket_Deprecated()->clock1+travel_time);

		scheduler_add_rx_signal_begin(rf_signal->GetBegin(), this, rf_signal);

		scheduler_add_rx_signal_end(rf_signal->GetEnd(), this, rf_signal);

	}

	return;
}

// maybe using templates this could be better implemented
// this is to be called when the signal is supposed to be sent to nodes (TXBegin)
void MultiBandRFSpectrumModel::SignalAddTxImpl(std::shared_ptr<Signal> signal){
	auto rf_signal = std::dynamic_pointer_cast<RFSignal>(signal);
	if (rf_signal){
		txing_signals_.insert(std::make_pair (rf_signal->GetUID(),rf_signal));

		scheduler_add_tx_signal_end(rf_signal->GetPacket_Deprecated()->clock1, this, rf_signal);

		for (auto freq : rf_signal->GetWaveform().lock()->GetAllFrequencyInterval()){
			txing_signals_search_tree_->Insert(freq);
		}

		SearchRxNodesForSignal(rf_signal);
	}

	return;
}

// maybe something with template should be done here to be able to receive a signal and delete all
// possible values
// I don't like the static cast as before
// It is called on TXEnd of the original signal
void MultiBandRFSpectrumModel::SignalTxEndImpl(std::weak_ptr<Signal> signal){
	SignallUid signal_id = signal.lock()->GetUID();

	// check if the signal is being transmitted
	if (txing_signals_.count(signal_id)){
		std::shared_ptr<RFSignal> rf_signal = std::static_pointer_cast<RFSignal>(std::shared_ptr<Signal>(signal));
		set_transceiver_to_tx_end(rf_signal);
		for (auto freq : rf_signal->GetWaveform().lock()->GetAllFrequencyInterval()){
			txing_signals_search_tree_->Delete(freq);
		}
		txing_signals_.erase(signal_id);
	}
}

void set_transceiver_to_tx_end(auto rf_signal){
  packet_t *packet = rf_signal->GetPacket_Deprecated();
  call_t     from0  = {-1, packet->node};
  array_t *transceivers_from = get_tranceiver_classesid(&from0);
  call_t from_transceiver = {transceivers_from->elts[0], packet->node};
  class_t *from_transceiver_class = get_class_by_id(transceivers_from->elts[0]);
  from_transceiver_class->methods->transceiver.tx_end(&from_transceiver, NULL, packet);
}

uint64_t get_travel_time(auto rf_signal, auto registered_rx_node){
  packet_t *packet = rf_signal->GetPacket_Deprecated();
  node_t      *node_rx  = get_node_by_id(registered_rx_node->GetNodeID());
  node_t      *node_tx   = get_node_by_id(packet->node);
  call_t     from0  = {-1, packet->node};
  array_t *interfaces_from = get_interface_classesid(&from0);
  call_t from_interface = {interfaces_from->elts[0], packet->node};
  from0.object = registered_rx_node->GetNodeID();
  array_t *interfaces_to = get_interface_classesid(&from0);
  call_t to_interface = {interfaces_to->elts[0], registered_rx_node->GetNodeID()};
  medium_t    *medium = get_medium_by_id(interface_get_medium(&from_interface, &from0));
  double      dist     = distance(&(node_tx->position), &(node_rx->position));
  double      travel_time   = dist / medium->speed_of_light;

  // set the to_interface to be used in the continuation
  rf_signal->SetTo_Deprecated(to_interface);
  rf_signal->SetFrom_Deprecated(from_interface);

  return (uint64_t) travel_time;
}
