#------------------------------------------------------------------------------
# CMake file for WSNET Models.
#
# Author: Luiz Henrique Suraty Filho
# ------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.11.0)

# -----------------------------------------------------------------------------
# Include the cmake auxiliary file for Models
# -----------------------------------------------------------------------------
include(WSNETModels)
include(WSNETCommons)


# -----------------------------------------------------------------------------
# Add the target for this type of models
# -----------------------------------------------------------------------------
get_filename_component(MODEL_TYPE ${CMAKE_CURRENT_SOURCE_DIR} NAME)
add_custom_target(${MODEL_TYPE})
add_dependencies(models ${MODEL_TYPE})

# -----------------------------------------------------------------------------
# Get the name of available models (folders)
# ----------------------------------------------------------------------------
subdirlist(${MODEL_TYPE}_MODELS "${CMAKE_CURRENT_LIST_DIR}")        

# -----------------------------------------------------------------------------
# Add the subdirs and configure the install command for all models 
# -----------------------------------------------------------------------------
wsnet_add_model_subdir_and_install(${MODEL_TYPE} "${${MODEL_TYPE}_MODELS}")