/**
 *  \file   nodearch.h
 *  \brief  Node architecture declarations
 *  \author Loic Lemaitre & Luiz Henrique Suraty Filho
 *  \date   2016
 **/

#ifndef __nodearch__
#define __nodearch__

#include <kernel/include/definitions/types.h>
#include <kernel/include/definitions/node.h>

/** \typedef nodearch_t
 * \brief Definition of a node architecture attributes
 **/
/** \struct _nodearch
 * \brief Definition of a node architecture attributes. Should use type nodearch_t.
 **/
typedef struct _nodearch {
  nodearchid_t 			id;
  char        			*name;
  char        			*birth;
  node_type_array_t 	types;

  array_t      			classes;

  array_t      			transceivers;
  array_t      			macs;
  array_t      			routings;
  array_t      			applications;
  array_t      			interfaces;
  array_t           phys;
  array_t           errors;
  array_t           codings;
  array_t           interferences;
  array_t           signal_trackers;
  array_t           modulators;

  array_t     			*up;
  array_t     			*down;

  array_t      			sensors;
  array_t      			monitors;

  classid_t    			energy;
  classid_t    			mobility;

} nodearch_t;

/** \typedef nodearch_array_t
 * \brief An array of node architectures containing its size.
 **/
/** \struct _nodearch_array
 * \brief An array of node architectures containing its size. Should use type nodearch_array_t.
 **/
typedef struct _nodearch_array {
  int size;
  nodearch_t *elts;
} nodearch_array_t;

/* Global variable for all node architectures loaded from the XML file*/
extern nodearch_array_t nodearchs;


/* ************************************************** */
/* ************************************************** */

#ifdef __cplusplus
extern "C"{
#endif

/**
 * \brief Give the id of a node architecture from its name.
 * \param The name of a node architecture.
 * \return The id of the node architecture.
 **/
nodearchid_t	get_nodearchid_by_name	(char *name);

/**
 * \brief Give the address of a node architecture from its name.
 * \param The name of a node architecture.
 * \return A pointer on a nodearch_t structure.
 **/
nodearch_t 		*get_nodearch_by_name  	(char *name);

/**
 * \brief Give the address of a node architecture from its id.
 * \param The id of a node architecture.
 * \return A pointer on a nodearch_t structure.
 **/
nodearch_t 		*get_nodearch_by_id    	(nodearchid_t id);

/**
 * \brief Free memory allocated for all node architectures
 * \return Void.
 **/
void        	nodearchs_clean       	(void);

#ifdef __cplusplus
}
#endif

#endif // __nodearch__
