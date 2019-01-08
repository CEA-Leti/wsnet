/**
 *  \file   bmac.h
 *  \brief  A set of macros to update BMAC/XMAC parameters
 *  \author Romain KUNTZ 
 *  \date   01/2009
 **/

/* Macros to update the B-MAC parameters from 
 * other layers using the IOCTL function. 
 * Example to enable the LL acknowledgments:
 *
 *    call_t c0;
 *    uint64_t in = MAC_ENABLED;
 *    c0.node = c->node;
 *    array_t *mac_layer = get_mac_entities(&c0);
 *    c0.entity = mac_layer->elts[0];
 *    IOCTL(&c0, MAC_UPDATE_LLACK, &in, 0);
 */

// Possible parameters (2nd argument of IOCTL())
#define MAC_UPDATE_CCA             0   // Clear Channel Assessment
#define MAC_UPDATE_LLACK           1   // LL ACK
#define MAC_UPDATE_INIT_BACKOFF    2   // Initial backoff value
#define MAC_UPDATE_CONG_BACKOFF    3   // Congestion backoff value
#define MAC_UPDATE_LPL_CHECKINT    4   // Low Power Listening period

// Possible values for the above parameters (3rd argument of IOCTL())
#define MAC_DISABLED   0  // for MAC_UPDATE_CCA or MAC_UPDATE_LLACK
#define MAC_ENABLED    1  // for MAC_UPDATE_CCA or MAC_UPDATE_LLACK

// LPL pre-defined values for the MAC_UPDATE_LPL_CHECKINT parameter
#define MAC_LPL_MODE_0 0            // Radio always on
#define MAC_LPL_MODE_1 10000000     // 10ms
#define MAC_LPL_MODE_2 25000000     // 25ms
#define MAC_LPL_MODE_3 50000000     // 50ms 
#define MAC_LPL_MODE_4 100000000    // 100ms (default one)
#define MAC_LPL_MODE_5 200000000    // 200ms 
#define MAC_LPL_MODE_6 400000000    // 400ms
#define MAC_LPL_MODE_7 800000000    // 800ms
#define MAC_LPL_MODE_8 1600000000   // 1600ms

// MAC_UPDATE_INIT_BACKOFF and MAC_UPDATE_CONG_BACKOFF 
// take values in nanosecond
