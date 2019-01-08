/**
 *  \file   error_model.h
 *  \brief  ErrorModel Abstract Class definition
 *  \author Luiz Henrique Suraty Filho
 *  \date   July 2017
 *  \version 1.0
 **/

#ifndef WSNET_CORE_DEFINITIONS_MODELS_ERROR_ERROR_MODEL_H_
#define WSNET_CORE_DEFINITIONS_MODELS_ERROR_ERROR_MODEL_H_

#include <iostream>
#include <kernel/include/definitions/types.h>
#include <kernel/include/definitions/types/signal/signal.h>

/** \brief The Abstract Base Class : ErrorModel Class
 * This means that no instance of the ErrorModel class can exist.
 * Only classes which inherit from the ErrorModel class can exist.
 *
 * \fn ApplyErrors() apply the errors on the signal
 * \fn Print() prints information about the ErrorModel
 * \fn GetUID() return the UID of the ErrorModel
 * \fn ApplyErrorsImpl() implements the application of errors in the signal (to be implemented by derived class)
 * \fn PrintImpl() implemetns the Print function (to be implemented by derived class)
 **/
class ErrorModel {
public:
	ErrorModel();
	virtual ~ErrorModel();
	void ApplyErrors(std::weak_ptr<Signal> signal);
	void Print() const;
	ErrorModelUid GetUID() const;
private:
	virtual void ApplyErrorsImpl(std::weak_ptr<Signal>) =0 ;
	virtual void PrintImpl() const;
	static ErrorModelUid uid_counter_;
	ErrorModelUid uid_;
};

#endif //WSNET_CORE_DEFINITIONS_MODELS_ERROR_ERROR_MODEL_H_
