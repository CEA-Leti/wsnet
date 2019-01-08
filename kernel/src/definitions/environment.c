/**
 *  \file   environment.c
 *  \brief  Environment module
 *  \author Loic Lemaitre
 *  \date   2010
 */

#include <string.h>
#include <stdio.h>
#include "environment.h"
#include "class.h"

environment_array_t environments = {0, NULL};


/* ************************************************** */
/* ************************************************** */
#define CLASS_BOOTSTRAP_INT(CLASS_TYPE)					\
  do									\
    {									\
      if (environment->CLASS_TYPE != -1)				\
	{								\
	  class_t *class = get_class_by_id(environment->CLASS_TYPE);	\
									\
	  if (class->bootstrap)						\
	    {								\
	      call_t to = {class->id, environment->id};			\
	      if (class->bootstrap(&to))				\
		return -1;						\
	    }								\
	}								\
    } while(0)


#define CLASS_BOOTSTRAP_ARRAY(CLASS_TYPE)				\
  do									\
    {									\
      if (environment->CLASS_TYPE.elts != NULL)				\
	{								\
	  int j;							\
	  for (j = 0; j < environment->CLASS_TYPE.size; j++)		\
	    {								\
	      class_t *class = get_class_by_id(environment->CLASS_TYPE.elts[j]); \
									\
	      if (class->bootstrap)					\
		{							\
		  call_t to = {class->id, environment->id};		\
		  if (class->bootstrap(&to))				\
		    return -1;						\
		}							\
	    }								\
	}								\
    } while(0)


/* ************************************************** */
/* ************************************************** */
environmentid_t get_environmentid_by_name(char *name) {
    int i = environments.size;
    while (i--)
	if (!strcmp(name, (environments.elts + i)->name))
	    break;
    return i;
}

environment_t *get_environment_by_id(environmentid_t id) {
    return environments.elts + id;
}
 
environment_t *get_environment_by_name(char *name) {
    environmentid_t id = get_environmentid_by_name(name);
    return (id < 0) ? NULL : get_environment_by_id(id);
}


/* ************************************************** */
/* ************************************************** */
int environments_bootstrap(void)
{
  environment_t *environment;
  int i;

  for (i = 0; i < environments.size; i++)
    {
      environment = environments.elts + i;

      CLASS_BOOTSTRAP_INT(map);

      CLASS_BOOTSTRAP_ARRAY(physicals);
      CLASS_BOOTSTRAP_ARRAY(monitors);
    }

  return 0;
}


/* ************************************************** */
/* ************************************************** */
void environments_clean(void) {
  int i;

  if (environments.elts && environments.size) {
    for (i = 0; i < environments.size; i++) {
      environment_t *environment = get_environment_by_id(i);
	
      if (environment->name) {
	free(environment->name);
      }

      if (environment->physicals.elts) {
	free(environment->physicals.elts);
      }

      if (environment->monitors.elts) {
	free(environment->monitors.elts);
      }

      if (environment->classes.elts ) {
        free(environment->classes.elts );
      }

      if (environment->private){
        free(environment->private);
      }
    }
    free(environments.elts);
  }
}


/* ************************************************** */
/* ************************************************** */
int environments_unbind(void) {
  int i = environments.size;
 
  for (i = 0; i < environments.size; i++)
    {
      int j;
      environment_t *environment = get_environment_by_id(i);
      for (j = 0; j < environment->classes.size; j++)
	{
	  class_t *class = get_class_by_id(environment->classes.elts[j]);
	  call_t to = {class->id, environment->id};
	  if (class->unbind)
	    {
	      if (class->unbind(&to))
		return -1;
	    }
	}
    }
  
  return 0;
}
