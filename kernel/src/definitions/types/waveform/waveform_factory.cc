/**
 *  \file   waveform_factory.cc
 *  \brief  Waveform Factory Class implementation : It is used to create waveforms
 *  \author Luiz Henrique Suraty Filho
 *  \date   July 2017
 *  \version 1.0
 **/

#include <vector>
#include <memory>
#include <kernel/include/definitions/types/waveform/waveform.h>
#include <kernel/include/definitions/types/waveform/waveform_factory.h>

std::shared_ptr<Waveform> WaveformFactory::CreateWaveform(SetOfFrequencyIntervalWaveform frequency_intervals){
		auto new_waveform = std::make_shared<Waveform>();
		for (std::shared_ptr<FrequencyIntervalWaveform> freq : frequency_intervals){
			new_waveform->AddFrequencyInterval(freq);
			freq->AddToWaveform(new_waveform);
		}
		return new_waveform;
}


