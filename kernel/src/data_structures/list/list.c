/* 
 * File:   list.c
 * Author: Frederic Evennou
 *
 * Created on february 24th, 2010, 15:29
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <kernel/include/data_structures/mem_fs/mem_fs.h>
#include "list.h"


/* ************************************************** */
/* ************************************************** */
static void *mem_list = NULL; /* memory slice for list */
static void *mem_list_elt = NULL; /* memory slice for list element */


/* ************************************************** */
/* ************************************************** */
void list_init(void) {
  mem_list = mem_fs_slice_declare(sizeof(list_t));
  mem_list_elt = mem_fs_slice_declare(sizeof(list_elt_t));
  return;
}


/* ************************************************** */
/* ************************************************** */
list_t *list_create() {
  list_t *list = (list_t *) mem_fs_alloc(mem_list);
    
  list->size     = 0;
  list->elements = NULL;
  list->trav     = NULL;
  list->elements_end = NULL;
  return list;
}


/* ************************************************** */
/* ************************************************** */
void list_destroy(list_t* list) {
  list_elt_t *elt;

  if (list == NULL) {
    return;
  }
  
  while ((elt = list->elements) != NULL) {
    list->elements = elt->next;
    mem_fs_dealloc(mem_list_elt, elt);       
    list->size--;
  }
  mem_fs_dealloc(mem_list, list);       
}


/* ************************************************** */
/* ************************************************** */
int list_getsize(list_t *list) {
	// edited by Luiz Henrique Suraty Filho
	if (list==NULL)
		return 0;
	// end edition

	return list->size;
}


/* ************************************************** */
/* ************************************************** */
void list_insert(list_t* list, void* data) {
  list_elt_t *elt = (list_elt_t *) mem_fs_alloc(mem_list_elt);
  
  elt->data = data;
  elt->next = NULL;
  elt->previous = NULL;
  
  if (list->elements == NULL) {
    list->elements = elt;
    list->elements_end = elt;
  } else {
    elt->next = list->elements;
    list->elements->previous = elt;
    list->elements = elt;
  }
  
  (list->size)++;
}

/* ************************************************** */
/* ************************************************** */
/* Added by Luiz Henrique Suraty Filho*/
/* pushes back to the last position on FIFO and it will
 * be the element to be out on next pop_FIFO
 * */
void list_push_back_FIFO(list_t* list, void* data) {
  list_elt_t *elt = (list_elt_t *) mem_fs_alloc(mem_list_elt);

  elt->data = data;
  elt->next = NULL;
  elt->previous = NULL;

  if (list->elements == NULL) {
    list->elements = elt;
    list->elements_end = elt;
  } else {
    list->elements_end->next = elt;
    elt->previous = list->elements_end;
    list->elements_end = elt;
  }

  (list->size)++;

}
/*end of edition*/

/* ************************************************** */
/* ************************************************** */
void* list_pop(list_t *list) {
  list_elt_t *elt = list->elements;
  void *data;
  
  if (elt == NULL) {
    return NULL;
  }
  data = elt->data;
  
  list->elements = elt->next;  
  if (list->elements != NULL) {
    list->elements->previous = NULL;
  } else {
    list->elements_end = NULL;
  }
  
  (list->size)--;  
  mem_fs_dealloc(mem_list_elt, elt);
  return data;
}


/* ************************************************** */
/* ************************************************** */
void* list_pop_FIFO(list_t *list) {
  list_elt_t *elt;
  void *data;
  
  if ((elt = list->elements_end) == NULL) {
    return NULL;
  }
  else if (list->elements_end == list->elements) {
    data = elt->data;
    list->elements_end = NULL;
    list->elements = NULL;
    (list->size)--;

    // Cleanup the memory
    mem_fs_dealloc(mem_list_elt, elt);

    return data;
  }
  else {
    data = elt->data;
    list->elements_end = list->elements_end->previous;
    list->elements_end->next = NULL;
    (list->size)--;

    // Cleanup the memory
    mem_fs_dealloc(mem_list_elt, elt);

    return data;
  }
}


/* ************************************************** */
/* ************************************************** */
void *list_pull(list_t *list) {
  list_elt_t *elt = list->elements;
  void *data;
  
  if (elt == NULL) {
    return NULL;
  } else if (list->elements_end == list->elements) {
    data = elt->data;
    list->elements_end = NULL;
    list->elements = NULL;
    (list->size)--;

    mem_fs_dealloc(mem_list_elt, elt);
    return data;
  } else {
    data = elt->data;
    list->elements_end = list->elements_end->previous;
    list->elements_end->next = NULL;
    (list->size)--;

    mem_fs_dealloc(mem_list_elt, elt);
    return data;
  }
}


/* ************************************************** */
/* ************************************************** */
int list_find(list_t *list, void *data) {
  list_elt_t *elt = list->elements;
  
  while (elt != NULL) {
    if (elt->data == data) {
      return 1;
    }
    elt = elt->next;
  }
  
  return 0;
}


