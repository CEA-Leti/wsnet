/**
 *  \file   error_model.h
 *  \brief  ErrorModel Abstract Class definition
 *  \author Luiz Henrique Suraty Filho
 *  \date   July 2017
 *  \version 1.0
 **/


#include <iostream>
#include <kernel/include/definitions/types.h>
#include <kernel/include/definitions/types/signal/signal.h>
#include <kernel/include/definitions/models/error/error_model.h>

ErrorModelUid ErrorModel::uid_counter_ = 0;

ErrorModel::ErrorModel() : uid_(uid_counter_) {
  ++uid_counter_;
}

ErrorModel::~ErrorModel() {};

void ErrorModel::ApplyErrors(std::weak_ptr<Signal> signal) {
  ApplyErrorsImpl(signal);
  return;
}

void ErrorModel::Print() const {
  PrintImpl();
}

ErrorModelUid ErrorModel::GetUID() const {
  return uid_;
}

void ErrorModel::PrintImpl() const {
  std::cout << "No Print Method implemented" << std::endl;
}
