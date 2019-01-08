/**
 *  \file   monitor.h
 *  \brief  Monitor module header
 *  \author Loic Lemaitre
 *  \date   2010
 */

#ifndef __monitor__
#define __monitor__

#include <stdint.h>
#include <kernel/include/definitions/types.h>
#include <kernel/include/definitions/class.h>


/* ************************************************** */
/* ************************************************** */
/* Signals masks:

   .... .... | .... .... | .... .... | .... ...X  : SIG_NODE_BIRTH
   .... .... | .... .... | .... .... | .... ..X.  : FREE
   .... .... | .... .... | .... .... | .... .X..  : FREE
   .... .... | .... .... | .... .... | .... X...  : FREE
   .... .... | .... .... | .... .... | ...X ....  : FREE
   .... .... | .... .... | .... .... | ..X. ....  : FREE
   .... .... | .... .... | .... .... | .X.. ....  : FREE
   .... .... | .... .... | .... .... | X... ....  : FREE

   .... .... | .... .... | .... ...X | .... ....  : FREE
   .... .... | .... .... | .... ..X. | .... ....  : FREE
   .... .... | .... .... | .... .X.. | .... ....  : FREE
   .... .... | .... .... | .... X... | .... ....  : FREE
   .... .... | .... .... | ...X .... | .... ....  : FREE
   .... .... | .... .... | ..X. .... | .... ....  : FREE
   .... .... | .... .... | .X.. .... | .... ....  : FREE
   .... .... | .... .... | X... .... | .... ....  : FREE

   .... .... | .... ...X | .... .... | .... ....  : FREE
   .... .... | .... ..X. | .... .... | .... ....  : FREE
   .... .... | .... .X.. | .... .... | .... ....  : FREE
   .... .... | .... X... | .... .... | .... ....  : FREE
   .... .... | ...X .... | .... .... | .... ....  : FREE
   .... .... | ..X. .... | .... .... | .... ....  : FREE
   .... .... | .X.. .... | .... .... | .... ....  : FREE
   .... .... | X... .... | .... .... | .... ....  : FREE

   .... ...X | .... .... | .... .... | .... ....  : FREE
   .... ..X. | .... .... | .... .... | .... ....  : FREE
   .... .X.. | .... .... | .... .... | .... ....  : FREE
   .... X... | .... .... | .... .... | .... ....  : FREE
   ...X .... | .... .... | .... .... | .... ....  : FREE
   ..X. .... | .... .... | .... .... | .... ....  : FREE
   .X.. .... | .... .... | .... .... | .... ....  : FREE
   X... .... | .... .... | .... .... | .... ....  : FREE
*/

#define SIG_NODE_BIRTH        1
//#define SIG_XXXX              2
//#define SIG_XXXX              3
// ...


/* ************************************************** */
/* ************************************************** */
typedef uint8_t  sigid_t;
typedef uint32_t sigmask_t;


/* ************************************************** */
/* ************************************************** */

#ifdef __cplusplus
extern "C"{
#endif

/**
 * \brief Init monitors module
 * \return 0 in case of success, -1 else
 **/
int       monitors_init      (void);

/**
 * \brief Clean monitors module
 **/
void      monitors_clean     (void);

/**
 * \brief Register a monitor
 * \param to a call_t pointer to bind the class with the signal
 * \param signal a sigid_t variable
 * \return 0 in case of success, -1 else
 **/
int       monitor_register   (call_t *to, sigid_t signal);

/**
 * \brief Unregister a monitor
 * \param to a call_t pointer to retrieve the signal to unregister
 * \return 0 in case of success, -1 else
 **/
int       monitor_unregister (call_t *to);

/**
 * \brief Fire a signal to the monitor
 * \param signal, the signal being fired
 * \param data, a void pointer on data associated to this signal
 **/
void      monitor_fire_signal(sigid_t signal, void *data);

#ifdef __cplusplus
}
#endif

#endif //__monitor__
