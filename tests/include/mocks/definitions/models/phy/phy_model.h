/**
 *  \file   phy_model.h
 *  \brief  PhyModel Abstract Class definition
 *  \author Luiz Henrique Suraty Filho
 *  \date   July 2017
 *  \version 1.0
 **/

#ifndef WSNET_TESTS_MOCKS_DEFINITIONS_MODELS_SPECTRUM_MODEL_H_
#define WSNET_TESTS_MOCKS_DEFINITIONS_MODELS_SPECTRUM_MODEL_H_

#include "gmock/gmock.h"
#include <kernel/include/definitions/models/phy/phy_model.h>


class MockPhyModel: public PhyModel{
public:
  MOCK_CONST_METHOD0(GetUID, PhyModellUid());
  MOCK_METHOD1(RegisterRxNode, void(SetOfFrequencyIntervals freq_intervals));
  MOCK_METHOD1(UnregisterRXNode, void());
  MOCK_METHOD1(ReceiveSignalFromSpectrum, void(std::shared_ptr<Signal> signal));
  MOCK_METHOD3(ReceivePacketFromUp_Deprecated, void(call_t *to, call_t *from,packet_t *packet));
  MOCK_METHOD1(SendPacketToUp_Deprecated, void(std::shared_ptr<Signal> signal));
  MOCK_METHOD1(SendSignalToSpectrum, void(std::shared_ptr<Signal> signal));
  MOCK_METHOD1(ReceivedSignalFromSpectrumRxEnd, void(std::shared_ptr<Signal> signal));

  MOCK_METHOD1(SetInterferenceModel, void(InterferenceModel *));
  MOCK_METHOD1(SetModulatorModel, void(ModulatorModel *));
  MOCK_METHOD1(SetErrorModel, void(ErrorModel *));
  MOCK_METHOD1(SetCodingModel, void(CodingModel *));
  MOCK_METHOD1(SetSignalTrackerModel, void(SignalTrackerModel *));
  MOCK_METHOD1(SetSpectrum, void(SpectrumModel *));

  MOCK_METHOD1(RegisterRxNodeImpl, void(SetOfFrequencyIntervals freq_intervals));
  MOCK_METHOD1(UnregisterRxNodeImpl, void());
  MOCK_METHOD1(ReceiveSignalFromSpectrumImpl, void(std::shared_ptr<Signal> signal));
  MOCK_METHOD3(ReceivePacketFromUp_DeprecatedImpl, void(call_t *to, call_t *from,packet_t *packet));
  MOCK_METHOD1(SendPacketToUp_DeprecatedImpl, void(std::shared_ptr<Signal> signal));
  MOCK_METHOD1(SendSignalToSpectrumImpl, void(std::shared_ptr<Signal> signal));
  MOCK_METHOD1(ReceivedSignalFromSpectrumRxEndImpl, void(std::shared_ptr<Signal> signal));
};


#endif //WSNET_TESTS_MOCKS_DEFINITIONS_MODELS_SPECTRUM_MODEL_H_
