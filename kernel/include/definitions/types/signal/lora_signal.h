/**
 *  \file   lora_signal.h
 *  \brief  LoRa Signal Concrete Class definition
 *  \author Luiz Henrique Suraty Filho
 *  \date   July 2017
 *  \version 1.0
 **/

#ifndef WSNET_CORE_DEFINITIONS_TYPES_SIGNAL_LORA_SIGNAL_H_
#define WSNET_CORE_DEFINITIONS_TYPES_SIGNAL_LORA_SIGNAL_H_


#include <iostream>
#include <memory>
#include <kernel/include/definitions/types/signal/rf_signal.h>
#include <kernel/include/definitions/packet.h>

/** \brief The Concrete Derived Class : JammingSignal Class
 *
 * Besides the waveform, Lora Signals have the spreading factor of the signal
 *
 * \fn GetWaveformImpl() return the waveform of the lora signal
 * \fn SetWaveformImpl() sets the waveform of the lora signal
 * \fn CloneImpl() return a deep copy of the lora signal
 * \fn GetSpreadingFactor() return the spreading factor;
 * \fn SetSpreadingFactor() set the spreading factor;
 **/
class LoRaSignal : public RFSignal, public std::enable_shared_from_this<LoRaSignal>{
 public:
  LoRaSignal(nodeid_t source) : RFSignal(source, nullptr) {};
  ~LoRaSignal(){};
  LoRaSignal(call_t *to, call_t *from_interface, SpreadingFactor sf,Frequency freq_center,
             Frequency bandwidth, packet_t *packet) : RFSignal(to,from_interface,packet) , sf_(sf), freq_center_(freq_center), bandwidth_(bandwidth){};
  LoRaSignal(nodeid_t source, SpreadingFactor sf, Frequency freq_center,
             Frequency bandwidth, packet_t *packet) : RFSignal(source,packet) , sf_(sf), freq_center_(freq_center), bandwidth_(bandwidth){};
  void SetSpreadingFactor(SpreadingFactor sf){
    sf_=sf;
  };
  SpreadingFactor GetSpreadingFactor() const {return sf_;};
  Frequency GetFrequencyCenter() const {return freq_center_;} ;
 private:
  std::weak_ptr<Waveform> GetWaveformImpl() const{ return waveform_;};
  std::shared_ptr<Signal> CloneImpl(){
    auto new_signal = std::make_shared<LoRaSignal>(source_,sf_, freq_center_, bandwidth_,packet_clone(packet_));
    new_signal->SetWaveform(waveform_->Clone());
    waveform_->SetSignal(new_signal);
    return new_signal;
  };
  packet_t *GetPacket_DeprecatedImpl() {return packet_;};
  void SetWaveformImpl(std::shared_ptr<Waveform> waveform) {waveform_ = waveform;};
  void PrintSignalImpl() const {
    std::cout<<"This is a LoRa Signal with SF = "<< sf_ << std::endl;
    waveform_->Print();
  };
  Frequency GetBandwidthImpl() const{return bandwidth_;}
  SpreadingFactor sf_ = 0;
  Frequency freq_center_ = 0;
  Frequency bandwidth_ = 0;
};

#endif // WSNET_CORE_DEFINITIONS_TYPES_SIGNAL_LORA_SIGNAL_H_
