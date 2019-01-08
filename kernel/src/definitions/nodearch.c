/**
 *  \file   nodearch.c
 *  \brief  Node architecture module
 *  \author Loic Lemaitre & Luiz Henrique Suraty Filho
 *  \date   2016
 */

#include <string.h>
#include <stdio.h>
#include "nodearch.h"


nodearch_array_t nodearchs = {0, NULL};


/* ************************************************** */
/* ************************************************** */
nodearchid_t get_nodearchid_by_name(char *name) {
  int i = nodearchs.size;
  while (i--)
    if (!strcmp(name, (nodearchs.elts + i)->name))
      break;
  return i;
}

nodearch_t *get_nodearch_by_name(char *name) {
  nodearchid_t id = get_nodearchid_by_name(name);
  return (id < 0) ? NULL : get_nodearch_by_id(id);
}

nodearch_t *get_nodearch_by_id(nodearchid_t id) {
  return nodearchs.elts + id;
}


/* ************************************************** */
/* ************************************************** */
void nodearchs_clean(void) {
  int i;

  if (nodearchs.elts && nodearchs.size) {
    for (i = 0; i < nodearchs.size; i++) {
      nodearch_t *nodearch = get_nodearch_by_id(i);

      if (nodearch->types.elts){
        free(nodearch->types.elts);
      }

      if (nodearch->classes.elts){
        free(nodearch->classes.elts);
      }

      if (nodearch->name) {
        free(nodearch->name);
      }

      if (nodearch->birth) {
        free(nodearch->birth);
      }
      ;
      if (nodearch->transceivers.elts) {
        free(nodearch->transceivers.elts);
      }

      if (nodearch->macs.elts) {
        free(nodearch->macs.elts);
      }

      if (nodearch->routings.elts) {
        free(nodearch->routings.elts);
      }

      if (nodearch->applications.elts) {
        free(nodearch->applications.elts);
      }
      if (nodearch->phys.elts) {
        free(nodearch->phys.elts);
      }
      if (nodearch->errors.elts) {
        free(nodearch->errors.elts);
      }
      if (nodearch->codings.elts) {
        free(nodearch->codings.elts);
      }
      if (nodearch->signal_trackers.elts) {
        free(nodearch->signal_trackers.elts);
      }
      if (nodearch->interferences.elts) {
        free(nodearch->interferences.elts);
      }
      if (nodearch->modulators.elts) {
    	  free(nodearch->modulators.elts);
      }

      if (nodearch->interfaces.elts) {
        free(nodearch->interfaces.elts);
      }

      if (nodearch->sensors.elts) {
        free(nodearch->sensors.elts);
      }

      if (nodearch->up)
      {
        int j;
        for (j = 0; j < nodearch->classes.size; j++) {
          if (nodearch->up[j].elts) {
            free(nodearch->up[j].elts);
          }
        }
        free(nodearch->up);
      }

      if (nodearch->down)
      {
        int j;
        for (j = 0; j < nodearch->classes.size; j++) {
          if (nodearch->down[j].elts) {
            free(nodearch->down[j].elts);
          }
        }
        free(nodearch->down);
      }
    }
    free(nodearchs.elts);
  }
}

