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
set(DATA_STRUCTURE_NAME heap) 

# The extra external libraries used by the data structure
set(DATA_STRUCTURE_EXTERNAL_LIBRARIES )

# The source files used by the data structure
set(DATA_STRUCTURE_SOURCES heap.c) 

# The folder(s) where your local includes (.h files) are located
set(DATA_STRUCTURE_LOCAL_INCLUDES ${WSNET_SRC_PATH}/kernel/include/data_structures/heap)

# The local headers used by the data structure
set(DATA_STRUCTURE_LOCAL_HEADERS ${DATA_STRUCTURE_LOCAL_INCLUDES}/heap.h) 

# The WSNET libraries used by the data structure
set(DATA_STRUCTURE_LIB_LOCAL_LINK )

# -----------------------------------------------------------------------------
# Add the data structure
# -----------------------------------------------------------------------------
set(DATA_STRUCTURE_ALL_SOURCES ${DATA_STRUCTURE_SOURCES} ${DATA_STRUCTURE_LOCAL_HEADERS})
wsnet_add_internal_library(${DATA_STRUCTURE_NAME} "${DATA_STRUCTURE_ALL_SOURCES}")

wsnet_include_all_internal_libs()

if(DATA_STRUCTURE_EXTERNAL_LIBRARIES)
    wsnet_find_external_libs(${DATA_STRUCTURE_NAME} "${DATA_STRUCTURE_EXTERNAL_LIBRARIES}")
endif()

if(DATA_STRUCTURE_LOCAL_INCLUDES)
    include_directories(${DATA_STRUCTURE_LOCAL_INCLUDES})
endif()