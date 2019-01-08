/**
 *  \file   node.c
 *  \brief  Node support 
 *  \author Loic Lemaitre & Luiz Henrique Suraty Filho
 *  \date   2016
 **/

#include <stdio.h>
#include <stdbool.h>

#include <kernel/include/definitions/nodearch.h>
#include <kernel/include/definitions/class.h>
#include <kernel/include/scheduler/scheduler.h>
#include <kernel/include/model_handlers/monitor.h>

#include "node.h"


node_array_t nodes = {0, NULL};


/* ************************************************** */
/* ************************************************** */

node_t *get_node_by_id(nodeid_t id) {
  return nodes.elts + id;
}

int	get_number_nodes_by_archid(nodearchid_t _node_arch){
  int nbr = 0;
  int i = nodes.size;
  while (i--){
    if (get_node_by_id(i)->nodearch == _node_arch)
      nbr++;
  }

  return nbr;
}

node_array_t get_nodes_by_nodearchid(nodearchid_t _nodearchid) {
  node_array_t nodes_by_arch = {0,NULL};

  nodes_by_arch.size = get_number_nodes_by_archid(_nodearchid);

  if (nodes_by_arch.size == 0)
    return nodes_by_arch;

  nodes_by_arch.elts = (node_t *) malloc(sizeof(node_t) * nodes_by_arch.size);

  int k = 0;
  int i;
  for (i=0;i<nodes.size;i++){
    if (get_node_by_id(i)->nodearch == _nodearchid){
      nodes_by_arch.elts[k]=nodes.elts[i];
      k++;
    }
  }
  return nodes_by_arch;

}

array_t get_nodesid_by_nodearchid(nodearchid_t _nodearchid) {
  array_t nodes_by_arch = {0,NULL};

  nodes_by_arch.size = get_number_nodes_by_archid(_nodearchid);

  //fprintf(stderr, "\nArch %s has %d nodes -> [ ", get_nodearch_by_id(_nodearchid)->name,nodes_by_arch.size );

  if (nodes_by_arch.size == 0)
    return nodes_by_arch;

  nodes_by_arch.elts = (int *) malloc(sizeof(int) * nodes_by_arch.size);

  int k = 0;
  int i;
  for (i=0;i<nodes.size;i++){
    if (get_node_by_id(i)->nodearch == _nodearchid){
      nodes_by_arch.elts[k]=nodes.elts[i].id;
      //fprintf(stderr, "%d ",nodes_by_arch.elts[k]);
      k++;
    }
  }
  //fprintf(stderr, "]\n");

  return nodes_by_arch;

}


void print_node_groups(nodeid_t _nodeid){
  array_t group = get_node_groups(_nodeid);

  int i;
  fprintf(stderr, "Groups that Node %d belong are : [ ",_nodeid);
  for (i=0;i<group.size;i++)
    fprintf(stderr, "%d ",group.elts[i]);
  fprintf(stderr, "]\n");
}
array_t get_node_groups(nodeid_t _nodeid){
  return get_node_by_id(_nodeid)->groups;
}

int add_group_to_node(nodeid_t _nodeid, groupid_t _groupid){
  node_t *node = get_node_by_id(_nodeid);

  //realloc the group size
  node->groups.size++;
  if ( (node->groups.elts = (int*) realloc(node->groups.elts, sizeof(int) * (node->groups.size)) ) == NULL )
    return -1;
  node->groups.elts[node->groups.size-1] = _groupid;

  return 0;
}


/* ************************************************** */
/* ************************************************** */
void nodes_clean(void) {

  if (nodes.elts && nodes.size) {
    int i;
    for (i=0;i<nodes.size;i++){
      //if ( (nodes.elts+i)->types.elts )
      //free((nodes.elts+i)->types.elts);

      if ( (nodes.elts+i)->groups.elts )
        free((nodes.elts+i)->groups.elts);

      if ( (nodes.elts+i)->private )
        free((nodes.elts+i)->private);
    }

    free(nodes.elts);
  }
}


/* ************************************************** */
/* ************************************************** */
void nodes_update_mobility(void) {
  int i;

  for (i = 0; i < nodes.size; i++) {
    node_t     *node     = get_node_by_id(i);
    nodearch_t *nodearch = get_nodearch_by_id(node->nodearch);
    class_t    *class    = get_class_by_id(nodearch->mobility);
    call_t      to       = {class->id, node->id};

    if ((node->state == NODE_DEAD) || (node->state == NODE_UNDEF)) {
      continue;
    }


    class->methods->mobility.update_position(&to, NULL);
  }
}


/* ************************************************** */
/* ************************************************** */
int get_node_count(void) {
  return nodes.size;
}


/* ************************************************** */
/* ************************************************** */
void node_birth(nodeid_t id) {
  node_t *node = get_node_by_id(id);
  int i;

  if (node->state != NODE_UNDEF) {
    return;
  }

  monitor_fire_signal(SIG_NODE_BIRTH, &id);

  /* bootstrap */
  for (i = 0; i < nodearchs.elts[node->nodearch].classes.size; i++) {
    class_t *class = get_class_by_id(nodearchs.elts[node->nodearch].classes.elts[i]);
    call_t to = {class->id, id};
    if ((class)->bootstrap) {
      (class)->bootstrap(&to);
    }
  }

  /* set node active */
  node->state = NODE_ACTIVE;
}


