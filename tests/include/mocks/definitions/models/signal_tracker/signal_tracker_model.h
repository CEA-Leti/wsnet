/**
 *  \file   signal_tracker_model.h
 *  \brief  Signal Tracker Model Abstract Class definition
 *  \author Luiz Henrique Suraty Filho
 *  \date   July 2017
 *  \version 1.0
 **/

#ifndef WSNET_TESTS_MOCKS_DEFINITIONS_MODELS_SIGNAL_TRACKER_MODEL_H_
#define WSNET_TESTS_MOCKS_DEFINITIONS_MODELS_SIGNAL_TRACKER_MODEL_H_

#include "gmock/gmock.h"
#include <kernel/include/definitions/models/signal_tracker/signal_tracker_model.h>


class MockSignalTrackerModel: public SignalTrackerModel {
public:
  MOCK_CONST_METHOD0(GetUID, SignalTrackerModelUid());
  MOCK_METHOD0(GetSelectedSignal, std::shared_ptr<Signal>());
  MOCK_METHOD1(VerifySignalIsSelected, bool(std::shared_ptr<Signal> signal));
  MOCK_METHOD1(ReceiveSignal, bool(std::shared_ptr<Signal> signal));
  MOCK_METHOD0(ResetSelectedSignal, void());
  MOCK_CONST_METHOD0(Print, void());

  MOCK_METHOD0(GetSelectedSignalImpl, std::shared_ptr<Signal>());
  MOCK_METHOD1(VerifySignalIsSelectedImpl, bool(std::shared_ptr<Signal> signal));
  MOCK_METHOD1(ReceiveSignalImpl, bool(std::shared_ptr<Signal> signal));
  MOCK_METHOD0(ResetSelectedSignalImpl, void());
  MOCK_CONST_METHOD0(PrintImpl, void());
};

#endif //WSNET_TESTS_MOCKS_DEFINITIONS_MODELS_SIGNAL_TRACKER_MODEL_H_
