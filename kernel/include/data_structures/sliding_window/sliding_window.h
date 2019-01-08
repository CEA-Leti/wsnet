/**
 *  \file   sliding_window.h
 *  \brief  Sliding Window - Header File
 *  \author Luiz Henrique Suraty Filho (CEA-LETI)
 *  \date   September 2014
 *  \version 1.0
**/

#ifndef _SLIDING_WINDOW_H
#define	_SLIDING_WINDOW_H


/* ************************************************** */
/* ************************************************** */

#include <kernel/include/data_structures/list/list.h>

/* ************************************************** */
/* ************************************************** */

/* Macro definitions for the possible types. */
#define SLIDING_TYPE_INT	0
#define SLIDING_TYPE_DOUBLE	1


/* Macro definitions for managing returns of functions.*/
#define UNSUCCESSFUL   -1
#define SUCCESSFUL      0

/* ************************************************** */
/* ************************************************** */

/** \brief A structure for sliding window
 *  \struct _sliding_window
 **/
typedef struct _sliding_window{

	int 	max_size;
	list_t	*elts;
	int		data_type;

} sliding_window_t;


/* ************************************************** */
/* ************************************************** */
#ifdef __cplusplus
extern "C"{
#endif

void sliding_window_init(void);

int sliding_window_insert_data(sliding_window_t *sliding_window, void *data);

double sliding_window_calculate_mean(sliding_window_t *sliding_window);

int sliding_window_verify_consistency(sliding_window_t *sliding_window);

sliding_window_t *sliding_window_create(int max_size, int sliding_type);

void sliding_window_print_history(sliding_window_t *sliding_window);

int sliding_window_destroy(sliding_window_t *sliding_window);

#ifdef __cplusplus
}
#endif

#endif // _SLIDING_WINDOW_H
