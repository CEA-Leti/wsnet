/**
 *  \file   mem_fs.h
 *  \brief  Fixed size memory management module.
 *  \author Guillaume Chelius
 *  \date   2009
 **/
#ifndef __mem_fs__
#define __mem_fs__

#ifdef __cplusplus
extern "C"{
#endif

/**
 * \brief Clean the mem_fs module. Called by the wsnet core.
 **/ 
void mem_fs_clean(void);


/**
 * \brief Declare a memory allocation slice. 
 * \param size the size of the memory bcoks that will be allocated using this slice.
 * \return an opaque pointer to the memory slice.
 **/
void *mem_fs_slice_declare(int size);


/**
 * \brief Allocate a memory block from a slice. 
 * \param slice the opaque pointer to the memory slice.
 * \return the allocated memory block.
 **/
void *mem_fs_alloc(void *slice);


/**
 * \brief Deallocate a memory block. 
 * \param slice the opaque pointer to the memory slice.
 * \return pointer the memory block to deallocate.
 **/
void mem_fs_dealloc(void *slice, void *pointer);

#ifdef __cplusplus
}
#endif

#endif //__mem_fs__

