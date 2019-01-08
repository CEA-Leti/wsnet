/**
 *  \file   time_interval.h
 *  \brief  TimeInterval Concrete Base Class definition
 *  \author Luiz Henrique Suraty Filho
 *  \date   July 2017
 *  \version 1.0
 **/

#ifndef WSNET_CORE_DEFINITIONS_TYPES_INTERVAL_TIME_INTERVAL_H_
#define WSNET_CORE_DEFINITIONS_TYPES_INTERVAL_TIME_INTERVAL_H_

#include <kernel/include/definitions/types.h>
#include <kernel/include/definitions/types/interval/interval.h>

/** \brief The Concrete Base Class : TimeInterval Class
 *
 * \fn GetLowPoint return the low value of the frequency interval
 * \fn GetHighPoint return the high value of the frequency interval
 * \fn Clone return a cloned frequency interval
 **/
class TimeInterval : public Interval {
 public:
  TimeInterval(const Time low,const Time high);
  std::shared_ptr<TimeInterval> Clone();
 private:
  virtual std::shared_ptr<TimeInterval> CloneImpl();
  void PrintImpl() const;
 protected:
  Time GetLowPointImpl() const;
  Time GetHighPointImpl() const;
  Time low_;
  Time high_;
};


using SetOfFrequencyIntervals = std::vector<std::shared_ptr<TimeInterval>>;
using FrequencyIntervalsIterator = SetOfFrequencyIntervals::const_iterator;

#endif //WSNET_CORE_DEFINITIONS_TYPES_INTERVAL_TIME_INTERVAL_H_
