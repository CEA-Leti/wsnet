/**
 *  \file   oqpsk.c
 *  \brief  IEEE802.15.4 OQPSK
 *  \author Mickael Maman Luiz Henrique Suraty Filho
 *  \date   2016
 **/
#include <math.h>

#include <kernel/include/modelutils.h>


/* ************************************************** */
/* ************************************************** */
model_t model =  {
  "IEEE802.15.4 oqpsk",
  "Mickael Maman & Luiz Henrique Suraty Filho",
  "0.2",
  MODELTYPE_MODULATIONS
};


/* ************************************************** */
/* ************************************************** */
int init(call_t *c, void *params) {
  return 0;
}

int destroy(call_t *c) {
  return 0;
}


/* ************************************************** */
/* ************************************************** */
int bind(call_t *to, void *params) {
  return 0;
}

int unbind(call_t *to) {
  return 0;
}


/*Modified by Luiz Henrique Suraty Filho*/
/* ************************************************** */
/* ************************************************** */
double modulate(call_t *to, call_t *from, double snr) {
	double ber154 = 0.0;

	/*Calculation is based on the calculations provided in P802.15.4REVb/D5, April, 2006 (Revision of IEEE Std 802.15.4-2003) page 266*/
	/* We don't use a binomial coeficient function, once it is faster and costs less to pre-calculate all using the preprocessor or calculating it before-hand, as we do*/
	if (snr != MAX_SNR) {
		ber154+=(120.0* exp(20.0 * snr * ((double) 1/2 - 1)) -560.0 * exp(20.0 * snr * ((double) 1/3 - 1)) + 1820.0 * exp(20.0 * snr * ((double) 1/4 - 1)) - 4368.0 * exp(20.0 * snr * ((double) 1/5 - 1)));
		ber154+=(8008.0 * exp(20.0 * snr * ((double) 1/6 - 1)) -11440.0 * exp(20.0 * snr * ((double) 1/7 - 1)) + 12870.0 * exp(20.0 * snr * ((double) 1/8 - 1)) - 11440.0 * exp(20.0 * snr * ((double) 1/9 - 1)));
		ber154+=(8008.0 * exp(20.0 * snr * ((double) 1/10 - 1)) -4368.0 * exp(20.0 * snr * ((double) 1/11 - 1)) + 1820.0 * exp(20.0 * snr * ((double) 1/12 - 1))- 560.0 * exp(20.0 * snr * ((double) 1/13 - 1)));
		ber154+=(120.0 * exp(20.0 * snr * ((double) 1/14 - 1)) -16.0 * exp(20.0 * snr * ((double) 1/15 - 1)) + 1 * exp(20.0 * snr * ((double) 1/16 - 1)));
		ber154 = (double) 8/15 * (double) 1/16 * ber154;
	}

	return ber154;
}
/*End of modification by Luiz Henrique Suraty Filho*/

int bit_per_symbol(call_t *to, call_t *from){
  return 4;
}
int  get_modulation_type(call_t *to, call_t *from)
{
    return 1;
}

void set_modulation_type(call_t *to, call_t *from, int modulation_type)
{
	return;
}

/* ************************************************** */
/* ************************************************** */
modulation_methods_t methods = {modulate, bit_per_symbol,get_modulation_type,set_modulation_type};
