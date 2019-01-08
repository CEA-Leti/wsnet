/**
 *  \file   none_modulator_model.cc
 *  \brief  NoneModulatorModel Concrete Class implementation
 *  \author Arturo Guizar
 *  \date   July 2017
 *  \version 1.0
 **/

#include <iostream>
#include <kernel/include/definitions/types.h>
#include <kernel/include/definitions/models.h>
#include <kernel/include/definitions/class.h>
#include <kernel/include/model_handlers/cxx_model_handlers.h>
#include "none_modulator_model.h"

/** \brief A structure to define the simulation module information
 *  \struct model_t
 **/
model_t model =  {
    (char*) "None Modulator Model ",
    (char*)"Arturo GUIZAR",
    (char*)"0.1",
    MODELTYPE_MODULATOR
};

// This ends up being the Factory method.
void *create_object(call_t *to, void *params) {
  if (!params)
    std::cout<<"NULL "<< to->object <<std::endl;
  void *p = TO_C(new NoneModulatorModel);
  return p;
}

void destroy_object(void *object) {
  delete TO_CPP(object,NoneModulatorModel);
}

int bootstrap(call_t *to) {
  class_t *my_c = get_class_by_id(to->classid);

  if (!my_c)
      std::cout<<"NULL "<< to->object <<std::endl;
  return SUCCESSFUL;
}

NoneModulatorModel::NoneModulatorModel(){

}

NoneModulatorModel::~NoneModulatorModel(){

}

void NoneModulatorModel::ApplyModulationImpl(std::shared_ptr<Signal>){
	return;
}

void NoneModulatorModel::PrintImpl() const{
	std::cout << "The Zero Modulator Model" << std::endl;
	return;
}
