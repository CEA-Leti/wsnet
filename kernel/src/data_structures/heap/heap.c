/* 
 * File:   heap.c
 * Author: Guillaume Chelius
 *
 * Created on march 19th, 2010, 11:44
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <kernel/include/data_structures/mem_fs/mem_fs.h>
#include "heap.h"


/* ************************************************** */
/* ************************************************** */
static void *mem_heap = NULL; /* memory slice for heap */


/* ************************************************** */
/* ************************************************** */
void heap_init(void) {
  mem_heap = mem_fs_slice_declare(sizeof(heap_t));
}


/* ************************************************** */
/* ************************************************** */
heap_t *heap_create(heap_compare_t compare) {
  heap_t *heap;
    
  if ((heap = (heap_t *) mem_fs_alloc(mem_heap)) == NULL) {
    return NULL;
  }
    
  if ((heap->elts = (heap_elt_t *) malloc((HEAP_MAX + 1) * sizeof(heap_elt_t))) == NULL) {
    return NULL;
  }
  heap->m_capacity = HEAP_MIN;
  heap->capacity = HEAP_MAX;
  heap->size = 0;
  heap->compare = compare;
  heap->elts[0].key = NULL;
  heap->elts[0].data = NULL;
    
  return heap;
}


/* ************************************************** */
/* ************************************************** */
void heap_destroy(heap_t *heap) {
  free(heap->elts);
  mem_fs_dealloc(mem_heap, heap);
}


/* ************************************************** */
/* ************************************************** */
void heap_insert(heap_t *heap, void *key, void *data) {
  int i = 0;
    
  /* double the heap capacity if it is full */
  if (heap->capacity == heap->size) {
    heap_elt_t *elts;
        
    if ((elts = (heap_elt_t *) malloc((heap->capacity * 2 + 1) * sizeof(heap_elt_t))) == NULL) {
      return;
    }
    memcpy(elts, heap->elts, (heap->size + 1) * sizeof(heap_elt_t));
    free(heap->elts);
    heap->elts = elts;
    heap->capacity *= 2;
  }
    
  /* add the new element */
  for (i = ++(heap->size); heap->compare(heap->elts[i/2].key, key) == -1; i /= 2) {
    heap->elts[i].key = heap->elts[i/2].key;
    heap->elts[i].data = heap->elts[i/2].data;
  }
  heap->elts[i].key = key;
  heap->elts[i].data = data;
}


/* ************************************************** */
/* ************************************************** */
void *heap_pop(heap_t *heap) {
  heap_elt_t m_elt, l_elt;
  int i, child, n_capacity = (2 * heap->capacity) / 3;
    
  if (heap->size == 0) {
    return NULL;
  }
  
  /* reduce the heap capacity if 2/3 of the queue is empty */
  if ((heap->capacity >= (3 * heap->size)) 
      && ((n_capacity) >= heap->m_capacity)) {
    heap_elt_t *elts;
        
    if ((elts = (heap_elt_t *) malloc((n_capacity + 1) * sizeof(heap_elt_t))) == NULL) {
      return NULL;
    }
        
    memcpy(elts, heap->elts, (heap->size + 1) * sizeof(heap_elt_t));
    free(heap->elts);
    heap->elts = elts;
    heap->capacity = n_capacity;
  }
    
  /* pop the first element */
  m_elt.key = heap->elts[1].key;
  m_elt.data = heap->elts[1].data;
  l_elt.key = heap->elts[heap->size].key;
  l_elt.data = heap->elts[heap->size].data;
  heap->size--;

  for(i = 1; i * 2 <= heap->size; i = child) {
    child = i * 2;
        
    if ((child != heap->size) 
	&& (heap->compare(heap->elts[child+1].key, heap->elts[child].key) == 1))
      child++;
        
    if (heap->compare(l_elt.key, heap->elts[child].key) == -1) {
      heap->elts[i].key = heap->elts[child].key;
      heap->elts[i].data = heap->elts[child].data;
    } else {
      break;
    }
  }
  heap->elts[i].key = l_elt.key;
  heap->elts[i].data = l_elt.data;
    
  return (void *) m_elt.data;
}


/* ************************************************** */
/* ************************************************** */
void *heap_see_first(heap_t *heap) {    
  if (heap->size == 0) {
    return NULL;
  }
  
  return  heap->elts[1].data;
}


/* ************************************************** */
/* ************************************************** */
/* edit by Ibrahim Amadou <ibrahim.amadou@insa-lyon.fr> */
void *heap_delete(heap_t *heap, void *key) {
  heap_elt_t elts, temp;
  int i = 0, p = 0, child;
  
  if (heap->size == 0) {
    return NULL;
  }

  if (heap->compare(heap->elts[1].key, key) == 0) {      
    return heap_pop(heap);
  } else if (heap->compare(heap->elts[heap->size].key, key) == 0) {
    elts.key  = heap->elts[heap->size].key;
    elts.data = heap->elts[heap->size].data;      
    heap->size--;      
    return (void *) elts.data;
  } else {
    while (p != 1) {
      for (i = 1; (2 * i) <= heap->size; i++) {
	child = 2 * i;
	if ((child != heap->size) && (heap->compare(heap->elts[child].key, key) == 0)) {
	  temp.key = heap->elts[i].key;
	  temp.data = heap->elts[i].data;
	  heap->elts[i].key = heap->elts[child].key;
	  heap->elts[i].data = heap->elts[child].data; 
	  heap->elts[child].key = temp.key;
	  heap->elts[child].data = temp.data;
	  p = i;
	  break;
	}
	child++;
		  
	if (heap->compare(heap->elts[child].key, key) == 0) {
	  temp.key  = heap->elts[i].key;
	  temp.data = heap->elts[i].data;
	  heap->elts[i].key = heap->elts[child].key;
	  heap->elts[i].data = heap->elts[child].data; 
	  heap->elts[child].key = temp.key;
	  heap->elts[child].data = temp.data;
	  p = i;
	  break;
	}
      }

      if (p == 1) {
	return heap_pop(heap);
      } else if (p == 0) {
	p = 1;
      }
    }
    return NULL;       
  }
}
/* end of edition */
