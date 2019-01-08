/**
 *  \file   signal.h
 *  \brief  Signal Abstract Class definition
 *  \author Luiz Henrique Suraty Filho
 *  \date   July 2017
 *  \version 1.0
 **/

#include <memory>
#include <map>
#include <vector>
#include <kernel/include/definitions/types.h>
#include <kernel/include/definitions/types/signal/signal.h>

SignallUid Signal::uid_counter_ = 0;

Signal::Signal (nodeid_t source): uid_(uid_counter_), source_(source), SINR_(0.0), SNR_(0.0) {
  ++uid_counter_;
  begin_=0;
  end_=0;
}

Signal::Signal (call_t *to,call_t *from_interface): uid_(uid_counter_), source_(from_interface->object), SINR_(0.0), SNR_(0.0) {
  from_interface_ = {from_interface->classid, from_interface->object};
  to_interface_ = {to->classid, to->object};
  ++uid_counter_;
  begin_=0;
  end_=0;
}

Signal::Signal (call_t *to,call_t *from_interface, Time T_begin, Time T_end): uid_(uid_counter_), source_(from_interface->object), SINR_(0.0), SNR_(0.0){
  from_interface_ = {from_interface->classid, from_interface->object};
  to_interface_ = {to->classid, to->object};
  ++uid_counter_;
  begin_ = T_begin;
  end_ = T_end;
}


Signal::~Signal(){};

void Signal::Print() const{
  PrintSignal();
}

SignallUid Signal::GetUID() const{
  return uid_;
}

std::shared_ptr<Signal> Signal::Clone() {
  return CloneImpl();
}

double Signal::GetSINR() const{
  return SINR_;
}

double Signal::GetSNR() const{
  return SNR_;
}

Time Signal::GetBegin() const{
  return begin_;
}

Time Signal::GetEnd() const {
  return end_;
}

void Signal::SetTo_Deprecated(call_t to){
  to_interface_ = to;
}

void Signal::SetFrom_Deprecated(call_t to){
  from_interface_ = to;
}

call_t Signal::GetTo_Deprecated() const{
  return to_interface_;
}

call_t Signal::GetFrom_Deprecated() const{
  return from_interface_;
}

void Signal::SetDestination(std::shared_ptr<RegisteredRxNode> destination) {
  destination_ = destination;
}

nodeid_t Signal::GetSource() const {
  return source_;
}

void Signal::SetSINR(double SINR){
  SINR_ = SINR;
}

void Signal::SetSNR(double SNR){
  SNR_ = SNR;
}

void Signal::SetBegin(Time T_begin){
	begin_ = T_begin;
}

void Signal::SetEnd(Time T_end){
	end_ = T_end;
}

std::shared_ptr<RegisteredRxNode> Signal::GetDestination() const{
  return destination_;
}

void Signal::PrintSignal() const{
  PrintSignalImpl();
}
