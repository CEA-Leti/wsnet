/**
 *  \file   signal.h
 *  \brief  Signal Abstract Class definition
 *  \author Luiz Henrique Suraty Filho
 *  \date   July 2017
 *  \version 1.0
 **/

#ifndef WSNET_CORE_DEFINITIONS_TYPES_SIGNAL_SIGNAL_H_
#define WSNET_CORE_DEFINITIONS_TYPES_SIGNAL_SIGNAL_H_

#include <memory>
#include <map>
#include <vector>
#include <kernel/include/definitions/types.h>
#include <kernel/include/definitions/types/interval/registered_rx_node.h>

/** \brief The Abstract Base Class : Signal Class
 *
 * \fn GetBegin() return the time on which the signal begin
 * \fn GetEnd() return the time on which the signal ends
 * \fn SetDestination() set the nodeid_t of the destination of the signal
 * \fn GetDestination() return the nodeid_t of the destination of the signal
 * \fn GetSource() return the nodeid_t of the source of the signal
 * \fn Clone() return a deep copy of the signal
 **/
class Signal {
  public:
    Signal (nodeid_t source);
    Signal (call_t *to,call_t *from_interface);
    Signal (call_t *to,call_t *from_interface, Time T_begin, Time T_end);
    virtual ~Signal();
    void Print() const;
    SignallUid GetUID() const;
    std::shared_ptr<Signal> Clone();
    Time GetBegin() const;
    Time GetEnd() const ;
    double GetSINR() const;
    double GetSNR() const;
    void SetSINR(double SINR);
    void SetSNR(double SNR);
    call_t GetTo_Deprecated() const;
    call_t GetFrom_Deprecated() const;
    void SetTo_Deprecated(call_t to);
    void SetFrom_Deprecated(call_t to);
    void SetDestination(std::shared_ptr<RegisteredRxNode> node_id);
    nodeid_t GetSource() const;
    void SetBegin(Time T_begin);
    void SetEnd(Time T_end);
    std::shared_ptr<RegisteredRxNode> GetDestination() const;
    void PrintSignal() const;
  private:
    virtual std::shared_ptr<Signal> CloneImpl() = 0;
    virtual void PrintSignalImpl() const = 0;
    static SignallUid uid_counter_;
    SignallUid uid_;
  protected:
    call_t to_interface_;
    call_t from_interface_;
    Time begin_;
    Time end_;
    nodeid_t source_;
    double SINR_;
    double SNR_;
    std::shared_ptr<RegisteredRxNode> destination_;
};

using MapOfSignals = std::map<SignallUid,std::shared_ptr<Signal>>;

#endif // WSNET_CORE_DEFINITIONS_TYPES_SIGNAL_SIGNAL_H_
