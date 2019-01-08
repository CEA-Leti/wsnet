/**
 *  \file   unit_tests.cc
 *  \brief  RFSignalInterferenceModelUnit Tests
 *  \author Luiz Henrique Suraty Filho
 *  \date   2018
 **/

#include <iostream>

#include <tests/include/fakes/definitions/class.h>
#include <tests/include/fakes/definitions/node.h>
#include <tests/include/fakes/modelutils.h>
#include <tests/include/fakes/model_handlers/transceiver.h>
#include <tests/include/fakes/model_handlers/modulation.h>
#include "gtest/gtest.h"

#include "rf_signal_interference.cc"

/* Preparation of the scheduler
 * We need to have the scheduler initialized
 */
// fixture
class RFSignalInterferenceModelUnitTest : public ::testing::Test {
protected:
  virtual void SetUp() {
	  list_init();
	  params_ = list_create();
	  to_ = {1,1};
	  from_ = {0,0};
	  packet_ = (packet_t *) malloc(sizeof(packet_t));
  }

  virtual void TearDown() {
	  list_destroy(params_);
	  free(packet_);
  }
public:
  list_t	*params_;
  call_t	to_;
  call_t	from_;
  packet_t *packet_;
};

std::shared_ptr<RFSignal> create_simple_rf_signal_tests(Frequency bandwidth, double power_dBm, Frequency freq_center, packet_t *packet){
  auto freq_min = freq_center - bandwidth/2;
  auto freq_max = freq_center + bandwidth/2;
  auto psd = dBm2mW(power_dBm) / bandwidth;
  auto freq_interval = std::make_shared<FrequencyIntervalWaveform>(freq_min, freq_max, freq_center, psd);
  auto waveform = std::make_shared<Waveform>();
  waveform->AddFrequencyInterval(freq_interval);
  std::shared_ptr<RFSignal> rf_signal = std::make_shared<RFSignal>(0,packet);
  rf_signal->SetWaveform(waveform);
  return rf_signal;
}

TEST_F(RFSignalInterferenceModelUnitTest, CreateObject){
  RFSignalInterferenceModel * interf = (RFSignalInterferenceModel *) create_object(&to_, (void*) params_);
  ASSERT_TRUE(interf != nullptr);
}



TEST_F(RFSignalInterferenceModelUnitTest, DestroyObject){
  RFSignalInterferenceModel * interf = (RFSignalInterferenceModel *) create_object(&to_, (void*) params_);
  destroy_object(interf);
}


TEST_F(RFSignalInterferenceModelUnitTest, SingleSignalNoSelfInterference){
  RFSignalInterferenceModel * interf = (RFSignalInterferenceModel *) create_object(&to_, (void*) params_);
  auto bandwidth = 125000;
  auto power_dBm = 14.0;
  auto freq_center = 868062500;
  auto desired_signal = create_simple_rf_signal_tests(bandwidth, power_dBm, freq_center, packet_);
  MapOfSignals interferers;
  interferers.insert(std::make_pair(desired_signal->GetUID(),desired_signal));

  interf->ApplyInterference(desired_signal, interferers);

  // SINR should be equal to SNR
  std::cout<<desired_signal->GetSINR()<<std::endl;
  EXPECT_FLOAT_EQ(desired_signal->GetSINR(), desired_signal->GetSNR());
  destroy_object(interf);
}

TEST_F(RFSignalInterferenceModelUnitTest, SameBandwidthZeroDuration){
  RFSignalInterferenceModel * interf = (RFSignalInterferenceModel *) create_object(&to_, (void*) params_);
  auto bandwidth = 125000;
  auto power_dBm = 14.0;
  auto freq_center = 868062500;
  auto desired_signal = create_simple_rf_signal_tests(bandwidth, power_dBm, freq_center, packet_);
  auto interferer_signal = create_simple_rf_signal_tests(bandwidth, power_dBm, freq_center, packet_);
  MapOfSignals interferers;
  interferers.insert(std::make_pair(desired_signal->GetUID(),desired_signal));
  interferers.insert(std::make_pair(interferer_signal->GetUID(),interferer_signal));
  interf->ApplyInterference(desired_signal, interferers);

  EXPECT_FLOAT_EQ(desired_signal->GetSINR(), desired_signal->GetSNR());

  destroy_object(interf);
}

TEST_F(RFSignalInterferenceModelUnitTest, SameBandwidthSameDuration){
  RFSignalInterferenceModel * interf = (RFSignalInterferenceModel *) create_object(&to_, (void*) params_);
  auto bandwidth = 125000;
  auto power_dBm = 14.0;
  auto freq_center = 868062500;
  auto desired_signal = create_simple_rf_signal_tests(bandwidth, power_dBm, freq_center, packet_);
  desired_signal->SetEnd(1*SECONDS);
  auto interferer_signal = create_simple_rf_signal_tests(bandwidth, power_dBm, freq_center, packet_);
  interferer_signal->SetEnd(1*SECONDS);
  MapOfSignals interferers;
  interferers.insert(std::make_pair(desired_signal->GetUID(),desired_signal));
  interferers.insert(std::make_pair(interferer_signal->GetUID(),interferer_signal));
  interf->ApplyInterference(desired_signal, interferers);

  EXPECT_FLOAT_EQ(desired_signal->GetSINR(), 1.0);

  destroy_object(interf);
}

TEST_F(RFSignalInterferenceModelUnitTest, SameBandwidthHalfDuration){
  RFSignalInterferenceModel * interf = (RFSignalInterferenceModel *) create_object(&to_, (void*) params_);
  auto bandwidth = 125000;
  auto power_dBm = 14.0;
  auto freq_center = 868062500;
  auto desired_signal = create_simple_rf_signal_tests(bandwidth, power_dBm, freq_center, packet_);
  desired_signal->SetEnd(1*SECONDS);
  auto interferer_signal = create_simple_rf_signal_tests(bandwidth, power_dBm, freq_center, packet_);
  interferer_signal->SetEnd(SECONDS/2);
  MapOfSignals interferers;
  interferers.insert(std::make_pair(desired_signal->GetUID(),desired_signal));
  interferers.insert(std::make_pair(interferer_signal->GetUID(),interferer_signal));

  interf->ApplyInterference(desired_signal, interferers);

  EXPECT_FLOAT_EQ(desired_signal->GetSINR(), 2.0);

  destroy_object(interf);
}

