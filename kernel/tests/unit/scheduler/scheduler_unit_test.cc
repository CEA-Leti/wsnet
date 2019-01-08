/**
 *  \file   scheduler_unit_test.cc
 *  \brief  Scheduler Unit Tests
 *  \author Luiz Henrique Suraty Filho
 *  \date   2018
 **/

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <inttypes.h>

#include "gtest/gtest.h"

#include <tests/include/fakes/definitions/class.h>
#include <tests/include/fakes/definitions/node.h>
#include <tests/include/mocks/definitions/models/spectrum/spectrum_model.h>


#include <kernel/include/definitions/types/signal/rf_signal.h>

#include <kernel/include/scheduler/scheduler.h>


#include <kernel/include/scheduler/scheduler_standard_containers.h>

/* Preparation of the scheduler
 * We need to have the scheduler initialized
 */
// fixture
class SchedulerTest : public ::testing::Test {
protected:
  virtual void SetUp() {
    arg_int_=1578520;
    clock_end_ = 1000000u;
    // create the scheduler
    scheduler_ = std::make_unique<SchedulerStandardContainers>();
    scheduler_->SimulationTimeSetEnd(clock_end_);
  }

  virtual void TearDown() {
    scheduler_clean();
  }

  bool simulation_run(){
	  return true;
  }

  std::unique_ptr<SchedulerStandardContainers> scheduler_;
  int arg_int_;
  Time clock_end_;
};

int * callback_fake (call_t *to, call_t *from, void *arg) {
	  call_t exp_to = {-1,1};
	  call_t exp_from = {0,0};
	  // check whether the args address is correct
	  //ASSERT_EQ(NULL, arg);
	  EXPECT_EQ(exp_to.classid, to->classid);
	  EXPECT_EQ(exp_to.object, to->object);
	  EXPECT_EQ(exp_from.classid, from->classid);
	  EXPECT_EQ(exp_from.object, from->object);

	  * (int*) arg = 1000;
	  return NULL;
}

TEST_F(SchedulerTest, EventCreation){
  Time clock = 950u;
  auto e = std::make_unique<Event>(clock, PRIORITY_QUIT);

  EXPECT_EQ(e->clock_, clock);
  EXPECT_EQ(e->priority_, PRIORITY_QUIT);
}

TEST_F(SchedulerTest, EventComparison){
  Time clock = 950u;
  // e1 < e3 < e2
  auto e1 = Event(clock, PRIORITY_QUIT);
  auto e2 = Event(clock+100, PRIORITY_QUIT);
  auto e3 = Event(clock+100, PRIORITY_BIRTH);

  // they are equal ids
  EXPECT_TRUE(e1==e1);
  EXPECT_TRUE(e2==e2);
  EXPECT_TRUE(e3==e3);

  // e1 < e2
  EXPECT_TRUE(e1<e2);
  EXPECT_FALSE(e2<e1);
  EXPECT_TRUE(e2>e1);
  EXPECT_FALSE(e1>e2);
  EXPECT_FALSE(e1==e2);

  // e1 < e3
  EXPECT_TRUE(e1<e3);
  EXPECT_FALSE(e3<e1);
  EXPECT_TRUE(e3>e1);
  EXPECT_FALSE(e1>e3);
  EXPECT_FALSE(e1==e3);

  // e2 > e3
  EXPECT_TRUE(e2>e3);
  EXPECT_FALSE(e3>e2);
  EXPECT_TRUE(e3<e2);
  EXPECT_FALSE(e2<e3);
  EXPECT_FALSE(e2==e3);

  e2.clock_ = clock;
  EXPECT_TRUE(e1<e2);
  EXPECT_FALSE(e2<e1);
  EXPECT_TRUE(e2>e1);
  EXPECT_FALSE(e1>e2);
  EXPECT_FALSE(e1==e2);

}


TEST_F(SchedulerTest, SetEnd){
  uint64_t clock = 1000000u;
  scheduler_->SimulationTimeSetEnd(clock);

  // advance the clock so that the nodes will be created

  EXPECT_EQ(scheduler_->SimulationTimeGetEnd(), clock);
}

