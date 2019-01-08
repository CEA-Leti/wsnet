/**
 *  \file   none_coding_model.cc
 *  \brief  NoneCodingModel Concrete Class implementation
 *  		This model introduces no (zero) coding to the signal
 *  \author Luiz Henrique Suraty Filho
 *  \date   July 2017
 *  \version 1.0
 **/

#include <iostream>
#include <kernel/include/modelutils.h>
#include "none_coding_model.h"

/** \brief A structure to define the simulation module information
 *  \struct model_t
 **/
model_t model =  {
    (char*) "None Coding Model ",
    (char*)"Luiz Henrique SURATY FILHO",
    (char*)"0.1",
    MODELTYPE_CODING
};

// This ends up being the Factory method.
void *create_object(call_t *to, void *params) {
  if (!params)
    std::cout<<"NULL "<< to->object <<std::endl;
  void *p = TO_C(new NoneCodingModel);
  return p;
}

void destroy_object(void *object) {
  delete TO_CPP(object,NoneCodingModel);
}

int bootstrap(call_t *to) {
  class_t *my_c = get_class_by_id(to->classid);
  if (!my_c)
      std::cout<<"NULL "<< to->object <<std::endl;

  return 0;
}

NoneCodingModel::NoneCodingModel(){

}


NoneCodingModel::~NoneCodingModel(){

}

void NoneCodingModel::ApplyCodingImpl(std::shared_ptr<Signal>){

}
void NoneCodingModel::PrintImpl() const{
	std::cout << "The None Coding Model" << std::endl;
}
