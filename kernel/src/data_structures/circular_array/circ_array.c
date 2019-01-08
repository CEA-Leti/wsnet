#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "circ_array.h"

 void circ_array_init(void){
	return;
}

 circ_array_t *circ_array_create(int _length, size_t _type_size){

	circ_array_t *c_array = (circ_array_t *) malloc(sizeof(circ_array_t));

	c_array->length = _length;
	c_array->start = 0;
	c_array->count = 0;
	c_array->data = (void*) malloc(_type_size*_length);

	return c_array;
}

 circ_array_t *circ_array_int_create(int _length){
	return circ_array_create(_length, sizeof(int));
}

 circ_array_t *circ_array_double_create(int _length){
	return circ_array_create(_length, sizeof(double));
}

 void circ_array_destroy(circ_array_t *_c_array){
	free(_c_array->data);
	free(_c_array);
}

 bool circ_array_is_empty(circ_array_t *_c_array){
	return ((_c_array->count == 0) ? true : false);
}

 bool circ_array_is_full(circ_array_t *_c_array){
	return ((_c_array->count == _c_array->length) ? true : false);
}

 void circ_array_rotate_right(circ_array_t *_c_array){
	_c_array->start = (((--_c_array->start)%_c_array->length) < 0 ? _c_array->start+_c_array->length : _c_array->start );
}

 void circ_array_rotate_left(circ_array_t *_c_array){
   _c_array->start--;
	_c_array->start = (_c_array->start)%_c_array->length;
}

 void *circ_array_get(circ_array_t *_c_array, int _index, size_t _type_size){
	return ( (circ_array_is_empty(_c_array) || (_index > _c_array->count-1)) ? NULL : (_c_array->data + ((_c_array->start+_index)%_c_array->length) * _type_size));
}

 int circ_array_int_get(circ_array_t *_c_array, int _index){
	return (int) *((int*) circ_array_get(_c_array, _index, sizeof(int)));
}

 double circ_array_double_get(circ_array_t *_c_array, int _index){
	return (double) *( (double*) circ_array_get(_c_array, _index, sizeof(double)) );
}

 void *circ_array_pop(circ_array_t *_c_array, size_t _type_size){
	if (circ_array_is_empty(_c_array))
		return NULL;
	void *value = _c_array->data + _c_array->start*_type_size;
	_c_array->start++;
	_c_array->start = (_c_array->start)%_c_array->length;
	_c_array->count--;
	return  value;
}

 int circ_array_int_pop(circ_array_t *_c_array){
	return (int) *((int*) circ_array_pop(_c_array, sizeof(int)));
}

 double circ_array_double_pop(circ_array_t *_c_array){
	return (double) *( (double*) circ_array_pop(_c_array, sizeof(double)) );
}

 int circ_array_push(circ_array_t *_c_array, void *_data, size_t _type_size){

	if (circ_array_is_full(_c_array))
		return -1;
	int index = ((_c_array->start + _c_array->count++)%_c_array->length) * _type_size;
	memcpy(_c_array->data+index,_data, _type_size);
	return 0;
}

 int circ_array_int_push(circ_array_t *_c_array, int _data){
	return circ_array_push(_c_array, (void *) &_data, sizeof(int));
}

 int circ_array_double_push(circ_array_t *_c_array, double _data){
	return circ_array_push(_c_array, (void *) &_data, sizeof(double));
}

 int circ_array_replace(circ_array_t *_c_array, int _index, void *_data, size_t _type_size){

	if (circ_array_is_empty(_c_array) || (_index > _c_array->count-1))
		return -1;

	memcpy( _c_array->data + ((_c_array->start+_index)%_c_array->length) * _type_size,_data, _type_size);
	return 0;
}

 int circ_array_double_replace(circ_array_t *_c_array, int _index, double _data){
	return circ_array_replace(_c_array, _index, (void *) &_data, sizeof(double));
}

 int circ_array_int_replace(circ_array_t *_c_array, int _index, int _data){
	return circ_array_replace(_c_array, _index,(void *) &_data, sizeof(int));
}

 void circ_array_int_print(circ_array_t *_c_array){
    int i;
    for (i=0;i<_c_array->count;i++)
        printf("c_array[%d] = %d\n",i,circ_array_int_get(_c_array,i));
}


 void circ_array_double_print(circ_array_t *_c_array){
    int i;
    for (i=0;i<_c_array->count;i++)
        printf("c_array[%d] = %lf\n",i,circ_array_double_get(_c_array,i));
}

