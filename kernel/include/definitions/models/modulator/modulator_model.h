/**
 *  \file   modulator_model.h
 *  \brief  Modulator Model Abstract Class definition
 *  \author Arturo Guizar
 *  \date   February 2018
 *  \version 1.0
 **/

#ifndef WSNET_CORE_DEFINITIONS_MODELS_MODULATOR_MODULATOR_MODEL_H_
#define WSNET_CORE_DEFINITIONS_MODELS_MODULATOR_MODULATOR_MODEL_H_

#include <iostream>
#include <kernel/include/definitions/types.h>
#include <kernel/include/definitions/types/signal/signal.h>

/** \brief The Abstract Base Class : ModulatorModel Class
 * This means that no instance of the ModulatorModel class can exist.
 * Only classes which inherit from the ModulatorModel class can exist.
 *
 * \fn ApplyModulation() apply the modulation to the signal after interferences, errors, coding
 * \fn Print() prints information about the ModulatorModel
 * \fn GetUID() return the UID of the ModulatorModel
 * \fn ApplyModulationImpl() implements the application of modulation on the tracked signal
 * \fn PrintImpl() implements the Print function
 **/
class ModulatorModel {
public:
	ModulatorModel();
	virtual ~ModulatorModel();
	void ApplyModulation(std::shared_ptr<Signal> tracked_signal);
	void Print() const;
	ModulatorModelUid GetUID() const;
private:
	virtual void ApplyModulationImpl(std::shared_ptr<Signal>) =0 ;
	virtual void PrintImpl() const;
	static ModulatorModelUid uid_counter_;
	ModulatorModelUid uid_;
};

#endif //WSNET_CORE_DEFINITIONS_MODELS_MODULATOR_MODULATOR_MODEL_H_
