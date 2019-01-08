#------------------------------------------------------------------------------
# WSNETSystemConfig.cmake
#
# CMake file for WSNET project to define provides system configurations
# for each supported platform and build configuration.
#
# Author: Luiz Henrique Suraty Filho
#------------------------------------------------------------------------------

# -----------------------------------------------------------------------------
# Determine the operating system
# -----------------------------------------------------------------------------
macro(wsnet_check_os)
    if (UNIX)
        if (APPLE)
            set(WSNET_OS_MAC_OS_X ON)
        else ()
            set(WSNET_OS_UNIX ON)
        endif ()
        message(STATUS "Configuring and generating files for ${CMAKE_SYSTEM_NAME}")
        set(WSNET_BUILD_OS ${CMAKE_SYSTEM_NAME})
    elseif (CMAKE_SYSTEM_NAME MATCHES "Windows")
        set(WSNET_OS_WINDOWS ON)
        message(STATUS "Configuring and generating files for ${CMAKE_SYSTEM_NAME}")
        set(WSNET_BUILD_OS ${CMAKE_SYSTEM_NAME})
    else ()
        message(FATAL_ERROR "Unknown OS '${CMAKE_SYSTEM_NAME}'")
    endif ()
endmacro()

# -----------------------------------------------------------------------------
# Creates a new target on the Makefile
# This target "distclean" is used to clean all files created by CMake
# Usage: make distclean
# -----------------------------------------------------------------------------
macro(wsnet_create_distclean_target)
    if(UNIX)
        if (NOT TARGET distclean)
            add_custom_target (distclean COMMENT "Cleaning source distribution")
            
            add_custom_command(
                COMMENT "Cleaning source distribution"
                COMMAND make
                ARGS -C ${CMAKE_CURRENT_BINARY_DIR} clean
                COMMAND find
                ARGS ${CMAKE_CURRENT_BINARY_DIR} -name "CMakeCache.txt" | xargs rm -rf
                COMMAND find
                ARGS ${CMAKE_CURRENT_BINARY_DIR} -name "CMakeFiles" | xargs rm -rf
                COMMAND find
                ARGS ${CMAKE_CURRENT_BINARY_DIR} -name "Makefile" | xargs rm -rf
                COMMAND find
                ARGS ${CMAKE_CURRENT_BINARY_DIR} -name "*.cmake" | xargs rm -rf
                COMMAND find
                ARGS ${CMAKE_CURRENT_SOURCE_DIR} -name "*.qm" | xargs rm -rf
                COMMAND rm
                ARGS -rf ${CMAKE_CURRENT_BINARY_DIR}/install_manifest.txt
                TARGET  distclean
            )
        endif()
    endif(UNIX) 
endmacro()

# -----------------------------------------------------------------------------
# Get the name of the user on the current session. 
# This name will be used to replace AUTHOR_NAME for new external modules
# -----------------------------------------------------------------------------
macro(wsnet_get_author_name)
    if(UNIX)
        if(NOT AUTHOR_NAME)
            execute_process(COMMAND getent passwd $ENV{USER} OUTPUT_VARIABLE GET_ENT_OUTPUT)

            string(REPLACE ":" ";" GET_ENT_OUTPUT ${GET_ENT_OUTPUT})
            
            list(GET GET_ENT_OUTPUT 4 GET_ENT_OUTPUT)
            
            string(REPLACE " " ";" GET_ENT_OUTPUT ${GET_ENT_OUTPUT})
            
            list(GET GET_ENT_OUTPUT 0 MYNAME)
            
            list(GET GET_ENT_OUTPUT 1 MYSURNAME)
            
            set(AUTHOR_NAME "${MYNAME} ${MYSURNAME}")
        endif()
    endif()
endmacro()
