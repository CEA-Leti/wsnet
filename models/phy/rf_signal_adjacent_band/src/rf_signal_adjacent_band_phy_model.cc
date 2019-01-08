/**
 *  \file   rf_signal_adjacent_band_phy_model.cc
 *  \brief  RFSignalAdjacentBandPhyModel Concrete Class implementation
 *  \author Luiz Henrique Suraty Filho
 *  \date   July 2018
 *  \version 1.0
 **/
#include <iostream>
#include <cmath>

#include "rf_signal_adjacent_band_phy_model.h"


RFSignalAdjacentBandPhyModel::~RFSignalAdjacentBandPhyModel() {
}

RFSignalAdjacentBandPhyModel::RFSignalAdjacentBandPhyModel(Frequency freq_center, double factor_adjacent, uint number_adjacent_bands,
                                                   Frequency delta_adjacent_bands, Frequency bandwidth, int log_status, nodeid_t nodeid) :
                                    freq_center_(freq_center), factor_adjacent_(factor_adjacent), number_adjacent_bands_(number_adjacent_bands), delta_adjacent_bands_(delta_adjacent_bands),
                                     bandwidth_(bandwidth), log_status_(log_status), nodeid_(nodeid){

  frequency_intervals_rx_ = create_setfrequencyintervals(freq_center_,bandwidth_, delta_adjacent_bands_,number_adjacent_bands_);
  frequency_intervals_tx_ = create_setfrequencyintervals(freq_center_,bandwidth_, delta_adjacent_bands_,number_adjacent_bands_);

}

SetOfFrequencyIntervals RFSignalAdjacentBandPhyModel::GetOperatingFrequencyIntervalsTx(){
  return frequency_intervals_tx_;
}

void RFSignalAdjacentBandPhyModel::ReceiveSignalFromSpectrumImpl(std::shared_ptr<Signal> signal) {

  //all received signals interfere
  received_signals_.insert(std::make_pair(signal->GetUID(),signal));

  // update the evaluation_window size
  evaluation_window_ = std::max(evaluation_window_,signal->GetEnd()-signal->GetBegin());

  auto rf_signal = std::dynamic_pointer_cast<RFSignal>(signal);

  if (rf_signal){
    call_t to = {rf_signal->GetTo_Deprecated().classid, rf_signal->GetTo_Deprecated().object};
    call_t from = {rf_signal->GetFrom_Deprecated().classid, rf_signal->GetFrom_Deprecated().object};
    // update pathloss, shadowing and fading of the signal
    // we use the OLD compute rxdBm from WSNETv3
    // WSNETv4 will not use the same functions
    medium_compute_rxdBm(rf_signal->GetPacket_Deprecated(), &to, &from);

    // convert the value of rxdBm into values of mW/Hz
    rf_signal->GetPacket_Deprecated()->rxmW = dBm2mW(rf_signal->GetPacket_Deprecated()->rxdBm);

    // update the signal
    auto factor=1.0-relative_sum_of_factors(factor_adjacent_, number_adjacent_bands_);
    auto current_adj_nbr=0;
    for (auto freq_interval : rf_signal->GetWaveform().lock()->GetAllFrequencyInterval()){
      auto psd = rf_signal->GetPacket_Deprecated()->rxmW*factor / (freq_interval->GetHighPoint() - freq_interval->GetLowPoint());
      freq_interval->SetPSDValue(psd);
      if (!current_adj_nbr){
        factor = factor_adjacent_/2;
      } else if (current_adj_nbr%2==0) {
        factor = factor/2;
      }
      ++current_adj_nbr;
    }

    //call signal_tracker to verify if it will take this signal or not
    //if the signal_tracker chose this signal, we start the interface_cs
    if (signal_tracker_model_->ReceiveSignal(rf_signal)){
      interface_cs(&to, &from, rf_signal->GetPacket_Deprecated());
    }
  }
}

void RFSignalAdjacentBandPhyModel::SendSignalToSpectrumImpl(std::shared_ptr<Signal> signal) {
  spectrum_->AddSignalTx(signal);
}

void RFSignalAdjacentBandPhyModel::RegisterRxNodeImpl(SetOfFrequencyIntervals freq_intervals) {
  if (registered_rx_node_==nullptr){
    registered_rx_node_ = RegisteredRxNodeFactory::CreateRegisteredRxNode(freq_intervals, nodeid_,this);
    auto signals_already_on_air = spectrum_->RegisterRXNode(registered_rx_node_);
    for (auto signal : signals_already_on_air){
      ReceiveSignalFromSpectrum(signal);
    }
  }
}

void RFSignalAdjacentBandPhyModel::UnregisterRxNodeRxNodeImpl() {
  if (registered_rx_node_!=nullptr){
    spectrum_->UnregisterRXNode(registered_rx_node_);
    received_signals_.clear();
    signals_already_treated_.clear();
    registered_rx_node_.reset();
  }
}

