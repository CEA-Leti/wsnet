/**
 *  \file   scheduler.h
 *  \brief  Scheduler abstract base class along with its API
 *  \author Luiz Henrique Suraty Filho
 *  \date   2018
 **/
#ifndef WSNET_CORE_SCHEDULER_EVENT_H_
#define WSNET_CORE_SCHEDULER_EVENT_H_

#ifdef __cplusplus
#include <memory>
#include <kernel/include/definitions/models/spectrum/spectrum_model.h>
#include <kernel/include/definitions/types/signal/signal.h>

typedef enum {
  PRIORITY_NONE,
  PRIORITY_BIRTH,
  PRIORITY_MOBILITY,
  PRIORITY_TX_END,
  PRIORITY_RX_END,
  PRIORITY_RX_BEGIN,
  PRIORITY_RX_SIGNAL_BEGIN,
  PRIORITY_TX_SIGNAL_END,
  PRIORITY_RX_SIGNAL_END,
  PRIORITY_CALLBACK,
  PRIORITY_MILESTONE,
  PRIORITY_QUIT
} event_priority_t;


struct Event{
  Time                clock_;    // event time
  event_priority_t    priority_; // event priority
  EventUid            uid_;      // event id

  static EventUid     uid_counter_;

  Event() : clock_(0), priority_(PRIORITY_NONE), uid_(uid_counter_){
    ++uid_counter_;
  }

  Event(Time clock, event_priority_t priority) : clock_(clock), priority_(priority), uid_(uid_counter_){
    ++uid_counter_;
  }

  bool operator >(const Event &rhs) const {
    if (clock_ > rhs.clock_){
      return true;
    }

    if (clock_ == rhs.clock_){
      if (priority_ > rhs.priority_){
        return true;
      }

      if (priority_ == rhs.priority_){
        if (uid_ > rhs.uid_){
          return true;
        }
      }
    }
    return false;
  }

  bool operator <(const Event &rhs) const {
    if (clock_ < rhs.clock_){
      return true;
    }
    if (clock_ == rhs.clock_){
      if (priority_ < rhs.priority_){
        return true;
      }
      if (priority_ == rhs.priority_){
        if (uid_ < rhs.uid_){
          return true;
        }
      }
    }
    return false;
  }

  bool operator ==(const Event &rhs) const {
    if (uid_ == rhs.uid_){
      return true;
    }
    return false;
  }
};

struct EventBirth : public Event{
  nodeid_t  nodeid_;

  EventBirth(Time clock, event_priority_t priority, nodeid_t nodeid) : Event(clock, priority), nodeid_(nodeid){
  };
};


struct EventCallback : public Event{
  call_t      to_;
  call_t      from_;
  callback_t  callback_;
  void        *arg_;

  EventCallback(Time clock, event_priority_t priority,
                call_t to, call_t from,
                callback_t callback, void *arg) : Event(clock, priority),
                    to_(to), from_(from), callback_(callback), arg_(arg){
  };
};

struct EventRxTx : public Event{
  call_t    to_;
  call_t    from_;
  packet_t  *packet_;

  EventRxTx(Time clock, event_priority_t priority,
                call_t to, call_t from,
                packet_t  *packet) : Event(clock, priority),
                    to_(to), from_(from), packet_(packet){
  };

};

struct EventRxTxSignal : public Event{
  std::shared_ptr<Signal> signal_;
  SpectrumModel           *spectrum_;

  EventRxTxSignal(Time clock, event_priority_t priority,
                SpectrumModel *spectrum,
                std::shared_ptr<Signal> signal) : Event(clock, priority),
                    signal_(signal), spectrum_(spectrum){
  };
};

struct CompareEventGreater : public std::binary_function<std::unique_ptr<Event> &, std::unique_ptr<Event> &, bool>{
  bool operator()(const std::unique_ptr<Event> & lhs, const std::unique_ptr<Event> & rhs) const{
     return *lhs > *rhs;
  }
};

struct CompareEventLess : public std::binary_function<std::unique_ptr<Event> &, std::unique_ptr<Event> &, bool>{
  bool operator()(const std::unique_ptr<Event> & lhs, const std::unique_ptr<Event> & rhs) const{
     return *lhs < *rhs;
  }
};
#endif

/** \typedef event_t
 *
 * \brief A scheduler event.
 **/
/** \struct _event
 * \brief A scheduler event. Should use type event_t.
 **/
typedef struct _event {
  uid_t                 uid_;       // event id
  uint64_t              clock_;    // event time
} event_t;

#endif //WSNET_CORE_SCHEDULER_EVENT_H_
