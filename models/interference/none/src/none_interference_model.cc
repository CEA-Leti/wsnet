/**
 *  \file   none_interference_model.cc
 *  \brief  NoneInterferenceModel Concrete Class implementation
 *  \author Luiz Henrique Suraty Filho
 *  \date   July 2017
 *  \version 1.0
 **/

#include <iostream>
#include <kernel/include/definitions/types.h>
#include <kernel/include/definitions/models.h>
#include <kernel/include/definitions/class.h>
#include <kernel/include/model_handlers/cxx_model_handlers.h>
#include "none_interference_model.h"

/** \brief A structure to define the simulation module information
 *  \struct model_t
 **/
model_t model =  {
    (char*) "None Interference Model ",
    (char*)"Luiz Henrique SURATY FILHO",
    (char*)"0.1",
    MODELTYPE_INTERFERENCE
};

// This ends up being the Factory method.
void *create_object(call_t *to, void *params) {
  if (!params)
    std::cout<<"NULL "<< to->object <<std::endl;
  void *p = TO_C(new NoneInterferenceModel);
  return p;
}

void destroy_object(void *object) {
  delete TO_CPP(object,NoneInterferenceModel);
}

int bootstrap(call_t *to) {
  class_t *my_c = get_class_by_id(to->classid);

  if (!my_c)
      std::cout<<"NULL "<< to->object <<std::endl;
  return SUCCESSFUL;
}

NoneInterferenceModel::NoneInterferenceModel(){

}

NoneInterferenceModel::~NoneInterferenceModel(){

}

void NoneInterferenceModel::ApplyInterferenceImpl(std::shared_ptr<Signal> , MapOfSignals){
	return;
}

void NoneInterferenceModel::PrintImpl() const{
	std::cout << "The Zero Interference Model" << std::endl;
	return;
}
