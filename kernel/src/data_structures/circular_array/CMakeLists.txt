#------------------------------------------------------------------------------
# CMake file for WSNET data structures.
#
# Author: Luiz Henrique Suraty Filho
# ------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.11.0)

# -----------------------------------------------------------------------------
# Configure the data structure variables
# -----------------------------------------------------------------------------

# The name of the data structure
set(DATA_STRUCTURE_NAME circ_array) 

# The extra external libraries used by the data structure
set(DATA_STRUCTURE_EXTERNAL_LIBRARIES )

# The source files used by the data structure
set(DATA_STRUCTURE_SOURCES circ_array.c) 

# The folder(s) where your local includes (.h files) are located
set(DATA_STRUCTURE_LOCAL_INCLUDES ${WSNET_SRC_PATH}/kernel/include/data_structures/circular_array)

# The local headers used by the data structure
set(DATA_STRUCTURE_LOCAL_HEADERS ${DATA_STRUCTURE_LOCAL_INCLUDES}/circ_array.h) 

# The WSNET libraries used by the data structure
set(DATA_STRUCTURE_LIB_LOCAL_LINK )

# -----------------------------------------------------------------------------
# Add the data structure
# -----------------------------------------------------------------------------
wsnet_add_internal_library(${DATA_STRUCTURE_NAME} ${DATA_STRUCTURE_SOURCES} ${DATA_STRUCTURE_LOCAL_HEADERS})

wsnet_include_all_internal_libs()

if(DATA_STRUCTURE_EXTERNAL_LIBRARIES)
    wsnet_find_external_libs(${DATA_STRUCTURE_NAME} "${DATA_STRUCTURE_EXTERNAL_LIBRARIES}")
endif()

if(DATA_STRUCTURE_LOCAL_INCLUDES)
    include_directories(${DATA_STRUCTURE_LOCAL_INCLUDES})
endif()