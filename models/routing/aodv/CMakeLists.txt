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
set(MODEL_NAME aodv) 

# The definitions used by the model
set(MODEL_DEFINES )

# The extra external libraries used by the model
set(MODEL_EXTERNAL_LIBRARIES GMODULE2)

# The source files used by the model
set(MODEL_SOURCES   ${CMAKE_CURRENT_SOURCE_DIR}/src/aodv_routing_functions.c
                    ${CMAKE_CURRENT_SOURCE_DIR}/src/aodv_routing_print_logs.c
                    ${CMAKE_CURRENT_SOURCE_DIR}/src/aodv_routing_route_management.c
                    ${CMAKE_CURRENT_SOURCE_DIR}/src/aodv_routing_rreq_management.c
                    ${CMAKE_CURRENT_SOURCE_DIR}/src/aodv_routing.c
                    ${CMAKE_CURRENT_SOURCE_DIR}/src/buffer_management.c
                    ) 

# The folder(s) where your local includes (.h files) are located
set(MODEL_LOCAL_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/include/)

# The local headers used by the model
set(MODEL_LOCAL_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/include/aodv_routing_functions.h
                        ${CMAKE_CURRENT_SOURCE_DIR}/include/aodv_routing_print_logs.h
                        ${CMAKE_CURRENT_SOURCE_DIR}/include/aodv_routing_route_management.h
                        ${CMAKE_CURRENT_SOURCE_DIR}/include/aodv_routing_rreq_management.h
                        ${CMAKE_CURRENT_SOURCE_DIR}/include/aodv_routing_types.h
                        ${CMAKE_CURRENT_SOURCE_DIR}/include/buffer_management.h
                        ) 

# The WSNET libraries used by the model
set(MODEL_LIB_LOCAL_LINK )

# -----------------------------------------------------------------------------
# Add the model
# -----------------------------------------------------------------------------
include(WSNETModels)
wsnet_add_model()
