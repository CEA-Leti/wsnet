/**
 *  \file   timer.h
 *  \brief  Generic timer
 *  \author Quentin Lampin
 *  \date   2009
 **/

#ifndef _PERIODIC_TIMER_H
#define	_PERIODIC_TIMER_H

/**
 * timer structure
 * parameters: parameters for next_trigger functions (eg: period for periodic timer)
 * conditional_end: pointer to a function that
 * return 1 if the timer must be destroyed and 0 otherwise
 * callback_function: function to callback when the timer triggers.
 **/
typedef struct qtimer_s {
  void *trigger_parameters;
  int (*conditional_end)(call_t *to, void *timer_id);
  void (*callback_function)(call_t *to, void *timer_id);
  uint64_t (*next_trigger)(call_t *to, void *timer_id);
  call_t *to;
}qtimer_t; /**
            * qtimer and not timer because timer_t is already defined in /usr/lib/time.c 
            * so I added my first name initial ;)
            * */

/* parameters structures */
typedef struct exponential_s{
  uint64_t initial_value;
  uint64_t ratio;
  uint64_t offset;
  uint64_t rank;
}exponential_parameters_t;

typedef struct uniform_random_s{
  uint64_t min_value;
  uint64_t max_value;
}uniform_random_parameters_t;

#ifdef __cplusplus
extern "C"{
#endif

/* common timer next trigger functions */
uint64_t periodic_trigger(call_t *to, void *timer_id);
uint64_t exponential_trigger(call_t *to, void *timer_id);
uint64_t uniform_random_trigger(call_t *to, void *timer_id);

/* common conditional end function */
int never_stop(call_t *to, void *timer_id);

/* create a new timer */
void *create_timer(call_t *to, void *callback_function, void *conditional_end,
		   void *next_trigger, void *trigger_parameters);

/* start a newly created timer */
void *start_timer(void *timer_id, uint64_t delay);

/* destroy a timer */
void destroy_timer(void *timer_id);

/* change the timer parameter value */
void change_parameter(void *timer_id, void *new_parameters);

/* init timers, used in main */
int timer_init(void);

/* clean up timers, used in main */
void timer_clean(void);

/* fetch a timer */
qtimer_t *fetch_timer(void *timer_id);

#ifdef __cplusplus
}
#endif

#endif	/* _PERIODIC_TIMER_H */



