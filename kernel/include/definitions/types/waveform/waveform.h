/**
 *  \file   waveform.h
 *  \brief  Waveform Base Class definition
 *  \author Luiz Henrique Suraty Filho
 *  \date   July 2017
 *  \version 1.0
 **/

#ifndef WSNET_CORE_DEFINITIONS_TYPES_WAVEFORM_WAVEFORM_H_
#define WSNET_CORE_DEFINITIONS_TYPES_WAVEFORM_WAVEFORM_H_

#include <memory>
#include <kernel/include/definitions/types.h>
#include <kernel/include/definitions/types/interval/frequency_interval_waveform.h>
#include <kernel/include/definitions/types/signal/signal.h>

/** \brief The Base Class : Waveform Class
 *  Although it is not necessary, this class can be inherited to
 *  create another specific waveform class.
 *
 * \fn AddFrequencyInterval - inserts a frequency interval in the waveform
 * \fn GetAllFrequencyInterval - return all the frequencies intervals that describe the waveform
 * \fn GetUID - return the UID of the waveform
 * \fn Clone return a cloned waveform
 **/
class Waveform : public std::enable_shared_from_this<Waveform>{
  public:
    Waveform(SetOfFrequencyIntervalWaveform frequency_bands);
    Waveform();
    ~Waveform();
    void AddFrequencyInterval(std::shared_ptr<FrequencyIntervalWaveform> freq);
    SetOfFrequencyIntervalWaveform GetAllFrequencyInterval();
    std::shared_ptr<Waveform> Clone();
    WaveformUid GetUID() const;
    std::weak_ptr<Signal> GetSignal();
    void SetSignal(std::weak_ptr<Signal>);
    void Print() const;
  private:
    WaveformUid uid_; // maybe we do not need uid counter, we can check the memory of each object and this will be the UID;
    static WaveformUid uid_counter_;
    SetOfFrequencyIntervalWaveform frequency_interval_; // the set of frequency intervals that describes the waveform
    std::weak_ptr<Signal> signal_; // pointer to the signal of which it belongs
};

#endif // WSNET_CORE_DEFINITIONS_TYPES_WAVEFORM_WAVEFORM_H_
