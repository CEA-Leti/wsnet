/**
 *  \file   registered_rx_node_factory.h
 *  \brief  RegisteredRxNodeFactory Class definition : It is used to create rx_nodes
 *  \author Luiz Henrique Suraty Filho
 *  \date   July 2017
 *  \version 1.0
 **/

#ifndef WSNET_CORE_DEFINITIONS_TYPES_INTERVAL_REGISTERED_RX_NODE_FACTORY_H_
#define WSNET_CORE_DEFINITIONS_TYPES_INTERVAL_REGISTERED_RX_NODE_FACTORY_H_

#include <memory>
#include <kernel/include/definitions/types.h>
#include <kernel/include/definitions/types/interval/frequency_interval.h>
#include <kernel/include/definitions/types/interval/registered_rx_node.h>


class RegisteredRxNodeFactory{
  public:
    static std::shared_ptr<RegisteredRxNode> CreateRegisteredRxNode(SetOfFrequencyIntervalRegisteredRxNode frequency_intervals, nodeid_t node_id);
    static std::shared_ptr<RegisteredRxNode> CreateRegisteredRxNode(SetOfFrequencyIntervalRegisteredRxNode frequency_intervals, nodeid_t node_id, PhyModel* phy_model);
    static std::shared_ptr<RegisteredRxNode> CreateRegisteredRxNode(SetOfFrequencyIntervals frequency_intervals, nodeid_t node_id, PhyModel* phy_model);
};

#endif // WSNET_CORE_DEFINITIONS_TYPES_INTERVAL_REGISTERED_RX_NODE_FACTORY_H_
