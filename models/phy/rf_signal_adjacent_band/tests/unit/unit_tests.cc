/**
 *  \file   unit_tests.cc
 *  \brief  RfInterferenceModelUnit Tests
 *  \author Luiz Henrique Suraty Filho
 *  \date   2018
 **/

#include <iostream>

#include <tests/include/fakes/definitions/class.h>
#include <tests/include/fakes/definitions/node.h>
#include <tests/include/fakes/definitions/medium.h>
#include <tests/include/fakes/modelutils.h>
#include <tests/include/fakes/model_handlers/transceiver.h>
#include <tests/include/fakes/model_handlers/modulation.h>
#include <tests/include/fakes/model_handlers/interface.h>
#include <tests/include/fakes/model_handlers/noise.h>
#include <tests/include/fakes/model_handlers/node_mobility.h>
#include <tests/include/fakes/model_handlers/media_rxtx.h>
#include <tests/include/mocks/definitions/models/interference/interference_model.h>
#include <tests/include/mocks/definitions/models/signal_tracker/signal_tracker_model.h>
#include <tests/include/mocks/definitions/models/spectrum/spectrum_model.h>
#include <kernel/include/scheduler/scheduler_standard_containers.h>
#include "gtest/gtest.h"

#include "rf_signal_adjacent_band_phy_model.cc"
#include "rf_signal_adjacent_band_phy_model_api.cc"
#include "rf_signal_adjacent_band_phy_model_helper.cc"


/* Preparation of the scheduler
 * We need to have the scheduler initialized
 */
