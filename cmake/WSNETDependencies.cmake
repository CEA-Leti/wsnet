#------------------------------------------------------------------------------
# WSNETDependencies.cmake
#
# CMake file for WSNET project which specifies all dependencies
# that are not directly included in the WSNET distribution.
#
# Author: Luiz Henrique Suraty Filho
#------------------------------------------------------------------------------

# -----------------------------------------------------------------------------
# Show library dependencies 
# -----------------------------------------------------------------------------
function(wsnet_show_dependency WSNET_SHOW_DEPENDENCY_DEP)
	get_property(DISPLAYED GLOBAL PROPERTY WSNET_${WSNET_SHOW_DEPENDENCY_DEP}_DISPLAYED)
	if (NOT DISPLAYED)
		set_property(GLOBAL PROPERTY WSNET_${WSNET_SHOW_DEPENDENCY_DEP}_DISPLAYED TRUE)
		message(STATUS "${WSNET_SHOW_DEPENDENCY_DEP} headers: ${${WSNET_SHOW_DEPENDENCY_DEP}_INCLUDE_DIRS}")
		message(STATUS "${WSNET_SHOW_DEPENDENCY_DEP} lib   : ${${WSNET_SHOW_DEPENDENCY_DEP}_LIBRARIES}")
		if (NOT("${${WSNET_SHOW_DEPENDENCY_DEP}_DLLS}" STREQUAL ""))
			message(STATUS "${WSNET_SHOW_DEPENDENCY_DEP} dll   : ${${WSNET_SHOW_DEPENDENCY_DEP}_DLLS}")
		endif()
	endif()
endfunction()

# -----------------------------------------------------------------------------
# Find and apply library dependencies 
# -----------------------------------------------------------------------------
function(wsnet_find_external_libs WSNET_FIND_EXTERNAL_LIBS_TARGET_NAME WSNET_FIND_EXTERNAL_LIBS_LIST)
   
    foreach(WSNET_FIND_EXTERNAL_LIBS_REQUIRED_LIB ${WSNET_FIND_EXTERNAL_LIBS_LIST})
        
        find_package(${WSNET_FIND_EXTERNAL_LIBS_REQUIRED_LIB} REQUIRED)
        
        # Some Libs does not have the name on UPPER
        string(TOUPPER ${WSNET_FIND_EXTERNAL_LIBS_REQUIRED_LIB} WSNET_FIND_EXTERNAL_LIBS_REQUIRED_LIB)

        if (${WSNET_FIND_EXTERNAL_LIBS_REQUIRED_LIB}_FOUND)
            set(INCLUDEDIR ${${WSNET_FIND_EXTERNAL_LIBS_REQUIRED_LIB}_INCLUDE_DIR})
    		target_include_directories(${WSNET_FIND_EXTERNAL_LIBS_TARGET_NAME} SYSTEM PUBLIC ${${WSNET_FIND_EXTERNAL_LIBS_REQUIRED_LIB}_INCLUDE_DIR})
    		target_link_libraries(${WSNET_FIND_EXTERNAL_LIBS_TARGET_NAME} ${${WSNET_FIND_EXTERNAL_LIBS_REQUIRED_LIB}_LIBRARIES})
    	elseif ()
    		message(FATAL_ERROR "${WSNET_FIND_EXTERNAL_LIBS_REQUIRED_LIB} library not found")
    	endif()
        	
    endforeach()
    
endfunction()


# -----------------------------------------------------------------------------
# Include all internal includes and libs needed by the project
# -----------------------------------------------------------------------------
macro(wsnet_include_all_internal_libs)
    include_directories(${WSNET_INCLUDE_DIRECTORIES})
    include_directories(${CMAKE_BINARY_DIR})
    include_directories(${WSNET_SRC_PATH})
endmacro()


# -----------------------------------------------------------------------------
# Link all internal libraries
# -----------------------------------------------------------------------------
macro(wsnet_link_all_internal_libs)
    get_property(LINK_INTERNAL_LIBRARIES DIRECTORY ${WSNET_SRC_PATH} PROPERTY WSNET_INTERNAL_LIBRARIES)
    # --whole-archive is to link the whole archive even if it is not used by kernel's main as it could be used by models
    # --start-group is to solve circular dependency
    target_link_libraries(wsnet -Wl,--start-group -Wl,--whole-archive ${LINK_INTERNAL_LIBRARIES} -Wl,--end-group -Wl,--no-whole-archive)
endmacro()


# -----------------------------------------------------------------------------
# Include and link external includes and libs needed by the project
# -----------------------------------------------------------------------------
macro(wsnet_include_all_external_libs)
    set(WSNET_USED_LIBS   M GMODULE2 LibXml2)
    wsnet_find_external_libs(wsnet "${WSNET_USED_LIBS}")    
endmacro()

macro(wsnet_include_external_libs LIBS_TARGET_NAME LIBS_LIST)
    message(STATUS "Libs ${LIBS_LIST}")
    wsnet_find_external_libs(${LIBS_TARGET_NAME} ${LIBS_LIST})
endmacro()



