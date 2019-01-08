/**
 *  \file   group.h
 *  \brief  Group declarations
 *  \author Luiz Henrique Suraty Filho
 *  \date   2016
 **/

#ifndef WSNET_KERNEL_DEFINITIONS_GROUP_H_
#define WSNET_KERNEL_DEFINITIONS_GROUP_H_

#include <kernel/include/definitions/types.h>

/* ************** Group Types for BAN **************** */
/* ************************************************** */
/* ************************************************** */
enum{
	GROUP_TYPE_STANDARD,
	GROUP_TYPE_BAN,
	GROUP_TYPE_15_4
};

/** \typedef group_t
 * \brief Definition of a group attributes
 **/
/** \struct _group
 * \brief Definition of a group attributes. Should use type group_t.
 **/
typedef struct _group {

	groupid_t     		id;

	grouparchid_t		grouparch;

	nodeid_t			leaderid;

	array_t 			membersid;

	int					type;

	int					size;

} group_t;


/** \typedef group_array_t
 * \brief An array of group containing its size.
 **/
/** \struct _group_array
 * \brief An array of group containing its size. Should use type group_array_t.
 **/
typedef struct _group_array {
  int size;
  group_t *elts;
} group_array_t;

/* Global variable for all group defined in the XML file*/
extern group_array_t groups;

#ifdef __cplusplus
extern "C"{
#endif

/**
 * \brief Give the address of a group from its id.
 * \param The id of a group.
 * \return A pointer on a group_t structure.
 **/
group_t *get_group_by_id(groupid_t _id);

/**
 * \brief Give the number of a group that has the same architecture.
 * \param The id of a group architecture.
 * \return The number of groups.
 **/
int	get_number_groups_by_grouparchid(grouparchid_t _group_arch);

/**
 * \brief Give an array groups that has the same architecture.
 * \param The id of a group architecture.
 * \return An array of all groups with the architecture.
 **/
group_array_t get_groups_by_grouparchid(grouparchid_t _grouparchid);

/**
 * \brief Create groups.
 * \return 0 in case of success, -1 else.
 **/
int groups_create(void);

/**
 * \brief Free groups memory space.
 **/
void groups_clean(void);

int calculate_default_number_of_groups(void);

int check_groups_size(void);

bool is_member_in_group(nodeid_t _memberid,groupid_t _id);

bool is_in_same_group(int _nbr_nodes, ...);

int	get_number_groups_by_type(int _group_type);

int get_group_number_nodes_with_nodetype(groupid_t _id, int _node_type);

array_t get_group_nodesid_by_nodetype(groupid_t _id, int _node_type);

array_t get_group_all_membersid(groupid_t _id);

nodeid_t get_group_leaderid(groupid_t _id);

#ifdef __cplusplus
}
#endif

#endif //WSNET_KERNEL_DEFINITIONS_GROUP_H_
