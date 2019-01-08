/*
 *  \file   mixed fading.c
 *  \brief  mixed fading model
 *  \author Mickael Maman
 *  \date   2014
 */
#include <kernel/include/modelutils.h>



// ************************************************** //
// ************************************************** //

// Defining module informations//
model_t model =  {
    "Mixed Fading",
    "Mickael Maman & Luiz Henrique Suraty Filho",
    "0.1",
    MODELTYPE_FADING,
};


//macro definitions for the selection of fading model
#define RICE 				1
#define NONE				2
//#define RAYLEIGH 			3
//#define NAKAGAMI 			4


/* Macro definition for computation of channel loss */
#define LOSS_UNDEFINED   -999


// ************************************************** //
// ************************************************** //

/** \brief A structure containing the computed fading loss and timestamp
 *  \struct fading_cache
 **/
struct fading_cache {
    uint64_t fading_time; /*!< fadingcomputation timestamp (ns). */
    double fading_value;  /*!< fading loss value (dB). */
};


struct classdata {
    int fading_model; /*!< The selected fading model. */
    uint64_t fading_coherence_time;              /*!< The selected fading coherence time (in nanosecond). */
    double fading_k_los;                          /*!< The selected fading K factor for LOS radio link condition. */
    double fading_k_nlos;                        /*!< The selected fading K factor for NLOS radio link condition. */
    double fading_k_nlos2;                        /*!< The selected fading K factor for NLOS2 radio link condition. */
    struct fading_cache **cache;   				/*!< A 2D matrix to store the computed fading loss and timestamp. */
    int symmetry;  								/*!< Define if link are symmetric i.e. channel reciprocity. */
};


/* ************************************************** */
/* ************************************************** */


