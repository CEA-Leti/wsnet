/**
 *  \file   rf_interference_model.h
 *  \brief  RFSignalInterferenceModel Concrete Class definition
 *  \author Luiz Henrique Suraty Filho
 *  \date   July 2018
 *  \version 1.0
 **/

#ifndef WSNET_EXTERNAL_MODELS_LORA_INTERFERENCE_MODEL_H_
#define WSNET_EXTERNAL_MODELS_LORA_INTERFERENCE_MODEL_H_

#include <kernel/include/definitions/types.h>
#include <kernel/include/definitions/models/interference/interference_model.h>
#include <kernel/include/definitions/types/signal/signal.h>

/** \brief The Concrete Derived Class : RFSignalInterferenceModel Class
 *		   This model introduces interference to the rf signal
 *
 * \fn ApplyInterferenceImpl() implements the application of interferences on the tracked signal
 * \fn PrintImpl() implemetns the Print function
 **/
class RFSignalInterferenceModel : public InterferenceModel {
public:
	RFSignalInterferenceModel();
	~RFSignalInterferenceModel();
private:
	void ApplyInterferenceImpl(std::shared_ptr<Signal> signal, MapOfSignals Map_Of_Signals);
	void PrintImpl() const;

};

#endif //WSNET_EXTERNAL_MODELS_LORA_INTERFERENCE_MODEL_H_
