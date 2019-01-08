#------------------------------------------------------------------------------
# WSNETBuidlConfig.cmake
#
# CMake file for WSNET build configurations
#
# Author: Luiz Henrique Suraty Filho
#------------------------------------------------------------------------------

# -----------------------------------------------------------------------------
# WSNET build configurations
# -----------------------------------------------------------------------------
macro(wsnet_build_config)
    
    # Check the OS
    include(WSNETSystemConfig)
    wsnet_check_os()
    
    # The version number
    set(WSNET_VERSION_MAJOR 3)
    set(WSNET_VERSION_MINOR 1)
    
    # Configure a header file to pass some of the CMake settings
    configure_file(
        ${WSNET_CMAKE_PATH}/templates/build_info.h.in
        ${PROJECT_BINARY_DIR}/wsnet_build_info.h
    )
    
endmacro()