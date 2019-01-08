#------------------------------------------------------------------------------
# Top-level CMake file for WSNET.
#
# Author: Luiz Henrique Suraty Filho
# ------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.11.0)

# -----------------------------------------------------------------------------
# Config paths to auxiliary files 
# -----------------------------------------------------------------------------
set(WSNET_CMAKE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake" CACHE PATH "The path to the cmake directory")
set(WSNET_SRC_PATH "${CMAKE_CURRENT_LIST_DIR}" CACHE PATH "The path to the wsnet source directory")

set(WSNET_INSTALLED_PATH "$ENV{WSNET_INSTALLED_PATH}" CACHE PATH "The path to the wsnet installation directory")

if(NOT WSNET_INSTALLED_PATH)
    message(STATUS "Using ${CMAKE_CURRENT_LIST_DIR}/installed as install path")
    set(WSNET_INSTALLED_PATH "${CMAKE_CURRENT_LIST_DIR}/installed" CACHE PATH "The path to the wsnet installation directory" FORCE) 
endif()   

list(APPEND CMAKE_MODULE_PATH ${WSNET_CMAKE_PATH})
list(APPEND CMAKE_MODULE_PATH ${WSNET_CMAKE_PATH}/Modules)

# -----------------------------------------------------------------------------
# Set the correct CMake policies
# -----------------------------------------------------------------------------
include(WSNETPolicy)
wsnet_policy()

# -----------------------------------------------------------------------------
# Declare the name of the project
# -----------------------------------------------------------------------------
project(WSNET)

# -----------------------------------------------------------------------------
# Load all auxiliary files 
# -----------------------------------------------------------------------------
include(WSNETSystemConfig)
include(WSNETBuildConfig)
include(WSNETCompilerSettings)
include(WSNETDependencies)
include(WSNETExecutable)
include(WSNETTests)

# -----------------------------------------------------------------------------
# Add ccache(if possible), compiler options, linker options and definitions
# -----------------------------------------------------------------------------
wsnet_try_ccache()
wsnet_add_compiler_options()
wsnet_add_linker_options()
wsnet_add_defitions()

# -----------------------------------------------------------------------------
# Build configurations
# -----------------------------------------------------------------------------
wsnet_build_config()

# -----------------------------------------------------------------------------
# Test configurations
# -----------------------------------------------------------------------------
wsnet_test_config_core()

# -----------------------------------------------------------------------------
# Add the subdirs of the project
# -----------------------------------------------------------------------------
add_subdirectory(libraries)
add_subdirectory(kernel)
add_subdirectory(models)



