/**
 *  \file   lora_signal_factory.h
 *  \brief  LoRa Signal Factory Concrete Class definition
 *  \author Luiz Henrique Suraty Filho
 *  \date   July 2017
 *  \version 1.0
 **/

#ifndef WSNET_CORE_DEFINITIONS_TYPES_SIGNAL_LORA_SIGNAL_FACTORY_H_
#define WSNET_CORE_DEFINITIONS_TYPES_SIGNAL_LORA_SIGNAL_FACTORY_H_

#include <memory>
#include <kernel/include/definitions/types.h>
#include <kernel/include/definitions/types/signal/signal_factory.h>
#include <kernel/include/definitions/types/signal/lora_signal.h>

/** \brief The LoRaSignalFactory class for lora signals
 *
 * It implements the SignalFactory to create and return a LoRaSignal
 * Use this class whenever you need to create a new LoRaSignal
 *
 * \fn CreateSignalImpl() return a newly created signal of type LoRaSignal;
 **/
class LoRaSignalFactory : public SignalFactory<Signal, LoRaSignalFactory>{
 public:
  static std::shared_ptr<LoRaSignal> CreateSignalImpl(call_t *to, call_t *from_interface,std::shared_ptr<Waveform> waveform, SpreadingFactor sf, Frequency freq_center, Frequency bandwidth,packet_t *packet){
    auto new_signal = std::make_shared<LoRaSignal>(to,from_interface,sf,freq_center, bandwidth, packet);

    new_signal->SetWaveform(waveform);

    waveform->SetSignal(new_signal);

    return new_signal;
  };
  static std::shared_ptr<LoRaSignal> CreateSignalImpl(nodeid_t source, std::shared_ptr<Waveform> waveform, SpreadingFactor sf, Frequency freq_center, Frequency bandwidth, packet_t *packet){
    auto new_signal = std::make_shared<LoRaSignal>(source,sf,freq_center, bandwidth, packet);

    new_signal->SetWaveform(waveform);

    waveform->SetSignal(new_signal);

    return new_signal;
  };
  static std::shared_ptr<LoRaSignal> CreateSignalImpl(nodeid_t source,std::shared_ptr<Waveform> waveform){
    auto new_signal = std::make_shared<LoRaSignal>(source);

    new_signal->SetWaveform(waveform);

    waveform->SetSignal(new_signal);

    return new_signal;
  };
};

#endif // WSNET_CORE_DEFINITIONS_TYPES_SIGNAL_LORA_SIGNAL_FACTORY_H_
