/**
 *  \file   rf_signal.h
 *  \brief  Radio Frequency Signal Abstract Class definition
 *  \author Luiz Henrique Suraty Filho
 *  \date   July 2017
 *  \version 1.0
 **/

#ifndef WSNET_CORE_DEFINITIONS_TYPES_SIGNAL_RF_SIGNAL_H_
#define WSNET_CORE_DEFINITIONS_TYPES_SIGNAL_RF_SIGNAL_H_

#include <memory>
#include <kernel/include/definitions/types/signal/signal.h>
#include <kernel/include/definitions/types/waveform/waveform.h>

/** \brief The Abstract Base Class : RFSignal Class
 *
 * \fn GetBegin() return the time on which the signal begin
 * \fn GetEnd() return the time on which the signal ends
 * \fn SetDestination() set the nodeid_t of the destination of the signal
 * \fn GetDestination() return the nodeid_t of the destination of the signal
 * \fn GetSource() return the nodeid_t of the source of the signal
 * \fn Clone() return a deep copy of the signal
 **/
class RFSignal : public Signal{
  public:
    RFSignal(nodeid_t source, packet_t* packet);
    RFSignal (call_t *to,call_t *from_interface, packet_t* packet);
    RFSignal(nodeid_t source, std::shared_ptr<Waveform> waveform);
    RFSignal (call_t *to,call_t *from_interface, packet_t* packet, Time T_begin, Time T_end);
    virtual ~RFSignal();
    std::weak_ptr<Waveform> GetWaveform() const;
    Frequency GetBandwidth() const;
    packet_t *GetPacket_Deprecated();
    void SetWaveform(std::shared_ptr<Waveform> waveform);
  private:
    Frequency GetBandwidthImpl() const;
    std::weak_ptr<Waveform> GetWaveformImpl() const;
    void SetWaveformImpl(std::shared_ptr<Waveform>);
    std::shared_ptr<Signal> CloneImpl();
    packet_t *GetPacket_DeprecatedImpl();
    void PrintSignalImpl() const;
  protected:
    std::shared_ptr<Waveform> waveform_;
    packet_t *packet_;
};

#endif // WSNET_CORE_DEFINITIONS_TYPES_SIGNAL_RF_SIGNAL_H_
