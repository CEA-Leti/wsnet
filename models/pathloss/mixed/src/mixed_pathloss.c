/*
 *  \file   mixed pathloss.c
 *  \brief  mixed pathloss model
 *  \author Mickael Maman & Luiz Henrique Suraty Filho
 *  \date   2014
 */
#include <stdio.h>
#include <kernel/include/modelutils.h>
#include <inttypes.h>


// ************************************************** //
// ************************************************** //

// Defining module informations//
model_t model =  {
    "Mixed Pathloss",
    "Mickael Maman & Luiz Henrique Suraty Filho",
    "0.1",
    MODELTYPE_PATHLOSS,
};

typedef enum {
  FREESPACE,
  TWORAYGROUND,
  NONE
} pathloss_type_t;


struct classdata {
  pathloss_type_t pathloss_model;						/*!< The selected pathloss model. */
  double pathloss_exponent_los;      	/*!< The selected pathloss decay exponent for the LOS radio link condition. */
  double pathloss_exponent_nlos;    	/*!< The selected pathloss decay exponent for the NLOS radio link condition. */
  double pathloss_exponent_nlos2;  		/*!< The selected pathloss decay exponent for the NLOS2 radio link condition. */
  double pathloss_exponent_nlos3;     /*!< The selected pathloss decay exponent for the NLOS2 radio link condition. */
  double pathloss_dist0_los;            /*!< The reference distance for the pathloss model for the LOS radio link condition(in meter). */
  double pathloss_dist0_nlos;           /*!< The reference distance for the pathloss model for the NLOS radio link condition(in meter). */
  double pathloss_dist0_nlos2;          /*!< The reference distance for the pathloss model for the NLOS2 radio link condition(in meter). */
  double pathloss_dist0_nlos3;          /*!< The reference distance for the pathloss model for the NLOS2 radio link condition(in meter). */
  double pathloss_ht;                   /*!< The antenna height at the transmitter (in meter). */
  double pathloss_hr;                   /*!< The antenna height at the receiver (in meter). */
  double pathloss_frequency;            /*!< The frequency (in MHz). */
  double pathloss_PL0_los;            	/*!< The pathloss at the reference distance for the LOS radio link condition (in dBm). */
  double pathloss_PL0_nlos;            	/*!< The pathloss at the reference distance for the NLOS radio link condition (in dBm). */
  double pathloss_PL0_nlos2;           	/*!< The pathloss at the reference distance for the NLOS2 radio link condition (in dBm). */
  double pathloss_PL0_nlos3;            /*!< The pathloss at the reference distance for the NLOS2 radio link condition (in dBm). */
  double factor;                        /*! Variable to optimize the computation of pathloss. */
  double pathloss_crossover_distance;   /*!< The cross-over distance for the two-ray ground reflection model (in meter). */
};


/* ************************************************** */
/* ************************************************** */


