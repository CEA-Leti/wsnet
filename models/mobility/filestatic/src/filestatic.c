/**
 *  \file   filestatic.c
 *  \brief  No mobility and position from file
 *  \author Guillaume Chelius
 *  \date   2007
 **/
#include <kernel/include/modelutils.h>


/* ************************************************** */
/* ************************************************** */
model_t model =  {
  "Position extracted from file",
  "Guillaume Chelius",
  "0.1",
  MODELTYPE_MOBILITY
};


/* ************************************************** */
/* ************************************************** */
struct classdata {
  FILE *file;
};


/* ************************************************** */
/* ************************************************** */
int init(call_t *to, void *params) {
  struct classdata *classdata = malloc(sizeof(struct classdata));
  param_t *param;
  char *filepath = NULL;

  /* default values */
  filepath = "mobility.data";

  /* get parameters */
  list_init_traverse(params);
  while ((param = (param_t *) list_traverse(params)) != NULL) {
    if (!strcmp(param->key, "file")) {
      filepath = param->value;
    }
  }

  /* open file */
  if ((classdata->file = fopen(filepath, "r")) == NULL) {
    fprintf(stderr, "filestatic: can not open file %s in init()\n", filepath);
    goto error;
  }

  set_class_private_data(to, classdata);
  return 0;

 error:
  free(classdata);
  return -1;
}

int destroy(call_t *to) {
  struct classdata *classdata = get_class_private_data(to);

  if (classdata->file != NULL) {
    fclose(classdata->file);
  }

  free(classdata);
  return 0;
}


/* ************************************************** */
/* ************************************************** */
int bind(call_t *to, void *params) {
  int id, found = 0;
  double x, y, z;
  struct classdata *classdata = get_class_private_data(to);
  char str[128];

  /* position at the beginning */
  fseek(classdata->file, 0L, SEEK_SET);
  while (fgets(str, 128, classdata->file) != NULL) {
    sscanf(str, "%d %lf %lf %lf\n",  &id, &x, &y, &z);
    if (id == to->object) {
      found = 1;
      get_node_position(to->object)->x = x;
      get_node_position(to->object)->y = y;
      get_node_position(to->object)->z = z;
      break; 
    }
  }

  if (found == 0) {
    fprintf(stderr, "filestatic: node %d position not found (bind())\n", to->object);
    return -1;       
  }

  return 0;
}

int unbind(call_t *to) {
  return 0;
}


/* ************************************************** */
/* ************************************************** */
int bootstrap(call_t *to) {
  PRINT_REPLAY("mobility %"PRId64" %d %lf %lf %lf\n", get_time(), to->object, 
	       get_node_position(to->object)->x, get_node_position(to->object)->y, 
	       get_node_position(to->object)->z);
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

angle_t angle;
angle.xy=0.0;
angle.z=0.0;

return angle;

}

/* ************************************************** */
/* ************************************************** */
mobility_methods_t methods = {update_position,
							  get_speed,
							  get_angle};
