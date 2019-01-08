/**
 *  \file   sliding_window.c
 *  \brief  Sliding Window - Source Code File
 *  \author Luiz Henrique Suraty Filho (CEA-LETI)
 *  \date   September 2014
 *  \version 1.0
**/

#include <stdio.h>
#include <stdlib.h>
#include "sliding_window.h"



void sliding_window_init(void) {
  return;
}



int sliding_window_insert_data(sliding_window_t *sliding_window, void *data){

	if (sliding_window==NULL)
		return UNSUCCESSFUL;

	// if it list has already reached the maximum size of the sliding window
	if (list_getsize(sliding_window->elts) == sliding_window->max_size)
		// delete first element
		free(list_pop_FIFO(sliding_window->elts));

	// insert a new one
	list_insert(sliding_window->elts, data);

	return SUCCESSFUL;
}

double sliding_window_calculate_mean(sliding_window_t *sliding_window){

	double mean = 0;

	if (list_getsize(sliding_window->elts) == 0)
		return -1;

	switch (sliding_window->data_type){

		case SLIDING_TYPE_INT:{

			int *value;

			list_init_traverse(sliding_window->elts);
			while ( (value = (int *) list_traverse(sliding_window->elts)) != NULL ){

				mean+=(double) *value;

			}

			break;
		}

		case SLIDING_TYPE_DOUBLE:{

			double *value;

			list_init_traverse(sliding_window->elts);
			while ( (value = (double *) list_traverse(sliding_window->elts)) != NULL ){

				mean+=*value;

			}

			break;
		}

		default:

			break;

	}

	return mean/list_getsize(sliding_window->elts);

}

int sliding_window_verify_consistency(sliding_window_t *sliding_window){

	int *current_data, previous_data = -2;

	// if it list has already reached the maximum size of the sliding window
	if (list_getsize(sliding_window->elts) < sliding_window->max_size)
		return UNSUCCESSFUL;

	list_init_traverse(sliding_window->elts);
	while ( (current_data = (int *) list_traverse(sliding_window->elts)) != NULL ){

		if (previous_data != -2)
			if (*current_data != previous_data)
				return UNSUCCESSFUL;

		previous_data = *current_data;

	}

	return previous_data;

}

sliding_window_t *sliding_window_create(int max_size, int sliding_type){

	sliding_window_t *sliding_window = malloc(sizeof(sliding_window_t));

	sliding_window->max_size = max_size;
	sliding_window->elts = list_create();
	sliding_window->data_type = sliding_type;

	return sliding_window;

}
/*
int sliding_window_most_common_value(sliding_window_t *sliding_window, int start, int end){

	int *value, number_read=0, i=0;


	int values[end-start];
	int count[end-start];

	// if it list has already reached the maximum size of the sliding window
	if (list_getsize(sliding_window->elts) == 0)
		return UNSUCCESSFUL;

	list_init_traverse(sliding_window->elts);
	while ( (value = (int *) list_traverse(sliding_window->elts)) != NULL ){

		if ( (number_read >= start) && (number_read <= end ) ){

			if new_value
				values[++i] = value;

			count[find_value_index] ++;
		}

	}

	return common_value;

}*/

void sliding_window_print_history(sliding_window_t *sliding_window){

	printf("sliding_history = [");

	switch (sliding_window->data_type){

		case SLIDING_TYPE_INT:{

			int *current_data;

			list_init_traverse(sliding_window->elts);
			while ( (current_data = (int *) list_traverse(sliding_window->elts)) != NULL ){

				printf("%d, ", *current_data);

			}

			break;
		}

		case SLIDING_TYPE_DOUBLE:{

			double *current_data;

			list_init_traverse(sliding_window->elts);
			while ( (current_data = (double *) list_traverse(sliding_window->elts)) != NULL ){

				printf("%f, ", *current_data);

			}

			break;
		}

		default:

			break;

	}


	printf("] \033[0m\n");
return;
}

int sliding_window_destroy(sliding_window_t *sliding_window){
	list_destroy(sliding_window->elts);
	free(sliding_window);

	return SUCCESSFUL;

}
