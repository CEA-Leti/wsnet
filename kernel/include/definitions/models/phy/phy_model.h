/**
 *  \file   phy_model.h
 *  \brief  PhyModel Abstract Class definition
 *  \author Luiz Henrique Suraty Filho
 *  \date   July 2017
 *  \version 1.0
 **/

#ifndef WSNET_CORE_DEFINITIONS_MODELS_PHY_PHY_MODEL_H_
#define WSNET_CORE_DEFINITIONS_MODELS_PHY_PHY_MODEL_H_

#include <iostream>
#include <kernel/include/definitions/types.h>
#include <kernel/include/definitions/types/signal/signal.h>
#include <kernel/include/definitions/types/interval/registered_rx_node.h>
#include <kernel/include/definitions/models/signal_tracker/signal_tracker_model.h>
#include <kernel/include/definitions/models/interference/interference_model.h>
#include <kernel/include/definitions/models/modulator/modulator_model.h>
#include <kernel/include/definitions/models/error/error_model.h>
#include <kernel/include/definitions/models/spectrum/spectrum_model.h>
#include <kernel/include/definitions/models/coding/coding_model.h>

/** \brief The Abstract Base Class : PhyModel Class
 * This means that no instance of the PhyModel class can exist.
 * Only classes which inherit from the PhyModel class can exist.
 * This model is used to connect nodes to the spectrum
 * The relation with spectrums are:
 *
 *     Spectrum has 1..N PhyModels connected to it
 *     PhyModel has only 1 Spectrum on which it is connected
 *
 * \fn ReceiveSignalFromSpectrum is used to receive a signal from the spectrum
 * \fn SendSignalToSpectrum is used to send a signal to the spectrum
 * \fn RegisterRxNode is used to register a node on the Rx list of the spectrum
 * \fn UnregisterRxNode is used to remove a node of the Rx list of the spectrum
 * \fn ReceiveSignalFromSpectrumImpl implements the reception of a signal from the spectrum
 * \fn SendSignalToSpectrum implements the transmition of a signal to the spectrum
 * \fn RegisterRxNode implements how to register the node on the Rx list of the spectrum
 * \fn UnregisterRxNode implements how to remove the node of the Rx list of the spectrum
 * \fn ReceivedSignalFromSpectrumRxEnd is to be called on Rx End of the signal
 **/
class PhyModel : public std::enable_shared_from_this<PhyModel>{
public:
	PhyModel();
	virtual ~PhyModel();
	void ReceiveSignalFromSpectrum(std::shared_ptr<Signal> signal);
	void ReceivePacketFromUp_Deprecated(call_t *to, call_t *from,packet_t *packet);
	void SendPacketToUp_Deprecated(std::shared_ptr<Signal> signal);
	void SendSignalToSpectrum(std::shared_ptr<Signal> signal);
	void RegisterRxNode(SetOfFrequencyIntervals freq_intervals);
	void UnregisterRxNode();
	void ReceivedSignalFromSpectrumRxEnd(std::shared_ptr<Signal> signal);
	void SetInterferenceModel(InterferenceModel *);
	void SetModulatorModel(ModulatorModel *);
	void SetErrorModel(ErrorModel *);
	void SetCodingModel(CodingModel *);
	void SetSignalTrackerModel(SignalTrackerModel *);
	void SetSpectrum(SpectrumModel *);
	void Print() const;
	PhyModellUid GetUID() const;
private:
	virtual void ReceiveSignalFromSpectrumImpl(std::shared_ptr<Signal>)= 0;
	virtual void ReceivePacketFromUp_DeprecatedImpl(call_t *to, call_t *from,packet_t *) = 0;
	virtual void SendPacketToUp_DeprecatedImpl(std::shared_ptr<Signal>) = 0;
	virtual void SendSignalToSpectrumImpl(std::shared_ptr<Signal>)= 0;
	virtual void RegisterRxNodeImpl(SetOfFrequencyIntervals) = 0;
	virtual void UnregisterRxNodeRxNodeImpl() = 0;
	virtual void ReceivedSignalFromSpectrumRxEndImpl(std::shared_ptr<Signal> )=0;
	virtual void PrintImpl() const;
	static PhyModellUid uid_counter_;
	PhyModellUid uid_;
protected:
	InterferenceModel * interference_model_;
	ModulatorModel * modulator_model_;
	ErrorModel * error_model_;
	CodingModel * coding_model_;
	SignalTrackerModel * signal_tracker_model_; // signal tracker is here temporarily. It should be on radio. Radio does not receive signals yet.
	MapOfSignals received_signals_; // a map containing all received signal being treated at the moment
	SpectrumModel * spectrum_;	// The spectrum on which it is connected
	std::shared_ptr<RegisteredRxNode> registered_rx_node_;
};

#endif //WSNET_CORE_DEFINITIONS_MODELS_PHY_PHY_MODEL_H_
