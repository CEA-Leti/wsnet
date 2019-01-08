/**
 *  \file   mobility.h
 *  \brief  Mobility declarations
 *  \author Guillaume Chelius
 *  \date   2007
 **/
#ifndef __mobility__
#define __mobility__

#include <stdint.h>

/* ************************************************** */
/* ************************************************** */
#define MOBILITY_UPDATE 200000000

/* ************************************************** */
/* ************************************************** */
#ifdef __cplusplus
extern "C"{
#endif


int  mobility_init     (void);
int  mobility_bootstrap(void);
void mobility_clean    (void);


/* ************************************************** */
/* ************************************************** */
void mobility_update(uint64_t clock);
void mobility_event (uint64_t clock);

/* ************************************************** */
/* ************************************************** */
angle_t mobility_get_angle(call_t *to);
double mobility_get_speed(call_t *to);
double mobility_get_mutual_orientation(call_t *mobility_src, call_t *mobility_dst);

#ifdef __cplusplus
}
#endif

#endif //__mobility__
