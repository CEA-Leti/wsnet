/**
 *  \file   waveform.h
 *  \brief  Waveform Base Class implementation
 *  \author Luiz Henrique Suraty Filho
 *  \date   July 2017
 *  \version 1.0
 **/

#include <iostream>
#include <memory>
#include <kernel/include/definitions/types/waveform/waveform.h>
#include <kernel/include/definitions/types/interval/frequency_interval_waveform.h>

WaveformUid Waveform::uid_counter_ = 0;

Waveform::Waveform(SetOfFrequencyIntervalWaveform frequency_bands) : uid_(uid_counter_) {
  frequency_interval_ = frequency_bands;
  ++uid_counter_;
}

Waveform::Waveform(): uid_(uid_counter_) {
  ++uid_counter_;
}

Waveform::~Waveform(){};

void Waveform::AddFrequencyInterval(std::shared_ptr<FrequencyIntervalWaveform> freq){
  frequency_interval_.push_back(freq);
};
SetOfFrequencyIntervalWaveform Waveform::GetAllFrequencyInterval(){
  return frequency_interval_;
}

void Waveform::SetSignal(std::weak_ptr<Signal> signal) {
  signal_ = signal;
}

std::weak_ptr<Signal> Waveform::GetSignal() {
  return signal_;
}

std::shared_ptr<Waveform> Waveform::Clone() {
  auto new_waveform = std::make_shared<Waveform>();
  for (std::shared_ptr<FrequencyIntervalWaveform> freq : frequency_interval_){
    auto new_freq = std::dynamic_pointer_cast<FrequencyIntervalWaveform>(freq->Clone());
    new_waveform->AddFrequencyInterval(new_freq);
    new_freq->AddToWaveform(new_waveform);
  }
  return new_waveform;
}

WaveformUid Waveform::GetUID() const {
  return uid_;
}
void Waveform::Print() const{
  auto i = 0;
  std::cout<<"  Waveform "<< shared_from_this().get()  << " nr_freq_bands = "<< frequency_interval_.size()<<std::endl;
  for (std::shared_ptr<FrequencyIntervalWaveform> freq : frequency_interval_){
    std::cout<<"    Freq_band "<<++i;
    freq->Print();
  }
}

