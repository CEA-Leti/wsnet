/**
 *  \file   transceiver.h
 *  \brief  Hardware transceiver handler declarations.
 *  \author Guillaume Chelius & Elyes Ben Hamida & Loic Lemaitre & Mickael Maman
 *  \date   2007
 **/

#ifndef __transceiver__
#define __transceiver__

#include <kernel/include/definitions/types.h>


/* ************************************************** */
/* ************************************************** */

#ifdef __cplusplus
extern "C"{
#endif

/**
 * \brief Carrier Sense mechanism.
 * \param to should be {transceiver id, node id}.
 * \param from is {calling class id, node id} if needed.
 * \return The signal strength of the currently received signal. MIN_DBM if no current signal.
 **/
double transceiver_get_cs(call_t *to, call_t *from);


/**
 * \brief Clear Channel Assesment mechanism.
 * \param to should be {transceiver id, node id}.
 * \param from is {calling class id, node id} if needed.
 * \return The noise strength on the transceiver interface. MIN_DBM if no noise.
 **/
double transceiver_get_noise(call_t *to, call_t *from);


/**
 * \brief Get the transceiver tx power.
 * \param to should be {transceiver id, node id}.
 * \param from is {calling class id, node id} if needed.
 * \return The current transmission power in dBm.
 **/
double transceiver_get_power(call_t *to, call_t *from);


/**
 * \brief Set the transceiver tx power.
 * \param to should be {transceiver id, node id}.
 * \param from is {calling class id, node id} if needed.
 * \param power the new transmission power in dBm.
 **/
void transceiver_set_power(call_t *to, call_t *from, double power);


/**
 * \brief Get the transceiver channel.
 * \param to should be {transceiver id, node id}.
 * \param from is {calling class id, node id} if needed.
 * \return The current transceiver channel.
 **/
int transceiver_get_channel(call_t *to, call_t *from);


/**
 * \brief Set the transceiver channel.
 * \param to should be {transceiver id, node id}.
 * \param from is {calling class id, node id} if needed.
 * \param channel the new transceiver channel.
 **/
void transceiver_set_channel(call_t *to, call_t *from, int channel);


/**
 * \brief Get the transceiver modulation.
 * \param to should be {transceiver id, node id}.
 * \param from is {calling class id, node id} if needed.
 * \return The current modulation entity id.
 **/
classid_t transceiver_get_modulation(call_t *to, call_t *from);


/**
 * \brief Set the transceiver modulation.
 * \param to should be {transceiver id, node id}.
 * \param from is {calling class id, node id} if needed.
 * \param modulation the new modulation entity id.
 **/
void transceiver_set_modulation(call_t *to, call_t *from, classid_t modulation);


/**
 * \brief Get the transceiver bandwidth.
 * \param to should be {transceiver id, node id}.
 * \param from is {calling class id, node id} if needed.
 * \return The current bandwidth, i.e. the time to transmit a bit in ns.
 **/
uint64_t transceiver_get_Tb(call_t *to, call_t *from);


/**
 * \brief Notifies the transceiver with a new signal.
 * \param to should be {transceiver id, node id}.
 * \param from is {interface id, node id}.
 * \param packet the new signal.
 **/
void transceiver_cs(call_t *to, call_t *from, packet_t *packet);


/**
 * \brief Set the transceiver sensibility.
 * \param to should be {transceiver id, node id}.
 * \param from is {calling class id, node id} if needed.
 * \param sensibility the new transceiver sensibility in dBm.
 **/
void transceiver_set_sensibility(call_t *to, call_t *from, double sensibility);


/**
 * \brief Get the transceiver sensibility.
 * \param to should be {transceiver id, node id}.
 * \param from is {calling class id, node id} if needed.
 * \return The current transceiver sensibility in dBm.
 **/
double transceiver_get_sensibility(call_t *to, call_t *from);


/**
 * \brief Set the transceiver in sleep mode.
 * \param to should be {transceiver id, node id}.
 * \param from is {calling class id, node id} if needed.
 **/
void transceiver_sleep(call_t *to, call_t *from);


/**
 * \brief Set the transceiver in active mode.
 * \param to should be {transceiver id, node id}.
 * \param from is {calling class id, node id} if needed.
 **/
void transceiver_wakeup(call_t *to, call_t *from);


/**
 * \brief Set the transceiver in rx mode.
 * \param to should be {transceiver id, node id}.
 * \param from is {calling class id, node id} if needed.
 **/
void transceiver_switch_rx(call_t *to, call_t *from);


/**
 * \brief Set the transceiver in idle mode.
 * \param to should be {transceiver id, node id}.
 * \param from is {calling class id, node id} if needed.
 **/
void transceiver_switch_idle(call_t *to, call_t *from);




/* edit by Christophe Savigny <christophe.savigny@insa-lyon.fr> */
/**
 * \brief get the number of bit per symbol for modulation associated
 * \param to should be {transceiver id, node id}.
 * \param from is {calling class id, node id} if needed.
 * \return the current number of bit per symbol
 **/
int transceiver_get_modulation_bit_per_symbol(call_t *to, call_t *from);


int transceiver_get_modulation_type(call_t *to, call_t *from);

void transceiver_set_modulation_type(call_t *to, call_t *from, int modulation_type);

/**
 * \brief Get the transceiver bandwidth.
 * \param to should be {transceiver id, node id}.
 * \param from is {calling class id, node id} if needed.
 * \return The current bandwidth, i.e. the time to transmit a symbol in ns.
**/
uint64_t transceiver_get_Ts(call_t *to, call_t *from);

/**
 * \brief Set the transceiver bandwidth.
 * Ts must be a bit_per_symbol's multiple
 * \param to should be {transceiver id, node id}.
 * \param from is {calling class id, node id} if needed.
 * \param Ts the new bandwidth, i.e. the time to transmit a symbol in ns.
**/
void transceiver_set_Ts(call_t *to, call_t *from, uint64_t Ts);


/* end of edition */



/*edit by Rida El Chall*/
/**
 * \brief functions to get and set the transceiver frequency bands begin (f_begin) and frequency bands end (f_end)
 **/
uint64_t transceiver_get_freq_low(call_t *to, call_t *from);

uint64_t transceiver_get_freq_high(call_t *to, call_t *from);

void transceiver_set_freq_low(call_t *to, call_t *from, uint64_t freq_low);

void transceiver_set_freq_high(call_t *to, call_t *from, uint64_t freq_high);
/* end of edition */


#ifdef __cplusplus
}
#endif

#endif // __transceiver__
