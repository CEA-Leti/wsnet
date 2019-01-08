/**
 *  \file   frequency_interval_waveform.cc
 *  \brief  FrequencyIntervalWaveform Concrete Derived Class implementation
 *  \author Luiz Henrique Suraty Filho
 *  \date   July 2017
 *  \version 1.0
 **/


#include <iostream>
#include <kernel/include/definitions/types/waveform/waveform.h>
#include <kernel/include/definitions/types/interval/frequency_interval_waveform.h>

FrequencyIntervalWaveform::FrequencyIntervalWaveform(const Frequency low,
    const Frequency high,
    const Frequency center,
    const PSDValue psd_value, std::weak_ptr<Waveform> waveform ) :
      FrequencyInterval(low,high,center), waveform_ptr_(waveform),psd_value_(psd_value) { };

FrequencyIntervalWaveform::FrequencyIntervalWaveform(const Frequency low,
    const Frequency high,
    const Frequency center,
    const PSDValue psd_value ) :
      FrequencyInterval(low,high, center), psd_value_(psd_value){ };

void FrequencyIntervalWaveform::AddToWaveform(std::weak_ptr<Waveform> waveform){
  waveform_ptr_ = waveform;
}

std::weak_ptr<Waveform> FrequencyIntervalWaveform::GetWaveform() {
  return waveform_ptr_;
}

PSDValue FrequencyIntervalWaveform::GetPSDValue() const {
  return psd_value_;
}

void FrequencyIntervalWaveform::SetPSDValue(PSDValue psd_value) {
  psd_value_=psd_value;
}

std::shared_ptr<FrequencyInterval> FrequencyIntervalWaveform::CloneImpl(){
  return std::make_shared<FrequencyIntervalWaveform>(low_,high_,center_,psd_value_);
}

void FrequencyIntervalWaveform::PrintImpl() const {
  std::cout<<" inside waveform = "<<waveform_ptr_.lock()<<" ["<<low_<<";"<< center_ <<";"<<high_<<"] = "<<psd_value_<<" mW/Hz"<<std::endl;
}
