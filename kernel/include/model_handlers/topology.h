/**
 *  \file   topology.h
 *  \brief  Topology declarations
 *  \author Guillaume Chelius
 *  \date   2007
 **/
#ifndef __topology__
#define __topology__

#include <kernel/include/definitions/types.h>


/* ************************************************** */
/* ************************************************** */
#ifdef __cplusplus
extern "C"{
#endif

int topology_init(void);
int topology_bootstrap(void);
void topology_clean(void);


/* ************************************************** */
/* ************************************************** */
/**
 * \brief Return the size of the network area.
 * \return The network area.
 **/
position_t *get_topology_area(void);
#ifdef __cplusplus
}
#endif

#endif //__topology__
