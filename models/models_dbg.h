/**
 *  \file   models_dbg.h
 *  \brief  Models debug functions
 *  \author Loic Lemaitre
 *  \date   2010
 **/

#ifndef __models_dbg__
#define __models_dbg__

#include <stdio.h>

/* ************************************************** */
/* ************************************************** */
#ifndef DBG_PRINT
#define DBG_PRINT(x...) fprintf(stderr,x)
#endif

#define _DBG_MSG_APPLICATION    /* application model debug messages   */
#define _DBG_MSG_ENERGY         /* energy model debug messages        */
#define _DBG_MSG_FADING         /* fading model debug messages        */
#define _DBG_MSG_GLOBAL_MAP     /* global_map model debug messages    */
#define _DBG_MSG_INTERFACE      /* interface model debug messages     */
#define _DBG_MSG_INTERFERENCES  /* interferences model debug messages */
#define _DBG_MSG_MAC            /* mac model debug messages           */
#define _DBG_MSG_MAP            /* map model debug messages           */
#define _DBG_MSG_MOBILITY       /* mobility model debug messages      */
#define _DBG_MSG_MODULATIONS    /* modulations model debug messages   */
#define _DBG_MSG_MONITOR        /* monitor model debug messages       */
#define _DBG_MSG_PATHLOSS       /* pathloss model debug messages      */
#define _DBG_MSG_PHYSICAL       /* physical model debug messages      */
#define _DBG_MSG_ROUTING        /* routing model debug messages       */
#define _DBG_MSG_SENSOR         /* sensor model debug messages        */
#define _DBG_MSG_SHADOWING      /* shadowing model debug messages     */


#ifdef DBG_MSG_APPLICATION
#define DBG_APPLICATION(x...) DBG_PRINT(x)
#else
#define DBG_APPLICATION(x...) do { } while (0)
#endif

#ifdef DBG_MSG_ENERGY
#define DBG_ENERGY(x...) DBG_PRINT(x)
#else
#define DBG_ENERGY(x...) do { } while (0)
#endif

#ifdef DBG_MSG_FADING
#define DBG_FADING(x...) DBG_PRINT(x)
#else
#define DBG_FADING(x...) do { } while (0)
#endif

#ifdef DBG_MSG_GLOBAL_MAP
#define DBG_GLOBAL_MAP(x...) DBG_PRINT(x)
#else
#define DBG_GLOBAL_MAP(x...) do { } while (0)
#endif

#ifdef DBG_MSG_INTERFACE
#define DBG_INTERFACE(x...) DBG_PRINT(x)
#else
#define DBG_INTERFACE(x...) do { } while (0)
#endif

#ifdef DBG_MSG_INTERFERENCES
#define DBG_INTERFERENCES(x...) DBG_PRINT(x)
#else
#define DBG_INTERFERENCES(x...) do { } while (0)
#endif

#ifdef DBG_MSG_MAC
#define DBG_MAC(x...) DBG_PRINT(x)
#else
#define DBG_MAC(x...) do { } while (0)
#endif

#ifdef DBG_MSG_MAP
#define DBG_MAP(x...) DBG_PRINT(x)
#else
#define DBG_MAP(x...) do { } while (0)
#endif

#ifdef DBG_MSG_MOBILITY
#define DBG_MOBILITY(x...) DBG_PRINT(x)
#else
#define DBG_MOBILITY(x...) do { } while (0)
#endif

#ifdef DBG_MSG_MODULATIONS
#define DBG_MODULATIONS(x...) DBG_PRINT(x)
#else
#define DBG_MODULATIONS(x...) do { } while (0)
#endif

#ifdef DBG_MSG_MONITOR
#define DBG_MONITOR(x...) DBG_PRINT(x)
#else
#define DBG_MONITOR(x...) do { } while (0)
#endif

#ifdef DBG_MSG_PATHLOSS
#define DBG_PATHLOSS(x...) DBG_PRINT(x)
#else
#define DBG_PATHLOSS(x...) do { } while (0)
#endif

#ifdef DBG_MSG_PHYSICAL
#define DBG_PHYSICAL(x...) DBG_PRINT(x)
#else
#define DBG_PHYSICAL(x...) do { } while (0)
#endif

#ifdef DBG_MSG_ROUTING
#define DBG_ROUTING(x...) DBG_PRINT(x)
#else
#define DBG_ROUTING(x...) do { } while (0)
#endif

#ifdef DBG_MSG_SENSOR
#define DBG_SENSOR(x...) DBG_PRINT(x)
#else
#define DBG_SENSOR(x...) do { } while (0)
#endif

#ifdef DBG_MSG_SHADOWING
#define DBG_SHADOWING(x...) DBG_PRINT(x)
#else
#define DBG_SHADOWING(x...) do { } while (0)
#endif


#endif // __models_dbg__
