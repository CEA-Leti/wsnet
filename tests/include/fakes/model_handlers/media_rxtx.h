/**
 *  \file   media_rxtx.h
 *  \brief  Fake functions and objects for RX and TX management declarations
 *  \author Luiz Henrique Suraty Filho
 *  \date   2018
 **/

#ifndef WSNET_TEST_FAKES_MODEL_HANDLERS_MEDIA_RXTX_H_
#define WSNET_TEST_FAKES_MODEL_HANDLERS_MEDIA_RXTX_H_

#include <kernel/include/model_handlers/media_rxtx.h>

/* ************************************************** */
/* ************************************************** */
#ifdef __cplusplus
extern "C"{
#endif

/**
 * \brief Return the transceiver medium noise on a given channel.
 * \param to should be {transceiver id, node id}.
 * \param from should be {antenna id, node id}.
 * \param channel the radio channel we are listening to.
 * \return the noise strength in dBm.
 **/
double __wrap_MEDIA_GET_NOISE(call_t *to, call_t *from, int channel){
  (void) to;
  (void) from;
  (void) channel;
  return 0.0;
}


/**
 * \brief Send a packet through the mediums to the reachable nodes.
 * \param to should be {-1, medium id}.
 * \param from should be {class id, node id}.
 * \param packet the transmitted packet.
 **/
void __wrap_MEDIA_TX(call_t *from_transceiver, call_t *from_interface, packet_t *packet){
  (void) packet;
  (void) from_transceiver;
  (void) from_interface;
  return;
}

void __wrap_medium_tx_end(packet_t *packet, call_t *from_transceiver, call_t *from_interface){
  (void) packet;
  (void) from_transceiver;
  (void) from_interface;
  return;
}
void __wrap_medium_cs(packet_t *packet, call_t *to_interface, call_t *from_interface){
  (void) packet;
  (void) to_interface;
  (void) from_interface;
  return;
}
void __wrap_medium_rx(packet_t *packet, call_t *to_interface, call_t *from_interface){
  (void) packet;
  (void) to_interface;
  (void) from_interface;
  return;
}
void __wrap_medium_compute_rxdBm(packet_t *packet, call_t *to_interface, call_t *from_interface){
  (void) packet;
  (void) to_interface;
  (void) from_interface;
  return;
}

#ifdef __cplusplus
void __wrap_medium_rx_signal_begin(SpectrumModel *spectrum, std::shared_ptr<Signal> signal){
  (void) spectrum;
  (void) signal;
  return;
}
#else
void __wrap_medium_rx_signal_begin(void *spectrum, void* signal){
  (void) spectrum;
  (void) signal;
  return;
}
#endif

#ifdef __cplusplus
void __wrap_medium_rx_signal_end(SpectrumModel *spectrum, std::shared_ptr<Signal> signal){
  (void) spectrum;
  (void) signal;
  return;
}
#else
void __wrap_medium_rx_signal_end(void *spectrum, void* signal){
  (void) spectrum;
  (void) signal;
  return;
}
#endif


#ifdef __cplusplus
void __wrap_medium_tx_signal_end(SpectrumModel *spectrum, std::shared_ptr<Signal> signal){
  (void) spectrum;
  (void) signal;
  return;
}

#else
void __wrap_medium_tx_signal_end(void *spectrum, void* signal){
  (void) spectrum;
  (void) signal;
  return;
}
#endif

#ifdef __cplusplus
}
#endif

#endif //WSNET_TEST_FAKES_MODEL_HANDLERS_MEDIA_RXTX_H_
