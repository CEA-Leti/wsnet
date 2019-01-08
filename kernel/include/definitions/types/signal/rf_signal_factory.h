/**
 *  \file   rf_signal_factory.h
 *  \brief  RFSignal Factory Concrete Class definition
 *  \author Luiz Henrique Suraty Filho
 *  \date   July 2017
 *  \version 1.0
 **/

#ifndef WSNET_CORE_DEFINITIONS_TYPES_SIGNAL_RFSIGNAL_FACTORY_H_
#define WSNET_CORE_DEFINITIONS_TYPES_SIGNAL_RFSIGNAL_FACTORY_H_

#include <memory>
#include <kernel/include/definitions/types.h>
#include <kernel/include/definitions/types/signal/signal_factory.h>
#include <kernel/include/definitions/types/signal/rf_signal.h>

/** \brief The LoRaSignalFactory class for lora signals
 *
 * It implements the SignalFactory to create and return a LoRaSignal
 * Use this class whenever you need to create a new LoRaSignal
 *
 * \fn CreateSignalImpl() return a newly created signal of type LoRaSignal;
 **/
class RFSignalFactory : public SignalFactory<Signal, RFSignalFactory>{
 public:
  static std::shared_ptr<RFSignal> CreateSignalImpl(call_t *to, call_t *from_interface,std::shared_ptr<Waveform> waveform, packet_t *packet){
    auto new_signal = std::make_shared<RFSignal>(to,from_interface,packet);

    new_signal->SetWaveform(waveform);

    waveform->SetSignal(new_signal);

    return new_signal;
  };
  static std::shared_ptr<RFSignal> CreateSignalImpl(call_t *to, call_t *from_interface,std::shared_ptr<Waveform> waveform, packet_t *packet, Time T_begin, Time T_end){
    auto new_signal = std::make_shared<RFSignal>(to,from_interface,packet,T_begin,T_end);

    new_signal->SetWaveform(waveform);

    waveform->SetSignal(new_signal);

    return new_signal;
  };
  static std::shared_ptr<RFSignal> CreateSignalImpl(nodeid_t source, std::shared_ptr<Waveform> waveform, packet_t *packet){
    auto new_signal = std::make_shared<RFSignal>(source,packet);

    new_signal->SetWaveform(waveform);

    waveform->SetSignal(new_signal);

    return new_signal;
  };
  static std::shared_ptr<RFSignal> CreateSignalImpl(nodeid_t source,std::shared_ptr<Waveform> waveform){
    auto new_signal = std::make_shared<RFSignal>(source, nullptr);

    new_signal->SetWaveform(waveform);

    waveform->SetSignal(new_signal);

    return new_signal;
  };
};

#endif // WSNET_CORE_DEFINITIONS_TYPES_SIGNAL_RFSIGNAL_FACTORY_H_
