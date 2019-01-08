/**
 *  \file   multiband_rf_spectrum_model.h
 *  \brief  MultiBandRFSpectrumModel Class definition
 *  \author Luiz Henrique Suraty Filho
 *  \date   July 2017
 *  \version 1.0
 **/

#ifndef WSNET_MODELS_SPECTRUM_MULTIBAND_RF_SPECTRUM_MODEL_H_
#define WSNET_MODELS_SPECTRUM_MULTIBAND_RF_SPECTRUM_MODEL_H_

#include <memory>
#include <list>
#include <kernel/include/definitions/types.h>
#include <kernel/include/definitions/models/spectrum/spectrum_model.h>
#include <kernel/include/definitions/types/signal/rf_signal.h>

class MultiBandRFSpectrumModel : public SpectrumModel{
public:
	MultiBandRFSpectrumModel(std::unique_ptr<IntervalTree>,
			std::unique_ptr<IntervalTree>,
			std::unique_ptr<RangeTree> ,
			RegisterMode);
	MultiBandRFSpectrumModel();
	~MultiBandRFSpectrumModel(){};
private:
	std::vector<std::shared_ptr<Signal>> RegisterRxNodeImpl(std::weak_ptr<RegisteredRxNode>);
	void UnregisterRxNodeImpl(std::weak_ptr<RegisteredRxNode>);
	void SignalAddTxImpl(std::shared_ptr<Signal>);
	void SignalRxBeginImpl(std::shared_ptr<Signal>);
	void SignalTxEndImpl(std::weak_ptr<Signal> signal);
	void SearchRxNodesForSignalImpl(std::weak_ptr<Signal>);
	std::vector<std::shared_ptr<Signal>> SearchSignalsForRxNodeImpl(std::weak_ptr<RegisteredRxNode>);
	void SignalRxEndImpl(std::shared_ptr<Signal>);
	std::map<nodeid_t, std::weak_ptr<RegisteredRxNode>> rx_nodes_registered_;
	std::map<SignallUid, std::weak_ptr<RFSignal>> txing_signals_;
  std::unique_ptr<IntervalTree> rx_nodes_search_tree_;
  std::unique_ptr<IntervalTree> txing_signals_search_tree_;
  std::unique_ptr<RangeTree> range_tree_;
  RegisterMode register_mode_;
};


#endif //WSNET_MODELS_SPECTRUM_MULTIBAND_RF_SPECTRUM_MODEL_H_
