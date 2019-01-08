/**
 *  \file   scheduler.cc
 *  \brief  Scheduler abstract base class along with its API implementations
 *  \author Luiz Henrique Suraty Filho
 *  \date   2018
 **/

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <inttypes.h>
#include <kernel/include/scheduler/scheduler.h>

#include <kernel/include/data_structures/mem_fs/mem_fs.h>
#include <kernel/include/data_structures/heap/heap.h>
#include <kernel/include/data_structures/list/list.h>

#include <kernel/include/definitions/types.h>
#include <kernel/include/definitions/packet.h>
#include <kernel/include/definitions/class.h>
#include <kernel/include/definitions/medium.h>
#include <kernel/include/definitions/environment.h>
#include <kernel/include/definitions/node.h>
#include <kernel/include/definitions/nodearch.h>
#include <kernel/include/definitions/simulation.h>

#include <kernel/include/model_handlers/node_mobility.h>
#include <kernel/include/model_handlers/media_rxtx.h>
#include <kernel/include/scheduler/scheduler_standard_containers.h>


/* ************************************************** */
/* ************************************************** */
#ifndef RUSAGE_SELF
#define RUSAGE_SELF 0
#endif /* RUSAGE_SELF */

/* ************************************************** */
/* ************************************************** */
#define SCHEDULER_MILESTONE_PERIOD 2000000000

EventUid Event::uid_counter_ = 0;

/* ************************************************** */
/* ************************************************** */

// create the scheduler
auto scheduler = std::make_unique<SchedulerStandardContainers>();

/* ************************************************** */
/* ************************************************** */

Scheduler::Scheduler() : running_(false), clock_(0), end_(0), nbr_events_executed_(0){
}

Scheduler::Scheduler(Time end) : running_(false), clock_(0), end_(end), nbr_events_executed_(0){
}

void Scheduler::SimulationRun(){
  running_ = true;

  while (running_) {
    std::unique_ptr<Event> event(NextEvent());

    if (event == nullptr){
      continue;
    }

    // advance the clock when necessary
    if (event->clock_ > clock_) {
      SimulationTimeAdvanceClock(event->clock_);
    }

    ExecuteEvent(std::move(event));

  }

  SimulationEnd();
}
void Scheduler::SimulationEnd(){
  // Print the stat log
  struct rusage ru;
  uint64_t unanotime;
  double speedup;

  getrusage(RUSAGE_SELF,&ru);
#define NANO  (1000*1000*1000)
#define MICRO (1000)
  unanotime = ((uint64_t) ru.ru_utime.tv_sec) * NANO + ((uint64_t) ru.ru_utime.tv_usec) * MICRO;
  speedup = ((double) SimulationTimeGet()) / ((double) unanotime);
  std::cout<<"-----------------------------------"<<std::endl;
  std::cout<<std::endl<<"Simulation stats:"<<std::endl;
  std::cout<<"  simulated time: "<<SimulationTimeGet()<<std::endl;
  std::cout<<"  simulation time: "<<unanotime<<std::endl;
  std::cout<<"  speedup: "<<speedup<<std::endl;
  std::cout<<"  events in queue: "<<CountEvents()<<std::endl;
  std::cout<<"  events executed: "<<CountEventsExecuted()<<std::endl;
  std::cout<<"-----------------------------------"<<std::endl;
}

Time Scheduler::SimulationTimeGet(){
  return clock_;
}

Time Scheduler::SimulationTimeGetEnd(){
  return end_;
}

void Scheduler::SimulationTimeSetEnd(Time end){
  end_=end;
}

void Scheduler::SimulationTimeAdvanceClock(Time clock){
  clock_ = clock;
  mobility_update(clock);
}


event_t Scheduler::AddBirth(Time clock, nodeid_t node_id){
  auto e = std::make_unique<EventBirth>(clock, PRIORITY_BIRTH, node_id);
  return AddEvent(std::move(e));
}
event_t Scheduler::AddQuit(Time clock){
  auto e = std::make_unique<Event>(clock, PRIORITY_QUIT);
  return AddEvent(std::move(e));
}
event_t Scheduler::AddMobility(Time clock){
  auto e = std::make_unique<Event>(clock, PRIORITY_MOBILITY);
  return AddEvent(std::move(e));
}
event_t Scheduler::AddMilestone(Time clock){
  auto e = std::make_unique<Event>(clock, PRIORITY_MILESTONE);
  return AddEvent(std::move(e));
}