// fixture
class RFSignalAdjacentBandPhyModelUnitTest : public ::testing::Test {
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

// WARNING - THIS IS ABSOLUTELY NOT RECOMMENDED.
// After version 4 is fully operational, we come back to this issue
// We are using this temporarily to advance the clock and get_time give us
// the correct value.
extern SchedulerStandardContainers *scheduler;

std::shared_ptr<RFSignal> create_rf_simple_signal_tests(Frequency bandwidth, double power_dBm, Frequency freq_center,packet_t *packet){
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

TEST_F(RFSignalAdjacentBandPhyModelUnitTest, CreateObject){
  RFSignalAdjacentBandPhyModel * rf_phy = (RFSignalAdjacentBandPhyModel *) create_object(&to_, (void*) params_);
  ASSERT_TRUE(rf_phy != nullptr);
}



TEST_F(RFSignalAdjacentBandPhyModelUnitTest, DestroyObject){
  RFSignalAdjacentBandPhyModel * rf_phy = (RFSignalAdjacentBandPhyModel *) create_object(&to_, (void*) params_);
  destroy_object(rf_phy);
}

TEST_F(RFSignalAdjacentBandPhyModelUnitTest, ReceiveRFSignalFromSpectrum){
  RFSignalAdjacentBandPhyModel * rf_phy = (RFSignalAdjacentBandPhyModel *) create_object(&to_, (void*) params_);
  MockSignalTrackerModel sig_track;
  MockInterferenceModel interf;
  rf_phy->SetInterferenceModel(&interf);
  rf_phy->SetSignalTrackerModel(&sig_track);
  auto bandwidth = 125000;
  auto power_dBm = 14.0;
  auto freq_center = 868062500;
  auto rf_signal = create_rf_simple_signal_tests(bandwidth, power_dBm, freq_center, packet_);

  EXPECT_CALL(sig_track, ReceiveSignalImpl(testing::_)).Times(1);

  rf_phy->ReceiveSignalFromSpectrum(rf_signal);

  destroy_object(rf_phy);
}


TEST_F(RFSignalAdjacentBandPhyModelUnitTest, ReceiveMoreRfSignalFromSpectrum){
  RFSignalAdjacentBandPhyModel * rf_phy = (RFSignalAdjacentBandPhyModel *) create_object(&to_, (void*) params_);
  MockSignalTrackerModel sig_track;
  MockInterferenceModel interf;
  rf_phy->SetInterferenceModel(&interf);
  rf_phy->SetSignalTrackerModel(&sig_track);
  auto bandwidth = 125000;
  auto power_dBm = 14.0;
  auto freq_center = 868062500;
  auto rf_signal = create_rf_simple_signal_tests(bandwidth, power_dBm, freq_center, packet_);

  EXPECT_CALL(sig_track, ReceiveSignalImpl(testing::_)).Times(1);

  rf_phy->ReceiveSignalFromSpectrum(rf_signal);

  auto rf_signal_two = create_rf_simple_signal_tests(bandwidth, power_dBm, freq_center, packet_);

  EXPECT_CALL(sig_track, ReceiveSignalImpl(testing::_)).Times(1);

  rf_phy->ReceiveSignalFromSpectrum(rf_signal_two);

  auto rf_signal_three = create_rf_simple_signal_tests(bandwidth, power_dBm, freq_center, packet_);

  EXPECT_CALL(sig_track, ReceiveSignalImpl(testing::_)).Times(1);

  rf_phy->ReceiveSignalFromSpectrum(rf_signal_three);

  destroy_object(rf_phy);
}


TEST_F(RFSignalAdjacentBandPhyModelUnitTest, RxEndRFSignal){
  RFSignalAdjacentBandPhyModel * rf_phy = (RFSignalAdjacentBandPhyModel *) create_object(&to_, (void*) params_);
  MockSignalTrackerModel sig_track;
  MockInterferenceModel interf;
  rf_phy->SetInterferenceModel(&interf);
  rf_phy->SetSignalTrackerModel(&sig_track);
  auto bandwidth = 125000;
  auto power_dBm = 14.0;
  auto freq_center = 868062500;
  auto rf_signal = create_rf_simple_signal_tests(bandwidth, power_dBm, freq_center, packet_);

  EXPECT_CALL(sig_track, ReceiveSignalImpl(testing::_)).Times(1);

  ON_CALL(sig_track, VerifySignalIsSelectedImpl(testing::_)).WillByDefault(testing::Return(true));

  EXPECT_CALL(interf, ApplyInterferenceImpl(testing::_, testing::SizeIs(1))).Times(1);

  rf_phy->ReceiveSignalFromSpectrum(rf_signal);

  rf_phy->ReceivedSignalFromSpectrumRxEnd(rf_signal);

  destroy_object(rf_phy);
}

TEST_F(RFSignalAdjacentBandPhyModelUnitTest, RxEndMultipleRFSignalFirst){
  RFSignalAdjacentBandPhyModel * rf_phy = (RFSignalAdjacentBandPhyModel *) create_object(&to_, (void*) params_);
  MockSignalTrackerModel sig_track;
  MockInterferenceModel interf;
  rf_phy->SetInterferenceModel(&interf);
  rf_phy->SetSignalTrackerModel(&sig_track);
  auto bandwidth = 125000;
  auto power_dBm = 14.0;
  auto freq_center = 868062500;
  auto rf_signal = create_rf_simple_signal_tests(bandwidth, power_dBm, freq_center, packet_);
  auto rf_signal_two = create_rf_simple_signal_tests(bandwidth, power_dBm, freq_center, packet_);
  auto rf_signal_three = create_rf_simple_signal_tests(bandwidth, power_dBm, freq_center, packet_);
  rf_signal->SetBegin(0);
  rf_signal->SetEnd((Time)3*SECONDS);

  rf_signal_two->SetBegin((Time)1*SECONDS);
  rf_signal_two->SetEnd((Time)4*SECONDS);

  rf_signal_three->SetBegin((Time)2*SECONDS);
  rf_signal_three->SetEnd((Time)5*SECONDS);

  rf_phy->ReceiveSignalFromSpectrum(rf_signal);
  rf_phy->ReceiveSignalFromSpectrum(rf_signal_two);
  rf_phy->ReceiveSignalFromSpectrum(rf_signal_three);


  ON_CALL(sig_track, VerifySignalIsSelectedImpl(testing::_)).WillByDefault(testing::Return(true));

  EXPECT_CALL(interf, ApplyInterferenceImpl(testing::_, testing::SizeIs(3))).Times(1);

  scheduler->SimulationTimeAdvanceClock((Time)3*SECONDS);

  rf_phy->ReceivedSignalFromSpectrumRxEnd(rf_signal);

  destroy_object(rf_phy);
}

TEST_F(RFSignalAdjacentBandPhyModelUnitTest, RxEndMultipleRFSignalSecond){
  RFSignalAdjacentBandPhyModel * rf_phy = (RFSignalAdjacentBandPhyModel *) create_object(&to_, (void*) params_);
  MockSignalTrackerModel sig_track;
  MockInterferenceModel interf;
  rf_phy->SetInterferenceModel(&interf);
  rf_phy->SetSignalTrackerModel(&sig_track);
  auto bandwidth = 125000;
  auto power_dBm = 14.0;
  auto freq_center = 868062500;
  auto rf_signal = create_rf_simple_signal_tests(bandwidth, power_dBm, freq_center, packet_);
  auto rf_signal_two = create_rf_simple_signal_tests(bandwidth, power_dBm, freq_center,  packet_);
  auto rf_signal_three = create_rf_simple_signal_tests(bandwidth, power_dBm, freq_center, packet_);
  rf_signal->SetBegin(0);
  rf_signal->SetEnd((Time)3*SECONDS);

  rf_signal_two->SetBegin((Time)1*SECONDS);
  rf_signal_two->SetEnd((Time)4*SECONDS);

  rf_signal_three->SetBegin((Time)2*SECONDS);
  rf_signal_three->SetEnd((Time)5*SECONDS);

  rf_phy->ReceiveSignalFromSpectrum(rf_signal);
  rf_phy->ReceiveSignalFromSpectrum(rf_signal_two);
  rf_phy->ReceiveSignalFromSpectrum(rf_signal_three);


  ON_CALL(sig_track, VerifySignalIsSelectedImpl(testing::_)).WillByDefault(testing::Return(true));

  scheduler->SimulationTimeAdvanceClock((Time)3*SECONDS);

  rf_phy->ReceivedSignalFromSpectrumRxEnd(rf_signal);

  EXPECT_CALL(interf, ApplyInterferenceImpl(testing::_, testing::SizeIs(3))).Times(1);

  scheduler->SimulationTimeAdvanceClock((Time)4*SECONDS);

  rf_phy->ReceivedSignalFromSpectrumRxEnd(rf_signal_two);

  destroy_object(rf_phy);
}


TEST_F(RFSignalAdjacentBandPhyModelUnitTest, RxEndMultipleRFSignalThird){
  RFSignalAdjacentBandPhyModel * rf_phy = (RFSignalAdjacentBandPhyModel *) create_object(&to_, (void*) params_);
  MockSignalTrackerModel sig_track;
  MockInterferenceModel interf;
  rf_phy->SetInterferenceModel(&interf);
  rf_phy->SetSignalTrackerModel(&sig_track);
  auto bandwidth = 125000;
  auto power_dBm = 14.0;
  auto freq_center = 868062500;
  auto rf_signal = create_rf_simple_signal_tests(bandwidth, power_dBm, freq_center, packet_);
  auto rf_signal_two = create_rf_simple_signal_tests(bandwidth, power_dBm, freq_center, packet_);
  auto rf_signal_three = create_rf_simple_signal_tests(bandwidth, power_dBm, freq_center, packet_);
  rf_signal->SetBegin(0);
  rf_signal->SetEnd((Time)3*SECONDS);

  rf_signal_two->SetBegin((Time)1*SECONDS);
  rf_signal_two->SetEnd((Time)4*SECONDS);

  rf_signal_three->SetBegin((Time)2*SECONDS);
  rf_signal_three->SetEnd((Time)5*SECONDS);

  rf_phy->ReceiveSignalFromSpectrum(rf_signal);
  rf_phy->ReceiveSignalFromSpectrum(rf_signal_two);
  rf_phy->ReceiveSignalFromSpectrum(rf_signal_three);


  ON_CALL(sig_track, VerifySignalIsSelectedImpl(testing::_)).WillByDefault(testing::Return(true));

  scheduler->SimulationTimeAdvanceClock((Time)3*SECONDS);

  rf_phy->ReceivedSignalFromSpectrumRxEnd(rf_signal);

  scheduler->SimulationTimeAdvanceClock((Time)4*SECONDS);

  rf_phy->ReceivedSignalFromSpectrumRxEnd(rf_signal_two);

  EXPECT_CALL(interf, ApplyInterferenceImpl(testing::_, testing::SizeIs(3))).Times(1);

  scheduler->SimulationTimeAdvanceClock((Time)5*SECONDS);

  rf_phy->ReceivedSignalFromSpectrumRxEnd(rf_signal_three);


  destroy_object(rf_phy);
}

TEST_F(RFSignalAdjacentBandPhyModelUnitTest, RxEndMultipleRFSignalFourth){
  RFSignalAdjacentBandPhyModel * rf_phy = (RFSignalAdjacentBandPhyModel *) create_object(&to_, (void*) params_);
  MockSignalTrackerModel sig_track;
  MockInterferenceModel interf;
  rf_phy->SetInterferenceModel(&interf);
  rf_phy->SetSignalTrackerModel(&sig_track);
  auto bandwidth = 125000;
  auto power_dBm = 14.0;
  auto freq_center = 868062500;
  auto rf_signal = create_rf_simple_signal_tests(bandwidth, power_dBm, freq_center, packet_);
  auto rf_signal_two = create_rf_simple_signal_tests(bandwidth, power_dBm, freq_center, packet_);
  auto rf_signal_three = create_rf_simple_signal_tests(bandwidth, power_dBm, freq_center, packet_);
  auto rf_signal_four = create_rf_simple_signal_tests(bandwidth, power_dBm, freq_center, packet_);

  rf_signal->SetBegin(0);
  rf_signal->SetEnd((Time)3*SECONDS);

  rf_signal_two->SetBegin((Time)1*SECONDS);
  rf_signal_two->SetEnd((Time)4*SECONDS);

  rf_signal_three->SetBegin((Time)2*SECONDS);
  rf_signal_three->SetEnd((Time)5*SECONDS);

  rf_signal_four->SetBegin((Time)6*SECONDS);
  rf_signal_four->SetEnd((Time)7*SECONDS);

  rf_phy->ReceiveSignalFromSpectrum(rf_signal);
  rf_phy->ReceiveSignalFromSpectrum(rf_signal_two);
  rf_phy->ReceiveSignalFromSpectrum(rf_signal_three);

  ON_CALL(sig_track, VerifySignalIsSelectedImpl(testing::_)).WillByDefault(testing::Return(true));

  scheduler->SimulationTimeAdvanceClock((Time)3*SECONDS);

  rf_phy->ReceivedSignalFromSpectrumRxEnd(rf_signal);

  scheduler->SimulationTimeAdvanceClock((Time)4*SECONDS);

  rf_phy->ReceivedSignalFromSpectrumRxEnd(rf_signal_two);

  scheduler->SimulationTimeAdvanceClock((Time)5*SECONDS);

  rf_phy->ReceivedSignalFromSpectrumRxEnd(rf_signal_three);

  EXPECT_CALL(interf, ApplyInterferenceImpl(testing::_, testing::SizeIs(1))).Times(1);

  rf_phy->ReceiveSignalFromSpectrum(rf_signal_four);

  scheduler->SimulationTimeAdvanceClock((Time)7*SECONDS);

  rf_phy->ReceivedSignalFromSpectrumRxEnd(rf_signal_four);

  destroy_object(rf_phy);
}

TEST_F(RFSignalAdjacentBandPhyModelUnitTest, RxEndMultipleRFSignalFourthIntersection){
  RFSignalAdjacentBandPhyModel * rf_phy = (RFSignalAdjacentBandPhyModel *) create_object(&to_, (void*) params_);
  MockSignalTrackerModel sig_track;
  MockInterferenceModel interf;
  rf_phy->SetInterferenceModel(&interf);
  rf_phy->SetSignalTrackerModel(&sig_track);
  auto bandwidth = 125000;
  auto power_dBm = 14.0;
  auto freq_center = 868062500;
  auto rf_signal = create_rf_simple_signal_tests(bandwidth, power_dBm, freq_center,  packet_);
  auto rf_signal_two = create_rf_simple_signal_tests(bandwidth, power_dBm, freq_center,  packet_);
  auto rf_signal_three = create_rf_simple_signal_tests(bandwidth, power_dBm, freq_center, packet_);
  auto rf_signal_four = create_rf_simple_signal_tests(bandwidth, power_dBm, freq_center, packet_);

  rf_signal->SetBegin(0);
  rf_signal->SetEnd((Time)3*SECONDS);

  rf_signal_two->SetBegin((Time)1*SECONDS);
  rf_signal_two->SetEnd((Time)4*SECONDS);

  rf_signal_three->SetBegin((Time)2*SECONDS);
  rf_signal_three->SetEnd((Time)6*SECONDS);

  rf_signal_four->SetBegin((Time)5*SECONDS);
  rf_signal_four->SetEnd((Time)7*SECONDS);

  rf_phy->ReceiveSignalFromSpectrum(rf_signal);
  rf_phy->ReceiveSignalFromSpectrum(rf_signal_two);
  rf_phy->ReceiveSignalFromSpectrum(rf_signal_three);


  ON_CALL(sig_track, VerifySignalIsSelectedImpl(testing::_)).WillByDefault(testing::Return(true));

  scheduler->SimulationTimeAdvanceClock((Time)3*SECONDS);

  rf_phy->ReceivedSignalFromSpectrumRxEnd(rf_signal);

  scheduler->SimulationTimeAdvanceClock((Time)4*SECONDS);

  rf_phy->ReceivedSignalFromSpectrumRxEnd(rf_signal_two);

  rf_phy->ReceiveSignalFromSpectrum(rf_signal_four);

  scheduler->SimulationTimeAdvanceClock((Time)6*SECONDS);

  rf_phy->ReceivedSignalFromSpectrumRxEnd(rf_signal_three);

  // 4 because of the evaluation window
  // This does not cause any problem because there is also calculation of
  // intersection time in interference, but it does not have good performance
  // and it is not the best solution
  EXPECT_CALL(interf, ApplyInterferenceImpl(testing::_, testing::SizeIs(4))).Times(1);

  scheduler->SimulationTimeAdvanceClock((Time)7*SECONDS);

  rf_phy->ReceivedSignalFromSpectrumRxEnd(rf_signal_four);

  destroy_object(rf_phy);
}
