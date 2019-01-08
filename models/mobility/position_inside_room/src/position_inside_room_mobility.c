/**
 *  \file   position_inside_room_mobility.c
 *  \brief  Position Inside Room mobility - Source Code File
 *  \author Luiz Henrique Suraty Filho
 *  \date   2015
 **/

#include <kernel/include/modelutils.h>
#include "position_inside_room_mobility.h"
#include <models/models_dbg.h>

/* ************************************************** */
/* ************************************************** */
model_t model =  {
    "No mobility, random position in each room ",
    "Luiz Henrique Suraty Filho",
    "0.1",
    MODELTYPE_MOBILITY,

};

/** \brief Set default values of the node.
 *  \fn void set_node_default_values(struct _802_15_4_s_csma_ca_mac_node_private *nodedata)
 *  \param nodedata is a pointer to the node data
 **/
void set_node_default_values(call_t *to, position_inside_room_mobility_node_private_t *nodedata){
  /* set the default values for the variable for printing*/

  double nb_room_per_line;

  nodedata->nb_node_per_room=4;
  nodedata->room_width=5;
  nodedata->room_length=5;

  if (to->object == 0){
    nodedata->min_X = 0.0;
    nodedata->min_Y = 0.0;
    nodedata->max_X = get_topology_area()->x;
    nodedata->max_Y = get_topology_area()->y;
    nodedata->room_id=61;
  }
  else
  {
    nodedata->room_id=ceil((to->object)/(nodedata->nb_node_per_room));

    nb_room_per_line=((get_topology_area()->x)/nodedata->room_width);

    nodedata->min_X = (nodedata->room_id - nb_room_per_line*(ceil(nodedata->room_id/nb_room_per_line)-1)-1)*((get_topology_area()->x)/nb_room_per_line);
    nodedata->min_Y = (ceil(nodedata->room_id/nb_room_per_line)-1)*((get_topology_area()->y)/nb_room_per_line);
    nodedata->max_X = (nodedata->room_id - nb_room_per_line*(ceil(nodedata->room_id/nb_room_per_line)-1))*((get_topology_area()->x)/nb_room_per_line);
    nodedata->max_Y = ceil(nodedata->room_id/nb_room_per_line)*((get_topology_area()->y)/nb_room_per_line);
  }
  nodedata->x_value = -1;
  nodedata->y_value = -1;

  nodedata->log_status = 0;

}


/** \brief Read parameters from the xml file.
 *  \fn int read_xml_file(struct _multi_channel_node_private *nodedata, void *params)
 *  \param nodedata is a pointer to the node data
 *  \param params is a pointer to the parameters read from the XML configuration file
 *  \return SUCCESSFUL if success, UNSUCCESSFUL otherwise
 **/
int read_nodedata_from_xml_file(call_t *to , position_inside_room_mobility_node_private_t *nodedata , void *params){

  param_t *param;
  /* get module parameters from the XML configuration file*/
  list_init_traverse(params);

  while ((param = (param_t *) list_traverse(params)) != NULL) {


    if (!strcmp(param->key, "min_X")) {
      if (get_param_double(param->value, &(nodedata->min_X))) {
        return UNSUCCESSFUL;
      }
    }

    if (!strcmp(param->key, "min_Y")) {
      if (get_param_double(param->value, &(nodedata->min_Y))) {
        return UNSUCCESSFUL;
      }
    }

    if (!strcmp(param->key, "max_X")) {
      if (get_param_double(param->value, &(nodedata->max_X))) {
        return UNSUCCESSFUL;
      }
    }

    if (!strcmp(param->key, "max_Y")) {
      if (get_param_double(param->value, &(nodedata->max_Y))) {
        return UNSUCCESSFUL;
      }
    }

    if (!strcmp(param->key, "x_value")) {
      if (get_param_double(param->value, &(nodedata->x_value))) {
        return UNSUCCESSFUL;
      }
    }

    if (!strcmp(param->key, "y_value")) {
      if (get_param_double(param->value, &(nodedata->y_value))) {
        return UNSUCCESSFUL;
      }
    }

    if (!strcmp(param->key, "nb_node_per_room")) {
      if (get_param_double(param->value, &(nodedata->nb_node_per_room))) {
        return UNSUCCESSFUL;
      }
    }

    if (!strcmp(param->key, "room_width")) {
      if (get_param_double(param->value, &(nodedata->room_width))) {
        return UNSUCCESSFUL;
      }
    }
    if (!strcmp(param->key, "room_length")) {
      if (get_param_double(param->value, &(nodedata->room_length))) {
        return UNSUCCESSFUL;
      }
    }

    /* reading the parameter related to the printing logs from the xml file */
    if (!strcmp(param->key, "log_status")) {
      if (get_param_integer(param->value, &(nodedata->log_status))) {
        return UNSUCCESSFUL;
      }
    }

  }



  return SUCCESSFUL;
}


