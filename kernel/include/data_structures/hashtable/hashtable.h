/* 
 * File:   hashtable.h
 * Author: Quentin Lampin
 *
 * Created on january 15th, 2010, 20:53
 */
#ifndef WSNET_CORE_DATA_STRUCTURE_HASHTABLE_H_
#define	WSNET_CORE_DATA_STRUCTURE_HASHTABLE_H_


/* ************************************************** */
/* ************************************************** */
#define HASHTABLE_SIZE 64


/* ************************************************** */
/* ************************************************** */
typedef unsigned long (* hash_function_t) (void *key);
typedef int (* equal_function_t) (void *key1, void *key2);
typedef void (* clean_function_t) (void *value);
typedef void * (* clone_function_t) (void *value);


/* ************************************************** */
/* ************************************************** */
typedef struct _hashtable_elt_t {
    void *key;
    void *value;
    struct _hashtable_elt_t *previous;
    struct _hashtable_elt_t *next;
} hashtable_elt_t;

typedef struct _deprecated_hashtable_t {
    int size;
    hash_function_t hash;
    equal_function_t equal;
    clean_function_t clean;
    clone_function_t clone;
    hashtable_elt_t* elements[HASHTABLE_SIZE];
} hashtable_t;

#ifdef __cplusplus
extern "C"{
#endif //__cplusplus
/* ************************************************** */
/* ************************************************** */
void         hashtable_init(void);
hashtable_t *hashtable_create(hash_function_t hash, equal_function_t equal,
			      clean_function_t clean, clone_function_t clone);
void         hashtable_destroy(hashtable_t *hashtable);
void         hashtable_insert(hashtable_t *hashtable, void *key, void *value);
void *       hashtable_retrieve(hashtable_t *hashtable, void *key);
void         hashtable_delete(hashtable_t *hashtable, void *key);

hashtable_t *clone_hashtable(hashtable_t* hashtable);

/* ************************************************** */
/* ************************************************** */
#define SIGNEDSHORT 0x7FFFFFFF
unsigned long hash_string(void *string);
int           equal_string(void *string1, void *string2);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif	// WSNET_CORE_DATA_STRUCTURE_HASHTABLE_H_

