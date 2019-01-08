/**
 *  \file   waveform_factory.h
 *  \brief  Waveform Factory Class definition : It is used to create waveforms
 *  \author Luiz Henrique Suraty Filho
 *  \date   July 2017
 *  \version 1.0
 **/

#ifndef WSNET_CORE_DEFINITIONS_TYPES_WAVEFORM_WAVEFORM_FACTORY_H_
#define WSNET_CORE_DEFINITIONS_TYPES_WAVEFORM_WAVEFORM_FACTORY_H_

#include <vector>
#include <memory>
#include <kernel/include/definitions/types/waveform/waveform.h>
#include <kernel/include/definitions/types/interval/frequency_interval_waveform.h>

/** \brief The Factory Class : WaveformFactory Class
 *  This class is used to create new waveforms from frequency intervals.
 *
 * \fn CreateWaveform - return a shared_ptr to the newly created waveform
 **/
class WaveformFactory{
public:
	static std::shared_ptr<Waveform> CreateWaveform(SetOfFrequencyIntervalWaveform frequency_intervals);
};

#endif // WSNET_CORE_DEFINITIONS_TYPES_WAVEFORM_WAVEFORM_FACTORY_H_
