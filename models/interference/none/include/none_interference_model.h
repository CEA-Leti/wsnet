/**
 *  \file   none_interference_model.h
 *  \brief  NoneInterferenceModel Concrete Class definition
 *  \author Luiz Henrique Suraty Filho
 *  \date   July 2017
 *  \version 1.0
 **/

#ifndef WSNET_MODELS_INTERFERENCE_NONE_INTERFERENCE_MODEL_H_
#define WSNET_MODELS_INTERFERENCE_NONE_INTERFERENCE_MODEL_H_

#include <kernel/include/definitions/types.h>
#include <kernel/include/definitions/models/interference/interference_model.h>
#include <kernel/include/definitions/types/signal/signal.h>

/** \brief The Concrete Derived Class : NoneInterferenceModel Class
 *		   This model introduces no (zero) interference to the signal
 *
 * \fn ApplyInterferenceImpl() implements the application of interferences on the tracked signal
 * \fn PrintImpl() implemetns the Print function
 **/
class NoneInterferenceModel : public InterferenceModel {
public:
	NoneInterferenceModel();
	~NoneInterferenceModel();
private:
	void ApplyInterferenceImpl(std::shared_ptr<Signal> , MapOfSignals);
	void PrintImpl() const;
};

#endif //WSNET_MODELS_INTERFERENCE_NONE_INTERFERENCE_MODEL_H_