event_t Scheduler::AddTxEnd(Time clock, call_t to, call_t from, packet_t *packet){
  auto e = std::make_unique<EventRxTx>(clock, PRIORITY_TX_END, to, from, packet);
  return AddEvent(std::move(e));
}
event_t Scheduler::AddRxBegin(Time clock, call_t to, call_t from, packet_t *packet){
  auto e = std::make_unique<EventRxTx>(clock, PRIORITY_RX_BEGIN, to, from, packet);
  return AddEvent(std::move(e));
}
event_t Scheduler::AddRxEnd(Time clock, call_t to, call_t from, packet_t *packet){
  auto e = std::make_unique<EventRxTx>(clock, PRIORITY_RX_END, to, from, packet);
  return AddEvent(std::move(e));
}

event_t Scheduler::AddTxEnd(Time clock,  SpectrumModel *spectrum, std::shared_ptr<Signal> signal){
  auto e = std::make_unique<EventRxTxSignal>(clock, PRIORITY_TX_SIGNAL_END, spectrum, signal);
  return AddEvent(std::move(e));
}
event_t Scheduler::AddRxBegin(Time clock,  SpectrumModel *spectrum, std::shared_ptr<Signal> signal){
  auto e = std::make_unique<EventRxTxSignal>(clock, PRIORITY_RX_SIGNAL_BEGIN, spectrum, signal);
  return AddEvent(std::move(e));
}
event_t Scheduler::AddRxEnd(Time clock,  SpectrumModel *spectrum, std::shared_ptr<Signal> signal){
  auto e = std::make_unique<EventRxTxSignal>(clock, PRIORITY_RX_SIGNAL_END, spectrum, signal);
  return AddEvent(std::move(e));
}

event_t Scheduler::AddCallback(Time clock, call_t to, call_t from, callback_t callback, void *arg){
  auto e = std::make_unique<EventCallback>(clock, PRIORITY_CALLBACK, to, from, callback, arg);
  return AddEvent(std::move(e));
}
void Scheduler::DeleteCallback(event_t event_info){
  DeleteEvent(event_info.uid_);
}

int Scheduler::CountEvents(){
  return CountEventsImpl();
}
int Scheduler::CountEventsExecuted(){
  return nbr_events_executed_;
}

void Scheduler::ExecuteEvent(std::unique_ptr<Event> event){
  nbr_events_executed_++;

  switch (event->priority_) {
    case PRIORITY_BIRTH:{
      std::unique_ptr<EventBirth> event_birth(static_cast<EventBirth*>(event.release()));
      node_birth(event_birth->nodeid_);
      break;}
    case PRIORITY_MOBILITY:{
      mobility_event(event->clock_);
      break;}
    case PRIORITY_RX_BEGIN:{
      std::unique_ptr<EventRxTx> event_rxtx(static_cast<EventRxTx*>(event.release()));
      medium_cs(event_rxtx->packet_, &(event_rxtx->to_), &(event_rxtx->from_));
      break;}
    case PRIORITY_RX_END:{
      std::unique_ptr<EventRxTx> event_rxtx(static_cast<EventRxTx*>(event.release()));
      medium_rx(event_rxtx->packet_, &(event_rxtx->to_), &(event_rxtx->from_));
      break;}
    case PRIORITY_TX_END:{
      std::unique_ptr<EventRxTx> event_rxtx(static_cast<EventRxTx*>(event.release()));
      medium_tx_end(event_rxtx->packet_, &(event_rxtx->to_), &(event_rxtx->from_));
      break;}
    case PRIORITY_RX_SIGNAL_BEGIN:{
      std::unique_ptr<EventRxTxSignal> event_rxtx_signal(static_cast<EventRxTxSignal*>(event.release()));
      medium_rx_signal_begin(event_rxtx_signal->spectrum_, event_rxtx_signal->signal_);
      break;}
    case PRIORITY_RX_SIGNAL_END:{
      std::unique_ptr<EventRxTxSignal> event_rxtx_signal(static_cast<EventRxTxSignal*>(event.release()));
      medium_rx_signal_end(event_rxtx_signal->spectrum_, event_rxtx_signal->signal_);
      break;}
    case PRIORITY_TX_SIGNAL_END:{
      std::unique_ptr<EventRxTxSignal> event_rxtx_signal(static_cast<EventRxTxSignal*>(event.release()));
      medium_tx_signal_end(event_rxtx_signal->spectrum_, event_rxtx_signal->signal_);
      break;}
    case PRIORITY_CALLBACK:{
      std::unique_ptr<EventCallback> event_cb(static_cast<EventCallback*>(event.release()));
      if ((event_cb->to_.object != -1) && (!is_node_alive(event_cb->to_.object))) {
        break;
      }
      event_cb->callback_(&(event_cb->to_), &(event_cb->from_), event_cb->arg_);
      break;}
    case PRIORITY_MILESTONE:
      AddMilestone(SimulationTimeGet() + SCHEDULER_MILESTONE_PERIOD);
      break;
    case PRIORITY_QUIT:
      running_=false;
      break;
    default:
      break;
  }
}


