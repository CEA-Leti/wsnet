#include "rf_signal_adjacent_band_phy_model.h"

bool primitive_is_set(char* primitive, packet_t *packet){

  uint * primitive_header = (uint *) packet_retrieve_field_value_ptr(packet, primitive);

  if (primitive_header!=nullptr){
    return true;
  }

  return false;

}

SetOfFrequencyIntervals create_setfrequencyintervals (const Frequency freq_center, const Frequency bandwidth,
                                                         const Frequency delta_freq_adjacent, const uint nbr_adjacent_bands) {
  SetOfFrequencyIntervals freq_intervals;

  auto freq_min = freq_center - bandwidth/2;
  auto freq_max = freq_center + bandwidth/2;


  freq_intervals.push_back(std::make_shared<FrequencyInterval>(freq_min, freq_max, freq_center));//

  auto  new_left_freq_min    = freq_min-delta_freq_adjacent;
  auto  new_left_freq_max    = freq_min-1;
  auto  new_left_freq_center = freq_min-delta_freq_adjacent/2;
  auto  new_right_freq_min    = freq_max+1;
  auto  new_right_freq_max    = freq_max+delta_freq_adjacent;
  auto  new_right_freq_center = freq_max+delta_freq_adjacent/2;

  for (uint n=0; n < nbr_adjacent_bands; ++n){
    freq_intervals.push_back(std::make_shared<FrequencyInterval>(new_left_freq_min, new_left_freq_max, new_left_freq_center));
    freq_intervals.push_back(std::make_shared<FrequencyInterval>(new_right_freq_min, new_right_freq_max , new_right_freq_center));

    new_left_freq_center = new_left_freq_min-delta_freq_adjacent/2;
    new_left_freq_max    = new_left_freq_min-1;
    new_left_freq_min    = new_left_freq_min-delta_freq_adjacent;

    new_right_freq_min    = new_right_freq_max+1;
    new_right_freq_center = new_right_freq_max+delta_freq_adjacent/2;
    new_right_freq_max    = new_right_freq_max+delta_freq_adjacent;
  }

  return freq_intervals;
}


std::shared_ptr<Waveform> create_rf_signal_waveform(const double factor_adjacent, const int number_adjacent_bands, const double txmW, const SetOfFrequencyIntervals &freq_interval_phy){
  SetOfFrequencyIntervalWaveform freq_intervals;
  auto factor=1.0-relative_sum_of_factors(factor_adjacent, number_adjacent_bands);
  auto current_adj_nbr=0;
  for (auto freq_interval : freq_interval_phy){
    auto psd = txmW*factor/(freq_interval->GetHighPoint() - freq_interval->GetLowPoint());
    freq_intervals.push_back(std::make_shared<FrequencyIntervalWaveform>(freq_interval->GetLowPoint(),freq_interval->GetHighPoint(),freq_interval->GetCenter(),psd));
    if (!current_adj_nbr){
      factor = factor_adjacent/2;
    } else if (current_adj_nbr%2==0) {
      factor = factor/2;
    }
    ++current_adj_nbr;
  }
  return WaveformFactory::CreateWaveform(freq_intervals);
}
