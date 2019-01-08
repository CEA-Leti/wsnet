/**
 *  \file   link.h
 *  \brief  Link declarations
 *  \author Luiz Henrique Suraty Filho
 *  \date   2016
 **/

#ifndef WSNET_KERNEL_MODEL_HANDLERS_LINK_H_
#define WSNET_KERNEL_MODEL_HANDLERS_LINK_H_

#include <kernel/include/definitions/types.h>

/* Enums definitions for the BAN link type.
 * Once we consider symetric links A_TO_B covers also the case B_TO_A */
enum{
	LINK_TYPE_BAN_BELT_TO_HAND,
	LINK_TYPE_BAN_BELT_TO_EAR,
	LINK_TYPE_BAN_HAND_TO_EAR,
	LINK_TYPE_BAN_BELT_TO_BELT,
	LINK_TYPE_BAN_HAND_TO_HAND,
	LINK_TYPE_BAN_EAR_TO_EAR,
	LINK_TYPE_BAN_BELT_TO_OFF,
	LINK_TYPE_BAN_HAND_TO_OFF,
	LINK_TYPE_BAN_EAR_TO_OFF,
	LINK_TYPE_BAN_OFF_TO_OFF
};

/* Enums for the maximum number of link types for each communication type.
 * (Note that once we consider symetric links, we avoid duplications here, ex. BELT_TO_HAND and HAND_TO_BELT are counted as 1)*/
enum{
	LINK_MAX_NUMBER_LINK_TYPE_15_4	= 1,	// Max defined number of different link types for IEEE 802.15.4 communication type
	LINK_MAX_NUMBER_LINK_TYPE_WIFI 	= 1,	// Max defined number of different link types for IEEE 802.11 communication type
	LINK_MAX_NUMBER_LINK_TYPE_BAN 	= 3,	// Max defined number of different link types for Body Area Network (IEEE 802.15.4 or IEEE 802.15.6) communication type
	LINK_MAX_NUMBER_LINK_TYPE_B2B	= 6		// Max defined number of different link types for Body to Body Network (IEEE 802.15.4 or IEEE 802.15.6) communication type
};
#ifdef __cplusplus
extern "C"{
#endif

link_condition_t link_get_link_condition(call_t *to_link, call_t *to_interface, call_t *from_interface);

link_communication_type_t link_get_communication_type(call_t *to_link, call_t *to_interface, call_t *from_interface);

int link_get_complementary_link_condition(call_t *to_link, call_t *to_interface, call_t *from_interface);

link_type_t link_get_link_type(call_t *to_link, call_t *to_interface, call_t *from_interface);

double link_get_mutual_orientation(call_t *to_link, call_t *to_interface, call_t *from_interface);

#ifdef __cplusplus
}
#endif

#endif // WSNET_KERNEL_MODEL_HANDLERS_LINK_H_
