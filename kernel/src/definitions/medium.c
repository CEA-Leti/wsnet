/**
 *  \file   medium.c
 *  \brief  Medium module
 *  \author Loic Lemaitre
 *  \date   2010
 */

#include <string.h>
#include "medium.h"
#include "class.h"
#include <stdio.h>
#include <kernel/include/model_handlers/interface.h>
#include <kernel/include/model_handlers/link.h>


medium_array_t mediums = {0, NULL};


/* ************************************************** */
/* ************************************************** */
#define CLASS_BOOTSTRAP_INT(CLASS_TYPE)				\
    do								\
    {								\
      if (medium->CLASS_TYPE != -1)				\
      {							\
        class_t *class = get_class_by_id(medium->CLASS_TYPE);	\
        \
        if (class->bootstrap)					\
        {							\
          call_t to = {class->id, medium->id};		\
          if (class->bootstrap(&to))			\
          return -1;					\
        }							\
      }							\
    } while(0)


#define CLASS_BOOTSTRAP_ARRAY(CLASS_TYPE)				\
    do									\
    {									\
      if (medium->CLASS_TYPE.elts != NULL)				\
      {								\
        int j;							\
        for (j = 0; j < medium->CLASS_TYPE.size; j++)			\
        {								\
          class_t *class = get_class_by_id(medium->CLASS_TYPE.elts[j]); \
          \
          if (class->bootstrap)					\
          {							\
            call_t to = {class->id, medium->id};			\
            if (class->bootstrap(&to))				\
            return -1;						\
          }							\
        }								\
      }								\
    } while(0)


/* ************************************************** */
/* ************************************************** */
mediumid_t get_mediumid_by_name(char *name) {
  int i = mediums.size;
  while (i--) {
    if (!strcmp(name, (mediums.elts + i)->name)) {
      break;
    }
  }
  return i;
}

medium_t *get_medium_by_id(mediumid_t id) {
  return mediums.elts + id;
}

medium_t *get_medium_by_name(char *name) {
  mediumid_t id = get_mediumid_by_name(name);
  return (id < 0) ? NULL : get_medium_by_id(id);
}


/* ************************************************** */
/* ************************************************** */
int mediums_bootstrap(void) {
  medium_t *medium;
  int i;

  for (i = 0; i < mediums.size; i++) {
    medium = mediums.elts + i;

    CLASS_BOOTSTRAP_INT(spectrum);
    CLASS_BOOTSTRAP_INT(pathloss);
    CLASS_BOOTSTRAP_INT(shadowing);
    CLASS_BOOTSTRAP_INT(fading);
    CLASS_BOOTSTRAP_INT(interferences);
    CLASS_BOOTSTRAP_INT(intermodulation);
    CLASS_BOOTSTRAP_INT(noise);

    CLASS_BOOTSTRAP_ARRAY(modulations);
    CLASS_BOOTSTRAP_ARRAY(monitors);
    CLASS_BOOTSTRAP_ARRAY(links);
  }

  return 0;
}


/* ************************************************** */
/* ************************************************** */
void mediums_clean(void) {
  int i;

  if (mediums.elts && mediums.size) {
    for (i = 0; i < mediums.size; i++) {
      medium_t *medium = get_medium_by_id(i);

      if (medium->name) {
        free(medium->name);
      }

      if (medium->modulations.elts) {
        free(medium->modulations.elts);
      }

      if (medium->monitors.elts) {
        free(medium->monitors.elts);
      }

      if (medium->links.elts) {
        free(medium->links.elts);
      }

      if(medium->classes.elts) {
        free(medium->classes.elts);
      }

      if (medium->private) {
        free(medium->private);
      }
    }
    free(mediums.elts);
  }
}


/* ************************************************** */
/* ************************************************** */
int mediums_unbind(void) {
  int i = mediums.size;

  for (i = 0; i < mediums.size; i++) {
    int j;
    medium_t *medium = get_medium_by_id(i);
    for (j = 0; j < medium->classes.size; j++) {
      class_t *class = get_class_by_id(medium->classes.elts[j]);
      call_t to = {class->id, medium->id};
      if (class->unbind) {
        if (class->unbind(&to))
          return -1;
      }
    }
  }

  return 0;
}

/* ************************************************** */
/* ************************************************** */
double medium_get_pathloss(call_t *to_interface, call_t *from_interface, packet_t *packet, double rxdBm) {
  medium_t *medium = get_medium_by_id(interface_get_medium(to_interface, from_interface));
  if (medium->pathloss == -1)
    return rxdBm;
  call_t to_pathloss = {medium->pathloss, medium->id};
  return get_class_by_id(medium->pathloss)->methods->pathloss.pathloss(&to_pathloss, to_interface, from_interface, packet, rxdBm);
}

double medium_get_fading(call_t *to_interface, call_t *from_interface, packet_t *packet, double rxdBm) {
  medium_t *medium = get_medium_by_id(interface_get_medium(to_interface, from_interface));
  if (medium->fading == -1)
    return 0.0;
  call_t to_fading = {medium->fading, medium->id};
  return get_class_by_id(medium->fading)->methods->fading.fading(&to_fading, to_interface, from_interface, packet, rxdBm);
}

double medium_get_shadowing(call_t *to_interface, call_t *from_interface, packet_t *packet, double rxdBm) {
  medium_t *medium = get_medium_by_id(interface_get_medium(to_interface, from_interface));
  if (medium->shadowing == -1)
    return 0.0;
  call_t to_shadowing = {medium->shadowing, medium->id};
  return get_class_by_id(medium->shadowing)->methods->shadowing.shadowing(&to_shadowing, to_interface, from_interface, packet, rxdBm);
}

void * medium_get_spectrum_object(call_t *interface) {
  call_t     from0  = {-1, -1};
  medium_t *medium = get_medium_by_id(interface_get_medium(interface, &from0));
  if (medium->spectrum == -1)
    return NULL;
  call_t to_spectrum = {medium->spectrum, medium->id};
  class_t *class = get_class_by_id(to_spectrum.class);
  return class->objects.object[to_spectrum.object];
}

int medium_get_radio_link_condition(call_t *to_interface, call_t *from_interface) {
  medium_t *medium = get_medium_by_id(interface_get_medium(to_interface, from_interface));
  if (medium->links.size == 0)
    return 0;
  call_t to_link ={medium->links.elts[0],medium->id};
  //call_t to_link = {medium->links.elts[0], medium->id};
  return link_get_link_condition(&to_link, to_interface, from_interface);
}
