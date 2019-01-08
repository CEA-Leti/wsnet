/**
 *  \file   noise.c
 *  \brief  Radio noise management
 *  \author Guillaume Chelius & Elyes Ben Hamida & Loic Lemaitre
 *  \date   2007
 **/

#include <string.h>
#include <stdio.h>

#include <kernel/include/data_structures/mem_fs/mem_fs.h>
#include <kernel/include/options.h>
#include <kernel/include/definitions/types.h>
#include <kernel/include/definitions/class.h>
#include <kernel/include/definitions/medium.h>
#include <kernel/include/definitions/nodearch.h>
#include <kernel/include/definitions/node.h>
#include <kernel/include/scheduler/scheduler.h>
#include <kernel/include/model_handlers/transceiver.h>
#include "media_rxtx.h"
#include "modulation.h"
#include "interface.h"

#define INTERMODULATION 0

const double default_white_noise = -174; // default -174 for KTB
/**
 * TODO:
 *  - optimize update_noise function, too many parameters, pointers and it seems incredible to have to use the factor correlation[packet->channel][packet->channe]
 **/

/* ************************************************** */
/* ************************************************** */
/* Liste doublement chaines */
typedef struct noise_interval {
    uint64_t begin;
    uint64_t end;
    int active;

    double power;
    int channel;

    double noise[CHANNELS_NUMBER];

    struct noise_interval *next;
    struct noise_interval *prev;
} noise_interval_t;

typedef struct noise {
    noise_interval_t *current;
    noise_interval_t *first;
    noise_interval_t *last;
} noise_t;


/* ************************************************** */
/* ************************************************** */
static void *mem_interval = NULL;

double AlphaCorr[MAX_MEDIUMS][CHANNELS_NUMBER][CHANNELS_NUMBER];
double BetaCorr[CHANNELS_NUMBER][CHANNELS_NUMBER][CHANNELS_NUMBER];
double GammaCorr[MAX_MEDIUMS][CHANNELS_NUMBER][CHANNELS_NUMBER];

//class_t *noise_class = NULL;
//class_t *interference_class = NULL;


/* ************************************************** */
/* ************************************************** */
//double get_white_noise(int node, int channel, mediumid_t medium);
double get_white_noise(call_t *from, int channel, mediumid_t medium);


/* ************************************************** */
/* ************************************************** */
/* Renvoie le bruit attribue au noeud */
noise_t *get_noise_byid(call_t *to) {
  node_t *node = get_node_by_id(to->object);
  noise_t *noise = ((noise_t *)  node->noises) + to->class;
  return noise;
}


/* ************************************************** */
/* ************************************************** */
/* Initialise la liste dbl chaines en memoire */
int noise_init(void) {
  if ((mem_interval = mem_fs_slice_declare(sizeof(noise_interval_t))) == NULL) {
    return -1;
  }

  return 0;
}

int noise_bootstrap(void) {
  int i, j, k;


  call_t from = {-1, -1};

#if (INTERMODULATION != 0)

  /* fill correlation array */
  for (i = 0; i < CHANNELS_NUMBER; i++) {
    for (j = 0; j < CHANNELS_NUMBER; j++) {
      for (k = 0; k < CHANNELS_NUMBER; k++) {
        medium_t *medium = get_medium_by_id(0);
        class_t  *class  = get_class_by_id(medium->intermodulation);
        BetaCorr[i][j][k] = class->methods->intermodulation.intermod(&to, &from,i, j, k);
        //fprintf(stderr,"%lf \t",BetaCorr[i][j][k]);
      }
      //fprintf(stderr,"\n");
    }
    //fprintf(stderr,"\n");
  }


  /* fill correlation array */
  for (i = 0; i < mediums.size; i++) {
    for (j = 0; j < CHANNELS_NUMBER; j++) {
      for (k = 0; k < CHANNELS_NUMBER; k++) {
        medium_t *medium = get_medium_by_id(i);
        class_t  *class  = get_class_by_id(medium->interferences);
        AlphaCorr[medium->id][j][k] = class->methods->interferences.interfere(&to, &from, j, k);
        //printf("%lf \t",AlphaCorr[i][j][k]);
      }
      //printf("\n");
    }
    //printf("\n");
  }
#else


  /* fill correlation array */
  for (i = 0; i < mediums.size; i++) {
    for (j = 0; j < CHANNELS_NUMBER; j++) {
      for (k = 0; k < CHANNELS_NUMBER; k++) {
        medium_t *medium = get_medium_by_id(i);
        call_t to   = {medium->interferences, -1};
        class_t  *class  = get_class_by_id(medium->interferences);
        AlphaCorr[medium->id][j][k] = class->methods->interferences.interfere(&to, &from, j, k);
        //fprintf(stderr , "%lf ", AlphaCorr[medium->id][j][k]);
      }
      //   fprintf(stderr,"\n");
    }
    //  fprintf(stderr,"\n");
  }
#endif
  /* initialize noises */
  for (i = 0; i < nodes.size; i++) {
    node_t *node = get_node_by_id(i);
    nodearch_t *nodearch = get_nodearch_by_id(node->nodearch);
    int j;

    /* allocate noises */
    node->noises = (void *) malloc(sizeof(noise_t) * classes.size);

    /* allocate antenna noises */
    for (j = 0; j < nodearch->interfaces.size; j++) {
      call_t to = {nodearch->interfaces.elts[j], i};
      noise_t *noise = get_noise_byid(&to);
      noise->first = NULL;
      noise->current = NULL;
      noise->last = NULL;
    }
  }

  return 0;
}

