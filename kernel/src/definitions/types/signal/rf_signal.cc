/**
 *  \file   rf_signal.h
 *  \brief  Radio Frequency Signal Abstract Class definition
 *  \author Luiz Henrique Suraty Filho
 *  \date   July 2017
 *  \version 1.0
 **/


#include <memory>
#include <kernel/include/definitions/types/signal/signal.h>
#include <kernel/include/definitions/types/waveform/waveform.h>
#include <kernel/include/definitions/types/signal/rf_signal.h>
#include <kernel/include/definitions/packet.h>

RFSignal::RFSignal(nodeid_t source, packet_t* packet) : Signal(source), packet_(packet){};

RFSignal::RFSignal(call_t *to, call_t *from_interface, packet_t* packet) : Signal(to, from_interface), packet_(packet){};

RFSignal::RFSignal(nodeid_t source,std::shared_ptr<Waveform> waveform) : Signal(source), waveform_(waveform){};

//RFSignal::RFSignal(call_t *to, call_t *from_interface, packet_t* packet, Time T_begin, Time T_end) : Signal(to, from_interface), packet_(packet), begin_(T_begin), end_(T_end){};
RFSignal::RFSignal(call_t *to, call_t *from_interface, packet_t* packet, Time T_begin, Time T_end) : Signal(to, from_interface, T_begin, T_end), packet_(packet){};

RFSignal::~RFSignal(){};

std::weak_ptr<Waveform> RFSignal::GetWaveform() const {
  return GetWaveformImpl();
}

Frequency RFSignal::GetBandwidth() const {
  return GetBandwidthImpl();
}

Frequency RFSignal::GetBandwidthImpl() const {
  Frequency signal_bandwidth_Hz = 0;
  for (auto freq_interval : waveform_->GetAllFrequencyInterval()){
    signal_bandwidth_Hz += (freq_interval->GetHighPoint() - freq_interval->GetLowPoint());
  }
  return signal_bandwidth_Hz;
}


void RFSignal::SetWaveform(std::shared_ptr<Waveform> waveform) {
  SetWaveformImpl(waveform);
  return;
}


packet_t *RFSignal::GetPacket_Deprecated(){
  return GetPacket_DeprecatedImpl();
}



std::weak_ptr<Waveform> RFSignal::GetWaveformImpl() const {
  return waveform_;
}

void RFSignal::SetWaveformImpl(std::shared_ptr<Waveform> waveform) {
  waveform_ = waveform;
}

packet_t *RFSignal::GetPacket_DeprecatedImpl(){
  return packet_;
}

std::shared_ptr<Signal> RFSignal::CloneImpl(){
  auto new_signal = std::make_shared<RFSignal>(source_,packet_clone(packet_));
  new_signal->SetWaveform(waveform_->Clone());
  waveform_->SetSignal(new_signal);
  return new_signal;
};

void RFSignal::PrintSignalImpl() const {
  std::cout<<" This is a RF Signal which contains the following packet:"<< std::endl;
  std::cout<<"   packet information : id="<<packet_->id<<", rxdBm="<<packet_->rxdBm<<std::endl;
  std::cout<<" This is a RF Signal which contains the following waveform:"<< std::endl;
  waveform_->Print();
}
