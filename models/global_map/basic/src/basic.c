/**
 *  \file   basic.c
 *  \brief  Basic global map model
 *  \author Loic Lemaitre
 *  \date   2010
 **/

#include <kernel/include/modelutils.h>
#include <models/models_dbg.h>


/* ************************************************** */
/* ************************************************** */
model_t model =  {
    "Basic global map model",
    "Loic Lemaitre",
    "0.1",
    MODELTYPE_GLOBAL_MAP
};


/* ************************************************** */
/* ************************************************** */
struct classdata {
  position_t *g_area;
};


/* ************************************************** */
/* ************************************************** */
int init(call_t *to, void *params) {
  param_t* param;
  struct classdata *classdata = malloc(sizeof(struct classdata));
  int param_x_ok = 0, param_y_ok = 0, param_z_ok = 0;

  classdata->g_area = malloc(sizeof(position_t));

  /* get parameters if set*/
  list_init_traverse(params);
  while ((param = (param_t *) list_traverse(params)) != NULL) {
    if (!strcmp(param->key, "x")) {
      if (param_x_ok) {
	fprintf(stderr, "basic_global_map.c: x parameter already set\n");
      }
      param_x_ok = 1; 
      if (get_param_x_position(param->value, &(classdata->g_area->x))) {
        free(classdata->g_area);
        free(classdata);
	return -1;
      }
    }
    if (!strcmp(param->key, "y")) {
     if (param_y_ok) {
	fprintf(stderr, "basic_global_map.c: y parameter already set\n");
      }
      param_y_ok = 1;  
      if (get_param_y_position(param->value, &(classdata->g_area->y))) {
        free(classdata->g_area);
        free(classdata);
	return -1;
      }
    }
    if (!strcmp(param->key, "z")) {
     if (param_z_ok) {
	fprintf(stderr, "basic_global_map.c: z parameter already set\n");
      }
      param_z_ok = 1; 
      if (get_param_z_position(param->value, &(classdata->g_area->z))) {
        free(classdata->g_area);
        free(classdata);
	return -1;
      }
    }
  }

  if (!param_x_ok || !param_y_ok || !param_z_ok) {
    free(classdata->g_area);
    free(classdata);
    fprintf(stderr, "basic_global_map.c: missing x or y or z parameter(s)\n");
    return -1;
  }

  /* set class private data */
  set_class_private_data(to, classdata);

  return 0;
}

int destroy(call_t *to) {
  struct classdata *classdata = get_class_private_data(to);
  free(classdata->g_area);
  free(classdata);
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


/* ************************************************** */
/* ************************************************** */
position_t *get_global_area(call_t *to, call_t *from) {
  struct classdata *classdata = get_class_private_data(to);
  return classdata->g_area;
}

global_map_methods_t methods = {get_global_area};
