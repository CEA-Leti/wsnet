/**
 *  \file   phy_model_factory.h
 *  \brief  PhyModel Factory Abstract Class definition
 *  \author Luiz Henrique Suraty Filho
 *  \date   July 2017
 *  \version 1.0
 **/

#ifndef WSNET_CORE_DEFINITIONS_MODELS_PHY_PHY_MODEL_FACTORY_H_
#define WSNET_CORE_DEFINITIONS_MODELS_PHY_PHY_MODEL_FACTORY_H_

#include <memory>
#include <kernel/include/definitions/types.h>
#include <kernel/include/definitions/models/phy/phy_model.h>


/** \brief The template class for phymodel factories
 *
 * Any phymodel factory should implement these methods:
 *
 * \fn CreatePhyModel() return a newly created signal of type TClass;
 **/
template <class TBaseClass, class TClass>
class PhyModelFactory {
public:
	static std::shared_ptr<TBaseClass> CreatePhyModel(std::shared_ptr<InterferenceModel> interference, std::shared_ptr<ErrorModel> error,
			std::shared_ptr<CodingModel> coding ,std::shared_ptr<SignalTrackerModel> signal_tracker, std::weak_ptr<SpectrumModel> spectrum){return TClass::CreatePhyModelImpl(interference,error,coding,signal_tracker,spectrum);}
};


#endif // WSNET_CORE_DEFINITIONS_MODELS_PHY_PHY_MODEL_FACTORY_H_
