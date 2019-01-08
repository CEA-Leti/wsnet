/**
 *  \file   registered_rx_node.h
 *  \brief  Rx Node Filter Concrete Class definition
 *  \author Luiz Henrique Suraty Filho
 *  \date   July 2017
 *  \version 1.0
 **/

#include <memory>
#include <kernel/include/definitions/types.h>
#include <kernel/include/definitions/types/interval/registered_rx_node.h>

RegisteredRxNode::RegisteredRxNode(SetOfFrequencyIntervalRegisteredRxNode freqs,
    nodeid_t node_id,
    PhyModel* phy_model) :
      frequency_bands_(freqs), node_id_(node_id), phy_model_ptr_(phy_model){ };

RegisteredRxNode::RegisteredRxNode(SetOfFrequencyIntervalRegisteredRxNode freqs ,
    nodeid_t node_id) :
      frequency_bands_(freqs),node_id_(node_id){ };

RegisteredRxNode::~RegisteredRxNode(){};

nodeid_t RegisteredRxNode::GetNodeID() const {
  return node_id_;
}

void RegisteredRxNode::AddFrequencyInterval(std::shared_ptr<FrequencyIntervalRegisteredRxNode> freq) {
  frequency_bands_.push_back(freq);
}

PhyModel* RegisteredRxNode::GetPhyModel() const{
  return phy_model_ptr_;
}

SetOfFrequencyIntervalRegisteredRxNode RegisteredRxNode::GetAllFrequencyInterval() const{
  return frequency_bands_;
}
