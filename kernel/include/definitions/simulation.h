/**
 *  \file   simulation.h
 *  \brief  Simulation declarations
 *  \author Loic Lemaitre
 *  \date   2010
 */

#ifndef __simulation__
#define __simulation__

#include <kernel/include/definitions/types.h>

/* ************************************************** */
/* ************************************************** */
extern array_t   monitors;
extern classid_t global_map;


/* ************************************************** */
/* ************************************************** */
#ifdef __cplusplus
extern "C"{
#endif

int  simulation_bootstrap(void);
void simulation_clean    (void);
int  simulation_unbind   (void);


#ifdef __cplusplus
}
#endif

#endif //__simulation__
