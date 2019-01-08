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
set(INTERNAL_LIB_NAME timer) 

# The extra external libraries used by the library
set(INTERNAL_LIB_EXTERNAL_LIBRARIES )

# The source files used by the library
set(INTERNAL_LIB_SOURCES timer.c) 

# The folder(s) where your local includes (.h files) are located
set(INTERNAL_LIB_LOCAL_INCLUDES .)

# The local headers used by the library
set(INTERNAL_LIB_LOCAL_HEADERS ${INTERNAL_LIB_LOCAL_INCLUDES}/timer.h) 

# The WSNET libraries used by the library
set(INTERNAL_LIB_LOCAL_LINK )

# -----------------------------------------------------------------------------
# Verify if the target name exists
# -----------------------------------------------------------------------------
if(NOT INTERNAL_LIB_NAME)
    message(FATAL_ERROR "You must define a name for your library (Check CMakeLists.txt )")
endif()


# -----------------------------------------------------------------------------
# Config paths to auxiliary files 
# -----------------------------------------------------------------------------
string(TOUPPER ${INTERNAL_LIB_NAME} INTERNAL_LIB_NAME_UPPER)
set(WSNET_INTERNAL_LIB_${INTERNAL_LIB_NAME_UPPER}_PATH "${CMAKE_CURRENT_LIST_DIR}" CACHE PATH "The path to the library directory")

# -----------------------------------------------------------------------------
# Declare the name of the project
# -----------------------------------------------------------------------------
project(${INTERNAL_LIB_NAME})

# -----------------------------------------------------------------------------
# Load all auxiliary files 
# -----------------------------------------------------------------------------
include(WSNETSystemConfig)
include(WSNETCompilerSettings)
include(WSNETDependencies)
include(WSNETInternalLibraries)

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
    include_directories(${INTERNAL_LIB_LOCAL_INCLUDES})
endif()

if(INTERNAL_LIB_LOCAL_LINK)
    target_link_libraries(${INTERNAL_LIB_NAME} "${INTERNAL_LIB_LOCAL_LINK}")
endif()