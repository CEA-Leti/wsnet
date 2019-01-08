/**
 *  \file   time_interval.h
 *  \brief  FrequencyInterval Concrete Base Class definition
 *  \author Luiz Henrique Suraty Filho
 *  \date   July 2017
 *  \version 1.0
 **/

#include <iostream>
#include <memory>
#include <vector>
#include <kernel/include/definitions/types/interval/time_interval.h>

TimeInterval::TimeInterval(const Time low,const Time high)
: Interval(), low_(low), high_(high)
{ };

std::shared_ptr<TimeInterval> TimeInterval::Clone(){
  return CloneImpl();
}

std::shared_ptr<TimeInterval> TimeInterval::CloneImpl(){
  return std::make_shared<TimeInterval>(low_,high_);
}

void TimeInterval::PrintImpl() const {
  std::cout<<"time: ["<<low_<<","<<high_<<"]"<<std::endl;
}

Time TimeInterval::GetLowPointImpl() const {
  return low_;
}

Time TimeInterval::GetHighPointImpl() const {
  return high_;
}
