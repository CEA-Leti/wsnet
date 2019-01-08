/**
 *  \file   mem_fs.c
 *  \brief  Fixed size memory management with preallocation
 *  \author Guillaume Chelius
 *  \date   2007
 **/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


/* ************************************************** */
/* ************************************************** */
#define PREALLOCATION_NUMBER 1024


/* ************************************************** */
/* ************************************************** */
typedef union _container {
    char             *data; /* pointer to the data */
    union _container *next; /* pointer to the next free container */
} container_t;

typedef struct _slice {
    int            size;   /* size of a container */
    int            length; /* slice length */
    int            free;   /* number of free containers */
    container_t   *f_free; /* first free container */
    struct _slice *next;   /* next slice */
} slice_t;

typedef struct _page {
    char         *memory; /* memory page */
    struct _page *next;   /* pointer to the next memory page */
} page_t;


/* ************************************************** */
/* ************************************************** */
slice_t *slices = NULL;
page_t *pages = NULL;


/* ************************************************** */
/* ************************************************** */
void mem_fs_clean(void) {
    /* free all declared slices */
    while (slices) {
        slice_t *slice = slices;
        slices = slice->next;
        free(slice);
    }
    
    /* free all memory pages */
    while (pages) {
        page_t *page = pages;
        pages = page->next;
        free(page->memory);
        free(page);
    }
    
    return;
}


/* ************************************************** */
/* ************************************************** */
void *mem_fs_slice_declare(int size) {
    container_t *container;
    char *containers;
    page_t *page;
    slice_t *slice = slices;
    int i, n_size = (size > (int) sizeof(container_t)) ? size : (int) sizeof(container_t);

    /* protect against too small containers*/
    if (size <= 0) {
        fprintf(stderr, "mem_fs: size too small (%dB) in slice declaration (mem_fs_slice_declare())\n", size);
        return NULL;
    }
    
    /* search for an already declared slice */
    while (slice) {
        if (slice->size == n_size) {
            return slice;
        }
        slice = slice->next;
    }
    
    /* allocate new slice */
    if ((slice = (slice_t *) malloc(sizeof(slice_t))) == NULL) {
        fprintf(stderr, "mem_fs: malloc error in slice declaration (mem_fs_slice_declare())\n");
        return NULL;
    }
    slice->size = n_size;
    
    /* Containers pre-allocation */
    if ((containers = malloc(slice->size * (slice->length = slice->free = PREALLOCATION_NUMBER))) == NULL) {
        fprintf(stderr, "mem_fs: malloc error in containers pre-allocation (mem_fs_slice_declare())\n");
        free(slice);
        return NULL;
    }
    
    /* chain the free containers */
    for (i = 0; i < (slice->length - 1); i++) {
        container = (container_t *) (containers + i * slice->size);
        container->next = (container_t *) (containers + (i + 1) * slice->size);
    }
    container = (container_t *) (containers + (slice->length - 1) * slice->size);
    container->next = NULL;
    
    /* point towards the first free container */
    slice->f_free = (container_t *) (containers);
    
    /* allocate a new page */
    if ((page = (page_t *) malloc(sizeof(page_t))) == NULL) {
        fprintf(stderr, "mem_fs: malloc error in slice declaration (mem_fs_slice_declare())\n");
        free(containers);
        free(slice);
        return NULL;
    }
    
    /* update pages */
    page->memory = containers;
    page->next = pages;
    pages = page;

    /* update slices */
    slice->next = slices;
    slices = slice;

    return (void *) slice;
}


/* ************************************************** */
/* ************************************************** */
int mem_fs_slice_realloc(slice_t *slice) {
    page_t *page;
    char *containers;
    container_t *container;
    int i;

    /* realloc memory */
    if ((containers = malloc(slice->size * slice->length)) == NULL) {
        fprintf(stderr, "mem_fs: malloc error in containers pre-allocation (mem_fs_slice_realloc())\n");
        return -1;
    }
    
    /* chain the free containers */
    for (i = 0; i < (slice->length - 1); i++) {
        container = (container_t *) (containers + i * slice->size);
        container->next = (container_t *) (containers + (i + 1) * slice->size);
    }
    container = (container_t *) (containers + (slice->length - 1) * slice->size);
    container->next = NULL;

    /* allocate new page */
    if ((page = (page_t *) malloc(sizeof(page_t))) == NULL) {
        fprintf(stderr, "mem_fs: malloc error in slice declaration (mem_fs_slice_realloc())\n");
        free(containers);
        return -1;
    }

    /* point towards the first free container */
    slice->f_free = (container_t *) containers;
    slice->length += (slice->free = slice->length);
    
    /* chain the page */
    page->memory = containers;
    page->next = pages;
    pages = page;

    return 0;
}


/* ************************************************** */
/* ************************************************** */
void *mem_fs_alloc(void *arg0) {
    slice_t *slice = (slice_t *) arg0;
    container_t *container;

    /* if there is no free container, realloc */
    if (slice->free == 0) {
        if (mem_fs_slice_realloc(slice)) {
            return NULL;
        }
    }

    /* get the first free container and update the list */
    container = slice->f_free;
    slice->f_free = container->next;
    slice->free--;

    /* return the allocated container */
    return (void *) container;
}

void mem_fs_dealloc(void *arg0, void *arg1) {
    slice_t *slice = (slice_t *) arg0;
    container_t *container = (container_t *) arg1;

    /* update the chained containers */
    container->next = slice->f_free;
    slice->f_free = container;
    slice->free++;
}
