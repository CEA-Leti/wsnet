/**
 *  \file   rf_simple_adjacent_band_phy_model.h
 *  \brief  RFSignalAdjacentBandPhyModel Concrete Class definition
 *  \author Luiz Henrique Suraty Filho
 *  \date   July 2017
 *  \version 1.0
 **/

#ifndef WSNET_MODELS_PHY_RF_SIGNAL_ADJACENT_BAND_PHY_MODEL_H_
#define WSNET_MODELS_PHY_RF_SIGNAL_ADJACENT_BAND_PHY_MODEL_H_

#include <iostream>
#include <set>
#include <kernel/include/modelutils.h>
#include <kernel/include/definitions/types/signal/rf_signal.h>
#include <kernel/include/definitions/types/interval/registered_rx_node_factory.h>
#include <kernel/include/definitions/types/signal/rf_signal_factory.h>
#include <kernel/include/definitions/types/interval/frequency_interval.h>
#include <kernel/include/definitions/types/interval/frequency_interval_waveform.h>
#include <kernel/include/definitions/models/phy/phy_model.h>

double relative_sum_of_factors(double initial_factor, int number_bands);

bool primitive_is_set(char* primitive, packet_t *packet);

SetOfFrequencyIntervals create_setfrequencyintervals(const Frequency freq_center, const Frequency bandwidth,
                                                         const Frequency delta_freq_adjacent, const uint nbr_adjacent_bands);

std::shared_ptr<Waveform> create_rf_signal_waveform(const double factor_adjacent, const int number_adjacent_bands, const double txmW, const SetOfFrequencyIntervals &freq_interval_phy);

/** \brief The Concrete Class : LoRaRFAdjacentBandPhyModel Class
 * This means that no instance of the PhyModel class can exist.
 * Only classes which inherit from the PhyModel class can exist.
 * This model is used to connect nodes to the spectrum
 * The relation with spectrums are:
 *
 *     Spectrum has 1..N PhyModels connected to it
 *     PhyModel has only 1 Spectrum on which it is connected
 *
 * \fn ReceiveSignalFromSpectrum is used to receive a signal from the spectrum
 * \fn SendSignalToSpectrum is used to send a signal to the spectrum
 * \fn RegisterRxNode is used to register a node on the Rx list of the spectrum
 * \fn UnregisterRxNode is used to remove a node of the Rx list of the spectrum
 * \fn ReceiveSignalFromSpectrumImpl implements the reception of a signal from the spectrum
 * \fn SendSignalToSpectrum implements the transmition of a signal to the spectrum
 * \fn RegisterRxNode implements how to register the node on the Rx list of the spectrum
 * \fn UnregisterRxNode implements how to remove the node of the Rx list of the spectrum
 * \fn ReceivedSignalFromSpectrumRxEnd is to be called on Rx End of the signal
 **/
class RFSignalAdjacentBandPhyModel : public PhyModel{
 public:

  RFSignalAdjacentBandPhyModel(Frequency freq_center, double factor_adjacent, uint number_adjacent_bands,
                           Frequency delta_adjacent_bands, Frequency bandwidth, int log_status, nodeid_t nodeid) ;
  ~RFSignalAdjacentBandPhyModel();
  SetOfFrequencyIntervals GetOperatingFrequencyIntervalsTx();
  void UpdateFrequencyIntervalsTx (SetOfFrequencyIntervals freq_intervals);
  void UpdateFrequencyIntervalsRx (SetOfFrequencyIntervals freq_intervals);
  int  GetLogStatus();
 private:
  void ReceiveSignalFromSpectrumImpl(std::shared_ptr<Signal>);
  void SendPacketToUp_DeprecatedImpl(std::shared_ptr<Signal>);
  void SendSignalToSpectrumImpl(std::shared_ptr<Signal>);
  void RegisterRxNodeImpl(SetOfFrequencyIntervals);
  void UnregisterRxNodeRxNodeImpl();
  void ReceivedSignalFromSpectrumRxEndImpl(std::shared_ptr<Signal> );
  void PrintImpl() const;
  void ReceivePacketFromUp_DeprecatedImpl(call_t *to, call_t *from,packet_t * packet);
  SetOfFrequencyIntervals frequency_intervals_tx_; // the set of frequency intervals the phy model is operating
  SetOfFrequencyIntervals frequency_intervals_rx_; // the set of frequency intervals the phy model is operating
  Frequency freq_center_ = 868075000;
  double factor_adjacent_;
  uint number_adjacent_bands_ = 0;
  Frequency delta_adjacent_bands_ = 1000;
  Frequency bandwidth_ = 100000;
  std::multimap<Time, SignallUid> signals_already_treated_;
  Time evaluation_window_ = 0;
  int log_status_;
  nodeid_t nodeid_;
};

#endif //WSNET_MODELS_PHY_RF_SIGNAL_ADJACENT_BAND_PHY_MODEL_H_
