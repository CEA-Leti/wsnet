/**
 *  \file   position_inside_room_mobility.h
 *  \brief  Position Inside Room mobility - Header File
 *  \author Luiz Henrique Suraty Filho
 *  \date   2015
 **/
#pragma once


/** \brief A structure containing the LOCAL node variables and parameters
 *  \struct _random_waypoint_mobility_node_private
 **/
typedef struct _position_inside_room_mobility_node_private{



	/* Position Parameters */
	double          max_X;            			/*!< Defines the maximum X coordinate */
	double        	max_Y;     					/*!< Defines the maximum Y coordinate  */
	double          min_X;            			/*!< Defines the minimum X coordinate */
	double        	min_Y;     					/*!< Defines the minimum Y coordinate  */

  angle_t     angle;            /*!< Defines the angle of the node*/


	double			y_value;
	double			x_value;

	double 			nb_node_per_room;
	double 			room_id;
	double 			room_width;
	double			room_length;

    /* Variable for printing*/
    int    			log_status;


} position_inside_room_mobility_node_private_t;

