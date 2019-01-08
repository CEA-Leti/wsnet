/**
 *  \file   registered_rx_node.h
 *  \brief  Rx Node Filter Concrete Class definition
 *  \author Luiz Henrique Suraty Filho
 *  \date   July 2017
 *  \version 1.0
 **/

#ifndef WSNET_CORE_DEFINITIONS_TYPES_INTERVAL_REGISTERED_RX_NODE_H_
#define WSNET_CORE_DEFINITIONS_TYPES_INTERVAL_REGISTERED_RX_NODE_H_

#include <memory>
#include <kernel/include/definitions/types.h>
#include <kernel/include/definitions/types/interval/frequency_interval_registered_rx_node.h>

// Forward declaration of PhyModel is needed as we use the phy_model_
// We may need to come back to this issue to avoid using forward declaration
class PhyModel;

class RegisteredRxNode{
public:
	RegisteredRxNode(SetOfFrequencyIntervalRegisteredRxNode freqs,nodeid_t node_id, PhyModel* phy_model);
	RegisteredRxNode(SetOfFrequencyIntervalRegisteredRxNode freqs ,nodeid_t node_id);
	~RegisteredRxNode();
	nodeid_t GetNodeID() const;
	void AddFrequencyInterval(std::shared_ptr<FrequencyIntervalRegisteredRxNode> freq);
	PhyModel* GetPhyModel() const;
	SetOfFrequencyIntervalRegisteredRxNode GetAllFrequencyInterval() const;
private:
	SetOfFrequencyIntervalRegisteredRxNode frequency_bands_;
	nodeid_t node_id_;
	PhyModel* phy_model_ptr_;
};


#endif //WSNET_CORE_DEFINITIONS_TYPES_INTERVAL_REGISTERED_RX_NODE_H_
