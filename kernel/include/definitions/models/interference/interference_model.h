/**
 *  \file   interference_model.h
 *  \brief  Interference Model Abstract Class definition
 *  \author Luiz Henrique Suraty Filho
 *  \date   July 2017
 *  \version 1.0
 **/

#ifndef WSNET_CORE_DEFINITIONS_MODELS_INTERFERENCE_INTERFERENCE_MODEL_H_
#define WSNET_CORE_DEFINITIONS_MODELS_INTERFERENCE_INTERFERENCE_MODEL_H_

#include <iostream>
#include <kernel/include/definitions/types.h>
#include <kernel/include/definitions/types/signal/signal.h>

/** \brief The Abstract Base Class : InterferenceModel Class
 * This means that no instance of the InterferenceModel class can exist.
 * Only classes which inherit from the InterferenceModel class can exist.
 *
 * \fn ApplyInterference() apply the interferences of interferers on the tracked signall
 * \fn Print() prints information about the InterferenceModel
 * \fn GetUID() return the UID of the InterferenceModel
 * \fn ApplyInterferenceImpl() implements the application of interferences on the tracked signal
 * \fn PrintImpl() implements the Print function
 **/
class InterferenceModel {
public:
	InterferenceModel();
	virtual ~InterferenceModel();
	void ApplyInterference(std::shared_ptr<Signal> tracked_signal, MapOfSignals interferers);
	void Print() const;
	InterferenceModelUid GetUID() const;
private:
	virtual void ApplyInterferenceImpl(std::shared_ptr<Signal> , MapOfSignals) =0 ;
	virtual void PrintImpl() const;
	static InterferenceModelUid uid_counter_;
	InterferenceModelUid uid_;
};

#endif //WSNET_CORE_DEFINITIONS_MODELS_INTERFERENCE_INTERFERENCE_MODEL_H_
