/**
 *  \file   static.c
 *  \brief  no mobility
 *  \author Guillaume Chelius
 *  \date   2007
 **/
#include <kernel/include/modelutils.h>
#include <libraries/wiplan/wiplan_parser.h>

/* ************************************************** */
/* ************************************************** */
model_t model =  {
    "No mobility",
    "Guillaume Chelius",
    "0.1",
    MODELTYPE_MOBILITY, 
};


/* ************************************************** */
/* ************************************************** */
struct classdata {
  FILE *file;
  xmlNodeSetPtr nodeset;
};


/* ************************************************** */
/* ************************************************** */
int init(call_t *to, void *params) {
  struct classdata *classdata = malloc(sizeof(struct classdata));
  param_t *param;
  char *filepath = NULL;

  /* default value */
  filepath = "wiplanout.xml";

  /* get parameters */
  list_init_traverse(params);
  while ((param = (param_t *) list_traverse(params)) != NULL) {
    if (!strcmp(param->key, "file")) {
      filepath = param->value;
    }
  }

  /* open file */
  classdata->nodeset = wiplan_parser_start(filepath);

  if (classdata->nodeset == NULL)
    goto error;

  set_class_private_data(to, classdata);
  return 0;

 error:
  fprintf(stderr, "static_wiplan: error when parsing %s\n", filepath);
  free(classdata);
  return -1;
}

int destroy(call_t *to) {
  wiplan_parser_close();

  return 0;
}


/* ************************************************** */
/* ************************************************** */
int bind(call_t *to, void *params) {
  int id, i, found = 0;

  struct classdata *classdata = get_class_private_data(to);
 
  for (i = 0 ; i < classdata->nodeset->nodeNr ; i++)
    {
      char *id_str;
      xmlNodePtr nd1 = classdata->nodeset->nodeTab[i];
      xmlNodePtr nd2;
      char *content;

      if ((id_str = wiplan_parser_get_xml_attr_content(nd1, XML_A_ID)) == NULL)
  	{
  	  fprintf(stderr, "static_wiplan: node %d not found (bind())\n", to->object);
  	  return -1;
  	}
      get_param_integer(id_str, &id);

      if (id == to->object)
  	{
  	  found = 1;
  	  for (nd2 = nd1->children; nd2; nd2 = nd2->next)
  	    {
  	      if (!strcmp((char *) nd2->name, XML_E_POSITION))
  		{
  		  if ((content = wiplan_parser_get_xml_attr_content(nd2, XML_A_X)) != NULL)
  		    {
  		      double x;
  		      get_param_double(content, &x);
  		      get_node_position(to->object)->x = x;
  		    }
  		  else
  		    {
  		      fprintf(stderr, "static_wiplan: node %d x position not found (bind())\n", to->object);
  		      return -1;
  		    }

  		  if ((content = wiplan_parser_get_xml_attr_content(nd2, XML_A_Y)) != NULL)
  		    {
  		      double y;
  		      get_param_double(content, &y);
  		      get_node_position(to->object)->y = y;
  		    }
  		  else
  		    {
  		      fprintf(stderr, "static_wiplan: node %d y position not found (bind())\n", to->object);
  		      return -1;
  		    }

  		  if ((content = wiplan_parser_get_xml_attr_content(nd2, XML_A_Z)) != NULL)
  		    {
  		      double z;
  		      get_param_double(content, &z);
  		      get_node_position(to->object)->z = z;
  		    }
  		  else
  		    {
  		      fprintf(stderr, "static_wiplan: node %d z position not found (bind())\n", to->object);
  		      return -1;
  		    }
		  DBG_MOBILITY("model static_wiplan.c: node %d position set to {%g, %g, %g}\n", to->object,
			       get_node_position(to->object)->x,
			       get_node_position(to->object)->y,
			       get_node_position(to->object)->z);
  		  break;
  		}
  	    }
  	  break;
  	}
    }

  if (found == 0)
    {
      fprintf(stderr, "static_wiplan: node %d position not found (bind())\n", to->object);
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

