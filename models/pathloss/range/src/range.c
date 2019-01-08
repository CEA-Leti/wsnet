/**
 *  \file   range.c
 *  \brief  Range propagation model
 *  \author Guillaume Chelius
 *  \date   2007
 **/
#include <kernel/include/modelutils.h>


/* ************************************************** */
/* ************************************************** */
model_t model =  {
  "Range propagation",
  "Guillaume Chelius",
  "0.1",
  MODELTYPE_PATHLOSS
};


/* ************************************************** */
/* ************************************************** */
struct classdata {
  double range;
};


/* ************************************************** */
/* ************************************************** */
int init(call_t *to, void *params) {
  struct classdata *classdata = malloc(sizeof(struct classdata));
  param_t *param;
    
  /* default values */
  classdata->range = 10;

  /* get parameters */
  list_init_traverse(params);
  while ((param = (param_t *) list_traverse(params)) != NULL) {
    if (!strcmp(param->key, "range")) {
      if (get_param_distance(param->value, &(classdata->range))) {
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
int bootstrap(call_t *to, void *params) {
  return 0;
}


/* ************************************************** */
/* ************************************************** */
int bind(call_t *to, void *params) {
  DBG_PATHLOSS("model range.c: binding class %s with medium %s\n",
	     get_class_by_id(to->class)->name, get_medium_by_id(to->object)->name);
  return 0;
}

int unbind(call_t *to) {
  return 0;
}


/* ************************************************** */
/* ************************************************** */
double pathloss(call_t *to_pathloss, call_t *to_interface ,call_t *from_interface, packet_t *packet, double rxdBm){
  struct classdata *classdata = get_class_private_data(to_pathloss);
  double dist = distance(get_node_position(from_interface->object), get_node_position(to_interface->object));

  if (dist > classdata->range) {
    return MIN_DBM;
  } else {
    return rxdBm;
  }
}


/* ************************************************** */
/* ************************************************** */
pathloss_methods_t methods = {pathloss};