void noise_clean(void) {
  int i;

  if (nodes.elts == NULL) {
    return;
  }

  for (i = 0; i < nodes.size; i++) {
    node_t *node = get_node_by_id(i);
    nodearch_t *nodearch = get_nodearch_by_id(node->nodearch);
    int j;

    if (node->noises == NULL) {
      continue;
    }

    for (j = 0; j < nodearch->interfaces.size; j++) {
      call_t to = {nodearch->interfaces.elts[j], i};
      noise_t *noise = get_noise_byid(&to);
      noise_interval_t *noise_interval;

      while ((noise_interval = noise->first)) {
        noise->first = noise_interval->next;
        mem_fs_dealloc(mem_interval, noise_interval);
      }
    }

    free(node->noises);
  }

  return;
}


/* ************************************************** */
/* ************************************************** */
//add_signal2noiseIntermod(interval,interval->noise, interval->channel, packet->channel, packet->rxmW, medium);
#if (INTERMODULATION != 0)
void add_signal2noiseIntermod(noise_interval_t *interval, double *noise, int channel1, int channel2, double signal, mediumid_t medium) {
  int i,j;
  double intermodThirdOrder;

  for (i = 0; i < CHANNELS_NUMBER; i++) {
    //printf("noise[%d] = %lf\n",i,mW2dBm(noise[i]));
    noise[i] += AlphaCorr[medium][i][channel2] * signal;
    //printf("noise[%d] = %lf, Alpha = %lf\n",i,mW2dBm(noise[i]),AlphaCorr[medium][i][channel2]);

    noise_interval_t *tmp_interval = (noise_interval_t *) mem_fs_alloc(mem_interval);
    memcpy(tmp_interval, interval, sizeof(noise_interval_t));


    for(j = 0; j <= tmp_interval->active; j++) {
      intermodThirdOrder = BetaCorr[i][tmp_interval->channel][channel2]*(tmp_interval->power)*(tmp_interval->power)*signal+ BetaCorr[i][channel2][tmp_interval->channel]*signal*(tmp_interval->power)*(tmp_interval->power);
      noise[i] += intermodThirdOrder;

      //printf("beta1*sig1*sig1*sig2 = %lf, beta2*sig1*sig1*sig2 = %lf \n",mW2dBm(BetaCorr[i][tmp_interval->channel][channel2]*(tmp_interval->power)*(tmp_interval->power)*signal),mW2dBm(BetaCorr[i][channel2][tmp_interval->channel]*signal*(tmp_interval->power)*(tmp_interval->power)));

      //printf("Intermod : noise[%d] %f, signal1 %f, signal2 %f, Beta1 %lf, Beta2 %lf, intermodThirdOrder %lf\n", i,mW2dBm(noise[i]), mW2dBm(signal), mW2dBm(tmp_interval->power),BetaCorr[i][tmp_interval->channel][channel2],BetaCorr[i][channel2][tmp_interval->channel], mW2dBm(intermodThirdOrder));
      if (tmp_interval->next == NULL) {
        break;
      }
      else {
        tmp_interval = tmp_interval->next;
      }
      //printf("packet : %lf, tmp_interval->power : %lf\n",mW2dBm(signal),mW2dBm(tmp_interval->power));
    }
    //printf("add_signal2noiseIntermod : noise[%d] %f, signal1 %f, signal2 %f, Alpha %lf, Beta %lf\n", i,mW2dBm(noise[i]), mW2dBm(signal), mW2dBm(tmp_interval->power),AlphaCorr[medium][i][channel1],BetaCorr[i][channel1][channel2]);
  }
}


