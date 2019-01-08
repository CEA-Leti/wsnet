#------------------------------------------------------------------------------
# CMake file for the models of WSNET.
#
# Author: Luiz Henrique Suraty Filho
# ------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.11.0)

# -----------------------------------------------------------------------------
# Add the target for models
# -----------------------------------------------------------------------------
add_custom_target(models)
set(WSNET_MODELS_FOLDER ${WSNET_SRC_PATH}/models)
# -----------------------------------------------------------------------------
# Include the cmake auxiliary file for Models
# -----------------------------------------------------------------------------
include(WSNETModels)
include(WSNETCommons)


# -----------------------------------------------------------------------------
# Get the name of available model types (folders)
# -----------------------------------------------------------------------------
subdirlist(WSNET_MODELS_TYPES "${CMAKE_CURRENT_LIST_DIR}")

# -----------------------------------------------------------------------------
# Add target for installation of type of model
# -----------------------------------------------------------------------------
add_custom_target(models-install)

# -----------------------------------------------------------------------------
# Add the subdirs and configure the install command for all models 
# -----------------------------------------------------------------------------
foreach(WSNET_MODEL_TYPE_NAME ${WSNET_MODELS_TYPES})
    add_subdirectory(${WSNET_MODEL_TYPE_NAME})
    add_dependencies(models-install ${WSNET_MODEL_TYPE_NAME}-install)
endforeach()



