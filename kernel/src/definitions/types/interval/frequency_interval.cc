/**
 *  \file   frequency_interval.h
 *  \brief  FrequencyInterval Concrete Base Class definition
 *  \author Luiz Henrique Suraty Filho
 *  \date   July 2017
 *  \version 1.0
 **/

#include <iostream>
#include <memory>
#include <vector>
#include <kernel/include/definitions/types/interval/frequency_interval.h>

FrequencyInterval::FrequencyInterval(const Frequency low,const Frequency high)
: Interval(), low_(low), high_(high), center_(0)
{ };

FrequencyInterval::FrequencyInterval(const Frequency low,const Frequency high,const Frequency center)
: Interval(), low_(low), high_(high), center_(center)
{ };

Frequency FrequencyInterval::GetCenter() const {
  return center_;
}

std::shared_ptr<FrequencyInterval> FrequencyInterval::Clone(){
  return CloneImpl();
}

std::shared_ptr<FrequencyInterval> FrequencyInterval::CloneImpl(){
  return std::make_shared<FrequencyInterval>(low_,high_,center_);
}

void FrequencyInterval::PrintImpl() const {
  std::cout<<"frequencies: ["<<low_<<","<<high_<<","<<center_<<"]"<<std::endl;
}

Frequency FrequencyInterval::GetLowPointImpl() const {
  return low_;
}

Frequency FrequencyInterval::GetHighPointImpl() const {
  return high_;
}