// Calcul de : alpha*signal + beta*signal^2 + gamma*signal^3
void add_new_signal2noiseIntermod(double *noise, int channel1, int channel2, double signal1, double signal2, mediumid_t medium) {
  int i;
  for (i = 0; i < CHANNELS_NUMBER; i++) {
    noise[i] = AlphaCorr[medium][i][channel1] * signal1;
    //printf("add_new_signal2noiseIntermod : noise[%d] %f, signal1 %f, signal2 %f, AlphaCorr %lf\n", i,mW2dBm(noise[i]), mW2dBm(signal1), mW2dBm(signal2),AlphaCorr[medium][i][channel1]);
  }
}
#else


void add_signal2noise(double *noise, int channel, double signal, mediumid_t medium) {
  int i;
  for (i = 0; i < CHANNELS_NUMBER; i++) {
    noise[i] += AlphaCorr[medium][i][channel] * signal;
    //fprintf(stderr,"add_signal2noise : noise[%d] %f, signal %f, Alpha %lf\n", i,mW2dBm(noise[i]), mW2dBm(signal), AlphaCorr[medium][i][channel]);
  }
}

void add_new_signal2noise(double *noise, int channel, double signal, mediumid_t medium) {
  int i;
  for (i = 0; i < CHANNELS_NUMBER; i++) {
    noise[i] = AlphaCorr[medium][i][channel] * signal;
    //fprintf(stderr,"add_new_signal2noise : noise[%d] %f, signal %f, Alpha %lf\n", i,mW2dBm(noise[i]), mW2dBm(signal), AlphaCorr[medium][i][channel]);
  }
}
#endif

/* ************************************************** */
/* ************************************************** */
void noise_update_current(noise_t *noise, uint64_t time) {
  while (noise->current && (noise->current->end <= time)) {
    noise->current = noise->current->next;
  }
}

int add_packet2interval(noise_t *noise, noise_interval_t *interval, packet_t *packet, mediumid_t medium) {
  if ((interval->begin >= packet->clock0) && (interval->end <= packet->clock1)) {
    //printf("=> In add_paquet2interval : interval->begin(%lf)>=clock0 & interval->end(%lf)<=clock1\n",(double)interval->begin,(double)interval->end);
#if (INTERMODULATION != 0)
    add_signal2noiseIntermod(interval,interval->noise, interval->channel, packet->channel, packet->rxmW, medium);
#else
    add_signal2noise(interval->noise, packet->channel, packet->rxmW, medium);
#endif
    interval->active++;
    if (interval->end == packet->clock1) {
      return -1;
    } else {
      return 0;
    }
  } else if ((interval->begin >= packet->clock0) && (interval->end > packet->clock1)) {
    //printf("=> In add_paquet2interval : begin>=clock0(%lf) & end>clock1(%lf)\n",(double)interval->begin,(double)interval->end);
    noise_interval_t *n_interval = (noise_interval_t *) mem_fs_alloc(mem_interval);

    /* add new interval at the end */
    memcpy(n_interval, interval, sizeof(noise_interval_t));
    n_interval->begin = packet->clock1;
    n_interval->prev = interval;

    /* update next prev link or noise->last */
    if (n_interval->next)
      n_interval->next->prev = n_interval;
    else
      noise->last = n_interval;

    /* update considered interval */
#if (INTERMODULATION != 0)
    add_signal2noiseIntermod(interval,interval->noise, interval->channel, packet->channel, packet->rxmW, medium);
#else
    add_signal2noise(interval->noise, packet->channel, packet->rxmW, medium);
#endif
    interval->active++;
    interval->end = packet->clock1;
    interval->next = n_interval;
    return -1;
  } else if ((interval->begin < packet->clock0) && (interval->end <= packet->clock1)) {
    //printf("=> In add_paquet2interval : interval->begin(%lf)<clock0 & interval->end(%lf)<=clock1\n",(double)interval->begin,(double)interval->end);
    noise_interval_t *p_interval = (noise_interval_t *) mem_fs_alloc(mem_interval);

    /* add new interval at the beginning */
    memcpy(p_interval, interval, sizeof(noise_interval_t));
    p_interval->end = packet->clock0;
    p_interval->next = interval;

    /* update next prev link or noise->last */
    if (p_interval->prev)
      p_interval->prev->next = p_interval;
    else
      noise->first = p_interval;

    /* update considered interval */
#if (INTERMODULATION != 0)
    add_signal2noiseIntermod(interval,interval->noise, interval->channel, packet->channel, packet->rxmW, medium);
#else
    add_signal2noise(interval->noise, packet->channel, packet->rxmW, medium);
#endif
    interval->active++;
    interval->begin = packet->clock0;
    interval->prev = p_interval;
    if (interval->end == packet->clock1) {
      return -1;
    } else {
      return 0;
    }
  } else if ((interval->begin < packet->clock0) && (interval->end > packet->clock1)) {
    //printf("=> In add_paquet2interval : interval->begin(%lf)<clock0 & interval->end(%lf)>clock1\n",(double)interval->begin,(double)interval->end);
    noise_interval_t *n_interval = (noise_interval_t *) mem_fs_alloc(mem_interval);
    noise_interval_t *p_interval = (noise_interval_t *) mem_fs_alloc(mem_interval);

    /* add new interval at the beginning */
    memcpy(p_interval, interval, sizeof(noise_interval_t));
    p_interval->end = packet->clock0;
    p_interval->next = interval;

    /* update next prev link or noise->last */
    if (p_interval->prev)
      p_interval->prev->next = p_interval;
    else
      noise->first = p_interval;

    /* add new interval at the end */
    memcpy(n_interval, interval, sizeof(noise_interval_t));
    n_interval->begin = packet->clock1;
    n_interval->prev = interval;

    /* update next prev link or noise->last */
    if (n_interval->next)
      n_interval->next->prev = n_interval;
    else
      noise->last = n_interval;

    /* update considered interval */
#if (INTERMODULATION != 0)
    add_signal2noiseIntermod(interval,interval->noise, interval->channel, packet->channel, packet->rxmW, medium);
#else
    add_signal2noise(interval->noise, packet->channel, packet->rxmW, medium);
#endif
    interval->active++;
    interval->begin = packet->clock0;
    interval->end = packet->clock1;
    interval->prev = p_interval;
    interval->next = n_interval;
    return -1;
  }

  return -1;
}


