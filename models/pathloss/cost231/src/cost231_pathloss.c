/*
 *  \file   cost231_pathloss.c
 *  \brief  COST 231 Pathloss Model
 *
 *          The attenuation (in dB) linearly grows the number of walls traversed.
 *          The effect of floors is non-linear.
 *          This path loss model is valid for 800-1900 MHz.
 *  \author Luiz Henrique Suraty Filho
 *  \date   2018
 */
#include <stdio.h>
#include <kernel/include/modelutils.h>


// ************************************************** //
// ************************************************** //

typedef enum {
  COST231_THIN_WALL=0,
  COST231_THICK_WALL=1,
}wall_type_t;

// Defining module informations//
model_t model =  {
    "COST 231 Pathloss",
    "Luiz Henrique Suraty Filho",
    "0.1",
    MODELTYPE_PATHLOSS,
};

typedef struct classdata {
  double pathloss_frequency;            /*!< The frequency (in MHz). */
  double pathloss_PL0_los;              /*!< The pathloss at the reference distance for the LOS radio link condition (in dBm). */
  double pathloss_dist0_los;            /*!< The reference distance for the pathloss model for the LOS radio link condition(in meter). */
  double pathloss_exponent_los;         /*!< The selected pathloss decay exponent for the LOS radio link condition. */
  double factor;                        /*!< Variable to optimize the computation of pathloss. */
  wall_type_t wall_type;                /*!< The type of the walls on the scenario */
  uint    thick_wall_present;           /*!< Set to 1 if thick walls on the scenario, 0 otherwise */
  uint    thin_wall_present;            /*!< Set to 1 if thin walls on the scenario, 0 otherwise */
} cost231_pathloss_classdata_t;


/* ************************************************** */
/* ************************************************** */


int init(call_t *to, void *params)
{
  cost231_pathloss_classdata_t *classdata = malloc(sizeof(cost231_pathloss_classdata_t));
  param_t *param;

  classdata->pathloss_dist0_los    = 1.0;
  classdata->pathloss_frequency    = 868;
  classdata->pathloss_PL0_los		   =	0;
  classdata->pathloss_dist0_los    = 1.0;
  classdata->pathloss_exponent_los = 2.0;
  classdata->wall_type             = COST231_THICK_WALL;

  list_init_traverse(params);
  while ((param = (param_t *) list_traverse(params)) != NULL){
    if (!strcmp(param->key, "pathloss_exponent_los")) {
      if (get_param_double(param->value, &(classdata->pathloss_exponent_los))) {
        goto error;
      }
    }
    if (!strcmp(param->key, "pathloss_PL0_los")) {
      if (get_param_double(param->value, &(classdata->pathloss_PL0_los))) {
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
    if (!strcmp(param->key, "pathloss_frequency")) {
      if (get_param_double(param->value, &(classdata->pathloss_frequency))) {
        goto error;
      }
    }

    if (!strcmp(param->key, "wall_type")) {
      if (get_param_unsigned_integer(param->value, &(classdata->wall_type))) {
        goto error;
      }
    }
  }

  if (classdata->wall_type == COST231_THICK_WALL){
    classdata->thick_wall_present = 1;
    classdata->thin_wall_present = 0;
  } else if (classdata->wall_type == COST231_THIN_WALL){
    classdata->thick_wall_present = 0;
    classdata->thin_wall_present = 1;
  } else {
    goto error;
  }

  // compute the factor variable to optimize the computation of pathloss
  classdata->factor = 300 / (4 * M_PI * classdata->pathloss_frequency);

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
double compute_freespace_pathloss(cost231_pathloss_classdata_t *classdata, nodeid_t src, nodeid_t dst, double link_distance, double rxdBm) {
  /*
   *  Pr_dBm(d) = Pr_dBm(d0) - 10 * beta * log10(d/d0)
   *
   *  Note: rxdBm = [Pt + Gt + Gr]_dBm, L = 1
   *
   *  cf p104-105 ref "Wireless Communications: Principles and Practice", Theodore Rappaport, 1996.
   *
   */
  if (link_distance == 0) {
    return rxdBm;
  }

  if (classdata->pathloss_PL0_los==0){
    double pathloss_Pr0  = dBm2mW(rxdBm) * pow(classdata->factor / classdata->pathloss_dist0_los, 2);
    return mW2dBm(pathloss_Pr0)  - 10.0 * classdata->pathloss_exponent_los * log10(link_distance / classdata->pathloss_dist0_los);
  } else {
    return rxdBm + classdata->pathloss_PL0_los  - 10.0 * classdata->pathloss_exponent_los * log10(link_distance / classdata->pathloss_dist0_los);
  }
}


/* ************************************************** */
/* ************************************************** */
double pathloss(call_t *to_pathloss, call_t *to_interface ,call_t *from_interface, packet_t *packet, double rxdBm){
  /*   L = Lfs + 37 + 3.4*kw1 + 6.9*kw2 + 18.3*n^((n+2)/(n+1)-0.46)
   *
   *   L   is the attenuation in dB
   *   Lfs  is the free space loss in dB
   *   n   is the number of traversed floors (reinforced concrete, but not thicker than 30 cm)
   *   kw1  is the number of light internal walls (e.g. plaster board), windows etc (not thicker than 10 cm)
   *   kw2  is the number of concrete or brick internal walls (thicker than 10 cm)
   *
   */
  cost231_pathloss_classdata_t *classdata = (cost231_pathloss_classdata_t*) get_class_private_data(to_pathloss);
  double link_distance = distance(get_node_position(from_interface->object), get_node_position(to_interface->object));
  double rx_dBm = rxdBm;
  call_t to_map = {get_map_classid(), to_pathloss->object};
  int kw2 = map_get_nbr_walls(&to_map, from_interface, from_interface->object, to_interface->object);
  int kw1 = kw2 * classdata->thin_wall_present;
  kw2 = kw2 *classdata->thick_wall_present;

  // we still don't have floor counts
  int n = 0;
  // Compute the pathloss
  double Lfs = compute_freespace_pathloss(classdata, from_interface->object, to_interface->object, link_distance, rx_dBm);
  rx_dBm = Lfs - 37 - 3.4*kw1 - 6.9*kw2 - 18.3*pow(n,((n+2)/(n+1)-0.46));
  return rx_dBm;
}


/* ************************************************** */
/* ************************************************** */
pathloss_methods_t methods = {pathloss};

