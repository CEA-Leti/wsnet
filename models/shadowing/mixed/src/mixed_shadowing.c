/*
 *  \file   mixed shadowing.c
 *  \brief  mixed shadowing model
 *  \author Mickael Maman
 *  \date   2014
 */
#include <stdio.h>
#include <kernel/include/modelutils.h>
#include <inttypes.h>


// ************************************************** //
// ************************************************** //

// Defining module informations//
model_t model =  {
    "Mixed Shadowing",
    "Mickael Maman & Luiz Henrique Suraty Filho",
    "0.1",
    MODELTYPE_SHADOWING,
};


//macro definitions for the selection of shadowing model
#define LOGNORMAL 		1
#define NONE		 	2


/* Macro definition for computation of channel loss */
#define LOSS_UNDEFINED   -999

/* ************************************************** */
/* ************************************************** */

/** \brief A structure containing the computed shadowing loss and timestamp
 *  \struct phy_losses_cache
 **/
struct phy_losses_cache {
  uint64_t shadowing_time; /*!< Shadowing computation timestamp (ns). */
  double shadowing_value;  /*!< Shadowing loss value (dB). */
};


struct classdata {
  int shadowing_model; //The selected shadowing model
  uint64_t shadowing_coherence_time;      	/*!< The selected shadowing coherence time (in nanosecond). */
  double shadowing_deviation_los;           /*!< The selected shadowing standard deviation for LOS radio link condition (in dB). */
  double shadowing_deviation_nlos;          /*!< The selected shadowing standard deviation for NLOS radio link condition (in dB). */
  double shadowing_deviation_nlos2;         /*!< The selected shadowing standard deviation for NLOS2 radio link condition (in dB). */
  struct phy_losses_cache **cache;   		/*!< A 2D matrix to store the computed Shadowing loss and timestamp. */
  
  double pathloss_ht;                   /*!< The antenna height at the transmitter (in meter). */
  double pathloss_hr;                   /*!< The antenna height at the receiver (in meter). */
  double pathloss_frequency;            /*!< The frequency (in MHz). */
  
  /* optimize computation */
  double factor;                        /*! Variable to optimize the computation of pathloss. */
  double pathloss_crossover_distance;   /*!< The cross-over distance for the two-ray ground reflection model (in meter). */
  int symmetry;  						/*!< Define if link are symmetric i.e. channel reciprocity. */
};


