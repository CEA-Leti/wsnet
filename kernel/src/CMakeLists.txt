#------------------------------------------------------------------------------
# CMake file for the kernel of WSNET.
#
# Author: Luiz Henrique Suraty Filho
# ------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.11.0)

# -----------------------------------------------------------------------------
# Add the target for kernel
# -----------------------------------------------------------------------------
add_custom_target(kernel)

# -----------------------------------------------------------------------------
# Add the subdirs
# -----------------------------------------------------------------------------
add_subdirectory(configuration_parser)
add_subdirectory(data_structures)
add_subdirectory(definitions)
add_subdirectory(tools)
add_subdirectory(model_handlers)
add_subdirectory(scheduler)

# -----------------------------------------------------------------------------
# Add the sources and headers 
# -----------------------------------------------------------------------------
set(WSNET_SOURCES 
            ${WSNET_KERNEL_FOLDER}/src/main.c
            ${WSNET_KERNEL_FOLDER}/src/modelutils.c
            )

set(WSNET_INCLUDE_DIRECTORIES
           ${WSNET_KERNEL_FOLDER}/include/configuration_parser
           ${WSNET_KERNEL_FOLDER}/include/definitions
           ${WSNET_KERNEL_FOLDER}/include/definitions/types
           ${WSNET_KERNEL_FOLDER}/include/definitions/types/interval
           ${WSNET_KERNEL_FOLDER}/include/definitions/types/waveform
           ${WSNET_KERNEL_FOLDER}/include/model_handlers
           ${WSNET_KERNEL_FOLDER}/include/scheduler
           
)
            
set(WSNET_HEADERS 
            
            )

# -----------------------------------------------------------------------------
# Add the wsnet executable
# -----------------------------------------------------------------------------
wsnet_add_executable()


# -----------------------------------------------------------------------------
# Include all external 
# -----------------------------------------------------------------------------
wsnet_include_all_external_libs()

# -----------------------------------------------------------------------------
# Include all internal libs needed 
# -----------------------------------------------------------------------------
wsnet_include_all_internal_libs()
wsnet_link_all_internal_libs()