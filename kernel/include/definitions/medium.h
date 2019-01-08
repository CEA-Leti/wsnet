/**
 *  \file   medium.h
 *  \brief  medium declarations
 *  \author Loic Lemaitre
 *  \date   2010
 **/
#ifndef __medium__
#define __medium__

#include <kernel/include/definitions/types.h>


/* ************************************************** */
/* ************************************************** */
typedef struct _medium {
  mediumid_t id;
  char      *name;
  double     propagation_range;
  double	 speed_of_light;		// The speed of light in meters per nanoseconds

  array_t    classes;

  classid_t  spectrum;
  classid_t  pathloss;
  classid_t  shadowing;
  classid_t  fading;
  classid_t  interferences;
  classid_t  intermodulation;
  classid_t  noise;

  array_t    links;
  array_t    modulations;
  array_t    monitors;

#ifdef __cplusplus
  void            **private_vars;
#else //__cplusplus
  void            **private;
#endif //__cplusplus

} medium_t;


typedef struct _medium_array {
  int size;
  medium_t *elts;
} medium_array_t;

extern medium_array_t mediums;

#ifdef __cplusplus
extern "C"{
#endif

/* ************************************************** */
/* ************************************************** */
int       get_mediumid_by_name(char *name);
medium_t *get_medium_by_name  (char *name);
medium_t *get_medium_by_id    (mediumid_t id);

int       mediums_bootstrap   (void);
void      mediums_clean       (void);
int       mediums_unbind      (void);

/* ************************************************** */
/* ************************************************** */

double medium_get_pathloss(call_t *to_interface, call_t *from_interface, packet_t *packet, double rxdBm);

double medium_get_fading(call_t *to_interface, call_t *from_interface, packet_t *packet, double rxdBm);

double medium_get_shadowing(call_t *to_interface, call_t *from_interface, packet_t *packet, double rxdBm);

int medium_get_radio_link_condition(call_t *to_interface, call_t *from_interface);

void * medium_get_spectrum_object(call_t *interface);

#ifdef __cplusplus
}
#endif
#endif // __medium__
