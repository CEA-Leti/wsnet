/**
 *  \file   monitor.c
 *  \brief  Monitor module
 *  \author Loic Lemaitre
 *  \date   2010
 */

#include <stdio.h>
#include <string.h>
#include "monitor.h"
#include "model_handlers_dbg.h"


/* ************************************************** */
/* ************************************************** */
#define MAXMONITORS 100


/* ************************************************** */
/* ************************************************** */
typedef struct _monitors
{
  int       count;                /* number of monitors registered      */
  sigmask_t signals;              /* set of masks (OR over the masks)   */
  call_t    monids[MAXMONITORS];  /* list of the monitor ids registered */
  sigid_t   sigids[MAXMONITORS];  /* list of the signal ids registered  */
} monitors_t;

static monitors_t monitors;


/* ************************************************** */
/* ************************************************** */
sigmask_t monitor_get_mask_from_sig_id(sigid_t id);


/* ************************************************** */
/* ************************************************** */
int monitors_init (void)
{
  DBG_MONITOR("monitor:init monitors\n");
  monitors.count   = 0;
  monitors.signals = 0;

  return 0;
}

void monitors_clean(void)
{
  
}


/* ************************************************** */
/* ************************************************** */
int monitor_register(call_t *to, sigid_t signal)
{
  memcpy(monitors.monids + monitors.count, to, sizeof(call_t));
  monitors.sigids[monitors.count] = signal;
  monitors.signals |= monitor_get_mask_from_sig_id(signal);
  monitors.count++;

  DBG_MONITOR("monitor:register: class id %d with signal %d registered\n", to->class, signal);

  return 0;
}

int monitor_unregister(call_t *to)
{
  int i = 0;

  while (i < monitors.count)
    {
      if (monitors.monids[i].class == to->class && monitors.monids[i].object == to->object)
	{
	  /* check if same signal is used by another monitor */
	  int j;
	  int rm_sigmask = 1;

	  for (j = 0; j < monitors.count; j++)
	    {
	      if (j == i)
		continue;
	      if (monitors.sigids[j] == monitors.sigids[i])
		{
		  rm_sigmask = 0;
		  break;
		}
	    }
	  
	  /* remove signal mask */
	  if (rm_sigmask)
	    {
	      monitors.signals &= ~monitor_get_mask_from_sig_id(monitors.sigids[i]);
	    }

	  /* remove monitor and fill the hole */
	  if (monitors.count > 1){
	    memcpy(monitors.monids + i, monitors.monids + (monitors.count - 1), sizeof(call_t));
	    monitors.sigids[i] = monitors.sigids[monitors.count - 1];
	  }
	  monitors.count--;

	  DBG_MONITOR("monitor:unregister: monitor {class %d, object %d} unregistered (new signals mask: 0x%02x)\n",
	      to->class, to->object, monitors.signals);
	  return 0;
	}
      i++;
    }
  
  DBG_MONITOR("monitor:unregister: monitor {class %d, object %d} not found\n", to->class, to->object);

  return -1;
}


/* ************************************************** */
/* ************************************************** */
void monitor_fire_signal(sigid_t signal, void *data)
{
  /* Check if this signal is monitored */
  if (monitor_get_mask_from_sig_id(signal) & monitors.signals)
    {
      /* parse signals list to call involved monitors */
      int i;
      for (i = 0; i < monitors.count; i++)
	{
	  if (monitors.sigids[i] == signal)
	    {
	      call_t *to   = monitors.monids + i;
	      call_t  from = {-1, -1};

	      /* call class matching this signal */
	      DBG_MONITOR("monitor:fire signal: calling monitor {class %d, object %d} matching signal %d\n",
			  to->class, to->object, signal);
	      get_class_by_id(to->class)->methods->monitor.fire(to, &from, data);
	    }
	}
    }
}


/* ************************************************** */
/* ************************************************** */
sigmask_t monitor_get_mask_from_sig_id(sigid_t id)
{
  return 1 << id;
}
