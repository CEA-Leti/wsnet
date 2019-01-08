/**
 *  \file   pick_first_signal_tracker_model.h
 *  \brief  Pick First Signal Tracker Model Concrete Class definition
 *  \author Luiz Henrique Suraty Filho
 *  \date   July 2017
 *  \version 1.0
 **/

#ifndef WSNET_MODELS_PICK_FIRST_SIGNAL_TRACKER_MODEL
#define WSNET_MODELS_PICK_FIRST_SIGNAL_TRACKER_MODEL

#include <iostream>
#include <kernel/include/definitions/types.h>
#include <kernel/include/definitions/models/signal_tracker/signal_tracker_model.h>
#include <kernel/include/definitions/types/signal/rf_signal.h>

/** \brief The Abstract Base Class : SignalTrackerModel Class
 * This means that no instance of the Interval class can exist.
 * Only classes which inherit from the Interval class can exist.
 *
 **/
class PickFirstSignalTrackerModel : public SignalTrackerModel {
public:
  PickFirstSignalTrackerModel();
	PickFirstSignalTrackerModel(Frequency);
	~PickFirstSignalTrackerModel();
private:
	std::shared_ptr<Signal> GetSelectedSignalImpl();
	bool ReceiveSignalImpl(std::shared_ptr<Signal>);
	bool VerifySignalIsSelectedImpl(std::shared_ptr<Signal>);
	void ResetSelectedSignalImpl();
	void PrintImpl() const;
	Frequency freq_center_;
	std::weak_ptr<Signal> current_signal_;
};

#endif //WSNET_CORE_MODELS_PICK_FIRST_SIGNAL_TRACKER_MODEL
