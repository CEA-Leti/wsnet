/**
 *  \file   scheduler_standard_containers.cc
 *  \brief  Scheduler implementation using C++'s standard containers
 *  \author Luiz Henrique Suraty Filho
 *  \date   2018
 **/

#include <kernel/include/scheduler/scheduler_standard_containers.h>

SchedulerStandardContainers::SchedulerStandardContainers(){
  events_queue_ = std::priority_queue<std::unique_ptr<Event>, std::vector<std::unique_ptr<Event>>, CompareEventGreater>();
  deleted_events_ = std::unordered_set<uid_t>();
}

SchedulerStandardContainers::SchedulerStandardContainers(Time end) : Scheduler(end){
  events_queue_ = std::priority_queue<std::unique_ptr<Event>, std::vector<std::unique_ptr<Event>>, CompareEventGreater>();
  deleted_events_ = std::unordered_set<uid_t>();
}

SchedulerStandardContainers::~SchedulerStandardContainers(){
  deleted_events_.clear();
}

void SchedulerStandardContainers::AddEventImpl(std::unique_ptr<Event> e_){
  events_queue_.push(std::move(e_));
}

void SchedulerStandardContainers::DeleteEventImpl(uid_t uid){
  deleted_events_.insert(uid);
}

std::unique_ptr<Event> SchedulerStandardContainers::NextEventImpl(){
  if (events_queue_.size() == 0){
    running_ = false;
    return nullptr;
  }
  std::unique_ptr<Event> current_event(std::move(const_cast<std::unique_ptr<Event>&>(events_queue_.top())));
  events_queue_.pop();

  // check whether event has been deleted
  // we currently check only for callbacks
  if (current_event->priority_ == PRIORITY_CALLBACK){
    if(deleted_events_.count(current_event->uid_)) {
      // remove from the unordered_set
      // it could be linear on worst-case
      // but as we keep this set with few numbers,
      // the penalty may not be an issue
      deleted_events_.erase(current_event->uid_);
      return nullptr;
    }
    else {
      return current_event;
    }
  }

  return current_event;

}

int SchedulerStandardContainers::CountEventsImpl(){
  return events_queue_.size();
}

