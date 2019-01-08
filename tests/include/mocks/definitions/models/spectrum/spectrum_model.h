/**
 *  \file   spectrum_model.h
 *  \brief  SpectrumModel Abstract Class definition
 *  \author Luiz Henrique Suraty Filho
 *  \date   July 2017
 *  \version 1.0
 **/

#ifndef WSNET_TESTS_MOCKS_DEFINITIONS_MODELS_SPECTRUM_MODEL_H_
#define WSNET_TESTS_MOCKS_DEFINITIONS_MODELS_SPECTRUM_MODEL_H_

#include "gmock/gmock.h"
#include <kernel/include/definitions/models/spectrum/spectrum_model.h>


class MockSpectrumModel: public SpectrumModel{
public:
  MOCK_CONST_METHOD0(GetUID, SpectrumUid());
	MOCK_METHOD1(RegisterRXNode, std::vector<std::shared_ptr<Signal>> (std::weak_ptr<RegisteredRxNode> rx_node));
	MOCK_METHOD1(UnregisterRXNode, void(std::weak_ptr<RegisteredRxNode> rx_node));
  MOCK_METHOD1(AddSignalTx, void(std::shared_ptr<Signal> signal));
  MOCK_METHOD1(SignalRxBegin, void(std::shared_ptr<Signal> signal));
  MOCK_METHOD1(SignalTxEnd, void(std::weak_ptr<Signal> signal));
  MOCK_METHOD1(SearchRxNodesForSignal, void(std::weak_ptr<Signal> signal));
  MOCK_METHOD1(SearchSignalsForRxNode, std::vector<std::shared_ptr<Signal>> (std::weak_ptr<RegisteredRxNode> rx_node));
  MOCK_METHOD1(SignalRxEnd, void(std::shared_ptr<Signal> signal));

  MOCK_METHOD1(RegisterRxNodeImpl, std::vector<std::shared_ptr<Signal>> (std::weak_ptr<RegisteredRxNode> rx_node));
  MOCK_METHOD1(UnregisterRxNodeImpl, void(std::weak_ptr<RegisteredRxNode> rx_node));
  MOCK_METHOD1(SignalAddTxImpl, void(std::shared_ptr<Signal> signal));
  MOCK_METHOD1(SignalRxBeginImpl, void(std::shared_ptr<Signal> signal));
  MOCK_METHOD1(SignalTxEndImpl, void(std::weak_ptr<Signal> signal));
  MOCK_METHOD1(SearchRxNodesForSignalImpl, void(std::weak_ptr<Signal> signal));
  MOCK_METHOD1(SearchSignalsForRxNodeImpl, std::vector<std::shared_ptr<Signal>> (std::weak_ptr<RegisteredRxNode> rx_node));
  MOCK_METHOD1(SignalRxEndImpl, void(std::shared_ptr<Signal> signal));
};


#endif //WSNET_TESTS_MOCKS_DEFINITIONS_MODELS_SPECTRUM_MODEL_H_
