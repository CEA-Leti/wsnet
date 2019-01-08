/**
 *  \file   none_error_model.h
 *  \brief  NoneErrorModel Concrete Class definition
 *  		This model introduces no (zero) error to the signal
 *  \author Luiz Henrique Suraty Filho
 *  \date   July 2017
 *  \version 1.0
 **/

#ifndef WSNET_MODELS_ERROR_NONE_ERROR_MODEL_H_
#define WSNET_MODELS_ERROR_NONE_ERROR_MODEL_H_

#include <iostream>
#include <kernel/include/definitions/types.h>
#include <kernel/include/definitions/models/error/error_model.h>
#include <kernel/include/definitions/types/signal/signal.h>

/** \brief The Concrete Derived Class : NoneErrorModel Class
 * 		   This model introduces no (zero) error to the signal
 *
 * \fn ApplyErrorsImpl() implements the application of errors in the signal
 * \fn PrintImpl() implemetns the Print function
 **/
class NoneErrorModel : public ErrorModel {
public:
	NoneErrorModel();
	~NoneErrorModel();
private:
	void ApplyErrorsImpl(std::weak_ptr<Signal>);
	void PrintImpl() const;
};

#endif //WSNET_MODELS_ERROR_NONE_ERROR_MODEL_H_
