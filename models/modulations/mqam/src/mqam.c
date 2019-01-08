/**
 *  \file   mqam.c
 *  \brief  modulation model
 *  \author ruifeng Zhang and Elyes Ben Hamida
 *  \date   2008
 **/
#include <math.h>

#include <kernel/include/modelutils.h>


/* ************************************************** */
/* ************************************************** */
model_t model =  {
  "mqam modulation",
  "Ruifeng Zhang",
  "0.1",
  MODELTYPE_MODULATIONS
};

struct classdata {
  int m_qam; /* order of modulation */
};

/* ************************************************** */
/* ************************************************** */
int init(call_t *to, void *params) {
  //    return 0;
  struct classdata *classdata = malloc(sizeof(struct classdata));
  param_t *param;


  /* default values */
  classdata->m_qam = 4;

  /* get parameters */
  list_init_traverse(params);
  while ((param = (param_t *) list_traverse(params)) != NULL) {
    if (!strcmp(param->key, "m_QAM")) {
      if (!strcmp(param->value, "4")) {
	classdata->m_qam = 4;
      }
      else if (!strcmp(param->value, "8")) {
	classdata->m_qam = 8;
      }
      else if (!strcmp(param->value, "16")) {
	classdata->m_qam = 16;
      }
      else if (!strcmp(param->value, "32")) {
	classdata->m_qam = 32;
      }
      else if (!strcmp(param->value, "64")) {
	classdata->m_qam = 64;
      }
      else if (!strcmp(param->value, "128")) {
	classdata->m_qam = 128;
      }
      else if (!strcmp(param->value, "256")) {
	classdata->m_qam = 256;
      }
      else {
	fprintf(stderr,"\n[MQAM_modulation] Unknown modulation type (%s) !\n", param->value);
	fprintf(stderr,"[MQAM_modulation] Possible modulation types are: 4, 8, 16, 32, 64, 128 and 256\n\n");
	goto error;
      }
    }
        
  }
    
  set_class_private_data(to, classdata);
  return 0;

 error:
  free(classdata);
  return -1;
}



int destroy(call_t *to) {

  free(get_class_private_data(to));
  return 0;
}


/* ************************************************** */
/* ************************************************** */
int bind(call_t *to, void *params) {
  return 0;
}

int unbind(call_t *to) {
  return 0;
}


/* ************************************************** */
/* ************************************************** */
double modulate(call_t *to, call_t *from, double snr) {
  struct classdata *classdata = get_class_private_data(to);

  if (snr == MAX_SNR) {
    return 0;
  }
  else {
    return 2*(sqrt(classdata->m_qam)-1)/sqrt(classdata->m_qam)/(log(classdata->m_qam)/log(2)) * erfc(sqrt(3/2*(log(classdata->m_qam)/log(2))/(classdata->m_qam-1)*snr));
  }
}

int bit_per_symbol(call_t *to, call_t *from){
  struct classdata *classdata = get_class_private_data(to);
  return log10(classdata->m_qam)/log10(2);
}

int  get_modulation_type(call_t *to, call_t *from)
{
    return 1;
}

void set_modulation_type(call_t *to, call_t *from, int modulation_type)
{
	return;
}
/* ************************************************** */
/* ************************************************** */
modulation_methods_t methods = {modulate, bit_per_symbol, get_modulation_type,set_modulation_type};



