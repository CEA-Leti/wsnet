/**
 *  \file   environment.h
 *  \brief  Environment declarations
 *  \author Loic Lemaitre
 *  \date   2010
 **/
#ifndef __environment__
#define __environment__

#include <kernel/include/definitions/types.h>


typedef struct _environment {
  environmentid_t id;
  char           *name;

  array_t         classes;

  classid_t       map;

  array_t         physicals;
  array_t         monitors;

#ifdef __cplusplus
  void            **private_vars;
#else //__cplusplus
  void            **private;
#endif //__cplusplus

} environment_t;


typedef struct _environment_array {
  int size;
  environment_t *elts;
} environment_array_t;

extern environment_array_t environments;

#ifdef __cplusplus
extern "C"{
#endif


/* ************************************************** */
/* ************************************************** */
int            get_environmentid_by_name(char *name);
environment_t *get_environment_by_name  (char *name);
environment_t *get_environment_by_id    (environmentid_t id);

int            environments_bootstrap   (void);
void           environments_clean       (void);
int            environments_unbind      (void);

#ifdef __cplusplus
}
#endif

#endif // __environment__
