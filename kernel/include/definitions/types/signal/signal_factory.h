/**
 *  \file   signal_factory.h
 *  \brief  Signal Factory Abstract Class definition
 *  \author Luiz Henrique Suraty Filho
 *  \date   July 2017
 *  \version 1.0
 **/

#ifndef WSNET_CORE_DEFINITIONS_TYPES_SIGNAL_SIGNAL_FACTORY_H_
#define WSNET_CORE_DEFINITIONS_TYPES_SIGNAL_SIGNAL_FACTORY_H_

#include <memory>
#include <kernel/include/definitions/types.h>
#include <kernel/include/definitions/types/waveform/waveform.h>

/** \brief The template class for signal factories
 *
 * Any signal factory should implement these methods:
 *
 * \fn CreateSignal() return a newly created signal of type TClass;
 **/
template <class TBaseClass, class TClass>
class SignalFactory {
  public:

    static std::shared_ptr<TBaseClass> CreateSignal(nodeid_t source,std::shared_ptr<Waveform> waveform, packet_t *packet){return TClass::CreateSignalImpl(source, waveform,packet);}

    static std::shared_ptr<TBaseClass> CreateSignal(nodeid_t source,std::shared_ptr<Waveform> waveform, SpreadingFactor sf, Frequency freq_center, Frequency bandwidth, packet_t *packet){return TClass::CreateSignalImpl(source, waveform,sf,freq_center,bandwidth,packet);}

    static std::shared_ptr<TBaseClass> CreateSignal(call_t *to_interface, call_t *from_interface,std::shared_ptr<Waveform> waveform,packet_t *packet){return TClass::CreateSignalImpl(to_interface,from_interface, waveform,packet);}

    static std::shared_ptr<TBaseClass> CreateSignal(call_t *to_interface, call_t *from_interface,std::shared_ptr<Waveform> waveform,packet_t *packet, Time T_begin, Time T_end){return TClass::CreateSignalImpl(to_interface,from_interface, waveform,packet, T_begin, T_end);}

    static std::shared_ptr<TBaseClass> CreateSignal(call_t *to_interface, call_t *from_interface,std::shared_ptr<Waveform> waveform, SpreadingFactor sf, Frequency freq_center, Frequency bandwidth, packet_t *packet){return TClass::CreateSignalImpl(to_interface,from_interface, waveform,sf,freq_center,bandwidth,packet);}

    static std::shared_ptr<TBaseClass> CreateSignal(nodeid_t source,std::shared_ptr<Waveform> waveform){return TClass::CreateSignalImpl(source, waveform);}
};


#endif // WSNET_CORE_DEFINITIONS_TYPES_SIGNAL_FACTORIES_SIGNAL_FACTORY_H_
