/**
 *  \file   modulation.h
 *  \brief  Modulation declarations
 *  \author Guillaume Chelius & Elyes Ben Hamida & Christophe Savigny
 *  \date   2009
 **/
#ifndef __modulation_public__
#define __modulation_public__

#include <kernel/include/definitions/types.h>


#ifdef __cplusplus
extern "C"{
#endif
/* edit by Christophe Savigny <christophe.savigny@insa-lyon.fr> */

/** 
 * \brief Returns the bit per sumbol rate for a given modulation scheme.
 * \param modulation : modulation id.
 * \return The bit per symbol.
 **/
int modulation_bit_per_symbol(classid_t modulation);

void modulation_set_modulation_type(classid_t modulation, int modulation_type);
int modulation_get_modulation_type(classid_t modulation);

/* end of edition */
#ifdef __cplusplus
}
#endif

#endif //__modulation_public__


#ifndef __modulation__
#define __modulation__

#include <kernel/include/data_structures/list/list.h>


/* ************************************************** */
/* ************************************************** */
extern list_t *modulation_entities;

#ifdef __cplusplus
extern "C"{
#endif
/* ************************************************** */
/* ************************************************** */
int modulation_init(void);
int modulation_bootstrap(void);
void modulation_clean(void);


/* ************************************************** */
/* ************************************************** */
//double do_modulate(classid_t modulation, double rxmW, double noise);
double do_modulate(call_t *from, classid_t modulation, double rxmW, double noise);
double do_modulate_snr(call_t *from, classid_t modulation, double snr);
void modulation_errors(packet_t *packet);

#ifdef __cplusplus
}
#endif
#endif //__modulation__