int init(call_t *to, void *params)
{
  struct classdata *classdata = malloc(sizeof(struct classdata));
  param_t *param;

  /* default values for the pathloss modeling */
  classdata->pathloss_model = FREESPACE;
  classdata->pathloss_exponent_los    	= 2.0;
  classdata->pathloss_exponent_nlos   	= 3.0;
  classdata->pathloss_exponent_nlos2  	= 3.3;
  classdata->pathloss_exponent_nlos3    = 5.0;
  classdata->pathloss_dist0_los          	= 1.0;
  classdata->pathloss_dist0_nlos         	= 1.0;
  classdata->pathloss_dist0_nlos2        	= 1.0;
  classdata->pathloss_dist0_nlos3         = 1.0;
  classdata->pathloss_frequency           = 2400;
  classdata->pathloss_ht                  = 0.5;
  classdata->pathloss_hr                  = 0.5;
  classdata->pathloss_PL0_los				=	0;
  classdata->pathloss_PL0_nlos			=	0;
  classdata->pathloss_PL0_nlos2			=	0;
  classdata->pathloss_PL0_nlos3     = 0;

  list_init_traverse(params);
  while ((param = (param_t *) list_traverse(params)) != NULL)
  {
    if (!strcmp(param->key, "pathloss")) {
      if (!strcmp(param->value, "freespace")) {
        classdata->pathloss_model= FREESPACE;
      } else if (!strcmp(param->value, "tworayground")) {
        classdata->pathloss_model= TWORAYGROUND;
      } else if (!strcmp(param->value, "none")) {
        classdata->pathloss_model= NONE;
      } else {
        goto error;
      }
    }
    if (!strcmp(param->key, "pathloss_exponent_los")) {
      if (get_param_double(param->value, &(classdata->pathloss_exponent_los))) {
        goto error;
      }
    }
    if (!strcmp(param->key, "pathloss_exponent_nlos")) {
      if (get_param_double(param->value, &(classdata->pathloss_exponent_nlos))) {
        goto error;
      }
    }
    if (!strcmp(param->key, "pathloss_exponent_nlos2")) {
      if (get_param_double(param->value, &(classdata->pathloss_exponent_nlos2))) {
        goto error;
      }
    }
    if (!strcmp(param->key, "pathloss_exponent_nlos3")) {
      if (get_param_double(param->value, &(classdata->pathloss_exponent_nlos3))) {
        goto error;
      }
    }
    if (!strcmp(param->key, "pathloss_PL0_los")) {
      if (get_param_double(param->value, &(classdata->pathloss_PL0_los))) {
        goto error;
      }
    }
    if (!strcmp(param->key, "pathloss_PL0_nlos")) {
      if (get_param_double(param->value, &(classdata->pathloss_PL0_nlos))) {
        goto error;
      }
    }
    if (!strcmp(param->key, "pathloss_PL0_nlos2")) {
      if (get_param_double(param->value, &(classdata->pathloss_PL0_nlos2))) {
        goto error;
      }
    }
    if (!strcmp(param->key, "pathloss_PL0_nlos3")) {
      if (get_param_double(param->value, &(classdata->pathloss_PL0_nlos3))) {
        goto error;
      }
    }

    if (!strcmp(param->key, "pathloss_dist0")) {
      if (get_param_distance(param->value, &(classdata->pathloss_dist0_los))) {
        goto error;
      }
    }
    if (!strcmp(param->key, "pathloss_dist0_los")) {
      if (get_param_distance(param->value, &(classdata->pathloss_dist0_los))) {
        goto error;
      }
    }
    if (!strcmp(param->key, "pathloss_dist0_nlos")) {
      if (get_param_distance(param->value, &(classdata->pathloss_dist0_nlos))) {
        goto error;
      }
    }
    if (!strcmp(param->key, "pathloss_dist0_nlos2")) {
      if (get_param_distance(param->value, &(classdata->pathloss_dist0_nlos2))) {
        goto error;
      }
    }
    if (!strcmp(param->key, "pathloss_dist0_nlos3")) {
      if (get_param_distance(param->value, &(classdata->pathloss_dist0_nlos2))) {
        goto error;
      }
    }

    if (!strcmp(param->key, "pathloss_frequency")) {
      if (get_param_double(param->value, &(classdata->pathloss_frequency))) {
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
  }
  // compute the factor variable to optimize the computation of pathloss
  classdata->factor = 300 / (4 * M_PI * classdata->pathloss_frequency);
  // compute the cross-over distance for the two-ray ground propagation model
  classdata->pathloss_crossover_distance = (classdata->pathloss_ht * classdata->pathloss_hr) / classdata->factor;
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


/** \brief A function that computes the pathloss according to the freespace (log-distance) propagation model.
 *  \fn double compute_freespace_pathloss(struct classdata *classdata, nodeid_t src, nodeid_t dst, double link_distance, int link_condition, double rxdBm)
 *  \param classdata is a pointer to the entity global variables
 *  \param src is the ID of the transmitter
 *  \param dst is the ID of the receiver
 *  \param link_distance is the distance between the transmitter and the receiver (in meter)
 *  \param link_condition is the radio link condition between the transmitter and the receiver (LOS, NLOS, or NLOS2)
 *  \param rxdBm is a variable equal to: P_tx + G_tx + G_rx (in dBm)
 *  \return the Received Signal Strength (RSS) after applying the pathloss model (in dBm)
 **/
double compute_freespace_pathloss(struct classdata *classdata, nodeid_t src, nodeid_t dst, double link_distance, int link_condition, double rxdBm) {
  /*
   *  Pr_dBm(d) = Pr_dBm(d0) - 10 * beta * log10(d/d0)
   *
   *  Note: rxdBm = [Pt + Gt + Gr]_dBm, L = 1
   *
   *  cf p104-105 ref "Wireless Communications: Principles and Practice", Theodore Rappaport, 1996.
   *
   */
  double exponent, pathloss_Pr0;
  double distance0,PL0;

  if (link_distance == 0) {
    return rxdBm;
  }

  // compute the pathloss exponent as a function of the link condition
  switch(link_condition) {
    case LINK_CONDITION_LOS:
      exponent = classdata->pathloss_exponent_los;
      distance0=classdata->pathloss_dist0_los;
      PL0=classdata->pathloss_PL0_los;
      break;
    case LINK_CONDITION_NLOS:
      exponent = classdata->pathloss_exponent_nlos;
      distance0=classdata->pathloss_dist0_nlos;
      PL0=classdata->pathloss_PL0_nlos;
      break;
    case LINK_CONDITION_NLOS2:
      exponent = classdata->pathloss_exponent_nlos2;
      distance0=classdata->pathloss_dist0_nlos2;
      PL0=classdata->pathloss_PL0_nlos2;
      break;
    case LINK_CONDITION_NLOS3:
      exponent = classdata->pathloss_exponent_nlos3;
      distance0=classdata->pathloss_dist0_nlos3;
      PL0=classdata->pathloss_PL0_nlos3;
      break;
    default:
      exponent = classdata->pathloss_exponent_nlos3;
      distance0=classdata->pathloss_dist0_nlos3;
      PL0=classdata->pathloss_PL0_nlos3;
      break;
  }

  if (PL0==0){
    pathloss_Pr0  = dBm2mW(rxdBm) * pow(classdata->factor / distance0, 2);
    return mW2dBm(pathloss_Pr0)  - 10.0 * exponent * log10(link_distance / distance0);
  } else {
    return rxdBm + PL0  - 10.0 * exponent * log10(link_distance / distance0);
  }
}


/** \brief A function that computes the pathloss according to the two-ray ground propagation model.
 *  \fn  double compute_tworayground(struct classdata *classdata, nodeid_t src, nodeid_t dst, double link_distance, int link_condition, double rxdBm)
 *  \param classdata is a pointer to the entity global variables
 *  \param src is the ID of the transmitter
 *  \param dst is the ID of the receiver
 *  \param link_distance is the distance between the transmitter and the receiver (in meter)
 *  \param link_condition is the radio link condition between the transmitter and the receiver (LOS, NLOS, or NLOS2)
 *  \param rxdBm is a variable equal to: P_tx + G_tx + G_rx (in dBm)
 *  \return the Received Signal Strength (RSS) after applying the pathloss model (in dBm)
 **/
double compute_tworayground(struct classdata *classdata, nodeid_t src, nodeid_t dst, double link_distance, int link_condition, double rxdBm) {
  /*
   * Cross over distance:
   *
   *        4 * pi * ht * hr
   *  dc = ------------------
   *             lambda
   *                               Pt * Gt * Gr * (ht * hr)^2
   *  Case 1: d >= dc  => Pr(d) = -----------------------------
   *                                        d^4 * L
   *
   *                               Pt * Gt * Gr * lambda^2
   *  Case 2: d < dc   => Pr(d) = -------------------------
   *                                 (4 * pi * d)^2 * L
   *
   *  Note: rxmW = Pt * Gt * Gr, and L = 1
   *  
   *  cf p89 ref "Wireless Communications: Principles and Practice", Theodore Rappaport, 1996.
   *
   */

  if (link_distance == 0) {
    return rxdBm;
  } 
  else if (link_distance < classdata->pathloss_crossover_distance) {
    // uses friis formula for distance < crossover distance or if (ht == hr == 0)
    return (rxdBm + 2 * mW2dBm(classdata->factor / link_distance));
  } else {
    // else two-ray ground model is more accurate for distance > crossover distance
    return (rxdBm + mW2dBm(classdata->pathloss_ht * classdata->pathloss_ht * 
                           classdata->pathloss_hr * classdata->pathloss_hr / pow(link_distance, 4.0)));
  }
}



/* ************************************************** */
/* ************************************************** */
double pathloss(call_t *to_pathloss, call_t *to_interface ,call_t *from_interface, packet_t *packet, double rxdBm){
  struct classdata *classdata = get_class_private_data(to_pathloss);
  double link_distance = distance(get_node_position(from_interface->object), get_node_position(to_interface->object));
  double rx_dBm = rxdBm;
  int link_condition;


  medium_t *medium = get_medium_by_id(interface_get_medium(to_interface, from_interface));
  call_t to_link ={medium->links.elts[0],medium->id};

  // Compute the link condition
  link_condition = link_get_link_condition(&to_link, to_interface, from_interface);

  // Compute the pathloss
  switch(classdata->pathloss_model){
    case FREESPACE    : rx_dBm = compute_freespace_pathloss(classdata, from_interface->object, to_interface->object, link_distance, link_condition, rxdBm); break;
    case TWORAYGROUND : rx_dBm = compute_tworayground(classdata, from_interface->object, to_interface->object, link_distance, link_condition, rxdBm); break;
    default           : rx_dBm = rxdBm;
  }	

  return rx_dBm;

}


/* ************************************************** */
/* ************************************************** */
pathloss_methods_t methods = {pathloss};