/* ************************************************** */
/* ************************************************** */
 
  
  int init(call_t *to, void *params)
{
  struct classdata *classdata = malloc(sizeof(struct classdata));
  param_t *param;
  int i, j, nbr_nodes;

  /* memory allocation of the cache for the computation of shadowing losses */
  nbr_nodes = get_node_count();
  classdata->cache = (struct phy_losses_cache **) malloc(sizeof(struct phy_losses_cache *) * nbr_nodes);
  for (i=0; i<nbr_nodes; i++) {
	classdata->cache[i] = (struct phy_losses_cache *) malloc(sizeof(struct phy_losses_cache) * nbr_nodes);
  }
  
    /* initialization of the cache structure */
  for (i=0; i<nbr_nodes; i++) {
	for (j=0; j<nbr_nodes; j++) {
		classdata->cache[i][j].shadowing_value = LOSS_UNDEFINED;
		classdata->cache[i][j].shadowing_time = get_time();
	}
  }

  
   /* default values */
	classdata->shadowing_model= NONE;

	 if (get_param_time("25s", &(classdata->shadowing_coherence_time))) {
     goto error;
  }
  classdata->shadowing_deviation_los    = 0.5;
  classdata->shadowing_deviation_nlos   = 3;
  classdata->shadowing_deviation_nlos2 = 5;
    
    classdata->pathloss_frequency           = 2400;
	classdata->pathloss_ht                  = 0.5;
	classdata->pathloss_hr                  = 0.5;

	// by default reciprocity of the shadowing
	classdata->symmetry = 1;
	
  /* get parameters */
  list_init_traverse(params);
  while ((param = (param_t *) list_traverse(params)) != NULL)
    {
      if (!strcmp(param->key, "shadowing"))
	{
	  if (!strcmp(param->value, "lognormal"))
	    {
		classdata->shadowing_model= LOGNORMAL;
		}
		else if (!strcmp(param->value, "none"))
		{
		classdata->shadowing_model= NONE;
		}
		else {
	      goto error;
	    }
	}
	
	 if (!strcmp(param->key, "pathloss_frequency")) {
      if (get_param_distance(param->value, &(classdata->pathloss_frequency))) {
	goto error;
      }
    }
    if (!strcmp(param->key, "pathloss_ht")) {
      if (get_param_double(param->value, &(classdata->pathloss_ht))) {
	goto error;
      }
    }
    if (!strcmp(param->key, "pathloss_hr")) {
      if (get_param_double(param->value, &(classdata->pathloss_hr))) {
	goto error;
      }
    }
	
	/* reading the shadowing model parameters from the xml file */
	 if (!strcmp(param->key, "shadowing_coherence_time")) {
       if (get_param_time(param->value, &(classdata->shadowing_coherence_time))) {
         goto error;
         }
     }
    if (!strcmp(param->key, "shadowing_deviation_los")) {
      if (get_param_double(param->value, &(classdata->shadowing_deviation_los))) {
	goto error;
      }
    }
    if (!strcmp(param->key, "shadowing_deviation_nlos")) {
      if (get_param_double(param->value, &(classdata->shadowing_deviation_nlos))) {
	goto error;
      }
    }
    if (!strcmp(param->key, "shadowing_deviation_nlos2")) {
      if (get_param_double(param->value, &(classdata->shadowing_deviation_nlos2))) {
	goto error;
      }
    }	
    if (!strcmp(param->key, "symmetry")) {
      if (get_param_integer(param->value, &(classdata->symmetry))) {
	goto error;
      }
    }
	
	}

	 /* compute the factor variable to optimize the computation of pathloss */
   classdata->factor = 300 / (4 * M_PI * classdata->pathloss_frequency);

  /* compute the cross-over distance for the two-ray ground propagation model */
  classdata->pathloss_crossover_distance = (classdata->pathloss_ht * classdata->pathloss_hr) / classdata->factor;
		
	
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


/** \brief A function that computes a log-normal shadowing loss.
 *  \fn  double compute_lognormal_shadowing(struct classdata *classdata, packet_t *packet, nodeid_t src, nodeid_t dst, double link_distance, int link_condition, double rx_dBm)
 *  \param classdata is a pointer to the entity global variables
 *  \param packet is a pointer to incoming packet
 *  \param src is the ID of the transmitter
 *  \param dst is the ID of the receiver
 *  \param link_distance is the distance between the transmitter and the receiver (in meter)
 *  \param link_condition is the radio link condition between the transmitter and the receiver (LOS, NLOS, or NLOS2)
 *  \param rx_dBm is the Received Signal Strength (RSS) after applying the pathloss model
 *  \return the Received Signal Strength (RSS) after applying the shadowing model (in dBm)
 **/
double compute_lognormal_shadowing(struct classdata *classdata, packet_t *packet, nodeid_t src, nodeid_t dst, double link_distance, int link_condition, double rx_dBm) {
  /*
   *  Pr_dBm(d) = Pr_dBm(d0) - 10 * beta * log10(d/d0) + X
   *
   *  Note: rxdBm = [Pt + Gt + Gr]_dBm, L = 1, and X a normal distributed RV (in dBm)
   *
   *  cf p104-105 ref "Wireless Communications: Principles and Practice", Theodore Rappaport, 1996.
   *
   */
  double deviation;
  //int nbr_readings;
  /*
  link_condition = NLOS2;

  if (link_distance< 8){
	  link_condition = LOS;
  }

  if ( (link_distance>=8) && (link_distance < 16)){
	link_condition = NLOS;
  }*/

  /* compute the pathloss exponent as a function of the link condition and the distance:
  *  deviation = shadowing_deviation_los       IFF (link_condition = LOS      AND   link_distance < pathloss_crossover_distance)
  *  deviation = shadowing_deviation_nlos     IFF (link_condition = NLOS    OR    link_distance >= pathloss_crossover_distance)
  *  deviation = shadowing_deviation_nlos2   IFF (link_condition = NLOS2)
  */
  switch(link_condition) {
  case LINK_CONDITION_LOS:    if (link_distance < classdata->pathloss_crossover_distance) {
							deviation = classdata->shadowing_deviation_los;
						}
						else {
							deviation = classdata->shadowing_deviation_nlos;
						}
						break;
  case LINK_CONDITION_NLOS:   deviation = classdata->shadowing_deviation_nlos; break;
  case LINK_CONDITION_NLOS2: deviation = classdata->shadowing_deviation_nlos2; break;
  default:          deviation = classdata->shadowing_deviation_los; break;
  }   
  
  /* check for the shadowing coherence time */
  //nbr_readings = ceil(((double)packet->duration)/((double)classdata->shadowing_coherence_time));

  /* if shadowing loss is not defined, compute a new value and update the local cache */
  if (classdata->cache[src][dst].shadowing_value == LOSS_UNDEFINED) {


	  classdata->cache[src][dst].shadowing_value = get_gaussian(0.0, deviation);
	  classdata->cache[src][dst].shadowing_time = get_time();
	  if (classdata->symmetry)
	  {
		classdata->cache[dst][src].shadowing_value = classdata->cache[src][dst].shadowing_value;
		classdata->cache[dst][src].shadowing_time = classdata->cache[src][dst].shadowing_time;
	  }
	return classdata->cache[src][dst].shadowing_value;
  }
  /* if the channel is still coherent, return the already computed shadowing loss value */
  else if ( (get_time() - classdata->cache[src][dst].shadowing_time) <= classdata->shadowing_coherence_time) {
	return classdata->cache[src][dst].shadowing_value;
  }
  /* compute a new value and update the local cache   */
  else {
	classdata->cache[src][dst].shadowing_value = get_gaussian(0.0, deviation);
	classdata->cache[src][dst].shadowing_time = get_time();
	  if (classdata->symmetry)
	  {
		classdata->cache[dst][src].shadowing_value = classdata->cache[src][dst].shadowing_value;
		classdata->cache[dst][src].shadowing_time = classdata->cache[src][dst].shadowing_time;
	  }
	return classdata->cache[src][dst].shadowing_value;	
  }

}

/* ************************************************** */
/* ************************************************** */
double shadowing(call_t *to_shadowing, call_t *to_interface, call_t *from_interface, packet_t *packet, double rxdBm)
{

    struct classdata *classdata = get_class_private_data(to_shadowing);
    double 	link_distance = distance(get_node_position(from_interface->object), get_node_position(to_interface->object));
	double rx_dBm = rxdBm;
	int link_condition;


	medium_t *medium = get_medium_by_id(interface_get_medium(to_interface, from_interface));
	call_t to_link ={medium->links.elts[0],medium->id};

	/* Compute the link condition */
	link_condition = link_get_link_condition(&to_link, to_interface, from_interface);


	/* Compute the shadowing */
	switch(classdata->shadowing_model){
	case LOGNORMAL : rx_dBm = compute_lognormal_shadowing(classdata, packet, from_interface->object, to_interface->object, link_distance, link_condition, rx_dBm); break;
	default        : rx_dBm = 0; break;
	}

   return rx_dBm;

}


/* ************************************************** */
/* ************************************************** */
shadowing_methods_t methods = {shadowing};