void RFSignalAdjacentBandPhyModel::ReceivedSignalFromSpectrumRxEndImpl(std::shared_ptr<Signal> signal) {

  // insert the signal as already treated
  signals_already_treated_.insert(std::make_pair(signal->GetEnd(), signal->GetUID()));

  // check if it is the selected signal
  if (signal_tracker_model_->VerifySignalIsSelected(signal)){
    call_t to_interface = {signal->GetTo_Deprecated().classid, signal->GetTo_Deprecated().object};
    // if signal is selected, it is a RF Signal, so no need to check
    auto desired_rf_signal = std::dynamic_pointer_cast<RFSignal>(signal);
    packet_t *packet = desired_rf_signal->GetPacket_Deprecated();
    auto noise_mW = noise_get_noise(&to_interface, desired_rf_signal->GetBandwidth());
    desired_rf_signal->SetSNR(packet->rxmW/noise_mW);
    desired_rf_signal->SetSINR(packet->rxmW/noise_mW);

    interference_model_->ApplyInterference(signal, received_signals_);
    signal_tracker_model_->ResetSelectedSignal();

    // calculate BER and PER
    double ber = do_modulate_snr(&to_interface,packet->modulation, desired_rf_signal->GetSINR());
    packet->PER = 1 - pow((1-ber),packet->size*8);

    SendPacketToUp_Deprecated(signal);
  }

  // clear the received signals if no more signals to be interfered
  if (signals_already_treated_.size()==received_signals_.size()){
    evaluation_window_=0;
    received_signals_.clear();
    signals_already_treated_.clear();
  }
  else {
    // for all elements that their END is lower than
    // the current time minus the evaluation_window
    // i.e. they are no longer inside the evaluation window
    Time delta = 0;
    if (get_time()>evaluation_window_){
      delta = get_time() - evaluation_window_;
    }
    for (auto elem=signals_already_treated_.begin(); elem!=signals_already_treated_.upper_bound(delta); ++elem){
      signals_already_treated_.erase(elem);
      received_signals_.erase(elem->second);
    }
  }
  return;

}

void RFSignalAdjacentBandPhyModel::SendPacketToUp_DeprecatedImpl(std::shared_ptr<Signal> signal) {
  auto rf_signal = std::static_pointer_cast<RFSignal>(signal);
  call_t to = {signal->GetTo_Deprecated().classid, signal->GetTo_Deprecated().object};
  call_t from = {signal->GetFrom_Deprecated().classid, signal->GetFrom_Deprecated().object};
  RX(&to,&from,rf_signal->GetPacket_Deprecated());
}

void RFSignalAdjacentBandPhyModel::PrintImpl() const {
}

void RFSignalAdjacentBandPhyModel::ReceivePacketFromUp_DeprecatedImpl(call_t *to, call_t *from_interface,packet_t * packet) {
  node_t    *node   = get_node_by_id(from_interface->object);
  array_t *transceivers = get_tranceiver_classesid(to);
  call_t from_transceiver = {transceivers->elts[0], to->object};

  // This is temporarily being done here.
  // There should be a proper interface between radio and phy to provide
  // the exchange of primitives
  if (primitive_is_set((char*)"PHY-RESET-REGISTER-RX.request",packet)){
    // unregister rx
    UnregisterRxNode();
    packet_dealloc(packet);
    return;
  }

  if (primitive_is_set((char*)"PHY-SET-REGISTER-RX.request",packet)){
    // register rx
    RegisterRxNode(frequency_intervals_rx_);
    packet_dealloc(packet);
    return;
  }

  // fill packet info
  packet->node       = node->id;
  packet->interface  = from_interface->classid;
  packet->txdBm      = transceiver_get_power(&from_transceiver, from_interface);
  packet->channel    = transceiver_get_channel(&from_transceiver, from_interface);
  packet->modulation = transceiver_get_modulation(&from_transceiver, from_interface);
  packet->Tb         = transceiver_get_Tb(&from_transceiver, from_interface);
  packet->duration   = packet->real_size * packet->Tb;
  packet->clock0     = get_time();
  packet->clock1     = packet->clock0 + packet->duration;
  packet->rxdBm	   = 0;
  packet->rxmW	   = 0;
  packet->PER		   = 0;
  packet->RSSI	   = 0;
  packet->LQI		   = 0;
  packet->SINR	   = 0;

  packet->noise_mW = NULL;
  packet->ber = NULL;

  // create the rf signal that will be transmitted to spectrum
  auto waveform = create_rf_signal_waveform(factor_adjacent_, number_adjacent_bands_, dBm2mW(packet->txdBm), frequency_intervals_tx_);
  auto rf_signal = RFSignalFactory::CreateSignal(to, from_interface,waveform,packet);

  packet->signal     = TO_C(&rf_signal);

  SendSignalToSpectrum(rf_signal);
}

double relative_sum_of_factors(double initial_factor, int number_bands) {
  if (number_bands == 0){
    return 0.0;
  }
  return relative_sum_of_factors(initial_factor,number_bands-1) + (initial_factor)/(std::pow(2,number_bands-1));
}

int RFSignalAdjacentBandPhyModel::GetLogStatus(){
  return log_status_;
}

void RFSignalAdjacentBandPhyModel::UpdateFrequencyIntervalsTx(SetOfFrequencyIntervals freq_intervals){
  frequency_intervals_tx_.clear();
  frequency_intervals_tx_ = freq_intervals;
}


void RFSignalAdjacentBandPhyModel::UpdateFrequencyIntervalsRx(SetOfFrequencyIntervals freq_intervals){
  frequency_intervals_rx_.clear();
  frequency_intervals_rx_ = freq_intervals;
}
