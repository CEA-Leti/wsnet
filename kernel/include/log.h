/**
 *  \file   log.h
 *  \brief  log declarations
 *  \author Guillaume Chelius & Elyes Ben Hamida
 *  \date   2007
 **/
#ifndef __log__
#define __log__

#include <kernel/include/options.h>


#ifdef LOG_REPLAY
#define PRINT_REPLAY(x...) printf(x)
#else //LOG_REPLAY
#define PRINT_REPLAY(x...) do { } while (0)
#endif //LOG_REPLAY

#ifdef LOG_APPLICATION
#define PRINT_APPLICATION(x...) printf(x)
#else //LOG_APPLICATION
#define PRINT_APPLICATION(x...) do { } while (0)
#endif //LOG_APPLICATION

#ifdef LOG_ROUTING
#define PRINT_ROUTING(x...) printf(x)
#else //LOG_ROUTING
#define PRINT_ROUTING(x...) do { } while (0)
#endif //LOG_ROUTING

#ifdef LOG_MAC
#define PRINT_MAC(x...) printf(x)
#else //LOG_MAC
#define PRINT_MAC(x...) do { } while (0)
#endif //LOG_MAC

#ifdef LOG_TRANSCEIVER
#define PRINT_TRANSCEIVER(x...) printf(x)
#else //LOG_TRANSCEIVER
#define PRINT_TRANSCEIVER(x...) do { } while (0)
#endif //LOG_TRANSCEIVER

#ifdef LOG_INTERFACE
#define PRINT_INTERFACE(x...) printf(x)
#else //LOG_INTERFACE
#define PRINT_INTERFACE(x...) do { } while (0)
#endif //LOG_INTERFACE

#ifdef LOG_MOBILITY
#define PRINT_MOBILITY(x...) printf(x)
#else //LOG_MOBILITY
#define PRINT_MOBILITY(x...) do { } while (0)
#endif //LOG_MOBILITY

#ifdef LOG_ENERGY
#define PRINT_ENERGY(x...) printf(x)
#else //LOG_ENERGY
#define PRINT_ENERGY(x...) do { } while (0)
#endif //LOG_ENERGY

#ifdef LOG_GLOBAL_MAP
#define PRINT_GLOBAL_MAP(x...) printf(x)
#else //LOG_GLOBAL_MAP
#define PRINT_GLOBAL_MAP(x...) do { } while (0)
#endif //LOG_GLOBAL_MAP

#ifdef LOG_MAP
#define PRINT_MAP(x...) printf(x)
#else //LOG_MAP
#define PRINT_MAP(x...) do { } while (0)
#endif //LOG_MAP

#ifdef LOG_MONITOR
#define PRINT_MONITOR(x...) printf(x)
#else //LOG_MONITOR
#define PRINT_MONITOR(x...) do { } while (0)
#endif //LOG_MONITOR

#ifdef LOG_MODULATIONS
#define PRINT_MODULATIONS(x...) printf(x)
#else //LOG_MODULATIONS
#define PRINT_MODULATIONS(x...) do { } while (0)
#endif //LOG_MODULATIONS

#ifdef LOG_INTERFERENCES
#define PRINT_INTERFERENCES(x...) printf(x)
#else //LOG_INTERFERENCES
#define PRINT_INTERFERENCES(x...) do { } while (0)
#endif //LOG_INTERFERENCES

#ifdef LOG_PATHLOSS
#define PRINT_PATHLOSS(x...) printf(x)
#else //LOG_PATHLOSS
#define PRINT_PATHLOSS(x...) do { } while (0)
#endif //LOG_PATHLOSS

#ifdef LOG_FADING
#define PRINT_FADING(x...) printf(x)
#else //LOG_FADING
#define PRINT_FADING(x...) do { } while (0)
#endif //LOG_FADING

#ifdef LOG_SHADOWING
#define PRINT_SHADOWING(x...) printf(x)
#else //LOG_SHADOWING
#define PRINT_SHADOWING(x...) do { } while (0)
#endif //LOG_SHADOWING

#ifdef LOG_PHYSICAL
#define PRINT_PHYSICAL(x...) printf(x)
#else //LOG_PHYSICAL
#define PRINT_PHYSICAL(x...) do { } while (0)
#endif //LOG_PHYSICAL

#ifdef LOG_SENSOR
#define PRINT_SENSOR(x...) printf(x)
#else //LOG_SENSOR
#define PRINT_SENSOR(x...) do { } while (0)
#endif //LOG_SENSOR

#endif //__log__
