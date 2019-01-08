/**
 *  \file   class.h
 *  \brief  Parameter parsing declarations
 *  \author Guillaume Chelius & Elyes Ben Hamida
 *  \date   2007
 **/
#ifndef __param_public__
#define __param_public__

#include <kernel/include/definitions/types.h>

#ifdef __cplusplus
extern "C"{
#endif
/**
 * \brief Parse a x axis position parameter. Accept "random" parameter value.
 * \param value the parameter value.
 * \param position a pointer to a double that will be filled.
 * \return 0 upon success, -1 otherwise.
 **/
int get_param_x_position(char *value, double *position);


/**
 * \brief Parse a y axis position parameter. Accept "random" parameter value.
 * \param value the parameter value.
 * \param position a pointer to a double that will be filled.
 * \return 0 upon success, -1 otherwise.
 **/
int get_param_y_position(char *value, double *position);


/**
 * \brief Parse a z axis position parameter. Accept "random" parameter value.
 * \param value the parameter value.
 * \param position a pointer to a double that will be filled.
 * \return 0 upon success, -1 otherwise.
 **/
int get_param_z_position(char *value, double *position);


/**
 * \brief Parse a distance parameter. Accept "random" parameter value.
 * \param value the parameter value.
 * \param distance a pointer to a double that will be filled.
 * \return 0 upon success, -1 otherwise.
 **/
int get_param_distance(char *value, double *distance);


/**
 * \brief Parse a double parameter. Accept "random" parameter value.
 * \param value the parameter value.
 * \param res a pointer to a double that will be filled.
 * \return 0 upon success, -1 otherwise.
 **/
int get_param_double(char *value, double *res);


/**
 * \brief Parse a double parameter. Accept "random" parameter value. The parsed value is checked to be in [min,max].
 * \param value the parameter value.
 * \param res a pointer to a double that will be filled.
 * \param min the minimum accepted value.
 * \param max the maximum accepted value.
 * \return 0 upon success, -1 otherwise.
 **/
int get_param_double_range(char *value, double *res, double min, double max);


/**
 * \brief Parse a time parameter. Accept "random" parameter value.
 * \param value the parameter value.
 * \param time a pointer to a uin64 that will be filled.
 * \return 0 upon success, -1 otherwise.
 **/
int get_param_time(char *value, uint64_t *time);


/**
 * \brief Parse an integer parameter. Accept "random" parameter value.
 * \param value the parameter value.
 * \param integer a pointer to an integer that will be filled.
 * \return 0 upon success, -1 otherwise.
 **/
int get_param_integer(char *value, int *integer);


/*edit to support uint64 input (Arturo Guizar */
/**
 * \brief Parse an uint64 parameter. Accept "random" parameter value.
 * \param value the parameter value.
 * \param u_integer_64 a pointer to an uint64 integer that will be filled.
 * \return 0 upon success, -1 otherwise.
 **/
int get_param_uint64_integer(char *value, uint64_t *u_integer_64) ;
/*end of edition*/

/**
 * \brief Parse an integer parameter. Accept "random" parameter value.
 * \param value the parameter value.
 * \param u_integer a pointer to an unsigned intege that will be filled.
 * \return 0 upon success, -1 otherwise.
 **/
int get_param_unsigned_integer(char *value, unsigned int *u_integer);


/**
 * \brief Parse a node id parameter. Accept "random" parameter value. The returned param value will be different than "myself". 
 * \param value the parameter value.
 * \param node a pointer to a node id that will be filled.
 * \param myself a node id to exclude.
 * \return 0 upon success, -1 otherwise.
 **/
int get_param_nodeid(char *value, nodeid_t *node, nodeid_t myself);


/**
 * \brief Parse an class parameter.
 * \param value the parameter value.
 * \param class a pointer to an class id that will be filled.
 * \return 0 upon success, -1 otherwise.
 **/
int get_param_class(char *value, classid_t *classid);


#ifdef __cplusplus
}
#endif

#endif //__param_public__
