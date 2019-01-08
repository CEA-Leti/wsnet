/**
 *  \file   options.h
 *  \brief  User options declarations
 *  \author Guillaume Chelius & Elyes Ben Hamida
 *  \date   2007
 **/
#ifndef __options__
#define __options__


//#define LOG_REPLAY
#define LOG_APPLICATION
//#define LOG_ROUTING
//#define LOG_MAC
//#define LOG_TRANSCEIVER
//#define LOG_INTERFACE
//#define LOG_ENERGY
//#define LOG_MOBILITY
//#define LOG_MONITOR
//#define LOG_INTERFERENCES
//#define LOG_MODULATIONS
//#define LOG_ENVIRONMENT
//#define LOG_GLOBAL_MAP
//#define LOG_MAP
//#define LOG_PATHLOSS
//#define LOG_PHYSICAL
//#define LOG_SENSOR
//#define LOG_SHADOWING


/** \def MAX_MEDIUMS 
 * \brief Define the maximum number of mediums
 **/
#define MAX_MEDIUMS 2


/** \def CHANNELS_NUMBER 
 * \brief Define the number of simulated radio channels
 **/
#define CHANNELS_NUMBER 6



/** \def SNR_STEP
 * \brief Define the interference support policy. 0 = no interference, -1 = SINR computation for each packet byte, x = packet divided in x slices for SINR computation.
 **/
#define SNR_STEP        1

/** \def SNR_ERRORS
 * \brief Define the packet error policy. 0 = no error introduction in packet data, 1 = error introduction in the packet data according to the computed BER.
 **/
#define SNR_ERRORS      0


#endif // __options__
