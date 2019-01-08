/**
 *  \file   jamming_rf_signal.h
 *  \brief  JammingRFSignal Concrete Class definition
 *  \author Luiz Henrique Suraty Filho
 *  \date   July 2017
 *  \version 1.0
 **/

#ifndef WSNET_CORE_DEFINITIONS_TYPES_SIGNAL_JAMMING_RF_SIGNAL_H_
#define WSNET_CORE_DEFINITIONS_TYPES_SIGNAL_JAMMING_RF_SIGNAL_H_

#include <memory>
#include <kernel/include/definitions/types/signal/rf_signal.h>

/** \brief The Concrete Derived Class : JammingRFSignal Class
 *
 * Jamming RF Signals do not need a packet as they only generate noisy waveforms
 *
 * \fn GetWaveformImpl() return the waveform of the jamming signal
 * \fn SetWaveformImpl() sets the waveform of the jamming signal
 * \fn CloneImpl() return a deep copy of the jamming signal
 **/
class JammingRFSignal : public RFSignal{
  public:
    JammingRFSignal(nodeid_t source) : RFSignal(source, nullptr){};
    ~JammingRFSignal(){};
  private:
    std::weak_ptr<Waveform> GetWaveformImpl(){ return waveform_;};
    void PrintSignalImpl() const {
      std::cout<<"This is a Jamming Signal "<<std::endl;
      waveform_->Print();
    };
    std::shared_ptr<Signal> CloneImpl(){
      auto new_signal = std::make_shared<JammingRFSignal>(source_);
      new_signal->SetWaveform(waveform_->Clone());
      waveform_->SetSignal(new_signal);
      return new_signal;
    };
    packet_t *GetPacket_DeprecatedImpl() {return nullptr;};
    void SetWaveformImpl(std::shared_ptr<Waveform> waveform) {waveform_ = waveform;};
    std::shared_ptr<Waveform> waveform_;
};

#endif // WSNET_CORE_DEFINITIONS_TYPES_SIGNAL_JAMMING_RF_SIGNAL_H_
