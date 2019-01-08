/**
 *  \file   simulation.c
 *  \brief  Simulation module
 *  \author Loic Lemaitre
 *  \date   2010
 */

#include <string.h>
#include <stdio.h>
#include "simulation.h"
#include "class.h"


/* ************************************************** */
/* ************************************************** */
array_t   monitors;
classid_t global_map = -1;


/* ************************************************** */
/* ************************************************** */
#define CLASS_BOOTSTRAP_INT(CLASS_TYPE)			\
  do							\
    {							\
      if (CLASS_TYPE != -1)				\
	{						\
	  class_t *class = get_class_by_id(CLASS_TYPE);	\
							\
	  if (class->bootstrap)				\
	    {						\
	      call_t to = {class->id, -1};		\
	      if (class->bootstrap(&to))		\
		return -1;				\
	    }						\
	}						\
    } while(0)


#define CLASS_BOOTSTRAP_ARRAY(CLASS_TYPE)				\
  do									\
    {									\
      if (CLASS_TYPE.elts != NULL)					\
	{								\
	  int j;							\
	  for (j = 0; j < CLASS_TYPE.size; j++)				\
	    {								\
	      class_t *class = get_class_by_id(CLASS_TYPE.elts[j]);	\
									\
	      if (class->bootstrap)					\
		{							\
		  call_t to = {class->id, -1};				\
		  if (class->bootstrap(&to))				\
		    return -1;						\
		}							\
	    }								\
	}								\
    } while(0)


/* ************************************************** */
/* ************************************************** */
int simulation_bootstrap(void)
{
  CLASS_BOOTSTRAP_INT(global_map);
  CLASS_BOOTSTRAP_ARRAY(monitors);

  return 0;
}


/* ************************************************** */
/* ************************************************** */
void simulation_clean(void)
{
  if (monitors.elts)
    {
      free(monitors.elts);
    }
}


/* ************************************************** */
/* ************************************************** */
int simulation_unbind(void)
{
  if (monitors.elts)
    {
      int i;
      for (i = 0; i< monitors.size; i++)
	{
	  class_t *class = get_class_by_id(monitors.elts[i]);
	  if(class->unbind)
	    {
	      call_t to = {class->id, -1};
	      if (class->unbind(&to))
		return -1;
	    }
	}
    }

  if (global_map != -1)
    {
      class_t *class = get_class_by_id(global_map);
      if(class->unbind)
	{
	  call_t to = {class->id, -1};
	  if (class->unbind(&to))
	    return -1;
	}
    }

  return 0;
}
