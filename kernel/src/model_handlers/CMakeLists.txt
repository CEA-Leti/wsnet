#------------------------------------------------------------------------------
# CMake file for WSNET Internal Library.
#
# Author: Luiz Henrique Suraty Filho
# ------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.11.0)

# -----------------------------------------------------------------------------
# Configure the library variables
# -----------------------------------------------------------------------------

# The name of the library
set(INTERNAL_LIB_NAME model_handlers) 

# The extra external libraries used by the library
set(INTERNAL_LIB_EXTERNAL_LIBRARIES GMODULE2)

# The source files used by the library
set(INTERNAL_LIB_SOURCES ${WSNET_KERNEL_FOLDER}/src/model_handlers/energy.c 
                         ${WSNET_KERNEL_FOLDER}/src/model_handlers/interface.c
                         ${WSNET_KERNEL_FOLDER}/src/model_handlers/link.c
                         ${WSNET_KERNEL_FOLDER}/src/model_handlers/map.c
                         ${WSNET_KERNEL_FOLDER}/src/model_handlers/media_rxtx.cc
                         ${WSNET_KERNEL_FOLDER}/src/model_handlers/modulation.c
                         ${WSNET_KERNEL_FOLDER}/src/model_handlers/monitor.c
                         ${WSNET_KERNEL_FOLDER}/src/model_handlers/node_mobility.c
                         ${WSNET_KERNEL_FOLDER}/src/model_handlers/noise.c
                         ${WSNET_KERNEL_FOLDER}/src/model_handlers/topology.c
                         ${WSNET_KERNEL_FOLDER}/src/model_handlers/transceiver.c
                         ) 

# The folder(s) where your local includes (.h files) are located
set(INTERNAL_LIB_LOCAL_INCLUDES ${WSNET_KERNEL_FOLDER}/include/model_handlers)

# The local headers used by the library
set(INTERNAL_LIB_LOCAL_HEADERS ${WSNET_KERNEL_FOLDER}/include/model_handlers/energy.h 
                               ${WSNET_KERNEL_FOLDER}/include/model_handlers/interface.h
                               ${WSNET_KERNEL_FOLDER}/include/model_handlers/link.h
                               ${WSNET_KERNEL_FOLDER}/include/model_handlers/map.h
                               ${WSNET_KERNEL_FOLDER}/include/model_handlers/media_rxtx.h
                               ${WSNET_KERNEL_FOLDER}/include/model_handlers/modulation.h
                               ${WSNET_KERNEL_FOLDER}/include/model_handlers/monitor.h
                               ${WSNET_KERNEL_FOLDER}/include/model_handlers/node_mobility.h
                               ${WSNET_KERNEL_FOLDER}/include/model_handlers/noise.h
                               ${WSNET_KERNEL_FOLDER}/include/model_handlers/topology.h
                               ${WSNET_KERNEL_FOLDER}/include/model_handlers/transceiver.h
                               ) 

# The WSNET libraries used by the library
set(INTERNAL_LIB_LOCAL_LINK )

# -----------------------------------------------------------------------------
# Add the library
# -----------------------------------------------------------------------------
set(INTERNAL_LIB_ALL_SOURCES ${INTERNAL_LIB_SOURCES} ${INTERNAL_LIB_LOCAL_HEADERS})
wsnet_add_internal_library(${INTERNAL_LIB_NAME} "${INTERNAL_LIB_ALL_SOURCES}")

# -----------------------------------------------------------------------------
# Include all external and internal libs needed
# -----------------------------------------------------------------------------
wsnet_include_all_internal_libs()

if(INTERNAL_LIB_EXTERNAL_LIBRARIES)
    wsnet_find_external_libs(${INTERNAL_LIB_NAME} "${INTERNAL_LIB_EXTERNAL_LIBRARIES}")
endif()

if(INTERNAL_LIB_LOCAL_INCLUDES)
    target_include_directories(${INTERNAL_LIB_NAME} PRIVATE "${INTERNAL_LIB_LOCAL_INCLUDES}")
endif()

if(INTERNAL_LIB_LOCAL_LINK)
    target_link_libraries(${INTERNAL_LIB_NAME} "${INTERNAL_LIB_LOCAL_LINK}")
endif()