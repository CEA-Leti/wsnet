#------------------------------------------------------------------------------
# CMake file for WSNET Data Structures.
#
# Author: Luiz Henrique Suraty Filho
# ------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.11.0)

# -----------------------------------------------------------------------------
# Declare the name of the project
# -----------------------------------------------------------------------------
project(tools)

# -----------------------------------------------------------------------------
# Include the cmake auxiliary file for data structures
# -----------------------------------------------------------------------------
include(WSNETCommons)

# -----------------------------------------------------------------------------
# Get the name of available model types (folders)
# -----------------------------------------------------------------------------
subdirlist(WSNET_TOOLS "${CMAKE_CURRENT_LIST_DIR}")

# -----------------------------------------------------------------------------
# Add the target for data structures
# -----------------------------------------------------------------------------
add_custom_target(tools)
add_dependencies(kernel tools)

# -----------------------------------------------------------------------------
# Add the data structures
# -----------------------------------------------------------------------------
foreach(WSNET_TOOLS_NAME ${WSNET_TOOLS})
    add_subdirectory(${WSNET_TOOLS_NAME})
endforeach()