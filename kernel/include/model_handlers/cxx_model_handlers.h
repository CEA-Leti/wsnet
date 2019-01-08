/**
 *  \file   cxx_model_handlers.h
 *  \brief  A handler of C++ models.
 *          Any new C++ model should include this file and implement at least the create_object and destroy_object functions.
 *  \author Luiz Henrique Suraty Filho
 *  \date   July 2017
 *  \version 1.0
 **/

#ifndef WSNET_CORE_MODEL_HANDLERS_CXX_MODEL_HANDLERS_H_
#define WSNET_CORE_MODEL_HANDLERS_CXX_MODEL_HANDLERS_H_

#include <kernel/include/definitions/types.h>
#include <kernel/include/definitions/models.h>


// Use this define to cast the pointer to the object (obj) of type (class) cla.
#define TO_CPP(obj,cla) (reinterpret_cast<cla*>(obj))

// Use this define get the pointer from a C++ object(obj)
#define TO_C(obj)   (reinterpret_cast<void*>(obj))

#ifdef __cplusplus
extern "C"{
#endif

/**
 * \brief The create-object interface, responsible for creating an object of the class and is called by WSNET simulation core during simulation configurations
 * \fn void *create_object(call_t *to, void *params)
 * \param to is a pointer to the called class for the given node
 * \param params is a pointer to the parameters read from the XML configuration file
 * \return pointer to the newly created object
 **/
void *create_object(call_t *to, void *params);

/**
 * \brief The destroy-object interface, responsible for destroying an object and is called by WSNET simulation core at the end of the simulation (cleaning period)
 * \fn void destroy_object(void *object)
 * \param object is the pointer to the object to be destroyed
 **/
void destroy_object(void *object);

/** \brief The bootstrap input-interface which is automatically called by the WSNet simulation core at the beginning of the simulation.
 *  \fn int bootstrap(call_t *to)
 *  \param to is a pointer to the called class for the given node
 *  \return SUCCESSFUL if success, UNSUCCESSFUL otherwise
 **/
int bootstrap(call_t *to);

#ifdef __cplusplus
}
#endif


#endif // WSNET_CORE_MODEL_HANDLERS_CXX_MODEL_HANDLERS_H_
