/*
 *  \file   circ_array.h
 *  \brief  Circular array (buffer) data structure definitions
 *  \author Luiz Henrique Suraty Filho
 *  \date   2016
 */

#ifndef _CIRC_ARRAY_H
#define	_CIRC_ARRAY_H

typedef struct _circ_array{

	int length;
	int count;
	int start;
	void *data;

}circ_array_t;


#ifdef __cplusplus
extern "C"{
#endif //__cplusplus

void circ_array_init(void);

circ_array_t *circ_array_int_create(int _length);

circ_array_t *circ_array_double_create(int _length);

void circ_array_destroy(circ_array_t *_c_array);

int circ_array_int_pop(circ_array_t *_c_array);

double circ_array_double_pop(circ_array_t *_c_array);

int circ_array_double_replace(circ_array_t *_c_array, int _index, double _data);

int circ_array_int_replace(circ_array_t *_c_array, int _index, int _data);

void circ_array_double_print(circ_array_t *_c_array);

void circ_array_int_print(circ_array_t *_c_array);

void circ_array_rotate_right(circ_array_t *_c_array);

void circ_array_rotate_left(circ_array_t *_c_array);

double circ_array_double_get(circ_array_t *_c_array, int _index);
int circ_array_int_get(circ_array_t *_c_array, int _index);

int circ_array_int_push(circ_array_t *_c_array, int _data);

int circ_array_double_push(circ_array_t *_c_array, double _data);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif	/* _CIRC_ARRAY_H */
