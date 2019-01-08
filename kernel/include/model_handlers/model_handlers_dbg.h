/**
 *  \file   model_handlers_dbg.h
 *  \brief  Model handlers debug functions
 *  \author Loic Lemaitre
 *  \date   2010
 **/

#ifndef __model_handlers_dbg__
#define __model_handlers_dbg__

#include <stdio.h>

/* ************************************************** */
/* ************************************************** */
#ifndef DBG_PRINT
#define DBG_PRINT(x...) fprintf(stderr,x)
#endif

//#define DBG_MSG_MONITOR         /* monitor debug messages   */

#ifdef DBG_MSG_MONITOR
#define DBG_MONITOR(x...) DBG_PRINT(x)
#else
#define DBG_MONITOR(x...) do { } while (0)
#endif


#endif  //__model_handlers_dbg__
