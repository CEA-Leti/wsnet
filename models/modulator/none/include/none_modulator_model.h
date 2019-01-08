/**
 *  \file   none_modulator_model.h
 *  \brief  NoneModulatorModel Concrete Class definition
 *  \author Arturo Guizar
 *  \date   July 2017
 *  \version 1.0
 **/

#ifndef WSNET_MODELS_MODULATOR_NONE_MODULATOR_MODEL_H_
#define WSNET_MODELS_MODULATOR_NONE_MODULATOR_MODEL_H_

#include <kernel/include/definitions/types.h>
#include <kernel/include/definitions/models/modulator/modulator_model.h>
#include <kernel/include/definitions/types/signal/signal.h>

/** \brief The Concrete Derived Class : NoneModulatorModel Class
 *		   This model introduces no modulation to the signal
 *
 * \fn ApplyModulationImpl() implements the application of modulation on the tracked signal
 * \fn PrintImpl() implemetns the Print function
 **/
class NoneModulatorModel : public ModulatorModel {
public:
	NoneModulatorModel();
	~NoneModulatorModel();
private:
	void ApplyModulationImpl(std::shared_ptr<Signal>);
	void PrintImpl() const;
};

#endif //WSNET_MODELS_MODULATOR_NONE_MODULATOR_MODEL_H_
