/**
 *  \file   coding_model.h
 *  \brief  Coding Model Abstract Class definition
 *  \author Luiz Henrique Suraty Filho
 *  \date   July 2017
 *  \version 1.0
 **/

#ifndef WSNET_TESTS_MOCKS_DEFINITIONS_MODELS_CODING_CODING_MODEL_H_
#define WSNET_TESTS_MOCKS_DEFINITIONS_MODELS_CODING_CODING_MODEL_H_

#include "gmock/gmock.h"
#include <kernel/include/definitions/models/coding/coding_model.h>

class MockCodingModel : public CodingModel {
public:
  MOCK_CONST_METHOD0(GetUID, CodingModelUid());
  MOCK_METHOD1(ApplyCoding, void(std::shared_ptr<Signal> signal));
  MOCK_CONST_METHOD0(Print, void());

  MOCK_METHOD1(ApplyInterferenceImpl, void(std::shared_ptr<Signal> signal));
  MOCK_CONST_METHOD0(PrintImpl, void());
};

#endif //WSNET_TESTS_MOCKS_DEFINITIONS_MODELS_CODING_CODING_MODEL_H_
