/*
 * File:   field.c
 * Author: Quentin Lampin
 *
 * Created on 12 janvier 2010, 09:50
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "field.h"

field_t *field_create(int type, int size, void *value)
{
  field_t* field = malloc(sizeof(field_t));
  field->type = type;
  field->size = size;
  field->value = value;
  return field;
}

field_t *field_alloc(void){
  field_t* field = malloc(sizeof(field_t));
  return field;
}

void field_destroy(field_t* field)
{
  free(field->value);
  free(field);
  return;
}

void hashtable_field_destroy(void *field)
{
  field_destroy((field_t *)field);
  return;
}

void *hashtable_field_clone(void *field)
{
  field_t *src_field  = (field_t *) field;
  field_t *copy_field = malloc(sizeof(field_t));

  copy_field->type = src_field->type;
  copy_field->size = src_field->size;
  copy_field->value = (void *) malloc(src_field->size);
  memcpy(copy_field->value, src_field->value, src_field->size);

  return (void *) copy_field;
}

void field_setValue(field_t* field, void* val)
{
  field->value = val;
}

void* field_getValue(field_t *field)
{
  return field->value;
}
