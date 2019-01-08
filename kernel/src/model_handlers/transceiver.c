/**
 *  \file   transceiver.c
 *  \brief  Handle hardware transceiver models (radio, cpl, ...).
 *  \author Loic Lemaitre
 *  \date   2010
 **/

#include <kernel/include/definitions/class.h>
#include "media_rxtx.h"


/* ************************************************** */
/* ************************************************** */
double transceiver_get_noise(call_t *to, call_t *from)
{
  class_t *class = get_class_by_id(to->class);
  if (class->model->type != MODELTYPE_TRANSCEIVER)
    return MIN_DBM;
  return class->methods->transceiver.get_noise(to, from);    
}

double transceiver_get_power(call_t *to, call_t *from)
{
  class_t *class = get_class_by_id(to->class);
  if (class->model->type != MODELTYPE_TRANSCEIVER)
    return MIN_DBM; 
  return class->methods->transceiver.get_power(to, from);    
}

void transceiver_set_power(call_t *to, call_t *from, double power)
{
  class_t *class = get_class_by_id(to->class);
  if (class->model->type != MODELTYPE_TRANSCEIVER)
    return;
  class->methods->transceiver.set_power(to, from, power);    
}

double transceiver_get_cs(call_t *to, call_t *from)
{
  class_t *class = get_class_by_id(to->class);
  if (class->model->type != MODELTYPE_TRANSCEIVER)
    return MIN_DBM; 
  return class->methods->transceiver.get_cs(to, from);
}

int transceiver_get_channel(call_t *to, call_t *from)
{
  class_t *class = get_class_by_id(to->class);
  if (class->model->type != MODELTYPE_TRANSCEIVER)
    return 0;
  return class->methods->transceiver.get_channel(to, from);    
}

void transceiver_set_channel(call_t *to, call_t *from, int channel)
{
  class_t *class = get_class_by_id(to->class);
  if (class->model->type != MODELTYPE_TRANSCEIVER)
    return;
  class->methods->transceiver.set_channel(to, from, channel);    
}

classid_t transceiver_get_modulation(call_t *to, call_t *from)
{
  class_t *class = get_class_by_id(to->class);
  if (class->model->type != MODELTYPE_TRANSCEIVER)
    return -1;
  return class->methods->transceiver.get_modulation(to, from);    
}

void transceiver_set_modulation(call_t *to, call_t *from, classid_t modulation)
{
  class_t *class = get_class_by_id(to->class);
  if (class->model->type != MODELTYPE_TRANSCEIVER)
    return;
  class->methods->transceiver.set_modulation(to, from, modulation);    
}

uint64_t transceiver_get_Tb(call_t *to, call_t *from)
{
  class_t *class = get_class_by_id(to->class);
  if (class->model->type != MODELTYPE_TRANSCEIVER)
    return 0;
  return class->methods->transceiver.get_Tb(to, from);    
}

void transceiver_set_Ts(call_t *to, call_t *from, uint64_t Ts)
{
  class_t *class = get_class_by_id(to->class);
  if (class->model->type != MODELTYPE_TRANSCEIVER)
    return;
  class->methods->transceiver.set_Ts(to, from, Ts);    
}

uint64_t transceiver_get_Ts(call_t *to, call_t *from)
{
  class_t *class = get_class_by_id(to->class);
  if (class->model->type != MODELTYPE_TRANSCEIVER)
    return 0;
  return class->methods->transceiver.get_Ts(to, from);
}

void transceiver_cs(call_t *to, call_t *from, packet_t *packet)
{
  class_t *class = get_class_by_id(to->class);
  if (class->model->type != MODELTYPE_TRANSCEIVER)
    return;
  return class->methods->transceiver.cs(to, from, packet);    
}

void transceiver_set_sensibility(call_t *to, call_t *from, double sensibility) {
  class_t *class = get_class_by_id(to->class);
  if (class->model->type != MODELTYPE_TRANSCEIVER)
    return;
  class->methods->transceiver.set_sensibility(to, from, sensibility);
}

double transceiver_get_sensibility(call_t *to, call_t *from)
{
  class_t *class = get_class_by_id(to->class);
  if (class->model->type != MODELTYPE_TRANSCEIVER)
    return MIN_DBM;
  return class->methods->transceiver.get_sensibility(to, from);
}

void transceiver_sleep(call_t *to, call_t *from)
{
  class_t *class = get_class_by_id(to->class);
  if (class->model->type != MODELTYPE_TRANSCEIVER)
    return;
  class->methods->transceiver.sleep(to, from);
}

void transceiver_wakeup(call_t *to, call_t *from)
{
  class_t *class = get_class_by_id(to->class);
  if (class->model->type != MODELTYPE_TRANSCEIVER)
    return;
  class->methods->transceiver.wakeup(to, from);
}

void transceiver_switch_rx(call_t *to, call_t *from)
{
  class_t *class = get_class_by_id(to->class);
  if (class->model->type != MODELTYPE_TRANSCEIVER)
    return;
  class->methods->transceiver.switch_rx(to, from);
}

void transceiver_switch_idle(call_t *to, call_t *from)
{
  class_t *class = get_class_by_id(to->class);
  if (class->model->type != MODELTYPE_TRANSCEIVER)
    return;
  class->methods->transceiver.switch_idle(to, from);
}


/* edit by Christophe Savigny <christophe.savigny@insa-lyon.fr> */
int transceiver_get_modulation_bit_per_symbol(call_t *to, call_t *from)
{
  class_t *class = get_class_by_id(to->class);
  if (class->model->type != MODELTYPE_TRANSCEIVER)
    return 0;
  return class->methods->transceiver.get_modulation_bit_per_symbol(to, from);
}
/* end of edition */


int transceiver_get_modulation_type(call_t *to, call_t *from)
{
	  class_t *class = get_class_by_id(to->class);
	  if (class->model->type != MODELTYPE_TRANSCEIVER)
	    return 0;
	  return class->methods->transceiver.get_modulation_type(to, from);

}

void transceiver_set_modulation_type(call_t *to, call_t *from, int modulation_type)
{
	  class_t *class = get_class_by_id(to->class);
	  if (class->model->type == MODELTYPE_TRANSCEIVER)
		  class->methods->transceiver.set_modulation_type(to, from, modulation_type);

}



/*edit by Rida El Chall*/
uint64_t transceiver_get_freq_low(call_t *to, call_t *from)
{
  class_t *class = get_class_by_id(to->class);
  if (class->model->type != MODELTYPE_TRANSCEIVER)
    return 0;
  return class->methods->transceiver.get_freq_low(to, from);
}

uint64_t transceiver_get_freq_high(call_t *to, call_t *from)
{
  class_t *class = get_class_by_id(to->class);
  if (class->model->type != MODELTYPE_TRANSCEIVER)
    return 0;
  return class->methods->transceiver.get_freq_high(to, from);
}

void transceiver_set_freq_low(call_t *to, call_t *from, uint64_t freq_low)
{
  class_t *class = get_class_by_id(to->class);
  if (class->model->type != MODELTYPE_TRANSCEIVER)
    return;
  class->methods->transceiver.set_freq_low(to, from, freq_low);
}

void transceiver_set_freq_high(call_t *to, call_t *from, uint64_t freq_high)
{
  class_t *class = get_class_by_id(to->class);
  if (class->model->type != MODELTYPE_TRANSCEIVER)
    return;
  class->methods->transceiver.set_freq_high(to, from, freq_high);
}
/*end of edition*/

