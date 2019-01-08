/**
 *  \file   timer.c
 *  \brief  Generic timer
 *  \author Quentin Lampin
 *  \date   2009
 **/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <kernel/include/data_structures/hashtable/hashtable.h>
#include <kernel/include/definitions/types.h>
#include <kernel/include/tools/math/rng/rng.h>
#include <kernel/include/scheduler/scheduler.h>
#include "timer.h"

static void *hash_index = (void *)1;
static void *timers = NULL;

unsigned long timer_hash(void *key) {
  return (unsigned long) key;
}

int timer_equal(void *key0, void *key1) {
  return (int) (key0 == key1);
}

/**
 * \brief Return a constant value given as parameter.
 * \param timer_id: the timer hash key.
 * \return the given period
 **/
uint64_t periodic_trigger(call_t *to, void *timer_id) {
  qtimer_t *timer = (qtimer_t *) hashtable_retrieve(timers, timer_id);
  uint64_t *period = (uint64_t *) (timer->trigger_parameters);
  return (uint64_t) (*period);
}

/**
 * \brief Return values of an exponential sequence of form r^n + offset
 * \param timer_id: the timer hash key.
 * \return value of the sequence.
 **/
uint64_t exponential_trigger(call_t *to, void *timer_id) {
  qtimer_t *timer = (qtimer_t *) hashtable_retrieve(timers, timer_id);
  exponential_parameters_t *parameters =
    (exponential_parameters_t *) timer->trigger_parameters;
  uint64_t a = parameters->initial_value;
  uint64_t r = parameters->ratio;
  uint64_t o = parameters->offset;
  uint64_t n = parameters->rank;
  parameters->rank += 1;
  uint64_t result = a * pow(r, n) - o;
  return result;
}

/**
 * \brief Return a random time chosen in the given interval.
 * \param timer_id: the timer hash key.
 * \return a random value.
 **/
uint64_t uniform_random_trigger(call_t *to, void *timer_id) {
  qtimer_t *timer = (qtimer_t *) hashtable_retrieve(timers, timer_id);  
  uniform_random_parameters_t *parameters =
    (uniform_random_parameters_t *) timer->trigger_parameters;
  uint64_t min = parameters->min_value;
  uint64_t max = parameters->max_value;
  return get_random_time_range(min, max);
}

/**
 * \brief simple conditional_end callback function
 * \param timer_id: not used.
 * \return 1
 **/
int never_stop(call_t *to, void *timer_id) {
  return 0;
}

/**
 * \brief create a new timer
 * \param to:call information
 * \param callback_function: function that is called when the timer fires
 * \param conditional_end: function that specify when the then timer must stop
 * \param next_trigger: return the time that the timer has to wait for the next tick.
 * \param trigger_parameters: parameters for next_trigger
 * \return ID of the new timer
 */
void *create_timer(call_t *to, void *callback_function, void *conditional_end,
		   void *next_trigger, void *trigger_parameters) {
  qtimer_t *timer = malloc(sizeof (qtimer_t));
  timer->to = malloc(sizeof (call_t));
  memcpy(timer->to, to, sizeof (call_t));
  timer->callback_function = callback_function;
  timer->conditional_end = conditional_end;
  timer->next_trigger = next_trigger;
  timer->trigger_parameters = trigger_parameters;
  hashtable_insert(timers, hash_index, (void *) timer);
  void *key = hash_index;
  hash_index++;
  return key;
}

/**
 * \brief retrieve a timer
 * \param timer_id: ID of the timer to fetch.
 * \return ID of the new timer
 */
qtimer_t *fetch_timer(void *timer_id) {
  qtimer_t *timer = (qtimer_t *) hashtable_retrieve(timers, timer_id);
  return timer;
}

/**
 * \brief destroy a given timer
 * \param timer_id: ID of the timer to destroy
 **/
void destroy_timer(void *timer_id) {
  qtimer_t *timer = (qtimer_t *) hashtable_retrieve(timers, timer_id);
  if (timer != NULL) {
    free(timer->to);
    free(timer);
  }
  hashtable_delete(timers, timer_id);
}

/**
 * \brief change the trigger parameters
 * \param timer_id: ID of the timer
 * \param new_parameters: self explanatory
 **/
void change_parameter(void *timer_id, void *new_parameters) {
  qtimer_t *timer = (qtimer_t *) hashtable_retrieve(timers, timer_id);
  timer->trigger_parameters = new_parameters;
}

/**
 * \brief main loop function for timers
 * \param to: call informations
 * \param from: unused
 * \param timer_id; ID of the timer
 * \return 0
 **/
int timer_callback(call_t *to, call_t *from, void *timer_id) {
  qtimer_t *timer = (qtimer_t *) hashtable_retrieve(timers, timer_id);
  if (timer != NULL) {
    if (timer->conditional_end(timer->to, timer_id) == 1) {
      destroy_timer(timer_id);
    } else {
      uint64_t next_trigger = timer->next_trigger(timer->to, timer_id);
      scheduler_add_callback(get_time() + next_trigger,
			     timer->to,
			     from,
			     (callback_t) timer_callback,
			     timer_id);
      timer->callback_function(timer->to, timer_id);
    }
  }
  return 0;
}

/**
 * \brief start a given timer
 * \param timer_id: ID of the timer to start
 * \param delay: delay before firing the first tick
 * \return timer ID
 **/
void *start_timer(void *timer_id, uint64_t delay) {
  qtimer_t *timer = (qtimer_t *) hashtable_retrieve(timers, timer_id);
  call_t from = {-1, -1};
  scheduler_add_callback(get_time() + delay,
			 timer->to,
			 &from,
			 (callback_t) timer_callback,
			 timer_id);
  return timer_id;
}

/**
 * initialize the timers' struct
 **/
int timer_init(void) {
  timers = hashtable_create(timer_hash, timer_equal, NULL, NULL);
  return 0;
}

/**
 * terminate the timer struct
 **/
void timer_clean(void) {
  if (timers)
    hashtable_destroy(timers);
}
