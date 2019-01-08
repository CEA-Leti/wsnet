/* 
 * File:   hashtable.c
 * Author: Quentin Lampin
 *
 * Created on january 15th, 2010, 20:53
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <kernel/include/data_structures/mem_fs/mem_fs.h>
#include "hashtable.h"


/* ************************************************** */
/* ************************************************** */
static void *mem_hashtable = NULL; /* memory slice for list */
static void *mem_hashtable_elt = NULL; /* memory slice for list element */


/* ************************************************** */
/* ************************************************** */
void hashtable_init(void) {
  mem_hashtable = mem_fs_slice_declare(sizeof(hashtable_t));
  mem_hashtable_elt = mem_fs_slice_declare(sizeof(hashtable_elt_t));
  return;
}


/* ************************************************** */
/* ************************************************** */
hashtable_t *hashtable_create(hash_function_t hash, equal_function_t equal,
			      clean_function_t clean, clone_function_t clone) {
  int index;
  hashtable_t *hashtable = (hashtable_t *) mem_fs_alloc(mem_hashtable);
  hashtable->hash = hash;
  hashtable->equal = equal;
  hashtable->clean = clean;
  hashtable->clone = clone;
  hashtable->size = 0;
  for (index = 0; index < HASHTABLE_SIZE; index++) {
    hashtable->elements[index] = NULL;
  }
  return hashtable;
}


/* ************************************************** */
/* ************************************************** */
void hashtable_destroy(hashtable_t *hashtable) {
  int index;
  hashtable_elt_t *element;
  
  if(hashtable->clean == NULL)
    {
      for (index = 0; index < HASHTABLE_SIZE; index++) {
	while ((element = hashtable->elements[index]) != NULL) {
	  hashtable->elements[index] = element->next;
	  mem_fs_dealloc(mem_hashtable_elt, element);       

	}
      }
    } 
  else 
    {
      for (index = 0; index < HASHTABLE_SIZE; index++) {
	while ((element = hashtable->elements[index]) != NULL) {
	  hashtable->elements[index] = element->next;
	  hashtable->clean(element->value);
	  mem_fs_dealloc(mem_hashtable_elt, element);       
	}
      }
      mem_fs_dealloc(mem_hashtable, hashtable);       
    }
  return;
}


/* ************************************************** */
/* ************************************************** */
void hashtable_insert(hashtable_t *hashtable, void* key, void* value) {
  unsigned long hash = hashtable->hash(key) % HASHTABLE_SIZE;
  hashtable_elt_t *element =  (hashtable_elt_t *) mem_fs_alloc(mem_hashtable_elt);
  element->key = key;
  element->value = value;
  element->next = NULL;
  element->previous = NULL;
  if (hashtable->elements[hash] == NULL) {
    hashtable->elements[hash] = element;
  } else {
    element->next = hashtable->elements[hash];
    hashtable->elements[hash]->previous = element;
    hashtable->elements[hash] = element;
  }
  hashtable->size++;
  return;
}


/* ************************************************** */
/* ************************************************** */
void *hashtable_retrieve(hashtable_t *hashtable, void *key) {
  unsigned long hash = hashtable->hash(key) % HASHTABLE_SIZE;
  hashtable_elt_t *element = hashtable->elements[hash];

  while (element != NULL) {
    if (hashtable->equal(key, element->key)) {
      return element->value;
    } else {
      element = element->next;
    }
  }
  return NULL;
}


/* ************************************************** */
/* ************************************************** */
void hashtable_delete(hashtable_t *hashtable, void *key) {
  unsigned long hash = hashtable->hash(key) % HASHTABLE_SIZE;
  hashtable_elt_t *element = hashtable->elements[hash], *o_element = NULL;
  while (element != NULL) {
    if (hashtable->equal(key, element->key)) {
      /* delete from the begining*/
      if (o_element == NULL) {
	hashtable->elements[hash] = element->next;
	if (hashtable->elements[hash] != NULL) {
	  hashtable->elements[hash]->previous = NULL;
	}
      } else {
	o_element->next = element->next;
	if (element->next != NULL){ 
	  element->next->previous = o_element;
	}
      }
            
      hashtable->size--;
      mem_fs_dealloc(mem_hashtable_elt, element);
      return;
    }
    o_element = element;
    element = element->next;
  }

  return;
}


/* ************************************************** */
/* ************************************************** */
unsigned long hash_string(void *string) {
  int index;
  unsigned long hash = 0;
  for (index = 0; index < (int)strlen((char *) string); index++) {
    hash += ((char *) string)[index] + (hash << 6) + (hash << 16) - hash;
  }
  return (unsigned long) hash & SIGNEDSHORT;
}


/* ************************************************** */
/* ************************************************** */
int equal_string(void *string1, void *string2) {
  return !strcmp((char *) string1, (char *) string2);
}


/* ************************************************** */
/* ************************************************** */
hashtable_t *clone_hashtable(hashtable_t* hashtable)
{
  int index;
  hashtable_t *copy_hashtable = (hashtable_t *) mem_fs_alloc(mem_hashtable);
  copy_hashtable->hash  = hashtable->hash;
  copy_hashtable->equal = hashtable->equal;
  copy_hashtable->clean = hashtable->clean;
  copy_hashtable->clone = hashtable->clone;
  copy_hashtable->size  = hashtable->size;

  for (index = 0; index < HASHTABLE_SIZE; index++) {
    hashtable_elt_t *element =  hashtable->elements[index];
    hashtable_elt_t *copy_element;
    hashtable_elt_t *copy_element_previous = NULL;
    if (element != NULL) {
      int first_element = 1;
      do {
	copy_element = (hashtable_elt_t *) mem_fs_alloc(mem_hashtable_elt);
	copy_element->key      = element->key;
	copy_element->value    = hashtable->clone(element->value);
	copy_element->previous = copy_element_previous;
	copy_element->next     = NULL;

	if (copy_element_previous != NULL)
	  copy_element_previous->next = copy_element;

	copy_element_previous = copy_element;
	element = element->next;

	if (first_element) {
	  copy_hashtable->elements[index] = copy_element;
	  first_element = 0;
	}
      }	while (element != NULL);
    }
    else {
      copy_hashtable->elements[index] = NULL;
    }
  }
  return copy_hashtable;
}

/* hashtable_t *clone_hashtable(hashtable_t* hashtable) */
/* { */
/*   int index; */
/*   hashtable_t *copy_hashtable = malloc(sizeof (hashtable_t)); */
/*   copy_hashtable->hash = hashtable->hash; */
/*   copy_hashtable->equal = hashtable->equal; */
/*   copy_hashtable->clean = hashtable->clean; */
/*   copy_hashtable->size = hashtable->size; */
/*   for (index = 0; index < HASHTABLE_SIZE; index++) { */
/*     copy_hashtable->elements[index] = hashtable->elements[index]; */
/*   } */
/*   return copy_hashtable; */
/* } */
