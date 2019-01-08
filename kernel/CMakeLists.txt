#------------------------------------------------------------------------------
# CMake file for the kernel of WSNET.
#
# Author: Luiz Henrique Suraty Filho
# ------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.11.0)

# -----------------------------------------------------------------------------
# Add the subdirs
# -----------------------------------------------------------------------------
set(WSNET_KERNEL_FOLDER ${WSNET_SRC_PATH}/kernel)

add_subdirectory(src)
add_subdirectory(tests)