TEST_F(SchedulerTest, AddQuit){
  EXPECT_EQ(scheduler_->SimulationTimeGetEnd(), clock_end_);

  scheduler_->AddQuit(scheduler_->SimulationTimeGetEnd());

  scheduler_->SimulationRun();
}

TEST_F(SchedulerTest, AdvanceClock){
  Time clock = 1500u;
  scheduler_->SimulationTimeAdvanceClock(clock);
  EXPECT_EQ(scheduler_->SimulationTimeGet(), clock);

  clock = clock*2;
  scheduler_->SimulationTimeAdvanceClock(clock);
  EXPECT_EQ(scheduler_->SimulationTimeGet(), clock);
}


TEST_F(SchedulerTest, AddBirth){
  Time clock = 0u;
  int nr_nodes = 10;

  scheduler_->AddQuit(clock_end_);

  for (auto i=0 ; i<nr_nodes; i++){
    scheduler_->AddBirth(clock,i);
  }
  EXPECT_EQ(scheduler_->CountEvents(), nr_nodes+1);

  EXPECT_EQ(scheduler_->SimulationTimeGetEnd(), clock_end_);

  // advance the clock so that the nodes will be created
  scheduler_->SimulationRun();

  // check whether the number of nodes is correct
  EXPECT_EQ(get_node_count(), nr_nodes);
}

TEST_F(SchedulerTest, AddCallback){
	call_t to = {-1,1};
	call_t from = {0,0};
	uint64_t clock = 1u;
	scheduler_->AddQuit(clock_end_);
	event_t event_info= scheduler_->AddCallback(clock, to, from, (callback_t) callback_fake, (void*) &arg_int_);

	// run the simulations
	scheduler_->SimulationRun();

	// if we entered the callback, arg_int_ has changed
	EXPECT_EQ(arg_int_, 1000);

	(void) event_info;
}


TEST_F(SchedulerTest, DeleteCallback){
  call_t to = {-1,1};
  call_t from = {0,0};
  uint64_t clock = 1u;
  scheduler_->AddQuit(clock_end_);
  event_t event_info= scheduler_->AddCallback(clock, to, from, (callback_t) callback_fake, (void*) &arg_int_);

  scheduler_->DeleteCallback(event_info);

  // run the simulations
  scheduler_->SimulationRun();

  // if we did not enter the callback, arg_int_ is still the same
  EXPECT_EQ(arg_int_, 1578520);

  (void) event_info;
}

TEST_F(SchedulerTest, AddRxBegin){
  packet_t packet;
  nodeid_t source = 0;
  MockSpectrumModel spectrum;
  std::shared_ptr<RFSignal> signal = std::make_shared<RFSignal>(source, &packet);

  EXPECT_CALL(spectrum, SignalRxBeginImpl(testing::_));

  scheduler_->AddQuit(clock_end_);

  scheduler_->AddRxBegin(100, &spectrum, signal);

  // run the simulations
  scheduler_->SimulationRun();

}

TEST_F(SchedulerTest, AddRxEnd){
  packet_t packet;
  nodeid_t source = 0;
  MockSpectrumModel spectrum;
  std::shared_ptr<RFSignal> signal = std::make_shared<RFSignal>(source, &packet);

  EXPECT_CALL(spectrum, SignalRxEndImpl(testing::_));

  scheduler_->AddQuit(clock_end_);

  scheduler_->AddRxEnd(100, &spectrum, signal);

  // run the simulations
  scheduler_->SimulationRun();

}

TEST_F(SchedulerTest, AddTxEnd){
  packet_t packet;
  nodeid_t source = 0;
  MockSpectrumModel spectrum;
  std::shared_ptr<RFSignal> signal = std::make_shared<RFSignal>(source, &packet);

  EXPECT_CALL(spectrum, SignalTxEndImpl(testing::_));

  scheduler_->AddQuit(clock_end_);

  scheduler_->AddTxEnd(100, &spectrum, signal);

  // run the simulations
  scheduler_->SimulationRun();

}
