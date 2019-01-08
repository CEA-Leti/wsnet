#------------------------------------------------------------------------------
# CMake file for WSNET Models.
#
# Author: Luiz Henrique Suraty Filho
# ------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.11.0)

# -----------------------------------------------------------------------------
# Configure the model variables
# -----------------------------------------------------------------------------

# The name of your  model
set(MODEL_NAME multiband_rf) 

# The definitions used by the  model
set(MODEL_DEFINES ) 

# The extra  libraries used by your  model 
set(MODEL_EXTERNAL_LIBRARIES GMODULE2)

# The source files used by your  model   
set(MODEL_SOURCES src/multiband_rf_spectrum_model.cc
                  src/multiband_rf_spectrum_model_api.cc
                  ${WSNET_SRC_PATH}/kernel/src/data_structures/interval_tree/redblack_interval_tree.cc   
) 

# The folder(s) where your local includes (.h files) are located
set(MODEL_LOCAL_INCLUDES include/)

# The headers used by your  model   
set(MODEL_LOCAL_HEADERS ${MODEL_LOCAL_INCLUDES}/multiband_rf_spectrum_model.h
                        ${WSNET_SRC_PATH}/kernel/include/data_structures/interval_tree/redblack_interval_tree.h
) 

# The WSNET libraries used by the model
set(MODEL_LIB_LOCAL_LINK )

# -----------------------------------------------------------------------------
# Add the model
# -----------------------------------------------------------------------------
include(WSNETModels)
wsnet_add_model()