event_t Scheduler::AddEvent(std::unique_ptr<Event> e){
  event_t event_info = {e->uid_, e->clock_};
  AddEventImpl(std::move(e));
  return event_info;
}
void Scheduler::DeleteEvent(uid_t uid){
  DeleteEventImpl(uid);
}
std::unique_ptr<Event> Scheduler::NextEvent(void){
  return NextEventImpl();
}

/* ************************************************** */
/* ************************************************** */


void scheduler_clean(void) {
}

int scheduler_bootstrap(void) {
  // set end event
  if (scheduler->SimulationTimeGetEnd()) {
    scheduler->AddQuit(scheduler->SimulationTimeGetEnd());
  }

  return 0;
}


/* ************************************************** */
/* ************************************************** */
int do_observe(void) {
  scheduler->SimulationRun();
  return 0;
}

/* ************************************************** */
/* ************************************************** */
void scheduler_add_birth(uint64_t clock, nodeid_t id) {
  scheduler->AddBirth(clock,id);
}

void scheduler_add_quit(uint64_t clock) {
  scheduler->AddQuit(clock);
}

void scheduler_add_mobility(uint64_t clock) {
  scheduler->AddMobility(clock);
}

void scheduler_add_milestone(uint64_t clock) {
  scheduler->AddMilestone(clock);
}

event_t *scheduler_add_callback(uint64_t clock, call_t *to, call_t *from, callback_t callback, void *arg) {
  if ( ( (scheduler->SimulationTimeGetEnd()) && (clock > scheduler->SimulationTimeGetEnd()) ) || (clock < scheduler->SimulationTimeGet()) ) {
    return NULL;
  }

  // create a pointer to an event for backward compatibility
  event_t *event = (event_t *) malloc(sizeof(event_t));

  event_t event_info = scheduler->AddCallback(clock, *to, *from, callback, arg);

  event->uid_ = event_info.uid_;
  event->clock_ = event_info.clock_;

  return event;
}

void scheduler_add_rx_begin(uint64_t clock, call_t *to, call_t *from, packet_t *packet) {
  scheduler->AddRxBegin(clock, *to, *from, packet);
  return;
}

void scheduler_add_rx_signal_begin(uint64_t clock, SpectrumModel *spectrum, std::shared_ptr<Signal> signal) {
  scheduler->AddRxBegin(clock,spectrum,signal);
  return;
}

void scheduler_add_rx_end(uint64_t clock, call_t *to, call_t *from, packet_t *packet) {
  scheduler->AddRxEnd(clock, *to, *from, packet);
  return;
}

void scheduler_add_rx_signal_end(uint64_t clock, SpectrumModel *spectrum, std::shared_ptr<Signal> signal) {
  scheduler->AddRxEnd(clock,spectrum,signal);
  return;
}

void scheduler_add_tx_end(uint64_t clock, call_t *to, call_t *from, packet_t *packet) {
  scheduler->AddTxEnd(clock, *to, *from, packet);
  return;
}

void scheduler_add_tx_signal_end(uint64_t clock, SpectrumModel *spectrum, std::shared_ptr<Signal> signal) {
  scheduler->AddTxEnd(clock,spectrum,signal);
  return;
}

void scheduler_delete_callback(event_t *event) {
  scheduler->DeleteCallback(*event);
  free(event);
  return;
}

void scheduler_set_end(uint64_t end) {
  scheduler->SimulationTimeSetEnd(end);
}

uint64_t scheduler_get_end(void) {
  return scheduler->SimulationTimeGetEnd();
}

uint64_t get_time(void) {
  return scheduler->SimulationTimeGet();
}



