/**
 *  \file   scheduler.h
 *  \brief  Scheduler abstract base class along with its API declarations
 *  \author Luiz Henrique Suraty Filho
 *  \date   2018
 **/
#ifndef WSNET_CORE_SCHEDULER_SCHEDULER_H_
#define WSNET_CORE_SCHEDULER_SCHEDULER_H_


#include <kernel/include/definitions/types.h>
#include <kernel/include/definitions/node.h>
#include <kernel/include/scheduler/event.h>

/* ************************************************** */
/* ************************************************** */
#ifdef __cplusplus

/** \brief Scheduler: The Scheduler abstract base class
 *
 * \fn AddBirth() add a birth event
 * \fn AddQuit() add a quit event (end of simulation)
 * \fn AddMobility() add a mobility event
 * \fn AddMilestone() add a milestone event
 * \fn AddTxEnd() add a tx_end event
 * \fn AddRxBegin() add a rx_begin event
 * \fn AddRxEnd() add a rx_end event
 * \fn AddCallback() add a rx_end event
 * \fn AddRxEnd() add a callback event
 * \fn DeleteCallback() delete a callback event
 * \fn NextEvent() return the next event to be executed
 * \fn CountEvents() return the number of events to be executed
 * \fn CountEventsExecuted() return the number of events already executed
 * \fn AddEventImpl() add an event - to be implemented
 * \fn DeleteEventImpl() delete an event - to be implemented
 * \fn NextEventImpl() return the next event to be executed - to be implemented
 * \fn CountEventsImpl() return the number of events to be executed - to be implemented
 **/
class Scheduler{
 public:
  Scheduler();
  Scheduler(Time end);
  virtual ~Scheduler(){};

  void SimulationRun();
  void SimulationEnd();

  Time SimulationTimeGet();
  Time SimulationTimeGetEnd();
  void SimulationTimeSetEnd(Time end);
  void SimulationTimeAdvanceClock(Time clock);

  event_t AddBirth(Time clock, nodeid_t id);
  event_t AddQuit(Time clock);
  event_t AddMobility(Time clock);
  event_t AddMilestone(Time clock);

  event_t AddTxEnd(Time clock, call_t to, call_t from, packet_t *packet);
  event_t AddRxBegin(Time clock, call_t to, call_t from, packet_t *packet);
  event_t AddRxEnd(Time clock, call_t to, call_t from, packet_t *packet);

  event_t AddTxEnd(Time clock,  SpectrumModel *spectrum, std::shared_ptr<Signal> signal);
  event_t AddRxBegin(Time clock,  SpectrumModel *spectrum, std::shared_ptr<Signal> signal);
  event_t AddRxEnd(Time clock,  SpectrumModel *spectrum, std::shared_ptr<Signal> signal);

  event_t AddCallback(Time clock, call_t to, call_t from, callback_t callback, void *arg);
  void DeleteCallback(event_t event_info);

  int CountEvents();
  int CountEventsExecuted();

 protected:
  bool running_;
  Time clock_;
  Time end_;
  int nbr_events_executed_;

 private:
  void ExecuteEvent(std::unique_ptr<Event> e);
  event_t AddEvent(std::unique_ptr<Event> e);
  void DeleteEvent(uid_t uid);
  std::unique_ptr<Event> NextEvent();
  virtual int CountEventsImpl() = 0;
  virtual void AddEventImpl(std::unique_ptr<Event> e) = 0;
  virtual void DeleteEventImpl(uid_t uid) = 0;
  virtual std::unique_ptr<Event> NextEventImpl(void) = 0;

};
#endif

/* ************************************************** */
/* ************************************************** */

