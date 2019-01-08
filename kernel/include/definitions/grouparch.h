/**
 *  \file   grouparch.h
 *  \brief  Group architecture declarations
 *  \author Luiz Henrique Suraty Filho
 *  \date   2016
 */

#ifndef WSNET_KERNEL_DEFINITIONS_GROUPARCH_H_
#define WSNET_KERNEL_DEFINITIONS_GROUPARCH_H_

#include <kernel/include/definitions/types.h>
#include <kernel/include/definitions/group.h>

/** \typedef memberarch_t
 * \brief A member architecture with its id and maximum number of nodes
 **/
/** \struct _memberarch
 * \brief A member architecture with its id and maximum number of nodes. Should use type memberarch_t.
 **/
typedef struct _memberarch {

	nodearchid_t		id;

	int					nbr_nodes;

} memberarch_t;

/** \typedef memberarch_array_t
 * \brief An array of member architectures containing its size.
 **/
/** \struct _memberarch_array
 * \brief An array of member architectures containing its size. Should use type memberarch_array_t.
 **/
typedef struct _memberarch_array {
  int size;
  memberarch_t *elts;
} memberarch_array_t;

/** \typedef grouparch_t
 * \brief Definition of a group architecture attributes
 **/
/** \struct _grouparch
 * \brief Definition of a group architecture attributes. Should use type grouparch_t.
 **/
typedef struct _grouparch {
	grouparchid_t		id;

	char 				*name;

	nodearchid_t		leader_nodearchid;

	memberarch_array_t	membersarch;

	int 				type;

	int					nbr_nodes;

} grouparch_t;


/** \typedef grouparch_array_t
 * \brief An array of group architectures containing its size.
 **/
/** \struct _grouparch_array
 * \brief An array of group architectures containing its size. Should use type grouparch_array_t.
 **/
typedef struct _grouparch_array {
  int size;
  grouparch_t *elts;
} grouparch_array_t;

/* Global variable for all group architectures loaded from the XML file*/
extern grouparch_array_t grouparchs;

/* ************************************************** */
/* ************************************************** */

#ifdef __cplusplus
extern "C"{
#endif

/**
 * \brief Give the id of a group architecture from its name.
 * \param The name of a group architecture.
 * \return The id of the group architecture.
 **/
grouparchid_t	get_grouparchid_by_name	(char *_name);

/**
 * \brief Give the address of a group architecture from its name.
 * \param The name of a group architecture.
 * \return A pointer on a grouparch_t structure.
 **/
grouparch_t 		*get_grouparch_by_name  	(char *_name);

/**
 * \brief Give the address of a group architecture from its id.
 * \param The id of a group architecture.
 * \return A pointer on a grouparch_t structure.
 **/
grouparch_t 		*get_grouparch_by_id    	(grouparchid_t _id);

/**
 * \brief Allocate memory for all group architectures
 * \return 0 in case of success, -1 else.
 **/
int grouparchs_malloc(void);

/**
 * \brief Initialize a group architecture with default values
 * \param The id of a group architecture.
 * \return Void.
 **/
void grouparch_init(grouparchid_t _id);

/**
 * \brief Free memory allocated for all group architectures
 * \return Void.
 **/
void        	grouparchs_clean       	(void);

#ifdef __cplusplus
}
#endif

#endif //WSNET_KERNEL_DEFINITIONS_GROUPARCH_H_
