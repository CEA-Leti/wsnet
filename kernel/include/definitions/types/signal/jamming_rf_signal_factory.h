/**
 *  \file   jamming_rf_signal_factory.h
 *  \brief  JammingRFSignalFactory Concrete Class definition
 *  \author Luiz Henrique Suraty Filho
 *  \date   July 2017
 *  \version 1.0
 **/

#ifndef WSNET_CORE_DEFINITIONS_TYPES_SIGNAL_JAMMING_RF_SIGNAL_FACTORY_H_
#define WSNET_CORE_DEFINITIONS_TYPES_SIGNAL_JAMMING_RF_SIGNAL_FACTORY_H_

#include <memory>
#include <kernel/include/definitions/types/signal/signal_factory.h>
#include <kernel/include/definitions/types/signal/jamming_rf_signal.h>


/** \brief The Factory of JammingRFSignal Class
 *
 * It implements the SignalFactory to create and return a JammingRFSignal
 * Use this class whenever you need to create a new JammingRFSignal
 *
 * \fn CreateSignalImpl() return the newly created jamming signal
 **/
class JammingRFSignalFactory : public SignalFactory<Signal, JammingRFSignal>{
 public:
  static std::shared_ptr<JammingRFSignal> CreateSignalImpl(nodeid_t source,std::shared_ptr<Waveform> waveform){
    auto new_signal = std::make_shared<JammingRFSignal>(source);

    new_signal->SetWaveform(waveform);

    waveform->SetSignal(new_signal);

    return new_signal;
  };
  /*
	static std::shared_ptr<LoraSignal> CopySignal(std::shared_ptr<LoraSignal> signal){
			return CreateSignalImpl(WaveformFactory::CopyWaveform(signal->GetWaveform()),signal->GetSpreadingFactor());
		};*/
};

#endif // WSNET_CORE_DEFINITIONS_TYPES_SIGNAL_JAMMING_RF_SIGNAL_FACTORY_H_
