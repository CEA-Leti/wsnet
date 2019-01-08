/**
 *  \file   coding_model.cc
 *  \brief  Coding Model Abstract Class implementation
 *  \author Luiz Henrique Suraty Filho
 *  \date   July 2017
 *  \version 1.0
 **/

#include <iostream>
#include <kernel/include/definitions/types.h>
#include <kernel/include/definitions/types/signal/signal.h>
#include <kernel/include/definitions/models/coding/coding_model.h>

CodingModelUid CodingModel::uid_counter_ = 0;

CodingModel::CodingModel() : uid_(uid_counter_) {
  ++uid_counter_;
}

CodingModel::~CodingModel() {}

void CodingModel::ApplyCoding(std::shared_ptr<Signal> signal) {
  ApplyCodingImpl(signal);
  return;
}

void CodingModel::Print() const {
  PrintImpl();
}

CodingModelUid CodingModel::GetUID() const {return uid_;};

void CodingModel::PrintImpl() const {
  std::cout << "No Print Method implemented" << std::endl;
};
