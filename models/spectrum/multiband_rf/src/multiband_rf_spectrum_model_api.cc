/**
 *  \file   multiband_rf_spectrum_model.cc
 *  \brief  MultiBandRFSpectrumModel Class API implementation
 *
 *          Current implementation is still a transition between
 *          WSNETv3 to WSNETv4.
 *
 *  \author Luiz Henrique Suraty Filho
 *  \date   July 2017
 *  \version 1.0
 **/

#include <kernel/include/modelutils.h>
#include "multiband_rf_spectrum_model.h"

/** \brief A structure to define the simulation module information
 *  \struct model_t
 **/
model_t model =  {
    (char*) "MultiBand RF Spectrum Model",
    (char*)"Luiz Henrique SURATY FILHO",
    (char*)"0.1",
    MODELTYPE_SPECTRUM
};

void *create_object(call_t *to, void *params) {
  if (!params){
    std::cout<<"NULL params for "<<to->classid<<std::endl;
  }
  void *p = TO_C(new MultiBandRFSpectrumModel);
  return p;
}

void destroy_object(void *object) {
  delete TO_CPP(object,MultiBandRFSpectrumModel);
}

int bootstrap(call_t *to) {
  MultiBandRFSpectrumModel *my_object = TO_CPP(get_object(to),MultiBandRFSpectrumModel);
  if (!my_object)
    std::cout<<"NULL object for "<<to->classid<<std::endl;
  return 0;
}