/* ************************************************** */
/* ************************************************** */
int init(call_t *to, void *params) {
  return 0;
}

int destroy(call_t *to) {
  return 0;
}


/* ************************************************** */
/* ************************************************** */
int bind(call_t *to, void *params) {

  position_inside_room_mobility_node_private_t *nodedata = malloc(sizeof(position_inside_room_mobility_node_private_t));

  /* Set default values for node private data*/
  set_node_default_values(to,nodedata);

  /* Read values for node private data */
  read_nodedata_from_xml_file(to,nodedata,params);

  nodedata->room_id=ceil((to->object)/(nodedata->nb_node_per_room));
  double nb_room_per_line=((get_topology_area()->x)/nodedata->room_width);
  nodedata->min_X = (nodedata->room_id - nb_room_per_line*(ceil(nodedata->room_id/nb_room_per_line)-1)-1)*((get_topology_area()->x)/nb_room_per_line);
  nodedata->min_Y = (ceil(nodedata->room_id/nb_room_per_line)-1)*((get_topology_area()->y)/nb_room_per_line);
  nodedata->max_X = (nodedata->room_id - nb_room_per_line*(ceil(nodedata->room_id/nb_room_per_line)-1))*((get_topology_area()->x)/nb_room_per_line);
  nodedata->max_Y = ceil(nodedata->room_id/nb_room_per_line)*((get_topology_area()->y)/nb_room_per_line);

  nodedata->angle.xy = get_random_double_range(0,6.2830);
  nodedata->angle.z = get_random_double_range(0,6.2830);

  /* Define positions*/
  get_node_position(to->object)->x = nodedata->min_X + (nodedata->max_X - nodedata->min_X)*get_random_double();
  get_node_position(to->object)->y = nodedata->min_Y + (nodedata->max_Y - nodedata->min_Y)*get_random_double();
  get_node_position(to->object)->z = 0;

  if ((nodedata->y_value>0) && (nodedata->x_value>0)){
    get_node_position(to->object)->x =nodedata->x_value;
    get_node_position(to->object)->y = nodedata->y_value;
  }

  /* Assign the initialized local variables to the node */
  set_node_private_data(to, nodedata);

  fprintf(stderr,"{%lfs} [POSITION] node %d (%f ,%f, %f) \n",get_time()*0.000000001,to->object, get_node_position(to->object)->x, get_node_position(to->object)->y, get_node_position(to->object)->z);


  return SUCCESSFUL;

}

int unbind(call_t *to) {
  return 0;
}


/* ************************************************** */
/* ************************************************** */
int bootstrap(call_t *to) {

  return 0;
}

int ioctl(call_t *to, int option, void *in, void **out) {
  return 0;
}


/* ************************************************** */
/* ************************************************** */
void update_position(call_t *to, call_t *from) {
  return;
}

double get_speed(call_t *to) {

  return 0.0;
}


angle_t get_angle(call_t *to) {

  position_inside_room_mobility_node_private_t *nodedata = get_node_private_data(to);

  return nodedata->angle;

}


/* ************************************************** */
/* ************************************************** */
mobility_methods_t methods = {update_position,
    get_speed,
    get_angle};
