/**
 *  \file   frequency_interval.h
 *  \brief  FrequencyInterval Concrete Base Class definition
 *  \author Luiz Henrique Suraty Filho
 *  \date   July 2017
 *  \version 1.0
 **/

#ifndef WSNET_CORE_DEFINITIONS_TYPES_INTERVAL_FREQUENCY_INTERVAL_H_
#define WSNET_CORE_DEFINITIONS_TYPES_INTERVAL_FREQUENCY_INTERVAL_H_

#include <kernel/include/definitions/types.h>
#include <kernel/include/definitions/types/interval/interval.h>

/** \brief The Concrete Base Class : FrequencyInterval Class
 *
 * \fn GetLowPoint return the low value of the frequency interval
 * \fn GetHighPoint return the high value of the frequency interval
 * \fn GetCenter return the frequency center
 * \fn Clone return a cloned frequency interval
 **/
class FrequencyInterval : public Interval {
  public:
    FrequencyInterval(const Frequency low,const Frequency high);
    FrequencyInterval(const Frequency low,const Frequency high,const Frequency center);
    Frequency GetCenter() const;
    std::shared_ptr<FrequencyInterval> Clone();
  private:
    virtual std::shared_ptr<FrequencyInterval> CloneImpl();
    void PrintImpl() const;
  protected:
    Frequency GetLowPointImpl() const;
    Frequency GetHighPointImpl() const;
    Frequency low_;
    Frequency high_;
    Frequency center_;
};


using SetOfFrequencyIntervals = std::vector<std::shared_ptr<FrequencyInterval>>;
using FrequencyIntervalsIterator = SetOfFrequencyIntervals::const_iterator;

#endif //WSNET_CORE_DEFINITIONS_TYPES_INTERVAL_FREQUENCY_INTERVAL_H_
