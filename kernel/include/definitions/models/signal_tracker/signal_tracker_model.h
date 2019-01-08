/**
 *  \file   signal_tracker_model.h
 *  \brief  Signal Tracker Model Abstract Class definition
 *  \author Luiz Henrique Suraty Filho
 *  \date   July 2017
 *  \version 1.0
 **/

#ifndef WSNET_CORE_DEFINITIONS_MODELS_SIGNAL_TRACKER_SIGNAL_TRACKER_MODEL_H_
#define WSNET_CORE_DEFINITIONS_MODELS_SIGNAL_TRACKER_SIGNAL_TRACKER_MODEL_H_

#include <iostream>
#include <kernel/include/definitions/types.h>
#include <kernel/include/definitions/types/signal/signal.h>

/** \brief The Abstract Base Class : SignalTrackerModel Class
 * This means that no instance of the Interval class can exist.
 * Only classes which inherit from the Interval class can exist.
 *
 * \fn GetSelectedSignal() return the selected signal
 * \fn ResetSelectedSignal() resets the selected signal
 * \fn VerifySignalIsSelected() checks if the signal is selected or not
 * \fn ReceiveSignal() receive a new signal
 * \fn Print() prints information about the SignalTrackerModel
 * \fn GetUID() return the UID of the SignalTrackerModel
 * \fn VerifySignalIsSelectedImpl() implements the method that checks if the signal is selected or not (to be implemented by derived class)
 * \fn GetSelectedSignalImpl() implements the function that get the selected the signal that will be received (to be implemented by derived class)
 * \fn ResetSelectedSignalImpl() implements the function that resets the selected the signal (to be implemented by derived class)
 * \fn ReceiveSignalImpl() implements the function that selects the signal that will be received (to be implemented by derived class)
 * \fn PrintImpl() implemetns the Print function (to be implemented by derived class)
 **/
class SignalTrackerModel {
public:
	SignalTrackerModel();
	virtual ~SignalTrackerModel();
	std::shared_ptr<Signal> GetSelectedSignal();
	bool VerifySignalIsSelected(std::shared_ptr<Signal> signal);
	bool ReceiveSignal(std::shared_ptr<Signal> signal);
	void ResetSelectedSignal();
	void Print() const;
	SignalTrackerModelUid GetUID() const;
private:
	virtual std::shared_ptr<Signal> GetSelectedSignalImpl() =0 ;
	virtual void ResetSelectedSignalImpl() = 0;
	virtual void PrintImpl() const;
	virtual bool ReceiveSignalImpl(std::shared_ptr<Signal>) = 0;
	virtual bool VerifySignalIsSelectedImpl(std::shared_ptr<Signal>) = 0;
	static SignalTrackerModelUid uid_counter_;
	SignalTrackerModelUid uid_;
};

#endif //WSNET_CORE_DEFINITIONS_MODELS_SIGNAL_TRACKER_SIGNAL_TRACKER_MODEL_H_
