/**
 *  \file   mobility.c
 *  \brief  Mobility module : event-driven
 *  \author Guillaume Chelius
 *  \date   2007
 **/
#include <stdlib.h>
#include <stdio.h>

#include <kernel/include/definitions/types.h>
#include <kernel/include/definitions/node.h>
#include <kernel/include/model_handlers/node_mobility.h>
#include <kernel/include/definitions/class.h>

/* ************************************************** */
/* ************************************************** */
static uint64_t l_update = 0;


/* ************************************************** */
/* ************************************************** */
int mobility_init(void) {
    return 0;
}

void mobility_clean(void) {
    return;
}


/* ************************************************** */
/* ************************************************** */
int mobility_bootstrap(void) {
    return 0;
}


/* ************************************************** */
/* ************************************************** */
void mobility_update(uint64_t clock) {
    if (clock - l_update < MOBILITY_UPDATE) {
        return;
    }

    nodes_update_mobility();
    l_update = clock;
}

void mobility_event(uint64_t clock) {
    if (l_update == clock) {
        return;
    }

    nodes_update_mobility();
    l_update = clock;
}

/* ************************************************** */
/* ************************************************** */
angle_t mobility_get_angle(call_t *to){
	  class_t *class = get_class_by_id(to->class);
	  if (class->model->type != MODELTYPE_MOBILITY){
		  angle_t new_angle ={2*3.1415,2*3.1415};
		  return new_angle;
	  }
	  return class->methods->mobility.get_angle(to);
}

double mobility_get_speed(call_t *to){
	  class_t *class = get_class_by_id(to->class);
	  if (class->model->type != MODELTYPE_MOBILITY)
	    return 0.0;
	  return class->methods->mobility.get_speed(to);
}

double mobility_get_mutual_orientation(call_t *mobility_src, call_t *mobility_dst){

	double distX,distY,dist;
	double alpha1, alpha2;
	angle_t angle_BAN1,angle_BAN2;

	angle_BAN1 = mobility_get_angle(mobility_src);
	angle_BAN2 = mobility_get_angle(mobility_dst);

	/* Calculate the distance between new waypoint and current position*/
	distX = (get_node_position(mobility_dst->object)->x - get_node_position(mobility_src->object)->x);
	distY = (get_node_position(mobility_dst->object)->y - get_node_position(mobility_src->object)->y);
	dist = sqrt(pow(distX,2)+pow(distY,2));

	alpha1=acos(distX/dist*cos(angle_BAN1.xy)+distY/dist*sin(angle_BAN1.xy))*180/M_PI;

	alpha2=acos(-distX/dist*cos(angle_BAN2.xy)-distY/dist*sin(angle_BAN2.xy))*180/M_PI;

	return fmax(alpha1,alpha2);
}
