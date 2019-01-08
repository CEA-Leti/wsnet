#------------------------------------------------------------------------------
# CMake file for WSNET Internal Library.
#
# Author: Luiz Henrique Suraty Filho
# ------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.11.0)

# -----------------------------------------------------------------------------
# Configure the library variables
# -----------------------------------------------------------------------------

# The name of the library
set(INTERNAL_LIB_NAME configuration_parser) 

# The extra external libraries used by the library
set(INTERNAL_LIB_EXTERNAL_LIBRARIES GMODULE2 LibXml2)

# The source files used by the library
set(INTERNAL_LIB_SOURCES ${WSNET_KERNEL_FOLDER}/src/configuration_parser/class_configuration.c 
                         ${WSNET_KERNEL_FOLDER}/src/configuration_parser/configuration.c
                         ${WSNET_KERNEL_FOLDER}/src/configuration_parser/environment_configuration.c
                         ${WSNET_KERNEL_FOLDER}/src/configuration_parser/grouparch_configuration.c
                         ${WSNET_KERNEL_FOLDER}/src/configuration_parser/medium_configuration.c
                         ${WSNET_KERNEL_FOLDER}/src/configuration_parser/nodearch_configuration.c
                         ${WSNET_KERNEL_FOLDER}/src/configuration_parser/param.c
                         ${WSNET_KERNEL_FOLDER}/src/configuration_parser/simulation_configuration.c
                         ) 

# The folder(s) where your local includes (.h files) are located
set(INTERNAL_LIB_LOCAL_INCLUDES ${WSNET_KERNEL_FOLDER}/include/configuration_parser)

# The local headers used by the library
set(INTERNAL_LIB_LOCAL_HEADERS ${WSNET_KERNEL_FOLDER}/include/configuration_parser/class_configuration.h 
                               ${WSNET_KERNEL_FOLDER}/include/configuration_parser/environment_configuration.h
                               ${WSNET_KERNEL_FOLDER}/include/configuration_parser/grouparch_configuration.h
                               ${WSNET_KERNEL_FOLDER}/include/configuration_parser/medium_configuration.h
                               ${WSNET_KERNEL_FOLDER}/include/configuration_parser/param.h
                               ${WSNET_KERNEL_FOLDER}/include/configuration_parser/simulation_configuration.h
                               ${WSNET_KERNEL_FOLDER}/include/configuration_parser/xmlparser.h
	                           ) 

# The WSNET libraries used by the library
set(INTERNAL_LIB_LOCAL_LINK )

# -----------------------------------------------------------------------------
# Add the library
# -----------------------------------------------------------------------------
set(INTERNAL_LIB_ALL_SOURCES ${INTERNAL_LIB_SOURCES} ${INTERNAL_LIB_LOCAL_HEADERS})
wsnet_add_internal_library(${INTERNAL_LIB_NAME} "${INTERNAL_LIB_ALL_SOURCES}")

# -----------------------------------------------------------------------------
# Include all external and internal libs needed
# -----------------------------------------------------------------------------
wsnet_include_all_internal_libs()

if(INTERNAL_LIB_EXTERNAL_LIBRARIES)
    wsnet_find_external_libs(${INTERNAL_LIB_NAME} "${INTERNAL_LIB_EXTERNAL_LIBRARIES}")
endif()

if(INTERNAL_LIB_LOCAL_INCLUDES)
    target_include_directories(${INTERNAL_LIB_NAME} PRIVATE "${INTERNAL_LIB_LOCAL_INCLUDES}")
endif()

if(INTERNAL_LIB_LOCAL_LINK)
    target_link_libraries(${INTERNAL_LIB_NAME} "${INTERNAL_LIB_LOCAL_LINK}")
endif()