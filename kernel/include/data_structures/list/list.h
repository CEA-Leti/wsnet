/* 
 * File:   list.h
 * Author: Frederic Evennou
 *
 * Created on february 24th, 2010, 15:29
 */
#ifndef _LIST_H
#define	_LIST_H


/* ************************************************** */
/* ************************************************** */
typedef int (* list_delete_func_t)(void *key1, void *key2);


/* ************************************************** */
/* ************************************************** */
typedef struct _list_elt_t {
  void *data;
  struct _list_elt_t *next;
  struct _list_elt_t *previous;
} list_elt_t;

typedef struct _list_t {
  int size;
  list_elt_t *trav;
  list_elt_t *elements;
  list_elt_t *elements_end; 
} list_t;

#ifdef __cplusplus
extern "C"{
#endif //__cplusplus
/* ************************************************** */
/* ************************************************** */
void    list_init(void);
list_t *list_create();
void    list_destroy(list_t *list);
void    list_insert(list_t *list, void *data);
void list_push_back_FIFO(list_t* list, void* data);
void *  list_pop(list_t *list);
void *  list_pop_FIFO(list_t *list);
void *  list_pull(list_t *list);
int     list_find(list_t *list, void *data);
void    list_delete(list_t *d, void *data);
void    list_selective_delete(list_t *d, list_delete_func_t delete_func, void *arg);
void    list_init_traverse(list_t *d);
void *  list_traverse(list_t *d);
int     list_getsize(list_t *d);
void*   list_push_back(list_t *list , void* data);
void 	list_delete_from_list(list_t *list , void* data);
void* 	list_take(list_t *list);
void    list_push_first(list_t *list , void* data);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif	/* _LIST_H */

