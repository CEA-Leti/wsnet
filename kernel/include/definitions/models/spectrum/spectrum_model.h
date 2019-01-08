/**
 *  \file   spectrum_model.h
 *  \brief  SpectrumModel Abstract Class definition
 *  \author Luiz Henrique Suraty Filho
 *  \date   July 2017
 *  \version 1.0
 **/

#ifndef WSNET_CORE_DEFINITIONS_MODELS_SPECTRUM_SPECTRUM_MODEL_H_
#define WSNET_CORE_DEFINITIONS_MODELS_SPECTRUM_SPECTRUM_MODEL_H_

#include <memory>
#include <list>
#include <map>
#include <kernel/include/definitions/types.h>
#include <kernel/include/data_structures/interval_tree/interval_tree.h>
#include <kernel/include/data_structures/range_tree/range_tree.h>
#include <kernel/include/definitions/types/interval/registered_rx_node.h>
#include <kernel/include/definitions/types/signal/signal.h>
#include <kernel/include/definitions/types/interval/frequency_interval.h>

/** \brief The Abstract Base Class : SpectrumModel Class
 This means that no instance of the SpectrumModel class can exist.  Only classes which inherit from the SpectrumModel class can exist 
The relation with PHY models are: Spectrum has 1..N PhyModels connected to it whereas PhyModel has only 1 Spectrum on which it is connected.
* \fn UnregisterRXNode is used to unregister a node on the spectrum
 * \fn AddSignalTx is used to notify the spectrum a signal will be transmitted
 * \fn SignalRxBegin is called by the scheduler when a given signal will start to be received
 * \fn SignalRxEnd is called by the scheduler when the reception of a given signal is at the end
 * \fn SignalTxEnd is called by the scheduler when the transmittion of a given signal is at the end
**/
class SpectrumModel{
public:
	SpectrumModel();
	virtual ~SpectrumModel();
	std::vector<std::shared_ptr<Signal>> RegisterRXNode(std::weak_ptr<RegisteredRxNode> rx_node);
	void UnregisterRXNode(std::weak_ptr<RegisteredRxNode> rx_node);
	void AddSignalTx(std::shared_ptr<Signal> signal);
	void SignalRxBegin(std::shared_ptr<Signal> signal);
	void SignalTxEnd(std::weak_ptr<Signal> signal);
	void SignalRxEnd(std::shared_ptr<Signal> signal);
	SpectrumUid GetUID() const;
private:
	//virtual RxNodeFilter GetRegisteredNodeByIdImpl(nodeid_t) = 0;
	virtual std::vector<std::shared_ptr<Signal>> RegisterRxNodeImpl(std::weak_ptr<RegisteredRxNode>)=0;
	virtual void UnregisterRxNodeImpl(std::weak_ptr<RegisteredRxNode>)=0;
	virtual void SignalAddTxImpl(std::shared_ptr<Signal>) = 0;
	virtual void SignalRxBeginImpl(std::shared_ptr<Signal>) = 0;
	virtual void SignalTxEndImpl(std::weak_ptr<Signal> signal) = 0;
	virtual void SearchRxNodesForSignalImpl(std::weak_ptr<Signal>) = 0;
	virtual std::vector<std::shared_ptr<Signal>> SearchSignalsForRxNodeImpl(std::weak_ptr<RegisteredRxNode>) = 0;
	virtual void SignalRxEndImpl(std::shared_ptr<Signal>) = 0;
protected:
  void SearchRxNodesForSignal(std::weak_ptr<Signal> signal);
  std::vector<std::shared_ptr<Signal>> SearchSignalsForRxNode(std::weak_ptr<RegisteredRxNode> rx_node);
	static SpectrumUid uid_counter_;
	SpectrumUid uid_;
};


#endif //WSNET_CORE_DEFINITIONS_MODELS_SPECTRUM_SPECTRUM_MODEL_H_
