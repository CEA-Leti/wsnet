#------------------------------------------------------------------------------
# WSNETExecutable.cmake
#
# CMake file for WSNET project which specifies the executable and installation
# settings for each supported platform and build configuration.
#
# Author: Luiz Henrique Suraty Filho
#------------------------------------------------------------------------------

# -----------------------------------------------------------------------------
# Add executable with install properties
# -----------------------------------------------------------------------------
macro(wsnet_add_executable)

    # Create distclean targets
    include(WSNETSystemConfig)
    wsnet_create_distclean_target()
    
    # Get all sources and headers defined
    set(WSNET_EXECUTABLE_SOURCES_AND_HEADERS ${WSNET_SOURCES} ${WSNET_HEADERS})

    # Add target for wsnet executable 
    add_executable(wsnet "${WSNET_EXECUTABLE_SOURCES_AND_HEADERS}")
    
    # Add target for installation of wsnet executable 
    install(TARGETS wsnet DESTINATION "${WSNET_INSTALLED_PATH}/bin" COMPONENT kernel)
    
    # Add the config.xsd file to the installed/shared folder
    install(FILES "${WSNET_SRC_PATH}/kernel/src/configuration_parser/config.xsd" DESTINATION "${WSNET_INSTALLED_PATH}/share")
    
    # Add target for installation of wsnet
    add_custom_target(kernel-install
      DEPENDS wsnet
      COMMAND 
           "${CMAKE_COMMAND}" -DCMAKE_INSTALL_COMPONENT=kernel -P "${CMAKE_BINARY_DIR}/cmake_install.cmake"
    )
        
endmacro()