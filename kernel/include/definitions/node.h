/**
 *  \file   node.h
 *  \brief  Node declarations
 *  \author Loic Lemaitre & Luiz Henrique Suraty Filho
 *  \date   2016
 **/

#ifndef __node__
#define __node__

#include <kernel/include/definitions/types.h>

/* ************************************************** */
/* ************************************************** */
#define NODE_UNDEF             0
#define NODE_DEAD              1
#define NODE_ACTIVE            2

#define NODE_WSNET             0
#define NODE_WSIM_DISCONNECTED 1
#define NODE_WSIM_CONNECTED    2

/* *************** Node FAMILIES ******************** */
/* ************************************************** */
/* ************************************************** */
enum{
  NODE_TYPE_FAMILY_BAN,
  NODE_TYPE_FAMILY_15_4,
  NODE_TYPE_FAMILY_WIFI
};
/* ************** Node Types for BAN **************** */
/* ************************************************** */
/* ************************************************** */
enum{
  NODE_TYPE_BAN_BELT,
  NODE_TYPE_BAN_HAND,
  NODE_TYPE_BAN_EAR
};

/* ************** Node Types for 15.4 *************** */
/* ************************************************** */
/* ************************************************** */
enum{
  NODE_TYPE_15_4_CFD,
  NODE_TYPE_15_4_FFD,
  NODE_TYPE_15_4_RFD
};

/* ************************************************** */
/* ************************************************** */
typedef struct _node_type{
	int	family;
	int	type;
} node_type_t;

typedef struct _node_type_array{
	int	size;
	node_type_t	*elts;
} node_type_array_t;

typedef struct _node {
  nodeid_t     			id;
  nodearchid_t 			nodearch;
  uint64_t     			birth;
  int          			state;
  position_t   			position;

#ifdef __cplusplus
  void       			**private_vars;
#else //__cplusplus
  void            **private;
#endif //__cplusplus

  void        			*noises;
  
  node_type_array_t 	types;
  
  array_t				groups;

} node_t;


typedef struct _node_array {
  int size;
  node_t *elts;
} node_array_t;

extern node_array_t nodes;

#ifdef __cplusplus
extern "C"{
#endif

/* ************************************************** */
/* ************************************************** */
/**
 * \brief Give the address of a node from its id.
 * \param The id of a node.
 * \return A pointer on a node_t structure.
 **/
node_t *get_node_by_id(nodeid_t id);

/** 
 * \brief Return a node's position. For read-only, the position should not be modified.
 * \param node the node id.
 * \return The node's position.
 **/
position_t *get_node_position(nodeid_t node);

/**
 * \brief Free nodes space.
 **/
void nodes_clean(void);

/**
 * \brief Update nodes positions.
 **/
void nodes_update_mobility(void);

/**
 * \brief Return the number of simulated nodes.
 * \return The number of nodes.
 **/
int get_node_count(void);

/**
 * \brief Bootstrap a node id.
 * \param The id of the node to bootstrap.
 **/
void node_birth(nodeid_t id);

/**
 * \brief Return whether a node is alive or not.
 * \return Return 1 if the node is alive, 0 else.
 **/
int is_node_alive(nodeid_t id);

/**
 * \brief Kill node.
 * \param The id of the node to be killed.
 **/
void node_kill(nodeid_t id);

/**
 * \brief Bootstrap nodes.
 * \return 0 in case of success, -1 else.
 **/
int nodes_bootstrap(void);

/**
 * \brief Create nodes.
 * \return 0 in case of success, -1 else.
 **/
int nodes_create(void);

/**
 * \brief Node module init.
 * \return 0 in case of success, -1 else.
 **/
int nodes_init(void);

/**
 * \brief Unbind nodes.
 * \return 0 in case of success, -1 else.
 **/
int nodes_unbind(void);

/**
 * \brief Get private memory of the node.
 * \param c, the called node, according the called class.
 * \return Return a void pointer on the private node memory.
 **/
void *get_node_private_data(call_t *to);

/**
 * \brief Set private memory of the node.
 * \param c, the called node, according the called class.
 * \param data, a pointer on the memory to attach to the node.
 **/
void set_node_private_data(call_t *to, void *data);

/**
 * \brief Return the distance between two positions.
 * \param position0 a position_t pointer to the first position.
 * \param position1 a position_t pointer to the second position.
 * \return Return a double corresponding to the distance between the two positions.
 **/
double distance(position_t *position0, position_t *position1);

void print_node_groups(nodeid_t _nodeid);

array_t get_node_groups(nodeid_t _nodeid);

int add_group_to_node(nodeid_t _nodeid, groupid_t _groupid);

int get_node_type_by_family(nodeid_t id, int family);

node_type_t *get_node_types(nodeid_t id);

int	get_node_types_size(nodeid_t id);

bool check_node_type(nodeid_t id, int type);

array_t get_nodesid_by_nodearchid(nodearchid_t _nodearchid);

#ifdef __cplusplus
}
#endif


#endif //__node__
