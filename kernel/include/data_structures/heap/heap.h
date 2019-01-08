/* 
 * File:   heap.h
 * Author: Guillaume Chelius
 *
 * Created on march 19th, 2010, 11:44
 */
#ifndef _HEAP_H
#define	_HEAP_H


/* ************************************************** */
/* ************************************************** */
#define HEAP_MAX 100
#define HEAP_MIN 100


/* ************************************************** */
/* ************************************************** */
typedef int (* heap_compare_t) (void *key0, void *key1);


/* ************************************************** */
/* ************************************************** */
typedef struct _heap_elt {
  void *key;
  void *data; 
} heap_elt_t;

typedef struct _heap {
  int m_capacity;
  int capacity;
  int size;
  heap_compare_t compare;
  heap_elt_t *elts;
} heap_t;


/* ************************************************** */
/* ************************************************** */
#ifdef __cplusplus
extern "C"{
#endif

void     heap_init(void);
heap_t * heap_create(heap_compare_t compare);
void     heap_destroy(heap_t *heap);
void     heap_insert(heap_t *heap, void *key, void *data);
void *   heap_pop(heap_t *heap);
void *   heap_see_first(heap_t *heap);
void *   heap_delete(heap_t *heap, void *key);

#ifdef __cplusplus
}
#endif //__cplusplus


#endif /* _HEAP_H */

