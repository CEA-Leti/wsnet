/**
 *  \file   frequency_interval_waveform.h
 *  \brief  FrequencyIntervalWaveform Concrete Derived Class definition
 *  \author Luiz Henrique Suraty Filho
 *  \date   July 2017
 *  \version 1.0
 **/

#ifndef WSNET_CORE_DEFINITIONS_TYPES_INTERVAL_FREQUENCY_INTERVAL_WAVEFORM_H_
#define WSNET_CORE_DEFINITIONS_TYPES_INTERVAL_FREQUENCY_INTERVAL_WAVEFORM_H_

#include <vector>
#include <memory>
#include <kernel/include/definitions/types.h>
#include <kernel/include/definitions/types/interval/interval.h>
#include <kernel/include/definitions/types/interval/frequency_interval.h>

// Forward declaration of Waveform is needed as we use the waveform_ptr_
// We may need to come back to this issue to avoid using forward declaration
class Waveform;

/** \brief The Concrete Derived Class : FrequencyIntervalWaveform Class
 * 		It is a specialization of the FrequencyInterval Class that
 * 		is used to create an waveform. In fact, one of its member function
 * 		is a weak_ptr to the waveform it belongs.
 *
 * \fn GetLowPoint return the low value of the frequency interval
 * \fn GetHighPoint return the high value of the frequency interval
 * \fn GetCenter return the frequency center
 * \fn GetPSDValue return the PSD value of the frequency interval
 * \fn SetPSDValue changes the PSD value of the frequency interval
 * \fn CloneImpl return a copy of the frequency interval
 **/
class FrequencyIntervalWaveform : public FrequencyInterval {
  public:
    FrequencyIntervalWaveform(const Frequency low,const Frequency high,const Frequency center,
        const PSDValue psd_value, std::weak_ptr<Waveform> waveform );
    FrequencyIntervalWaveform(const Frequency low,const Frequency high,const Frequency center,
        const PSDValue psd_value );
    void AddToWaveform(std::weak_ptr<Waveform> waveform);
    std::weak_ptr<Waveform> GetWaveform();
    PSDValue GetPSDValue() const;
    void SetPSDValue(PSDValue psd_value);
  private:
    std::shared_ptr<FrequencyInterval> CloneImpl();
    void PrintImpl() const;
    std::weak_ptr<Waveform> waveform_ptr_;
    PSDValue psd_value_;
};

using SetOfFrequencyIntervalWaveform = std::vector<std::shared_ptr<FrequencyIntervalWaveform>>;
using FrequencyIntervalWaveformIterator = SetOfFrequencyIntervalWaveform::const_iterator;

#endif //WSNET_CORE_DEFINITIONS_TYPES_INTERVAL_FREQUENCY_INTERVAL_WAVEFORM_H_