void noise_packet_cs(call_t *to, packet_t *packet) {
  uint64_t time = packet->clock0;
  noise_t *noise = get_noise_byid(to);
  noise_interval_t *interval;
  call_t from = {-1, -1};
  mediumid_t medium = interface_get_medium(to, &from);

  /* edit by Luiz Henrique Suraty Filho*/
  /* Calculate the LQI*/
  /* TODO: Improve the calculation (to become more realistic) as indicated in https://www.anaren.com/air-wiki-zigbee/Link_Quality_Indication and
   * http://e2e.ti.com/support/wireless_connectivity/w/design_notes/calculation-and-usage-of-lqi-and-rssi
   * In this way, the LQI would be a value between 0x00 and 0xFF.
   */
  //packet->LQI = pow(1-do_modulate(packet->modulation,packet->rxmW,get_white_noise(to->object,packet->channel,medium)), packet->real_size);
  packet->LQI = pow(1-do_modulate(to, packet->modulation,packet->rxmW,get_white_noise(to,packet->channel,medium)), packet->real_size);

  /* end of edition */

  /* go to current */
  noise_update_current(noise, time);
  //printf("packet %d, clock0 %lf, clock1 %lf\n",packet->id,(double)packet->clock0,(double)packet->clock1);
  if ((noise->current) == NULL && (noise->last == NULL)) {
    //printf("Aucun bruit dans la liste noise Node ID : %d, time : %ju\n",to->object,get_time());
    interval = (noise_interval_t *) mem_fs_alloc(mem_interval);
    interval->active = 1;
    interval->power = packet->rxmW;
    interval->channel = packet->channel;
    interval->begin = time;
    interval->end = packet->clock1;
    interval->next = NULL;
    interval->prev = NULL;
#if (INTERMODULATION != 0)
    add_new_signal2noiseIntermod(interval->noise, packet->channel, interval->channel, packet->rxmW, interval->power, medium);
    //printf("Add_new_signal2noise, node %d packet %d, channel %d\n",to->object,packet->id,packet->channel);
#else
    add_new_signal2noise(interval->noise, packet->channel, packet->rxmW, medium);
#endif
    noise->first = noise->current = noise->last = interval;
    return;
  } else if( (noise->current) == NULL && (noise->last != NULL)) {
    //printf("Il existe encore du bruit dans la liste noise Node ID : %d\n",to->object);
    /* successive */
    interval = (noise_interval_t *) mem_fs_alloc(mem_interval);
    interval->active = 1;
    interval->power = packet->rxmW;
    interval->channel = packet->channel;
    interval->begin = time;
    interval->end = packet->clock1;
    interval->next = NULL;
    interval->prev = noise->last;
#if (INTERMODULATION != 0)
    add_new_signal2noiseIntermod(interval->noise, packet->channel, interval->channel, packet->rxmW, interval->power, medium);
    //printf("Add_new_signal2noise, packet %d, channel %d\n",packet->id,packet->channel);
#else
    add_new_signal2noise(interval->noise, packet->channel, packet->rxmW, medium);
#endif

    noise->last->next = interval;
    noise->last = interval;
    return;
  }

  /* update intervals */
  interval = noise->current;
  //printf("~~~~~~~~~~~ Add Interference  Node %d, Packetid %d, active %d \n",to->object,packet->id,interval->active);
  while (interval) {
    if (add_packet2interval(noise, interval, packet, medium)) {
      //printf("add_packet2interval packet : %d, Node : %d\n",packet->id,to->object);
      return;
    }
    interval = interval->next;
    //printf("interval go next Node %d\n",to->object);
  }

  /* append a new interval */
  if (interval == NULL) {
    //printf("interval = NULL\n");
    interval = (noise_interval_t *) mem_fs_alloc(mem_interval);
    interval->active = 1;
    interval->power = packet->rxmW;
    interval->channel = packet->channel;
    interval->begin = noise->last->end;
    interval->end = packet->clock1;
    interval->next = NULL;
    interval->prev = noise->last;
#if (INTERMODULATION != 0)
    add_new_signal2noiseIntermod(interval->noise, packet->channel, interval->channel, packet->rxmW, interval->power, medium);
#else
    add_new_signal2noise(interval->noise, packet->channel, packet->rxmW, medium);
#endif

    noise->last->next = interval;
    noise->last = interval;
  }
}


