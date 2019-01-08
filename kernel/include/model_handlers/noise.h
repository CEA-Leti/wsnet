/**
 *  \file   noise.h
 *  \brief  Noise management declarations
 *  \author Guillaume Chelius
 *  \date   2007
 **/
#ifndef __noise__
#define __noise__

#include <kernel/include/modelutils.h>


/* ************************************************** */
/* ************************************************** */
extern class_t *noise_class;
extern class_t *interference_class;


/* ************************************************** */
/* ************************************************** */
#ifdef __cplusplus
extern "C"{
#endif

int noise_init(void);
int noise_bootstrap(void);
void noise_clean(void);


/* ************************************************** */
/* ************************************************** */
void noise_packet_cs(call_t *to, packet_t *packet);
void noise_packet_rx(call_t *to, packet_t *packet);
double medium_get_noise(call_t *to, int channel);

double noise_get_noise(call_t *to_interface, uint64_t bandwidth_signal_Hz);

#ifdef __cplusplus
}
#endif

#endif //__noise__
