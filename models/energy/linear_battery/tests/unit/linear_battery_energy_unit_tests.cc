/**
 *  \file   linear_battery_energy_unit_tests.cc
 *  \brief  Linear Battery Energy Model Unit Tests
 *  \author Luiz Henrique Suraty Filho
 *  \date   2018
 **/

#include <iostream>

#include <tests/include/fakes/definitions/class.h>
#include <tests/include/fakes/definitions/node.h>
#include <tests/include/fakes/modelutils.h>
#include "gtest/gtest.h"

#include "linear_battery_energy.c"

/* Preparation of the scheduler
 * We need to have the scheduler initialized
 */
// fixture
class LinearBatteryTest : public ::testing::Test {
protected:
  virtual void SetUp() {
	  list_init();
	  params_ = list_create();
	  to_ = {0,0};
	  from_ = {-1,-1};
  }

  virtual void TearDown() {
	  list_destroy(params_);
  }
public:
  list_t	*params_;
  call_t	to_;
  call_t	from_;
};


TEST_F(LinearBatteryTest, Destroy){
  init(&to_, (void*) params_);
  destroy(&to_);
  // check whether the class data has been released
  linear_battery_energy_class_private_t *classdata = (linear_battery_energy_class_private_t *) get_class_private_data(&to_);
  ASSERT_TRUE(classdata == nullptr);
}


TEST_F(LinearBatteryTest, InitNoParam){
	init(&to_, (void*) params_);

	// check whether the class data has been initialized
	linear_battery_energy_class_private_t *classdata = (linear_battery_energy_class_private_t *) get_class_private_data(&to_);
	ASSERT_TRUE(classdata != nullptr);

	// check whether default values were created
	EXPECT_EQ(0, classdata->global_consumption_J);

	destroy(&to_);
}

TEST_F(LinearBatteryTest, Unbind){
  bind(&to_, (void*) params_);
  unbind(&to_);
  // check whether the class data has been released
  linear_battery_energy_node_private_t *nodedata = (linear_battery_energy_node_private_t *) get_node_private_data(&to_);
  ASSERT_TRUE(nodedata == NULL);
}

TEST_F(LinearBatteryTest, BindNoParam){
	bind(&to_, (void*) params_);

	// check whether the class data has been initialized
	linear_battery_energy_node_private_t *nodedata = (linear_battery_energy_node_private_t *) get_node_private_data(&to_);
	ASSERT_TRUE(nodedata != nullptr);

	// check whether default values were created
	EXPECT_EQ(1000, nodedata->battery_capacity_mAh);
	EXPECT_EQ(0, nodedata->consumed_energy_J);
	EXPECT_EQ(0, nodedata->recharged_energy_J);
	EXPECT_EQ(3, nodedata->voltage_V);
	EXPECT_EQ(10800, nodedata->battery_capacity_J);
	EXPECT_EQ(10800, nodedata->available_energy_J);
	unbind(&to_);
}

TEST_F(LinearBatteryTest, Recharge){
	bind(&to_, (void*) params_);

	// check whether the class data has been initialized
	linear_battery_energy_node_private_t *nodedata = (linear_battery_energy_node_private_t *) get_node_private_data(&to_);
	ASSERT_TRUE(nodedata != nullptr);

	// check whether it is correctly recharging
	EXPECT_EQ(0, nodedata->recharged_energy_J);
	EXPECT_GE(nodedata->battery_capacity_J, nodedata->available_energy_J) << "Available energy should not exceed the battery capacity";

	recharge(&to_, 3.298);

	EXPECT_EQ(3.298, nodedata->recharged_energy_J);
	EXPECT_GE(nodedata->battery_capacity_J, nodedata->available_energy_J) << "Available energy should not exceed the battery capacity";

	recharge(&to_, 10.0);

	EXPECT_EQ(13.298, nodedata->recharged_energy_J);
	EXPECT_GE(nodedata->battery_capacity_J, nodedata->available_energy_J) << "Available energy should not exceed the battery capacity";

	unbind(&to_);
}

TEST_F(LinearBatteryTest, Consume){
	bind(&to_, (void*) params_);

	// check whether the class data has been initialized
	linear_battery_energy_node_private_t *nodedata = (linear_battery_energy_node_private_t *) get_node_private_data(&to_);
	ASSERT_TRUE(nodedata != nullptr);

	// check whether it is correctly recharging
	EXPECT_EQ(0, nodedata->consumed_energy_J);
	EXPECT_GE(nodedata->battery_capacity_J, nodedata->available_energy_J) << "Available energy should not exceed the battery capacity";

	// consume 500 mA during 1 hour
	consume(&to_, 500 , (uint64_t) 3600*SECONDS);
	double expected_consumption = (double) 3600 * 3 * 500 * 1e-3;

	EXPECT_EQ(expected_consumption, nodedata->consumed_energy_J);
	EXPECT_LE(0, nodedata->available_energy_J) << "Available energy should not go bellow 0";

  // consume 600 mA during 1 hour
	consume(&to_, 600 , (uint64_t) 3600*SECONDS);
	expected_consumption += (double) 3600 * 3 * 600 * 1e-3;

	EXPECT_EQ(expected_consumption, nodedata->consumed_energy_J);
	EXPECT_LE(0, nodedata->available_energy_J) << "Available energy should not go bellow 0";

	unbind(&to_);
}

TEST_F(LinearBatteryTest, Status){
  bind(&to_, (void*) params_);

  // check whether the class data has been initialized
  linear_battery_energy_node_private_t *nodedata = (linear_battery_energy_node_private_t *) get_node_private_data(&to_);
  ASSERT_TRUE(nodedata != nullptr);

  EXPECT_EQ(1, energy_status(&to_));

  // consume half of the capacity during 1 second
  consume(&to_, 500 , (uint64_t) 3600*SECONDS);

  EXPECT_EQ(0.5, energy_status(&to_));

  // consume other half of the capacity during 1 second
  consume(&to_, 500 ,(uint64_t) 3600*SECONDS);

  EXPECT_EQ(0, energy_status(&to_));

  unbind(&to_);
}
