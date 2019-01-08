/**
 *  \file   error_model.h
 *  \brief  ErrorModel Abstract Class definition
 *  \author Luiz Henrique Suraty Filho
 *  \date   July 2017
 *  \version 1.0
 **/

#ifndef WSNET_TESTS_MOCKS_DEFINITIONS_MODELS_ERROR_ERROR_MODEL_H_
#define WSNET_TESTS_MOCKS_DEFINITIONS_MODELS_ERROR_ERROR_MODEL_H_

#include "gmock/gmock.h"
#include <kernel/include/definitions/models/error/error_model.h>

class MockErrorModel : public ErrorModel {
public:
  MOCK_CONST_METHOD0(GetUID, ErrorModelUid());
  MOCK_METHOD1(ApplyErrors, void(std::weak_ptr<Signal> signal));
  MOCK_CONST_METHOD0(Print, void());

  MOCK_METHOD1(ApplyErrorsImpl, void(std::weak_ptr<Signal> signal));
  MOCK_CONST_METHOD0(PrintImpl, void());
};

#endif // WSNET_TESTS_MOCKS_DEFINITIONS_MODELS_ERROR_ERROR_MODEL_H_
