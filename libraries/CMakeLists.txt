#------------------------------------------------------------------------------
# CMake file for the internal librariess of WSNET.
#
# Author: Luiz Henrique Suraty Filho
# ------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.11.0)

# -----------------------------------------------------------------------------
# Add the subdirs
# -----------------------------------------------------------------------------
add_subdirectory(timer)
add_subdirectory(wiplan)
add_subdirectory(tests)

# -----------------------------------------------------------------------------
# Add the include folder
# -----------------------------------------------------------------------------
#set(WSNET_INCLUDE_DIRECTORIES libraries/ PARENT_SCOPE)

