/**
 *  \file   media_rxtx.h
 *  \brief  RX and TX management declarations
 *  \author Luiz Henrique Suraty Filho & Loic Lemaitre
 *  \date   2018
 **/

#ifndef WSNET_CORE_MODEL_HANDLERS_MEDIA_RXTX_H_
#define WSNET_CORE_MODEL_HANDLERS_MEDIA_RXTX_H_

#include <float.h>
#include <kernel/include/definitions/types.h>
#ifdef __cplusplus
#include <memory>
#include <kernel/include/definitions/types/signal/signal.h>
#endif

/* ************************************************** */
/* ************************************************** */
/**
 * \def MIN_DBM 
 * \brief Signal strength of a null signal in dBm.
 **/
#define MIN_DBM -DBL_MAX


/**
 * \def MAX_SNR 
 * \brief Maximum SNR value, when there is no interference nor noise.
 **/
#define MAX_SNR DBL_MAX


/* ************************************************** */
/* ************************************************** */
#ifdef __cplusplus
extern "C"{
#endif
/**
 * \brief Convert a dBm value into a mW value.
 * \param dBm the dBm value to convert.
 * \return The converted mW value.
 **/
static inline double dBm2mW(double dBm) {
    return (dBm == MIN_DBM) ? 0 : pow(10, dBm / 10);
}

/**
 * \brief Convert a mW value into a dbW value.
 * \param dBm the dBm value to convert.
 * \return The converted mW value.
 **/
static inline double mW2dBW(double mW) {
    return (mW == MIN_DBM) ? 0 : (log10(mW*0.001) * 10);
}



/**
 * \brief Convert a mW value into a dBm value.
 * \param mW the mW value to convert.
 * \return The converted dBm value.
 **/
static inline double mW2dBm(double mW) {
    return (mW == 0) ? MIN_DBM : (log10(mW) * 10);
}


/**
 * \brief Return the transceiver medium noise on a given channel.
 * \param to should be {transceiver id, node id}.
 * \param from should be {antenna id, node id}.
 * \param channel the radio channel we are listening to.
 * \return the noise strength in dBm.
 **/
double MEDIA_GET_NOISE(call_t *to, call_t *from, int channel);


/**
 * \brief Send a packet through the mediums to the reachable nodes.
 * \param to should be {-1, medium id}.
 * \param from should be {class id, node id}.
 * \param packet the transmitted packet.
 **/
void MEDIA_TX(call_t *from_transceiver, call_t *from_interface, packet_t *packet);

void medium_tx_end(packet_t *packet, call_t *from_transceiver, call_t *from_interface);
void medium_cs(packet_t *packet, call_t *to_interface, call_t *from_interface);
void medium_rx(packet_t *packet, call_t *to_interface, call_t *from_interface);
void medium_compute_rxdBm(packet_t *packet, call_t *to_interface, call_t *from_interface);

#ifdef __cplusplus
void medium_rx_signal_begin(SpectrumModel *spectrum, std::shared_ptr<Signal> signal);
#else
void medium_rx_signal_begin(void *spectrum, void* signal);
#endif

#ifdef __cplusplus
void medium_rx_signal_end(SpectrumModel *spectrum, std::shared_ptr<Signal> signal);
#else
void medium_rx_signal_end(void *spectrum, void* signal);
#endif


#ifdef __cplusplus
void medium_tx_signal_end(SpectrumModel *spectrum, std::shared_ptr<Signal> signal);
#else
void medium_tx_signal_end(void *spectrum, void* signal);
#endif

#ifdef __cplusplus
}
#endif

#endif //WSNET_CORE_INCLUDE_MODEL_HANDLERS_MEDIA_RXTX_H_
