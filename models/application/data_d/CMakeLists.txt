#------------------------------------------------------------------------------
# CMake file for WSNET Models with multiple targets.
#
# Author: Luiz Henrique Suraty Filho
# ------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.11.0)

set(MODEL_SUB_TYPE data_d)

# -----------------------------------------------------------------------------
# Set the name of available models
# ----------------------------------------------------------------------------
set(${MODEL_SUB_TYPE}_MODELS    ght
                                gossip
                                lbdd
                                data_d_sink
                                data_d_source
                                xy
                                )   
                                
set(SUB_TYPES_LIST "${${MODEL_SUB_TYPE}_MODELS}")     

# -----------------------------------------------------------------------------
# Add the subdirs and configure the install command for all models 
# -----------------------------------------------------------------------------
add_custom_target(${MODEL_TYPE}_${MODEL_SUB_TYPE}-install)

foreach(SUB_MOD_NAME ${SUB_TYPES_LIST})
    add_subdirectory(${SUB_MOD_NAME})
    add_dependencies(${MODEL_TYPE}_${MODEL_SUB_TYPE}-install ${MODEL_TYPE}_${SUB_MOD_NAME}-install)
endforeach()