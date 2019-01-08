#------------------------------------------------------------------------------
# CMake file for WSNET Models with multiple targets.
#
# Author: Luiz Henrique Suraty Filho
# ------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.11.0)

# -----------------------------------------------------------------------------
# Configure the model variables
# -----------------------------------------------------------------------------

# The name of the model
set(MODEL_NAME radio_802_15_4) 

# The extra external libraries used by the model
set(MODEL_EXTERNAL_LIBRARIES GMODULE2)

# The source files used by the model
set(MODEL_SOURCES ../src/radio_802_15_4.c) 

# The folder(s) where your local includes (.h files) are located
set(MODEL_LOCAL_INCLUDES )

# The local headers used by the model
set(MODEL_LOCAL_HEADERS ) 

# The WSNET libraries used by the model
set(MODEL_LIB_LOCAL_LINK )


# -----------------------------------------------------------------------------
# Verify if the target name exists
# -----------------------------------------------------------------------------
if(NOT MODEL_NAME)
    message(FATAL_ERROR "You must define a name for your model (Check CMakeLists.txt )")
endif()

# -----------------------------------------------------------------------------
# Add the target
# -----------------------------------------------------------------------------
add_custom_target(${MODEL_TYPE}_${MODEL_NAME})

# -----------------------------------------------------------------------------
# Set the name of available models
# ----------------------------------------------------------------------------
set(${MODEL_SUB_TYPE}_MODELS    868_bpsk
                                902_bpsk
                                2400_oqpsk
                                )   
                                
set(SUB_TYPES_LIST "${${MODEL_SUB_TYPE}_MODELS}")     

# -----------------------------------------------------------------------------
# Add the subdirs and configure the install command for all models 
# -----------------------------------------------------------------------------
add_custom_target(${MODEL_TYPE}_${MODEL_NAME}-install)

foreach(SUB_MOD_NAME ${SUB_TYPES_LIST})
    add_subdirectory(${SUB_MOD_NAME})
    add_dependencies(${MODEL_TYPE}_${MODEL_NAME}-install ${MODEL_TYPE}_${MODEL_NAME}_${SUB_MOD_NAME}-install)
endforeach()


