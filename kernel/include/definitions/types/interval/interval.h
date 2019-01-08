/**
 *  \file   interval.h
 *  \brief  Interval Abstract Class definition
 *  \author Luiz Henrique Suraty Filho
 *  \date   July 2017
 *  \version 1.0
 **/

#ifndef WSNET_CORE_DEFINITIONS_TYPES_INTERVAL_INTERVAL_H_
#define WSNET_CORE_DEFINITIONS_TYPES_INTERVAL_INTERVAL_H_

#include <kernel/include/definitions/types.h>

/** \brief The Abstract Base Class : Interval Class
 * This means that no instance of the Interval class can exist.
 * Only classes which inherit from the Interval class can exist.
 *
 * \fn GetLowPoint return the lowest point of the interval
 * \fn GetHighPoint return the highest point of the interval
 * \fn GetUID return the UID of the interval
 **/
class Interval {
  public:
    Interval();
    virtual ~Interval() {};
    IntervalBoundary GetLowPoint() const;
    IntervalBoundary GetHighPoint() const;
    void Print() const;
    IntervalUid GetUID();
  private:
    virtual IntervalBoundary GetLowPointImpl() const = 0;
    virtual IntervalBoundary GetHighPointImpl() const = 0;
    virtual void PrintImpl() const;
    static IntervalUid uid_counter_;
    IntervalUid uid_;
};

#endif //WSNET_CORE_DEFINITIONS_TYPES_INTERVAL_INTERVAL_H_
