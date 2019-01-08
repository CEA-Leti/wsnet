/**
 *  \file   interval.cc
 *  \brief  Interval Abstract Class Methods implementation
 *  \author Luiz Henrique Suraty Filho
 *  \date   July 2017
 *  \version 1.0
 **/

#include <iostream>

#include <kernel/include/definitions/types/interval/interval.h>

IntervalUid Interval::uid_counter_ = 0;

Interval::Interval() : uid_(uid_counter_) {
 ++uid_counter_;
}

IntervalBoundary Interval::GetLowPoint() const {
  return GetLowPointImpl();
}

IntervalBoundary Interval::GetHighPoint() const {
  return GetHighPointImpl();
}

IntervalUid Interval::GetUID() {
  return uid_;
}

void Interval::Print() const {
  PrintImpl();
}

void Interval::PrintImpl() const {
  std::cout << "No Print Method implemented" << std::endl;
}


