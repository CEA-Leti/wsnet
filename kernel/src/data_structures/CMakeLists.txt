#------------------------------------------------------------------------------
# CMake file for WSNET Data Structures.
#
# Author: Luiz Henrique Suraty Filho
# ------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.11.0)

# -----------------------------------------------------------------------------
# Declare the name of the project
# -----------------------------------------------------------------------------
project(data_structures)

# -----------------------------------------------------------------------------
# Include the cmake auxiliary file for data structures
# -----------------------------------------------------------------------------
include(WSNETCommons)

# -----------------------------------------------------------------------------
# Get the name of available model types (folders)
# -----------------------------------------------------------------------------
subdirlist(WSNET_DATA_STRUCTURES "${CMAKE_CURRENT_LIST_DIR}")

# -----------------------------------------------------------------------------
# Add the target for data structures
# -----------------------------------------------------------------------------
add_custom_target(data_structures)
add_dependencies(kernel data_structures)

# -----------------------------------------------------------------------------
# Add the data structures
# -----------------------------------------------------------------------------
foreach(WSNET_DATA_STRUCTURE_NAME ${WSNET_DATA_STRUCTURES})
    add_subdirectory(${WSNET_DATA_STRUCTURE_NAME})
endforeach()