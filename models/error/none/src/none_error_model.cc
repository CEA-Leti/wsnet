/**
 *  \file   zero_error_model.h
 *  \brief  ZeroErrorModel Concrete Class implementation
 *  		This model introduces no (zero) error to the signal
 *  \author Luiz Henrique Suraty Filho
 *  \date   July 2017
 *  \version 1.0
 **/



#include <iostream>
#include <kernel/include/definitions/types.h>
#include <kernel/include/definitions/models.h>
#include <kernel/include/definitions/class.h>
#include <kernel/include/model_handlers/cxx_model_handlers.h>
#include "none_error_model.h"

/** \brief A structure to define the simulation module information
 *  \struct model_t
 **/
model_t model =  {
    (char*) "Zero Error Model ",
    (char*)"Luiz Henrique SURATY FILHO",
    (char*)"0.1",
    MODELTYPE_ERROR
};

// This ends up being the Factory method.
void *create_object(call_t *to, void *params) {
  if (!params)
    std::cout<<"NULL "<< to->object <<std::endl;
  void *p = TO_C(new NoneErrorModel);
  return p;
}

void destroy_object(void *object) {
  delete TO_CPP(object,NoneErrorModel);
}

int bootstrap(call_t *to) {
  class_t *my_c = get_class_by_id(to->classid);
  if (!my_c)
      std::cout<<"NULL "<< to->object <<std::endl;
  return 0;
}

NoneErrorModel::NoneErrorModel(){

}

NoneErrorModel::~NoneErrorModel(){

}

void NoneErrorModel::ApplyErrorsImpl(std::weak_ptr<Signal>){
	return;
}

void NoneErrorModel::PrintImpl() const {
	std::cout << "The Zero Error Model" << std::endl;
}
