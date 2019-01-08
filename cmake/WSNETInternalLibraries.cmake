#------------------------------------------------------------------------------
# WSNETInternalLibraries.cmake
#
# CMake file for WSNET internal libraries modules which specifies the library and installation
# settings for each supported platform and build configuration.
#
# Author: Luiz Henrique Suraty Filho
#------------------------------------------------------------------------------

# -----------------------------------------------------------------------------
# Add internal library
# -----------------------------------------------------------------------------
macro(wsnet_add_internal_library INTERNAL_LIB_NAME INTERNAL_LIB_SOURCES)

    add_library(${INTERNAL_LIB_NAME} STATIC ${INTERNAL_LIB_SOURCES})
    
    install(TARGETS ${INTERNAL_LIB_NAME} DESTINATION "${WSNET_INSTALLED_PATH}/lib")
    
    set_property(DIRECTORY ${WSNET_SRC_PATH} APPEND PROPERTY WSNET_INTERNAL_LIBRARIES ${INTERNAL_LIB_NAME})
    
endmacro()