/* ************************************************** */
/* ************************************************** */
static inline uint64_t min(uint64_t a, uint64_t b) {
  return (a < b ? a : b);
}

/*static inline uint64_t max(uint64_t a, uint64_t b) {
  return (a > b ? a : b);
}*/

//#define AVG_NOISE

//void update_noise(noise_interval_t *interval, nodeid_t node, packet_t *packet, uint64_t *f_begin,
void update_noise(noise_interval_t *interval, call_t *to, packet_t *packet, uint64_t *f_begin,
    uint64_t *f_end, uint64_t *f_duration, int *f_current, mediumid_t medium) {
  classid_t noise_class = get_medium_by_id(medium)->noise;

  if (interval->begin > (*f_begin)) {
    //printf("  1a----> interval->begin (%.2fs) > (*f_begin) {%.2fs}  : Node %d, interval->noise %.2f, Noise_mW[%d] %.2f, fact %lf\n", TIME_TO_MILLISECONDS(interval->begin), TIME_TO_MILLISECONDS(*f_begin), node, fmax(-500,mW2dBm(interval->noise[packet->channel])), *f_current, fmax(-500,mW2dBm(packet->noise_mW[(*f_current)])), ((double) min((*f_end), interval->end) - interval->begin) / ((double) (*f_duration)));
    /* interval begins in this frame: update for min((*f_end), interval->end) - interval->begin */
#ifdef AVG_NOISE
    packet->noise_mW[(*f_current)] += (interval->noise[packet->channel] - AlphaCorr[medium][packet->channel][packet->channel] * packet->rxmW)
              * ((double) min((*f_end), interval->end) - interval->begin) / ((double) (*f_duration));
#else /*AVG_NOISE*/
    packet->noise_mW[(*f_current)] = fmax((interval->noise[packet->channel] - AlphaCorr[medium][packet->channel][packet->channel] * packet->rxmW),  packet->noise_mW[(*f_current)]);
#endif /*AVG_NOISE             */
    //printf(" Noise on interval = %.2f \n", fmax(-500,mW2dBm(fmax((interval->noise[packet->channel] - AlphaCorr[medium][packet->channel][packet->channel] * packet->rxmW),  packet->noise_mW[(*f_current)]))));
    //printf("  1b----> interval->begin (%.2fs) > (*f_begin) {%.2fs}  : Node %d, interval->noise %.2f, Noise_mW[%d] %.2f, fact %lf\n", TIME_TO_MILLISECONDS(interval->begin), TIME_TO_MILLISECONDS(*f_begin), node, fmax(-500,mW2dBm(interval->noise[packet->channel])), *f_current, fmax(-500,mW2dBm(packet->noise_mW[(*f_current)])), ((double) min((*f_end), interval->end) - interval->begin) / ((double) (*f_duration)));
    //printf("BER[%d] %lf\n", (*f_current),packet->ber[(*f_current)]);
  } else if (interval->begin == (*f_begin)) {

    //printf("  2a----> interval->begin (%.2fs) == (*f_begin) {%.2fs}  : Node %d, interval->noise %.2f, Noise_mW[%d] %.2f, fact %lf\n", TIME_TO_MILLISECONDS(interval->begin), TIME_TO_MILLISECONDS(*f_begin), node, fmax(-500,mW2dBm(interval->noise[packet->channel])), *f_current, fmax(-500,mW2dBm(packet->noise_mW[(*f_current)])), ((double) min((*f_end), interval->end) - interval->begin) / ((double) (*f_duration)));
    /* interval begins with this frame: update for min((*f_end), interval->end) - interval->begin */
#ifdef AVG_NOISE
    packet->noise_mW[(*f_current)] += (interval->noise[packet->channel] - AlphaCorr[medium][packet->channel][packet->channel] * packet->rxmW)
              * ((double) min((*f_end), interval->end) - interval->begin) / ((double) (*f_duration));
#else /*AVG_NOISE*/
    packet->noise_mW[(*f_current)] = fmax((interval->noise[packet->channel] - AlphaCorr[medium][packet->channel][packet->channel] * packet->rxmW), packet->noise_mW[(*f_current)]);
#endif /*AVG_NOISE*/
    //printf(" Noise on interval = %.2f \n", fmax(-500,mW2dBm(fmax((interval->noise[packet->channel] - AlphaCorr[medium][packet->channel][packet->channel] * packet->rxmW),  packet->noise_mW[(*f_current)]))));
    //printf("  2b----> interval->begin (%.2fs) == (*f_begin) {%.2fs}  : Node %d,(packet_id=%d) (packet_rxdbm=%lf) interval->noise %lf, Noise_mW[%d] %lf, fact %lf\n", TIME_TO_MILLISECONDS(interval->begin), TIME_TO_MILLISECONDS(*f_begin), node, packet->id, packet->rxdBm, fmax(-500,mW2dBm(interval->noise[packet->channel])), *f_current, fmax(-500,mW2dBm(packet->noise_mW[(*f_current)])), ((double) min((*f_end), interval->end) - interval->begin) / ((double) (*f_duration)));
    /* add white/statistical noise, modulate, go to next frame and next interval */
    if (noise_class != -1) {
      //packet->noise_mW[(*f_current)] += get_white_noise(node, packet->channel, medium);
      packet->noise_mW[(*f_current)] += get_white_noise(to, packet->channel, medium);
    }

    //printf("  2b (white)----> interval->begin (%.2fs) == (*f_begin) {%.2fs}  : Node %d, interval->noise %lf, Noise_mW[%d] %lf, fact %lf\n", TIME_TO_MILLISECONDS(interval->begin), TIME_TO_MILLISECONDS(*f_begin), node, fmax(-500,mW2dBm(interval->noise[packet->channel])), *f_current, fmax(-500,mW2dBm(packet->noise_mW[(*f_current)])), ((double) min((*f_end), interval->end) - interval->begin) / ((double) (*f_duration)));
    //packet->ber[(*f_current)] = do_modulate(packet->modulation, packet->rxmW, packet->noise_mW[(*f_current)]);
    packet->ber[(*f_current)] = do_modulate(to, packet->modulation, packet->rxmW, packet->noise_mW[(*f_current)]);
    //printf("\nBER[%d] %.20lf\n", (*f_current),packet->ber[(*f_current)]);
    packet->PER *= pow((1 - packet->ber[(*f_current)]), (*f_duration) / packet->Tb);

    //printf("-------->>>>>>>>> PER %lf, Factor %lf\n", packet->PER,((double)(*f_duration)/packet->Tb));
    (*f_current)--;
    (*f_end) = (*f_begin);
    if ((*f_current) == 0) {
      (*f_begin) = packet->clock0;
      (*f_duration) = (*f_end) - (*f_begin);
      packet->noise_mW[(*f_current)] = 0;
    } else if ((*f_current) > 0) {
      (*f_begin) = (*f_end) - (*f_duration);
      packet->noise_mW[(*f_current)] = 0;
    }

  } else if (interval->begin < (*f_begin)) {

    //printf("  3a ----> interval->begin (%.2fs) < (*f_begin) {%.2fs}  : Node %d, interval->noise %.2f, Noise_mW[%d] %.2f, fact %lf\n", TIME_TO_MILLISECONDS(interval->begin), TIME_TO_MILLISECONDS(*f_begin), node, fmax(-500,mW2dBm(interval->noise[packet->channel])), *f_current, fmax(-500,mW2dBm(packet->noise_mW[(*f_current)])), ((double) min((*f_end), interval->end) - interval->begin) / ((double) (*f_duration)));
    /* interval begins before this frame: update for min((*f_end), interval->end) - (*f_begin) */
#ifdef AVG_NOISE
    packet->noise_mW[(*f_current)] += (interval->noise[packet->channel] - AlphaCorr[medium][packet->channel][packet->channel] * packet->rxmW)
              * ((double) min((*f_end), interval->end) - (*f_begin)) / ((double) (*f_duration));
#else /*AVG_NOISE*/
    packet->noise_mW[(*f_current)] = fmax((interval->noise[packet->channel] - AlphaCorr[medium][packet->channel][packet->channel] * packet->rxmW), packet->noise_mW[(*f_current)]);
#endif /*AVG_NOISE*/
    //printf(" Noise on interval = %.2f \n", fmax(-500,mW2dBm(fmax((interval->noise[packet->channel] - AlphaCorr[medium][packet->channel][packet->channel] * packet->rxmW),  packet->noise_mW[(*f_current)]))));
    /* add white/statistical noise, modulate, go to next frame but keep same interval */
    if (noise_class != -1) {
      //packet->noise_mW[(*f_current)] += get_white_noise(node, packet->channel, medium);
      packet->noise_mW[(*f_current)] += get_white_noise(to, packet->channel, medium);
    }
    //printf("  3b ----> interval->begin (%.2fs) < (*f_begin) {%.2fs}  : Node %d, interval->noise %.2f, Noise_mW[%d] %.2f, fact %lf\n", TIME_TO_MILLISECONDS(interval->begin), TIME_TO_MILLISECONDS(*f_begin), node, fmax(-500,mW2dBm(interval->noise[packet->channel])), *f_current, fmax(-500,mW2dBm(packet->noise_mW[(*f_current)])), ((double) min((*f_end), interval->end) - interval->begin) / ((double) (*f_duration)));
    //packet->ber[(*f_current)] = do_modulate(packet->modulation, packet->rxmW,  packet->noise_mW[(*f_current)]);
    packet->ber[(*f_current)] = do_modulate(to, packet->modulation, packet->rxmW,  packet->noise_mW[(*f_current)]);
    //printf("BER[%d] %lf\n", (*f_current),packet->ber[(*f_current)]);
    packet->PER *= pow((1 - packet->ber[(*f_current)]), (*f_duration) / packet->Tb);
    //fprintf(stderr,"-------->>>>>>>>> PER %lf, Factor %lf\n",packet->PER,((double)(*f_duration)/packet->Tb));
    (*f_current)--;
    (*f_end) = (*f_begin);
    if ((*f_current) == 0) {
      (*f_begin) = packet->clock0;
      (*f_duration) = (*f_end) - (*f_begin);
      packet->noise_mW[(*f_current)] = 0;
    } else if ((*f_current) > 0) {
      (*f_begin) = (*f_end) - (*f_duration);
      packet->noise_mW[(*f_current)] = 0;
    }

    //update_noise(interval, node, packet, f_begin, f_end, f_duration, f_current, medium);
    update_noise(interval, to, packet, f_begin, f_end, f_duration, f_current, medium);
  }
}

