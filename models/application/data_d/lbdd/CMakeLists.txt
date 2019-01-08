#------------------------------------------------------------------------------
# CMake file for WSNET Models.
#
# Author: Luiz Henrique Suraty Filho
# ------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.11.0)

# -----------------------------------------------------------------------------
# Configure the model variables
# -----------------------------------------------------------------------------

# The name of the model
set(MODEL_NAME lbdd) 

# The definitions used by the model
set(MODEL_DEFINES )

# The extra external libraries used by the model
set(MODEL_EXTERNAL_LIBRARIES GMODULE2)

# The source files used by the model
set(MODEL_SOURCES src/lbdd_sensor.c) 

# The folder(s) where your local includes (.h files) are located
set(MODEL_LOCAL_INCLUDES ../include)

# The local headers used by the model
set(MODEL_LOCAL_HEADERS ${MODEL_LOCAL_INCLUDES}/data_d_common.h) 

# The WSNET libraries used by the model
set(MODEL_LIB_LOCAL_LINK )

# -----------------------------------------------------------------------------
# Add the model
# -----------------------------------------------------------------------------
include(WSNETModels)
wsnet_add_model()