int init(call_t *to, void *params)
{
  struct classdata *classdata = malloc(sizeof(struct classdata));
  param_t *param;
  int i, j, nbr_nodes;

  /* memory allocation of the cache for the computation of fading losses */
  nbr_nodes = get_node_count();
  classdata->cache = (struct fading_cache **) malloc(sizeof(struct fading_cache *) * nbr_nodes);
  for (i=0; i<nbr_nodes; i++) {
    classdata->cache[i] = (struct fading_cache *) malloc(sizeof(struct fading_cache) * nbr_nodes);
  }

  /* initialization of the cache structure */
  for (i=0; i<nbr_nodes; i++) {
    for (j=0; j<nbr_nodes; j++) {
      classdata->cache[i][j].fading_value = LOSS_UNDEFINED;
      classdata->cache[i][j].fading_time = get_time();
    }
  }


  /* default values */
  classdata->fading_model= NONE;
  if (get_param_time("20ms", &(classdata->fading_coherence_time))) {
    goto error;
  }

  classdata->fading_k_los   = 9;
  classdata->fading_k_nlos  = 5;
  classdata->fading_k_nlos2 = 1;	

  // by default reciprocity of the fading
  classdata->symmetry = 1;


  /* get parameters */
  list_init_traverse(params);
  while ((param = (param_t *) list_traverse(params)) != NULL)
  {
    if (!strcmp(param->key, "fading"))
    {
      if (!strcmp(param->value, "rice"))
      {
        classdata->fading_model= RICE;
      }
      else if (!strcmp(param->value, "none"))
      {
        classdata->fading_model= NONE;
      }
      else {
        goto error;
      }
    }

    if (!strcmp(param->key, "fading_coherence_time")) {
      if (get_param_time(param->value, &(classdata->fading_coherence_time))) {
        goto error;
      }
    }
    if (!strcmp(param->key, "fading_k_los")) {
      if (get_param_double(param->value, &(classdata->fading_k_los))) {
        goto error;
      }
    }
    if (!strcmp(param->key, "fading_k_nlos")) {
      if (get_param_double(param->value, &(classdata->fading_k_nlos))) {
        goto error;
      }
    }
    if (!strcmp(param->key, "fading_k_nlos2")) {
      if (get_param_double(param->value, &(classdata->fading_k_nlos2))) {
        goto error;
      }
    }		
    if (!strcmp(param->key, "symmetry")) {
      if (get_param_integer(param->value, &(classdata->symmetry))) {
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

  struct classdata *classdata = get_class_private_data(to);
  int nbr_nodes, i;

  /* free the allocated memory for the cache structure */
  nbr_nodes = get_node_count();
  for (i=0; i<nbr_nodes; i++) {
    free(classdata->cache[i]);
  }
  free(classdata->cache);

  free(get_class_private_data(to));
  return 0;
}

int bootstrap(call_t *to, void *params) {
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


/** \brief A function that picks and returns a normally-distributed random variable.
 *  \fn  double normal (double avg, double deviation)
 *  \param avg is the average value of the random variable
 *  \param deviation is the standard deviation of the random variable
 *  \return a normally-distributed random variable
 **/
double normal (double avg, double deviation) {
  return (avg + deviation * cos(2*M_PI*get_random_double()) * sqrt(-2.0 * log(get_random_double())));
}

/** \brief A function that computes a Rice fading model.
 *  \fn  double compute_rice_fading(struct classdata *classdata, packet_t *packet, double link_distance, int link_condition, double rxdBm)
 *  \param classdata is a pointer to the entity global variables
 *  \param packet is a pointer to incoming packet
 *  \param link_condition is the radio link condition between the transmitter and the receiver (LOS, NLOS, or NLOS2)
 *  \param rxdBm is the Received Signal Strength (RSS) after applying the pathloss and shadowing models
 *  \return the Received Signal Strength (RSS) after applying the rice fading model (in dBm)
 **/
double compute_rice_fading(struct classdata *classdata, packet_t *packet, nodeid_t src, nodeid_t dst, int link_condition, double rxdBm) {

  double fading_k, sigma, A, XX, YY, avg;
  int nbr_readings = 0, i = 0;


  /* determine the rice fading K parameter as a function of the link condition */
  switch(link_condition) {
    case LINK_CONDITION_LOS:     fading_k = classdata->fading_k_los; break;
    case LINK_CONDITION_NLOS:   fading_k = classdata->fading_k_nlos; break;
    case LINK_CONDITION_NLOS2: fading_k = classdata->fading_k_nlos2; break;
    default:          fading_k = classdata->fading_k_los; break;
  }

  /* check for the fading coherence time */
  if (classdata->fading_coherence_time <= packet->duration) {
    nbr_readings = ceil(((double)packet->duration)/((double)classdata->fading_coherence_time));
  }
  else {
    nbr_readings = 1;
  }

  if (nbr_readings == 1)
  {
    /* if fading loss is not defined, compute a new value and update the local cache */
    if (classdata->cache[src][dst].fading_value == LOSS_UNDEFINED) {

      A = sqrt(dBm2mW(rxdBm));
      sigma = A / sqrt(2*fading_k);
      XX = normal (A, sigma);
      YY = normal (0, sigma);

      classdata->cache[src][dst].fading_value = mW2dBm(sqrt(pow(XX, 2.0) + pow(YY, 2.0)));
      classdata->cache[src][dst].fading_time = get_time();
      if (classdata->symmetry)
      {
        classdata->cache[dst][src].fading_value = classdata->cache[src][dst].fading_value;
        classdata->cache[dst][src].fading_time = classdata->cache[src][dst].fading_time;
      }
      return classdata->cache[src][dst].fading_value;
    }
    /* if the channel is still coherent, return the already computed fading loss value */
    else if ( (get_time() - classdata->cache[src][dst].fading_time) <= classdata->fading_coherence_time) {
      return classdata->cache[src][dst].fading_value;
    }
    /* compute a new value and update the local cache   */
    else {

      A = sqrt(dBm2mW(rxdBm));
      sigma = A / sqrt(2*fading_k);
      XX = normal (A, sigma);
      YY = normal (0, sigma);

      classdata->cache[src][dst].fading_value = mW2dBm(sqrt(pow(XX, 2.0) + pow(YY, 2.0)));
      classdata->cache[src][dst].fading_time = get_time();
      if (classdata->symmetry)
      {
        classdata->cache[dst][src].fading_value = classdata->cache[src][dst].fading_value;
        classdata->cache[dst][src].fading_time = classdata->cache[src][dst].fading_time;
      }
      return classdata->cache[src][dst].fading_value;
    }

  }
  else
  {

    /* compute the rice fading as a the mean of the nbr_readings fading values  */
    avg = 0.0;
    for (i=0; i<nbr_readings; i++) {
      A = sqrt(dBm2mW(rxdBm));
      sigma = A / sqrt(2*fading_k);
      XX = normal (A, sigma);
      YY = normal (0, sigma);
      avg = avg + mW2dBm(sqrt(pow(XX, 2.0) + pow(YY, 2.0)));
    }

    /* return the average fading loss over the packet transmission duration */
    return ( avg /  ((double)nbr_readings) );


  }
} 




/* ************************************************** */
/* ************************************************** */
double fading(call_t *to_fading, call_t *to_interface, call_t *from_interface, packet_t *packet, double rxdBm)
{

  struct classdata *classdata = get_class_private_data(to_fading);
  double rx_dBm = rxdBm;
  int link_condition;

  medium_t *medium = get_medium_by_id(interface_get_medium(to_interface, from_interface));
  call_t to_link ={medium->links.elts[0],medium->id};

  /* Compute the link condition */
  link_condition = link_get_link_condition(&to_link, to_interface, from_interface);

  /* Compute the fading */
  switch(classdata->fading_model){

    case RICE : rx_dBm = compute_rice_fading(classdata, packet, from_interface->object, to_interface->object, link_condition, rx_dBm) - rx_dBm; break;
    default   : rx_dBm = 0; break;
  }

  return rx_dBm;


}


/* ************************************************** */
/* ************************************************** */
fading_methods_t methods = {fading};