#ifdef __cplusplus
extern "C"{
#endif

/** 
 * \brief Add a node birth event to the scheduler.
 * \param clock, the birth date.
 * \param id, the node id.
 **/
void scheduler_add_birth(uint64_t clock, nodeid_t id);

/** 
 * \brief Add a rx begin event to the scheduler.
 * \param clock, the rx begin date.
 * \param to a call parameter.
 * \param from a call parameter.
 * \param packet, a pointer on the packet associated to this event.
 **/
void scheduler_add_rx_begin(uint64_t clock, call_t *to, call_t *from, packet_t *packet);

/**
 * \brief Add a rx begin event to the scheduler.
 * \param clock, the rx begin date.
 * \param to a call parameter.
 * \param from a call parameter.
 * \param signal, a pointer on the signal associated to this event.
 * \param rx_node, a pointer on the rx_node associated to this event.
 **/
#ifdef __cplusplus
void scheduler_add_rx_signal_begin(uint64_t clock, SpectrumModel *spectrum,std::shared_ptr<Signal> signal);
#endif

/** 
 * \brief Add a rx end event to the scheduler.
 * \param clock, the rx end date.
 * \param to a call parameter.
 * \param from a call parameter.
 * \param packet, a pointer on the packet associated to this event.
 **/
void scheduler_add_rx_end(uint64_t clock, call_t *to, call_t *from, packet_t *packet);

/**
 * \brief Add a rx begin event to the scheduler.
 * \param clock, the rx begin date.
 * \param to a call parameter.
 * \param from a call parameter.
 * \param signal, a pointer on the signal associated to this event.
 * \param rx_node, a pointer on the rx_node associated to this event.
 **/
#ifdef __cplusplus
void scheduler_add_rx_signal_end(uint64_t clock, SpectrumModel *spectrum, std::shared_ptr<Signal> signal);
#endif

/** 
 * \brief Add a tx end event to the scheduler.
 * \param clock, the tx end date.
 * \param to a call parameter.
 * \param from a call parameter.
 * \param packet, a pointer on the packet associated to this event.
 **/
void scheduler_add_tx_end(uint64_t clock, call_t *to, call_t *from, packet_t *packet);

/**
 * \brief Add a rx begin event to the scheduler.
 * \param clock, the rx begin date.
 * \param to a call parameter.
 * \param from a call parameter.
 * \param signal, a pointer on the signal associated to this event.
 **/
#ifdef __cplusplus
void scheduler_add_tx_signal_end(uint64_t clock, SpectrumModel *spectrum, std::shared_ptr<Signal> signal);
#endif

/** 
 * \brief Add quit event to the scheduler.
 * \param clock, the quit date.
 **/
void scheduler_add_quit(uint64_t clock);

/** 
 * \brief Add mobility event to the scheduler.
 * \param clock, the mobility date.
 **/
void scheduler_add_mobility(uint64_t clock);

/** 
 * \brief Add a milestone event to the scheduler.
 * \param clock, the milestone date.
 **/
void scheduler_add_milestone(uint64_t clock);

/** 
 * \brief Schedule the callback of a function at a given time.
 * \param clock time of the callback.
 * \param to a call parameter given to the callback function.
 * \param from a call parameter given to the callback function.
 * \param callback the function that is called back.
 * \param arg a paramater that given to the callback function.
 * \return An opaque object that references the callback-associated event.
 **/
event_t *scheduler_add_callback(uint64_t clock, call_t *to, call_t *from, callback_t callback, void *arg);

/** 
 * \brief Delete an event from the Scheduler.
 * \param event a paramater that describe the event we want to delete.
 **/
void scheduler_delete_callback(event_t *event);

/** 
 * \brief Return the date of the end of the simulation.
 * \return Return a long long unsigned int.
 **/
uint64_t scheduler_get_end(void);

/** 
 * \brief Bootstrap the scheduler.
 * \return Return 0 in case of success, -1 else.
 **/
int scheduler_bootstrap(void);

/** 
 * \brief Free scheduler structures.
 **/
void scheduler_clean(void);

/** 
 * \brief Run the scheduler.
 * \return 0 if ok, 1 if errors
 **/
int do_observe(void);

/** 
 * \brief Give the scheduler time.
 * \return Return the time.
 **/
uint64_t get_time(void);


/** 
 * \brief Set the end of the simulation.
 * \param end the date of the end of the simulation.
 **/
void scheduler_set_end(uint64_t end);

#ifdef __cplusplus
}
#endif

#endif //WSNET_CORE_SCHEDULER_SCHEDULER_H_