/* ************************************************** */
/* ************************************************** */
int is_node_alive(nodeid_t id) {
  node_t *node = get_node_by_id(id);
  return ((node->state != NODE_DEAD) && (node->state != NODE_UNDEF));
}


/* ************************************************** */
/* ************************************************** */
void node_kill(nodeid_t id) {
  node_t *node = get_node_by_id(id);
  //int i;

  if (node->state == NODE_DEAD) {
    return;
  }

  node->state = NODE_DEAD;

  //PRINT_REPLAY("kill %"PRId64" %d %lf %lf %lf\n", get_time(), id, node->position.x, node->position.y, node->position.z);

  /* unbind node */
  // Does not unbind once we can recharge the node
  /*for (i = 0; i < nodearchs.elts[node->nodearch].classes.size; i++) {
    class_t *class = get_class_by_id(nodearchs.elts[node->nodearch].classes.elts[i]);
    call_t to = {class->id, id};
    if ((class)->unbind) {
      (class)->unbind(&to);
    }
  }*/
}


/* ************************************************** */
/* ************************************************** */
int nodes_bootstrap(void) {
  int i;

  for (i = 0; i < nodearchs.size; i++) {
    nodearch_t *nodearch = get_nodearch_by_id(i);
    class_t *class;
    //int j;

    /* check mobility entity */
    class = get_class_by_id(nodearch->mobility);
    if (class->model->type != MODELTYPE_MOBILITY) {
      fprintf(stderr, "class %s is not a mobility model\n", class->name);
      return -1;
    }
  }

  /*  set node birth */
  for (i = 0; i < nodes.size; i++) {
    node_t *node = get_node_by_id(i);
    scheduler_add_birth(node->birth, node->id);
  }

  return 0;
}


/* ************************************************** */
/* ************************************************** */
int nodes_create(void)
{
  int i = nodes.size;

  nodes.elts = (node_t *) malloc(sizeof(node_t) * nodes.size);

  while (i--)
  {
    node_t *node    	= get_node_by_id(i);
    node->id        	= i;
    node->nodearch  	= -1;
    node->state     	= NODE_UNDEF;
    node->birth     	= 0;
    node->types.size 	= 0;
    node->types.elts	= NULL;
    node->position.x  = 0;
    node->position.y  = 0;
    node->position.z  = 0;
    node->groups.size = 0;
    node->groups.elts = NULL;
  }

  return 0;
}


/* ************************************************** */
/* ************************************************** */
int nodes_init(void) {
  return 0;
}


/* ************************************************** */
/* ************************************************** */
int nodes_unbind(void)
{
  int i = get_node_count();
  while (i--)
  {
    int j;
    node_t *node = get_node_by_id(i);
    if ((node->state == NODE_DEAD) || (node->state == NODE_UNDEF))
    {
      continue;
    }
    for (j = 0; j < nodearchs.elts[node->nodearch].classes.size; j++)
    {
      class_t *class = get_class_by_id(nodearchs.elts[node->nodearch].classes.elts[j]);
      call_t to = {class->id, node->id};
      if ((class)->unbind)
      {
        (class)->unbind(&to);
      }
    }
  }
  return 0;
}


/* ************************************************** */
/* ************************************************** */
void *get_node_private_data(call_t *to) {
  node_t  *node  = get_node_by_id (to->object);
  class_t *class = get_class_by_id(to->class);

  return node->private[class->nodearchs.elts[node->nodearch]];
}

void set_node_private_data(call_t *to, void *data) {
  node_t  *node  = get_node_by_id (to->object);
  class_t *class = get_class_by_id(to->class);

  node->private[class->nodearchs.elts[node->nodearch]] = data;
}


/* ************************************************** */
/* ************************************************** */
position_t *get_node_position(nodeid_t id) {
  node_t *node = get_node_by_id(id);
  return &(node->position);
}


double distance(position_t *position0, position_t *position1) {
  double x = position0->x - position1->x;
  double y = position0->y - position1->y;
  double z = position0->z - position1->z;

  return sqrt(x*x + y*y + z*z);
}

/* ************************************************** */
/* ************************************************** */

node_type_t *get_node_types(nodeid_t id){
  node_t  *node  = get_node_by_id (id);
  if (node->types.size == 0)
    return NULL;
  return node->types.elts;
}

int	get_node_types_size(nodeid_t id){
  return  get_node_by_id(id)->types.size;
}

int get_node_type_by_family(nodeid_t id, int family){
  node_t  *node  = get_node_by_id (id);
  int i;
  for (i=0;i < node->types.size; i++)
    if (node->types.elts[i].family == family)
      return node->types.elts[i].type;
  return -1;
}

bool check_node_type(nodeid_t id, int type){
  node_t  *node  = get_node_by_id (id);
  int i;
  for (i=0;i < node->types.size; i++)
    if (node->types.elts[i].type == type)
      return true;
  return false;
}