void noise_packet_rx(call_t *to, packet_t *packet) {
  noise_t *noise = get_noise_byid(to);
  noise_interval_t *interval;
  uint64_t f_begin;
  uint64_t f_end;
  uint64_t f_duration = 0;
  int f_current = 0;
  call_t from = {-1, -1};
  mediumid_t medium = interface_get_medium(to, &from);

  /* set frame informations */
  f_end = packet->clock1;
#if (SNR_STEP > 0)
  f_duration = packet->duration / SNR_STEP;
  f_current = min(ceil(packet->real_size/8), SNR_STEP - 1);
  //printf("f_duration %ju, f_current %d\n",f_duration,f_current);
#elif (SNR_STEP < 0)
  f_duration = packet->duration / ceil(packet->real_size/8);
  f_current = ceil(packet->real_size/8) - 1;
  //printf("f_duration %ju, f_current %d\n",f_duration,f_current);
#endif /*SNR_STEP*/
  if (f_current == 0) {
    f_begin = packet->clock0;
    f_duration = f_end - f_begin;
  } else {
    f_begin = f_end - f_duration;
  }
  packet->noise_mW[f_current] = 0;

  /* get last considered frame */
  noise_update_current(noise, packet->clock1);
  if (noise->current == NULL) {
    interval = noise->last;
  } else {
    interval = noise->current->prev;
  }

  /* update activiy interval */
  while (interval) {
    /* we have reached the end of the packet */
    if (interval->end <= packet->clock0) {
      return;
    }

    /* update interval active packets */
    interval->active--;

    /* all previous intervals should be removed */
    if (interval->active == 0) {

      /* update first pointer */
      if ((noise->first = interval->next) == NULL) {
        noise->last = noise->current = NULL;
      } else {
        noise->first->prev = NULL;
      }

      /* remove intervals */
      while (interval) {
        noise_interval_t *t_interval = interval;


        /* update noise */
        if (interval->end > packet->clock0) {
          //printf("node %d, interval => noise[0]:%f, noise[1]:%f, noise[2]:%f,noise[3]:%f \n",to->object,mW2dBm(t_interval->noise[0]),mW2dBm(t_interval->noise[1]),mW2dBm(t_interval->noise[2]),mW2dBm(t_interval->noise[3]));
          //update_noise(interval, to->object, packet, &f_begin, &f_end, &f_duration, &f_current, medium);
          update_noise(interval, to, packet, &f_begin, &f_end, &f_duration, &f_current, medium);
        }

        /* destroy interval */
        interval = interval->prev;
        mem_fs_dealloc(mem_interval, t_interval);
      }

      return;
    }

    /* update noise */
    //update_noise(interval, to->object, packet, &f_begin, &f_end, &f_duration, &f_current, medium);
    update_noise(interval, to, packet, &f_begin, &f_end, &f_duration, &f_current, medium);

    interval = interval->prev;
  }

  return;
}


