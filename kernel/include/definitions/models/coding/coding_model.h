/**
 *  \file   coding_model.h
 *  \brief  Coding Model Abstract Class definition
 *  \author Luiz Henrique Suraty Filho
 *  \date   July 2017
 *  \version 1.0
 **/

#ifndef WSNET_CORE_DEFINITIONS_MODELS_CODING_CODING_MODEL_H_
#define WSNET_CORE_DEFINITIONS_MODELS_CODING_CODING_MODEL_H_

#include <iostream>
#include <kernel/include/definitions/types.h>
#include <kernel/include/definitions/types/signal/signal.h>

/** \brief The Abstract Base Class : CodingModel Class
 * This means that no instance of the CodingModel class can exist.
 * Only classes which inherit from the CodingModel class can exist.
 *
 * \fn ApplyCoding() apply the coding on the signal
 * \fn Print() prints information about the InterferenceModel
 * \fn GetUID() return the UID of the InterferenceModel
 * \fn ApplyCodingImpl() implements the application of coding on the signal
 * \fn PrintImpl() implemetns the Print function
 **/
class CodingModel {
public:
	CodingModel();
	virtual ~CodingModel();
	void ApplyCoding(std::shared_ptr<Signal> signal);
	void Print() const;
	CodingModelUid GetUID() const;
private:
	virtual void ApplyCodingImpl(std::shared_ptr<Signal>) =0 ;
	virtual void PrintImpl() const;
	static CodingModelUid uid_counter_;
	CodingModelUid uid_;
};

#endif //WSNET_CORE_DEFINITIONS_MODELS_CODING_CODING_MODEL_H_
