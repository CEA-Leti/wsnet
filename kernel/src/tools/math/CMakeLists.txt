#------------------------------------------------------------------------------
# CMake file for WSNET Data Structures.
#
# Author: Luiz Henrique Suraty Filho
# ------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.11.0)

# -----------------------------------------------------------------------------
# Include the cmake auxiliary file for data structures
# -----------------------------------------------------------------------------
include(WSNETCommons)

# -----------------------------------------------------------------------------
# Get the name of available model types (folders)
# -----------------------------------------------------------------------------
subdirlist(WSNET_TOOLS_MATH "${CMAKE_CURRENT_LIST_DIR}")

# -----------------------------------------------------------------------------
# Add the target for data structures
# -----------------------------------------------------------------------------
add_custom_target(math)
add_dependencies(tools math)

# -----------------------------------------------------------------------------
# Add the data structures
# -----------------------------------------------------------------------------
foreach(WSNET_TOOLS_MATH_NAME ${WSNET_TOOLS_MATH})
    add_subdirectory(${WSNET_TOOLS_MATH_NAME})
endforeach()