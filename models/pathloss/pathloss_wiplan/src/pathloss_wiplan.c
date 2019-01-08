/**
 *  \file   pathloss_wiplan.c
 *  \brief  Wiplan pathloss model (read power from the wiplan out file)
 *  \author Loic Lemaitre
 *  \date   2010
 **/

#include <kernel/include/modelutils.h>
#include <libraries/wiplan/wiplan_parser.h>

/* ************************************************** */
/* ************************************************** */
model_t model =  {
    "None pathloss model",
    "Loic Lemaitre",
    "0.1",
    MODELTYPE_PATHLOSS
};


/* ************************************************** */
/* ************************************************** */
struct classdata {
  FILE *file;
  xmlNodeSetPtr nodeset;
  double *wiplan_rxdBm;
};


/* ************************************************** */
/* ************************************************** */
int wiplan_parse(struct classdata *classdata);


/* ************************************************** */
/* ************************************************** */
int init(call_t *to, void *params) {
  struct classdata *classdata = malloc(sizeof(struct classdata));
  param_t *param;
  char *filepath = NULL;
  int file_param_found = 0;

  DBG_PATHLOSS("model pathloss_wiplan.c: initializing class %s\n",
	     get_class_by_id(to->class)->name);

  /* default value */
  filepath = "wiplanout.xml";

  /* get parameters */
  list_init_traverse(params);
  while ((param = (param_t *) list_traverse(params)) != NULL) {
    if (!strcmp(param->key, "file")) {
      filepath = param->value;
      file_param_found = 1;
    }
  }

  if (!file_param_found) {
    fprintf(stderr, "pathloss_wiplan.c: error: medium paramater not found\n");
    goto error;
  }

  /* open file */
  classdata->nodeset = wiplan_parser_start(filepath);

  if (classdata->nodeset == NULL)
    goto error;

 /* save wiplan rx power for each node */
  if (wiplan_parse(classdata))
    goto error;

  set_class_private_data(to, classdata);
  return 0;

 error:
  fprintf(stderr, "pathloss_wiplan.c: error when parsing %s\n", filepath);
  free(classdata);
  return -1;
}

int destroy(call_t *to) {
  struct classdata *classdata = malloc(sizeof(struct classdata));
  free(classdata);

  return 0;
}

int bootstrap(call_t *to, void *params) {
  return 0;
}


/* ************************************************** */
/* ************************************************** */
int bind(call_t *to, void *params) {
  DBG_PATHLOSS("model pathloss_wiplan.c: binding class %s with medium %s\n",
	     get_class_by_id(to->class)->name, get_medium_by_id(to->object)->name);
  return 0;
}

int unbind(call_t *to) {
  return 0;
}


/* ************************************************** */
/* ************************************************** */
double pathloss(call_t *to_pathloss, call_t *to_interface ,call_t *from_interface, packet_t *packet, double rxdBm)
{
  struct classdata *classdata = get_class_private_data(to_pathloss);
  DBG_PATHLOSS("model pathloss_wiplan.c: dst %d rx %gdBm from %d\n",
	  dst, *(classdata->wiplan_rxdBm + to_interface->object * nodes.size + from_interface->object), from_interface->object);
  return *(classdata->wiplan_rxdBm + to_interface->object * nodes.size + from_interface->object);
}


/* ************************************************** */
/* ************************************************** */
int wiplan_parse(struct classdata *classdata)
{
  int i = 0;
  int check_nodes_ids = 0;

  /* allocate memory to save Wiplan rxdBm values */
  classdata->wiplan_rxdBm = malloc(sizeof(double) * nodes.size * nodes.size);
  /* memory management:
     |0 from 0|0 from 1|...|0 from x||1 from 0|1 from 1|..|1 from x|...|x from 0||x from 1|...|x from x| */

  /* Parse Wiplan config file */
  for (i = 0 ; i < classdata->nodeset->nodeNr ; i++)
    {
      char *id_str;
      xmlNodePtr nd1 = classdata->nodeset->nodeTab[i];
      xmlNodePtr nd2;
      char *content;
      int id;
 
      if ((id_str = wiplan_parser_get_xml_attr_content(nd1, XML_A_ID)) == NULL)
	{
	  fprintf(stderr, "pathloss_wiplan_wiplan.c: missing id attribute in a node element of wiplan file\n");
	  return -1;
	}
      get_param_integer(id_str, &id);

      if (id < nodes.size)
	{
	  check_nodes_ids += id;
	  for (nd2 = nd1->children; nd2; nd2 = nd2->next)
	    {
	      if (!strcmp((char *) nd2->name, XML_E_RXNODE))
		{
		  if ((content = wiplan_parser_get_xml_attr_content(nd2, XML_A_ID)) != NULL)
		    {
		      int rxid;
		      get_param_integer(content, &rxid);
		      if ((content = wiplan_parser_get_xml_attr_content(nd2, XML_A_DBM)) != NULL)
			{
			  double rxdBm;
			  get_param_double(content, &rxdBm);
			  *(classdata->wiplan_rxdBm + id * nodes.size + rxid)= rxdBm;
			  DBG_PATHLOSS("model pathloss_wiplan_wiplan.c: node %d rx from node %d set to %gdBm\n",
				       id, rxid, rxdBm);
			}
		      else
			{
			  fprintf(stderr, "pathloss_wiplan_wiplan.c: node %d rx from node %d, dBm not found\n",
				  id, rxid);
			  return -1;
			}
		    }
		  else
		    {
		      fprintf(stderr, "pathloss_wiplan_wiplan.c: node %d: id attribute not found\n", id);
		      return -1;
		    }
		}
	    }
	}
      else
	{
	  fprintf(stderr, "pathloss_wiplan_wiplan.c: node number (%d) exceeds number of nodes (%d)\n", id, nodes.size);
	  return -1;
	}
    }

  if ( ((2 * check_nodes_ids)/(nodes.size - 1)) != nodes.size ) /* 0+1+2+...+(x-1)=((x-1)*x)/2 */
    {
      fprintf(stderr, "pathloss_wiplan_wiplan.c: error on nodes ids of wiplan file, %d, %d\n", check_nodes_ids, nodes.size);
      return -1;
    }

  return 0;
}

/* ************************************************** */
/* ************************************************** */
pathloss_methods_t methods = {pathloss};
