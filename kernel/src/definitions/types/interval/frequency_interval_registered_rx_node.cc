/**
 *  \file   frequency_interval_registered_rx_node.cc
 *  \brief  FrequencyIntervalRegisteredRxNode Concrete Class implementation
 *  \author Luiz Henrique Suraty Filho
 *  \date   July 2017
 *  \version 1.0
 **/

#include <vector>
#include <memory>
#include <iostream>
#include <kernel/include/definitions/types/interval/frequency_interval_registered_rx_node.h>


FrequencyIntervalRegisteredRxNode::FrequencyIntervalRegisteredRxNode(const Frequency low,
    const Frequency high,
    const Frequency center,
    std::weak_ptr<RegisteredRxNode> rx_node_filter ) :
      FrequencyInterval(low,high,center), registered_rx_node_ptr_(rx_node_filter)	{ };

FrequencyIntervalRegisteredRxNode::FrequencyIntervalRegisteredRxNode(const Frequency low,
    const Frequency high,
    const Frequency center)	:
	    FrequencyInterval(low,high,center){ };

void FrequencyIntervalRegisteredRxNode::AddToRxNode(std::weak_ptr<RegisteredRxNode> RxNode){
  registered_rx_node_ptr_ = RxNode;
}

std::weak_ptr<RegisteredRxNode> FrequencyIntervalRegisteredRxNode::GetRxNode() const {
  return registered_rx_node_ptr_;
}

std::shared_ptr<FrequencyInterval> FrequencyIntervalRegisteredRxNode::CloneImpl(){
  return std::make_shared<FrequencyIntervalRegisteredRxNode>(low_,high_,center_);
}

void FrequencyIntervalRegisteredRxNode::PrintImpl() const {
  std::cout<<" RxNode = "<<registered_rx_node_ptr_.lock()<<" listening to ["<<low_<<";"<<high_<<"]" <<std::endl;
}
