/**
 *  \file   scheduler_standard_containers.h
 *  \brief  Scheduler implementation using C++'s STD containers
 *      We use a priority queue for all events along with an
 *      unordered set used to store the uid of deleted_callbacks.
 *
 *      Thus, whenever a callback event is the next, we check whether
 *      it has been deleted or not by looking for its uid inside the
 *      unordered set. (O(1) complexity - considering our hash won't have collisions)
 *
 *      We could use only a priority queue, but this would imply that while
 *      deleting a callback we would need to either :
 *       - pop elements until you find and then insert them again
 *       - inherit the std::priority_queue and find the element on the protected
 *         container, delete it, and make a heap with the remaining elements.
 *      Both solutions would have a higher time complexity (O(nlogn) at least).
 *
 *      The extra memory needed is low compared to time gains, once we are only
 *      storing the uid's of the events that have been deleted.
 *  \author Luiz Henrique Suraty Filho
 *  \date   2018
 **/

#ifndef WSNET_CORE_SCHEDULER_SCHEDULER_STD_H_
#define WSNET_CORE_SCHEDULER_SCHEDULER_STD_H_

#include <unordered_set>
#include <queue>
#include <kernel/include/scheduler/scheduler.h>

/** \brief SchedulerStandardContainers: The Scheduler implementation using C++'s STD containers
 *
 * \fn AddEventImpl() add an event in the priority queue
 * \fn DeleteEventImpl() delete an event (mark an event as deleted on the unordered set)
 * \fn NextEventImpl() return the next event to be executed
 * \fn CountEventsImpl() return the number of events on the queue
 **/
class SchedulerStandardContainers : public Scheduler {
 public:
  SchedulerStandardContainers();
  SchedulerStandardContainers(Time end);
  ~SchedulerStandardContainers();
 private:
  void AddEventImpl(std::unique_ptr<Event> e_);
  void DeleteEventImpl(uid_t uid);
  std::unique_ptr<Event> NextEventImpl();
  int CountEventsImpl();
  std::priority_queue<std::unique_ptr<Event>, std::vector<std::unique_ptr<Event>>, CompareEventGreater> events_queue_;
  std::unordered_set<uid_t> deleted_events_;
};

#endif //WSNET_CORE_SCHEDULER_SCHEDULER_STD_H_
