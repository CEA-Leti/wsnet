/*
 *  \file   indoor_map.c
 *  \brief  indoor map model
 *  \author Luiz Henrique Suraty Filho
 *  \date   2016
 */
#include <kernel/include/modelutils.h>
#include <models/models_dbg.h>

// ************************************************** //
// ************************************************** //

// Defining module informations//
model_t model =  {
    "Indoor map model",
    "Luiz Henrique Suraty Filho",
    "0.3",
    MODELTYPE_MAP
};

/*Modified by Luiz Henrique Suraty Filho*/
/* ************************************************** */
/* ************************************************** */

typedef struct _room{
	int	room_id_x;
	int room_id_y;
	int room_id;
} room_t;

/*End of modification by Luiz Henrique Suraty Filho*/


/* ************************************************** */
/* ************************************************** */
struct classdata {
  int room_nbr;                       /*!< Total number of rooms or subareas (indoor). */
  double room_width;                  /*!< The room or subarea width (indoor). */
  double room_length;                 /*!< The room or subarea length (indoor). */
  int room_nbr_x;                  /*!< The number of rooms or subareas in the x-axis (indoor). */
  int room_nbr_y;                  /*!< The number of rooms or subareas in the y-axis (indoor). */
  
};


/* ************************************************** */
/* ************************************************** */
int init(call_t *to, void *params) {
  param_t* param;
  struct classdata *classdata = malloc(sizeof(struct classdata));


  /* default values for the environment modeling (only one room => by default outdoor environment) */
  classdata->room_width  = get_topology_area()->x;
  classdata->room_length = get_topology_area()->y;
  classdata->room_nbr_x  = ceil(get_topology_area()->x/classdata->room_width);
  classdata->room_nbr_y  = ceil(get_topology_area()->y/classdata->room_length);
  classdata->room_nbr    = classdata->room_nbr_x * classdata->room_nbr_y;
  
  //printf("\n topo_x=%f topo_y=%f room_wid =%f room_len = %f nr_room_x = %d nr_room_y = %d nb_room_total = %d", get_topology_area()->x, get_topology_area()->y, classdata->room_width, classdata->room_length, classdata->room_nbr_x, classdata->room_nbr_y,  classdata->room_nbr);
  /* get parameters if set*/
  list_init_traverse(params);
  while ((param = (param_t *) list_traverse(params)) != NULL) {
    	
	   /* reading the parameters related to the environment from the xml file */
    if (!strcmp(param->key, "room_width")) {
      if (get_param_double(param->value, &(classdata->room_width))) {
	goto error;
      }
    }
    if (!strcmp(param->key, "room_length")) {
      if (get_param_double(param->value, &(classdata->room_length))) {
	goto error;
      }
    }
	
  }

 /* compute the number of rooms */
  classdata->room_nbr_x = ceil(get_topology_area()->x/classdata->room_width);
  classdata->room_nbr_y = ceil(get_topology_area()->y/classdata->room_length);
  classdata->room_nbr = classdata->room_nbr_x * classdata->room_nbr_y;
  

  //printf("\n room_wid =%f room_len = %f [nr_room_x = %d nr_room_y = %d nb_room_total = %d]", classdata->room_width, classdata->room_length, classdata->room_nbr_x, classdata->room_nbr_y,  classdata->room_nbr);
  /* set class private data */
  set_class_private_data(to, classdata);

  return 0;
  
  error:
  free(classdata);
  return -1;
  
}

int destroy(call_t *to) {
  struct classdata *classdata = get_class_private_data(to);
  free(classdata);
  return 0;
}

int bootstrap(call_t *to, void *params) {
  struct classdata *classdata = get_class_private_data(to);
  fprintf(stderr,"\n[MAP] size_x=%g size_y=%g room_wid = %g room_len = %g [nr_room_x = %d nr_room_y = %d nb_room_total = %d]\n", get_topology_area()->x, get_topology_area()->y, classdata->room_width, classdata->room_length, classdata->room_nbr_x, classdata->room_nbr_y,  classdata->room_nbr);
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

/** \brief A function that verify if two rooms are on adjacent rooms
 *  \fn int is_adjacent_room(call_t *to, room_t src, room_t dst)
 *  \param to is a pointer to the called class
 *  \param src is the room information of the source node
 *  \param dst is the room information of the destination node
 *  \return 1 if they are on adjacent rooms, 0 otherwise
 **/
int nbr_walls(room_t src, room_t dst){
	// we calculate the "distance" between rooms on the x and y axis
	// the number of walls will be equal to the distance, as they are squares
	int nbr_walls_x = fabs(src.room_id_x - dst.room_id_x);
	int nbr_walls_y = fabs(src.room_id_y - dst.room_id_y);

	// the total number of walls is the max between both
	return fmax(nbr_walls_x,nbr_walls_y);
}

/** \brief A function that computes the room id of the node
 *  \fn room_t get_node_room_info(call_t *to, nodeid_t node_id)
 *  \param to is a pointer to the called class
 *  \param node_is is the ID of the node
 *  \return the room information in a variable of type room_t
 **/
room_t get_node_room_info(call_t *to, nodeid_t node_id){

	struct classdata *classdata = get_class_private_data(to);

	room_t room;

	room.room_id_x = fmin(floor((get_node_position(node_id))->x/classdata->room_width) + 1, classdata->room_nbr_x);
	room.room_id_y = fmin(floor(get_node_position(node_id)->y/classdata->room_length) + 1, classdata->room_nbr_y);
	room.room_id = room.room_id_x + classdata->room_nbr_x * (room.room_id_y - 1);

	return room;

}

/** \brief A function that computes the radio link condition (LOS, NLOS or NLOS2) according to the environment modeling.
 *  \fn int get_radio_link_condition(struct classdata *classdata, nodeid_t src, nodeid_t dst)
 *  \param classdata is a pointer to the entity global variables
 *  \param src is the ID of the transmitter
 *  \param dst is the ID of the receiver
 *  \return the radio link conditions: "SAMEROOM, OTHERROOM, or CORRIDOR"
 **/
map_conditions_t get_scenario_condition(call_t *to, call_t *from, nodeid_t src, nodeid_t dst) {

  room_t src_room = get_node_room_info(to,src);
  room_t dst_room = get_node_room_info(to,dst);

  switch (nbr_walls(src_room,dst_room)){
    case 0:
      return MAP_SAME_ROOM;
    case 1:
      return MAP_ADJACENT_ROOM;
    case 2:
      return MAP_OTHER_ROOM;
    case 3:
      return MAP_FAR_AWAY_ROOM;
    default:
      return MAP_FAR_AWAY_ROOM;
  }

}
/** \brief A function that gives the number of walls separating src and dst
 *  \fn int get_nbr_walls(call_t *to, call_t *from, nodeid_t src, nodeid_t dst)
 *  \param classdata is a pointer to the entity global variables
 *  \param src is the ID of the transmitter
 *  \param dst is the ID of the receiver
 *  \return the number of walls separating src and dst
 **/
int get_nbr_walls(call_t *to, call_t *from, nodeid_t src, nodeid_t dst) {

  room_t src_room = get_node_room_info(to,src);
  room_t dst_room = get_node_room_info(to,dst);

  return nbr_walls(src_room,dst_room);
}

map_methods_t methods = {get_scenario_condition,
                         get_nbr_walls};
