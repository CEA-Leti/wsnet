/**
 *  \file   none_coding_model.h
 *  \brief  NoneCodingModel Concrete Class definition
 *  		This model introduces no (zero) coding to the signal
 *  \author Luiz Henrique Suraty Filho
 *  \date   July 2017
 *  \version 1.0
 **/

#ifndef WSNET_MODELS_NONE_CODING_MODEL_H_
#define WSNET_MODELS_NONE_CODING_MODEL_H_

#include <kernel/include/definitions/types.h>
#include <kernel/include/definitions/models/coding/coding_model.h>
#include <kernel/include/definitions/types/signal/signal.h>

using CodingModelUid = uint;

/** \brief The Concrete Derived Class : NoneCodingModel Class
 *		   This model introduces no (zero) coding to the signal
 *
 * \fn ApplyCodingImpl() implements the application of coding on the signal
 * \fn PrintImpl() implemetns the Print function
 **/
class NoneCodingModel : public CodingModel{
public:
	NoneCodingModel();
	virtual ~NoneCodingModel();
private:
	void ApplyCodingImpl(std::shared_ptr<Signal>);
	void PrintImpl() const;
};

#endif //WSNET_MODELS_NONE_CODING_MODEL_H_
