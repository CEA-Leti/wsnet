/**
 *  \file   media_rxtx.cc
 *  \brief  RX and TX management through the mediums.
 *  \author Luiz Henrique Suraty Filho
 *  \date   2017
 **/

#include <kernel/include/options.h>

#include <kernel/include/definitions/class.h>
#include <kernel/include/definitions/node.h>
#include <kernel/include/definitions/medium.h>
#include <kernel/include/definitions/nodearch.h>
#include <kernel/include/definitions/packet.h>
#include <kernel/include/definitions/grouparch.h>
#include <kernel/include/definitions/group.h>

#include <kernel/include/scheduler/scheduler.h>

#include <kernel/include/model_handlers/media_rxtx.h>
#include <kernel/include/model_handlers/transceiver.h>
#include <kernel/include/model_handlers/interface.h>
#include <kernel/include/model_handlers/noise.h>

#include <kernel/include/model_handlers/cxx_model_handlers.h>
#include <kernel/include/definitions/models/spectrum/spectrum_model.h>
#include <kernel/include/definitions/types/signal/signal.h>

/* ************************************************** */
/* ************************************************** */
double MEDIA_GET_NOISE(call_t *from_transceiver, call_t *from_interface, int channel)
{
  get_node_by_id(from_transceiver->object);
  return medium_get_noise(from_interface, channel);
}


/* ************************************************** */
/* ************************************************** */
void MEDIA_TX(call_t *from_transceiver, call_t *from_interface, packet_t *packet) {
  int        i      = get_node_count();
  node_t    *node   = get_node_by_id(from_interface->object);
  call_t     from0  = {-1, -1};
  medium_t  *medium = get_medium_by_id(interface_get_medium(from_interface, &from0));
  uint64_t   clock;

  // check wether node is active
  if (node->state != NODE_ACTIVE) {
    packet_dealloc(packet);
    return;
  }

  // fill packet info
  packet->node       = node->id;
  packet->interface  = from_interface->classid;
  packet->txdBm      = transceiver_get_power(from_transceiver, from_interface);
  packet->channel    = transceiver_get_channel(from_transceiver, from_interface);
  packet->modulation = transceiver_get_modulation(from_transceiver, from_interface);
  packet->Tb         = transceiver_get_Tb(from_transceiver, from_interface);
  //packet->duration = packet->size * packet->Tb * 8;
  // edit by Quentin Lampin <quentin.lampin@orange-ftgroup.com>
  packet->duration   = packet->real_size * packet->Tb;
  // end of edition
  packet->clock0     = get_time();
  packet->clock1     = packet->clock0 + packet->duration;

  // scheduler tx_end event
  scheduler_add_tx_end(packet->clock1, from_transceiver, from_interface, packet);

  // scheduler rx_begin event for old WSNET version (packet based)
  while (i--) {
    node_t     *rx_node  = get_node_by_id(i);
    double      dist     = distance(&(node->position), &(rx_node->position));
    double      travel_time   = dist / medium->speed_of_light;
    nodearch_t *nodearch = get_nodearch_by_id(rx_node->nodearch);
    int j;

    if (rx_node->state == NODE_DEAD) {
      continue;
    }

    if ((medium->propagation_range) && (dist > medium->propagation_range)) {
      continue;
    }

    for (j = 0; j < nodearch->interfaces.size; j++) {
      class_t  *interface_class = get_class_by_id(nodearch->interfaces.elts[j]);
      call_t    to_interface   = {interface_class->id, rx_node->id};

      // rx interface receives signal only if it is connected to the same medium than tx interface
      if (medium->id == interface_get_medium(&to_interface, from_interface)) {
        packet_t *packet_rx = packet_rxclone(packet);
        clock = packet->clock0 + ((uint64_t) travel_time);
        packet_rx->clock0 = clock;
        packet_rx->clock1 = clock + packet->duration;
        scheduler_add_rx_begin(clock, &to_interface, from_interface, packet_rx);
      }
    }
  }

}


void medium_tx_end(packet_t *packet, call_t *from_transceiver, call_t *from_interface) {
  node_t  *node  = get_node_by_id(from_transceiver->object);
  class_t *from_transceiver_class = get_class_by_id(from_transceiver->classid);
  // check wether node is active
  if (node->state != NODE_ACTIVE) {
    packet_dealloc(packet);
    return;
  }

  from_transceiver_class->methods->transceiver.tx_end(from_transceiver, from_interface, packet);

  //TODO : to be moved at least in tx_end of transceiver but MAC in more appropriated
  packet_dealloc(packet);

  return;
}


