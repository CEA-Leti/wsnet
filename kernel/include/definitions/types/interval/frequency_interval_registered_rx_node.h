/**
 *  \file   frequency_interval_registered_rx_node.h
 *  \brief  FrequencyIntervalRegisteredRxNode Concrete Class definition
 *  \author Luiz Henrique Suraty Filho
 *  \date   July 2017
 *  \version 1.0
 **/

#ifndef WSNET_CORE_DEFINITIONS_TYPES_INTERVAL_FREQUENCY_INTERVAL_REGISTERED_RX_NODE_H_
#define WSNET_CORE_DEFINITIONS_TYPES_INTERVAL_FREQUENCY_INTERVAL_REGISTERED_RX_NODE_H_

#include <vector>
#include <memory>
#include <iostream>
#include <kernel/include/definitions/types.h>
#include <kernel/include/definitions/types/interval/interval.h>
#include <kernel/include/definitions/types/interval/frequency_interval.h>

class RegisteredRxNode;

class FrequencyIntervalRegisteredRxNode : public FrequencyInterval {
public:
	FrequencyIntervalRegisteredRxNode(const Frequency low,const Frequency high,const Frequency center,std::weak_ptr<RegisteredRxNode> rx_node_filter );
	FrequencyIntervalRegisteredRxNode(const Frequency low,const Frequency high, const Frequency center);
	void AddToRxNode(std::weak_ptr<RegisteredRxNode> RxNode);
	std::weak_ptr<RegisteredRxNode> GetRxNode() const;
private:
	std::shared_ptr<FrequencyInterval> CloneImpl();
	void PrintImpl() const;
	std::weak_ptr<RegisteredRxNode> registered_rx_node_ptr_;
};

using SetOfFrequencyIntervalRegisteredRxNode = std::vector<std::shared_ptr<FrequencyIntervalRegisteredRxNode>>;
using FrequencyIntervalRegisteredRxNodeIterator = SetOfFrequencyIntervalRegisteredRxNode::const_iterator;

#endif //WSNET_CORE_DEFINITIONS_TYPES_INTERVAL_FREQUENCY_INTERVAL_REGISTERED_RX_NODE_H_
