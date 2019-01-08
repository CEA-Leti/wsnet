/* 
 * File:   field.h
 * Author: Quentin Lampin
 *
 * Created on 12 janvier 2010, 09:50
 */

#ifndef _FIELD_H
#define	_FIELD_H

//field types
#define INT     0
#define UINT    1
#define LONG    2
#define ULONG   3
#define DBLE    4
#define STRING  5

typedef struct _field_t{
    int type;
    int size;
    void *value; 
} field_t;

#ifdef __cplusplus
extern "C"{
#endif

field_t *field_create(int type, int size, void *value);
field_t *field_alloc(void);
void field_destroy(field_t *field);
void hashtable_field_destroy(void *field);
void* hashtable_field_clone(void *field);
void field_setValue(field_t *field, void* val);
void* field_getValue(field_t *field);
#endif	/* _FIELD_H */

#ifdef __cplusplus
}
#endif
