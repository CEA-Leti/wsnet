/**
 *  \file   models.c
 *  \brief  Models definitions
 *  \author Loic Lemaitre
 *  \date   2010
 **/

#include "models.h"

char* model_type_to_str(int type)
{
  switch (type)
    {
    case MODELTYPE_SHADOWING     : return "MODELTYPE_SHADOWING";
    case MODELTYPE_SPECTRUM      : return "MODELTYPE_SPECTRUM";
    case MODELTYPE_PATHLOSS      : return "MODELTYPE_PATHLOSS";
    case MODELTYPE_FADING        : return "MODELTYPE_FADING";
    case MODELTYPE_INTERFERENCES : return "MODELTYPE_INTERFERENCES";
    case MODELTYPE_INTERMODULATION : return "MODELTYPE_INTERMODULATION";
    case MODELTYPE_NOISE         : return "MODELTYPE_NOISE";
    case MODELTYPE_MODULATIONS   : return "MODELTYPE_MODULATIONS";
    case MODELTYPE_PHYSICAL      : return "MODELTYPE_PHYSICAL";
    case MODELTYPE_MOBILITY      : return "MODELTYPE_MOBILITY";
    case MODELTYPE_INTERFACE     : return "MODELTYPE_INTERFACE";
    case MODELTYPE_TRANSCEIVER   : return "MODELTYPE_TRANSCEIVER";
    case MODELTYPE_MAC           : return "MODELTYPE_MAC";
    case MODELTYPE_ROUTING       : return "MODELTYPE_ROUTING";
    case MODELTYPE_APPLICATION   : return "MODELTYPE_APPLICATION";
    case MODELTYPE_ENERGY        : return "MODELTYPE_ENERGY";
    case MODELTYPE_MONITOR       : return "MODELTYPE_MONITOR";
    case MODELTYPE_MAP           : return "MODELTYPE_MAP";
    case MODELTYPE_GLOBAL_MAP    : return "MODELTYPE_GLOBAL_MAP";
    case MODELTYPE_SENSOR        : return "MODELTYPE_SENSOR";
    case MODELTYPE_LINK          : return "MODELTYPE_LINK";
    case MODELTYPE_PHY           : return "MODELTYPE_PHY";
    case MODELTYPE_CODING        : return "MODELTYPE_CODING";
    case MODELTYPE_INTERFERENCE : return "MODELTYPE_INTERFERENCE";
    case MODELTYPE_MODULATOR 	: return "MODELTYPE_MODULATOR";
    case MODELTYPE_SIGNAL_TRACKER : return "MODELTYPE_SIGNAL_TRACKER";
    case MODELTYPE_ERROR         : return "MODELTYPE_ERROR";
    }

  return "unknown";
}
