/**
 *  \file   registered_rx_node_factory.cc
 *  \brief  RegisteredRxNodeFactory Class implementation : It is used to create rx_nodes
 *  \author Luiz Henrique Suraty Filho
 *  \date   July 2017
 *  \version 1.0
 **/

#include <memory>
#include <vector>
#include <kernel/include/definitions/types.h>
#include <kernel/include/definitions/types/interval/frequency_interval.h>
#include <kernel/include/definitions/types/interval/registered_rx_node_factory.h>


std::shared_ptr<RegisteredRxNode> RegisteredRxNodeFactory::CreateRegisteredRxNode(SetOfFrequencyIntervalRegisteredRxNode frequency_intervals, nodeid_t node_id){
  auto new_rx_node = std::make_shared<RegisteredRxNode>(frequency_intervals,node_id);
  for (auto freq : frequency_intervals){
    freq->AddToRxNode(new_rx_node);
  }
  return new_rx_node;
}

std::shared_ptr<RegisteredRxNode> RegisteredRxNodeFactory::CreateRegisteredRxNode(SetOfFrequencyIntervalRegisteredRxNode frequency_intervals, nodeid_t node_id, PhyModel* phy_model){
  auto new_rx_node = std::make_shared<RegisteredRxNode>(frequency_intervals, node_id, phy_model);
  for (auto freq : frequency_intervals){
    freq->AddToRxNode(new_rx_node);
  }
  return new_rx_node;
}

std::shared_ptr<RegisteredRxNode> RegisteredRxNodeFactory::CreateRegisteredRxNode(SetOfFrequencyIntervals frequency_intervals, nodeid_t node_id, PhyModel* phy_model){
  // generate SetOfFrequencyIntervalRxNode
  SetOfFrequencyIntervalRegisteredRxNode freq_interval_rx_node;
  for (auto freq : frequency_intervals){
    auto freq_rx_node = std::make_shared<FrequencyIntervalRegisteredRxNode>(freq->GetLowPoint(),freq->GetHighPoint(),freq->GetCenter());
    freq_interval_rx_node.push_back(freq_rx_node);
  }
  auto new_rx_node = std::make_shared<RegisteredRxNode>(freq_interval_rx_node, node_id, phy_model);
  for (auto freq : freq_interval_rx_node){
    freq->AddToRxNode(new_rx_node);
  }
  return new_rx_node;
}
