/**
 *  \file   interference_model.h
 *  \brief  Interference Model Abstract Class definition
 *  \author Luiz Henrique Suraty Filho
 *  \date   July 2017
 *  \version 1.0
 **/

#ifndef WSNET_TESTS_MOCKS_DEFINITIONS_MODELS_INTERFERENCE_MODEL_H_
#define WSNET_TESTS_MOCKS_DEFINITIONS_MODELS_INTERFERENCE_MODEL_H_

#include "gmock/gmock.h"
#include <kernel/include/definitions/models/interference/interference_model.h>


class MockInterferenceModel: public InterferenceModel {
public:
  MOCK_CONST_METHOD0(GetUID, InterferenceModelUid());
  MOCK_METHOD2(ApplyInterference, void(std::shared_ptr<Signal> signal, MapOfSignals));
  MOCK_CONST_METHOD0(Print, void());

  MOCK_METHOD2(ApplyInterferenceImpl, void(std::shared_ptr<Signal> signal, MapOfSignals));
  MOCK_CONST_METHOD0(PrintImpl, void());
};

#endif //WSNET_TESTS_MOCKS_DEFINITIONS_MODELS_INTERFERENCE_MODEL_H_