/* ************************************************** */
/* ************************************************** */
void list_delete(list_t *list, void *data) {
  list_elt_t *elt = list->elements, *o_elt = NULL;
    
  while (elt != NULL) {        
    if (elt->data == data) {
      /* delete from the begining*/
      if (o_elt == NULL) {
	list->elements = elt->next;
	if (list->elements != NULL) {
	  list->elements->previous = NULL;
	}  else {
	  list->elements_end = NULL;
	}
      } else if (elt == list->elements_end) {
	list->elements_end = list->elements_end->previous;
	list->elements_end->next = NULL;
      } else {
	o_elt->next = elt->next;
	if (elt->next != NULL) { 
	  elt->next->previous = o_elt;
	}
      }
      
      (list->size)--;
      free(elt->data); // TODO: what we do with it ?
      mem_fs_dealloc(mem_list_elt, elt);
      return;
    }
    o_elt = elt;
    elt = elt->next;
  }
  return;
}


/* ************************************************** */
/* ************************************************** */
void list_selective_delete(list_t *list, list_delete_func_t delete, void *arg) {
  list_elt_t *elt = list->elements, *o_elt = NULL, *c_elt;
    
  while (elt != NULL) {
    if (delete(elt->data, arg)) {
      c_elt = elt;
      elt = c_elt->next;
            
      if (o_elt == NULL) {
	list->elements = c_elt->next;
	if (list->elements != NULL) {
	  list->elements->previous = NULL;
	} else {
	  list->elements_end = NULL;
	}
      } else if (c_elt == list->elements_end) {
	list->elements_end = list->elements_end->previous;
	list->elements_end->next = NULL;
      } else {
	o_elt->next = c_elt->next;
	if (c_elt->next != NULL) { 
	  c_elt->next->previous = o_elt;
	}
      }
	
      (list->size)--;
      free(c_elt->data); // TODO: what we do with it ?
      mem_fs_dealloc(mem_list_elt, c_elt);        
    } else {
      o_elt = elt;
      elt = elt->next;
    }
  }  
  return;
}


/* ************************************************** */
/* ************************************************** */
void list_init_traverse(list_t *list) {
  list->trav = NULL;
}


/* ************************************************** */
/* ************************************************** */
void *list_traverse(list_t *list) {
  if (list->trav == NULL) {
    list->trav = list->elements;
  } else {
    list->trav = list->trav->next;
  }
  
  if (list->trav) {
    return list->trav->data;
  } else {
    return NULL;
  }
}

/*
 * Add the element at the end of the list
 */
void* list_push_back(list_t *list , void* data){
	  list_elt_t *elt = (list_elt_t *) mem_fs_alloc(mem_list_elt);

	  elt->data = data;
	  elt->next = NULL;
	  elt->previous = NULL;

	  if (list->elements == NULL) {
	    list->elements = elt;
	    list->elements_end = elt;
	  } else {
		  list->elements_end->previous = list->elements_end;
		  list->elements_end = elt;
		  list->elements_end->next = NULL;
	  }

	  (list->size)++;

return data;
}

/*
 * Remove data from the list without dealloc the data
 */

void list_delete_from_list(list_t *list, void *data) {
  list_elt_t *elt = list->elements, *o_elt = NULL;

  while (elt != NULL) {
    if (elt->data == data) {
      /* delete from the begining*/
      if (o_elt == NULL) {
    	  list->elements = elt->next;
    	  if (list->elements != NULL) {
    		  list->elements->previous = NULL;
    	  }
    	  else {
    		  list->elements_end = NULL;
    	  }
      }
      else if (elt == list->elements_end) {
    	  list->elements_end = list->elements_end->previous;
    	  list->elements_end->next = NULL;
      }
      else {
    	  o_elt->next = elt->next;
    	  if (elt->next != NULL) {
    		  elt->next->previous = o_elt;
    	  }
      }

      (list->size)--;
      return;
    }
    o_elt = elt;
    elt = elt->next;
  }
  return;
}

void* list_take(list_t *list) {
  list_elt_t *elt;
  void *data;

  if ((elt = list->elements_end) == NULL) {
    data = NULL;
  }
  else if (list->elements_end == list->elements) {
    data = elt->data;
    list->elements_end = NULL;
    list->elements = NULL;
    (list->size)--;
  }
  else {
    data = elt->data;
    list->elements_end = list->elements_end->previous;
    list->elements_end->next = NULL;
    (list->size)--;

  }

  return data;
}


/*
 * Add the element at the first place of the list
 */
void list_push_first(list_t *list , void* data){
	  list_elt_t *elt = (list_elt_t *) mem_fs_alloc(mem_list_elt);

	  elt->data = data;
	  elt->next = NULL;
	  elt->previous = NULL;

	  if (list->elements == NULL) {
	    list->elements = elt;
	    list->elements_end = elt;
	  } else {
		  elt->next=list->elements;
		  list->elements->previous =elt;
		  list->elements = elt;
	  }

	  (list->size)++;

}