/* ************************************************** */
/* ************************************************** */
double medium_get_noise(call_t *to, int channel) {
  uint64_t time = get_time();
  noise_t *noise = get_noise_byid(to);
  call_t from = {-1, -1};
  mediumid_t medium = interface_get_medium(to, &from);
  classid_t noise_class = get_medium_by_id(medium)->noise;
  double value = 0;

  /* deterministic noise */
  noise_update_current(noise, time);
  if (noise->current) {
    value = noise->current->noise[channel];
  } else {
    value = 0;
  }

  /* white/statistical noise */
  if (noise_class != -1) {
    //value += get_white_noise(to->object, channel, medium);
    value += get_white_noise(to, channel, medium);
  }

  /* convert to dBm */
  return mW2dBm(value);
}


/* ************************************************** */
/* ************************************************** */
//double get_white_noise(int node, int channel, mediumid_t medium) {
double get_white_noise(call_t *from, int channel, mediumid_t medium) {
  int node = from->object;
  classid_t noise_class = get_medium_by_id(medium)->noise;

  if (noise_class == -1) {
    return 0;
  } else {
    call_t to = {noise_class, -1};
    //return get_class_by_id(noise_class)->methods->noise.noise(&to, NULL, node, channel);
    return get_class_by_id(noise_class)->methods->noise.noise(&to, from, node, channel);
  }
}

double get_thermal_white_noise(call_t *from, mediumid_t medium) {
  int node = from->object;
  classid_t noise_class = get_medium_by_id(medium)->noise;

  if (noise_class == -1) {
    return default_white_noise; // default -174 for KTB
  } else {
    call_t to = {noise_class, -1};
    //return get_class_by_id(noise_class)->methods->noise.noise(&to, NULL, node, channel);
    return get_class_by_id(noise_class)->methods->noise.noise(&to, from, node, 0);
  }
}

double noise_get_noise(call_t *to_interface, uint64_t bandwidth_signal_Hz){
  call_t to_transceiver = {get_tranceiver_classesid(to_interface)->elts[0], to_interface->object};
  mediumid_t medium = interface_get_medium(to_interface, &to_transceiver);
  double noise_factor_dB = transceiver_get_noise(&to_transceiver, to_interface);
  double No_dB = get_thermal_white_noise(to_interface,medium);
  return dBm2mW(No_dB + noise_factor_dB + 10 * log10(bandwidth_signal_Hz));
}
