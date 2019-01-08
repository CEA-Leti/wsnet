/**
 *  \file   class.h
 *  \brief  Class declarations
 *  \author Guillaume Chelius & Loic lemaitre
 *  \date   2010
 **/
#ifndef __class__
#define __class__

#include <kernel/include/options.h>

#include <glib.h>
#include <gmodule.h>

#include <kernel/include/definitions/types.h>
#include <kernel/include/definitions/models.h>


/* ************************************************** */
/* ************************************************** */
typedef enum {
  CXX_IMPLEM = 0,
  C_IMPLEM = 1,
  PYTHON_IMPLEM = 2,
  RUBY_IMPLEM = 3
} class_implementation_t;

enum {
  CLASS_NON_EXISTENT_CLASS_ID = -1
};
/* ************************************************** */
/* ************************************************** */

/** \typedef object_array_t
 * \brief An array of objects containing its size and indexes
 **/
/** \struct _object_array
 * \brief An array of objects containing its size, indexes
 **/
typedef struct _object_array {
    int size;  /**< array size **/
    list_t *saved_indexes;
    hashtable_t *indexes;
    void **object; /**< array elements **/
} object_array_t;

typedef struct _library {
    gchar *name;
    GModule *module;
    gchar *file;
} library_t;


typedef struct _all_implem {
    class_implementation_t  implem_type;
} all_implem_t;

typedef struct _cxx_implem {
    class_implementation_t  implem_type;
    library_t library;
} cxx_implem_t;

typedef struct _c_implem {
    class_implementation_t  implem_type;
    library_t library;
} c_implem_t;

typedef struct _python_implem {
    class_implementation_t  implem_type;
} python_implem_t;

typedef struct _ruby_implem {
    class_implementation_t  implem_type;
} ruby_implem_t;


typedef union _implem {
    all_implem_t    all;
    cxx_implem_t    cxx;
    c_implem_t      c;
    python_implem_t python;
    ruby_implem_t   ruby;
} implem_t;


typedef union _methods {
    generic_methods_t       generic;
    generic_cpp_methods_t	  generic_cpp;
    shared_methods_t        shared;
    mobility_methods_t      mobility;
    monitor_methods_t       monitor;
    energy_methods_t	      energy;
    interface_methods_t     interface;
    pathloss_methods_t      pathloss;
    fading_methods_t        fading;
    shadowing_methods_t     shadowing;
    noise_methods_t         noise;
    interferences_methods_t interferences;
    intermodulation_methods_t intermodulation;
    modulation_methods_t    modulation;
    transceiver_methods_t   transceiver;
    mac_methods_t           mac;
    routing_methods_t       routing;
    application_methods_t   application;
    global_map_methods_t    global_map;
    map_methods_t 		      map;
    link_methods_t		      link;
} methods_t;


typedef struct _class {
    classid_t  id;
    char      *name;
    implem_t   implem;

    model_t   *model;
    methods_t *methods;

    array_t    nodearchs;
    array_t    mediums;
    array_t    environments;
    object_array_t objects;

    void * (*create_object) (call_t *to,  void *params);
    void (*destroy_object) (void * object);
    int (*init)      (call_t *to, void *params);
    int (*destroy)   (call_t *to);
    int (*bootstrap) (call_t *to);
    int (*bind)      (call_t *to, void *params);
    int (*unbind)    (call_t *to);
    int (*ioctl)     (call_t *to, int option, void *in, void **out);

#ifdef __cplusplus
    void **private_values;
#else //__cplusplus
    void     **private;
#endif //__cplusplus
} class_t;

typedef struct _class_array {
    int size;
    class_t *elts;
} class_array_t;

extern class_array_t classes;

#ifdef __cplusplus
extern "C"{
#endif //__cplusplus
/* ************************************************** */
/* ************************************************** */
int      get_classid_by_name   (char *name);
class_t *get_class_by_name     (char *name);
class_t *get_class_by_id       (classid_t id);

void     classes_clean         (void);
int      classes_destroy       (void);

void    *get_class_private_data(call_t *to);
void     set_class_private_data(call_t *to, void *data);

int      get_class_type        (call_t *to);

/**
 * \brief Return an array containing the classes that are up "to->class" in "to->object" (node).
 * \param to sould be {class id, node id}.
 * \return An array of class ids.
 **/
array_t *get_class_bindings_up(call_t *to);

/**
 * \brief Return an array containing the classes that are down "to->class" in "to->object" (node).
 * \param to sould be {class id, node id}.
 * \return An array of class ids.
 **/
array_t *get_class_bindings_down(call_t *to);

/**
 * \brief Give the implementation name form the number.
 * \param type, the implementation number.
 * \return Return the implementation string.
 **/
char *implem_type_to_str(int type);

array_t *get_application_classesid(call_t *to);
array_t *get_routing_classesid(call_t *to);
array_t *get_mac_classesid(call_t *to);
array_t *get_tranceiver_classesid(call_t *to);
array_t *get_interface_classesid(call_t *to);
array_t *get_sensor_classesid(call_t *to);
array_t *get_monitor_classesid(call_t *to);
classid_t get_energy_classid(call_t *to);
classid_t get_mobility_classid(call_t *to);
classid_t get_mobility_classid_for_node_id(nodeid_t id);
classid_t get_map_classid(void);
unsigned long class_index_hash(void *key);
int class_index_equal(void *key0, void *key1);
void * get_object(call_t *to);
void * get_first_object_binding_down_by_type(call_t *to, int type);
void * get_object_binding_down_by_type_and_name(call_t *to, int type, char * name);
#ifdef __cplusplus
}
#endif //__cplusplus

#endif // __class__