/* ************************************************** */
/* ************************************************** */
void medium_cs(packet_t *packet, call_t *to_interface, call_t *from_interface) {
  uint64_t  clock = get_time() + packet->duration;
  node_t   *node  = get_node_by_id(to_interface->object);

  // check wether the node is able to receive
  if (node->state != NODE_ACTIVE) {
    packet_dealloc(packet);
    return;
  }

  //  propagation
  medium_compute_rxdBm(packet, to_interface, from_interface);

  // filter
  if (packet->rxdBm == MIN_DBM) {
    packet_dealloc(packet);
    return;
  }

  // update noise
#if (SNR_STEP != 0)

  noise_packet_cs(to_interface, packet);
#endif //SNR_STEP
  // start reception
  //printf("|%lu| medium_cs calling interface_cs for node %d\n",get_time(), to_interface->object);
  interface_cs(to_interface, from_interface, packet);
  // end reception
  scheduler_add_rx_end(clock, to_interface, from_interface, packet);
}


/* ************************************************** */
/* ************************************************** */
void medium_rx(packet_t *packet, call_t *to_interface, call_t *from_interface) {
  node_t *node = get_node_by_id(to_interface->object);

  // check whether the node is able to receive and if the packet wasn't send by him
  if ((node->state != NODE_ACTIVE) && (packet->node == to_interface->object) ) {
    packet_dealloc(packet);
    return;
  }

  // noise & PER
#if (SNR_STEP != 0)
  packet->PER = 1;
  //printf("\n%s[MEDIUM] Node %d has a packet (packet_id = %d) to be treated %s\n",KYEL, to_interface->object, packet->id, KNRM);
  noise_packet_rx(to_interface, packet);
  packet->PER = 1 - packet->PER;
  //printf("\n%s[MEDIUM] Node %d has a packet (packet_id = %d) with PER = %f (rxdbm = %f)%s\n",KMAG,to_interface->object, packet->id, packet->PER, packet->rxdBm, KNRM);
#ifdef SNR_ERRORS
  modulation_errors(packet);
#endif //SNR_ERRORS
#else //SNR_STEP == 0
  packet->PER = 0;
#endif //SNR_STEP

  // receive
  //printf("|%lu| medium_rx calling interface_rx for node %d\n", get_time(), to_interface->object);
  interface_rx(to_interface, from_interface, packet);

}


/* ************************************************** */
/* ************************************************** */
void medium_compute_rxdBm(packet_t *packet, call_t *to_interface, call_t *from_interface)
{
  double      rxdBm  = packet->txdBm;
  position_t *pos_tx = get_node_position(packet->node);
  position_t *pos_rx = get_node_position(to_interface->object);

  // antenna tx white noise
  rxdBm += interface_get_loss(from_interface, to_interface);

  // antenna tx gain (TODO: angle at the transmission moment)
  rxdBm += interface_gain_tx(from_interface, to_interface, pos_rx);

  // propagation
  if (to_interface->object != from_interface->object) // if receiving node is different from sending node
  {

    //rxdbm = pathloss + shadowing + fading      (pathloss has the original rxdBm value embedded)
    rxdBm = medium_get_pathloss(to_interface, from_interface, packet, rxdBm) +
        medium_get_fading(to_interface, from_interface, packet, rxdBm) +
        medium_get_shadowing(to_interface, from_interface, packet, rxdBm);

  }

  // antenna rx gain
  rxdBm += interface_gain_rx(to_interface, from_interface, pos_tx);

  // antenna rx white noise
  rxdBm += interface_get_loss(to_interface, from_interface);

  // rx power
  packet->rxdBm = rxdBm;
  packet->rxmW = dBm2mW(rxdBm);
  packet->RSSI = rxdBm; // By default RSSI is equal to rxdBm (RSS) in dBm
}

void medium_rx_signal_begin(SpectrumModel *spectrum, std::shared_ptr<Signal> signal) {

  spectrum->SignalRxBegin(signal);

  // add rx end
}

void medium_rx_signal_end(SpectrumModel *spectrum, std::shared_ptr<Signal> signal) {

  spectrum->SignalRxEnd(signal);

  // add rx end
}


void medium_tx_signal_end(SpectrumModel *spectrum, std::shared_ptr<Signal> signal) {

  spectrum->SignalTxEnd(signal);
}
