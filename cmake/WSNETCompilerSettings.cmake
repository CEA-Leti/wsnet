#------------------------------------------------------------------------------
# WSNETCompilerSettings.cmake
#
# CMake file for WSNET project which specifies the compiler settings
# for each supported platform and build configuration.
#
# Author: Luiz Henrique Suraty Filho
#------------------------------------------------------------------------------

# Clang seeks to be command-line compatible with GCC as much as possible, so
# most of our compiler settings are common between GCC and Clang.
#
# These settings then end up spanning all POSIX platforms (Linux, OS X, BSD, etc)

# -----------------------------------------------------------------------------
# Use ccache if available
# -----------------------------------------------------------------------------
macro(wsnet_try_ccache)
    find_program(CCACHE_FOUND ccache)
    message(STATUS "Checking for ccache")
    if(CCACHE_FOUND)
    	message(STATUS "Checking for ccache -- found")
    	message(STATUS "Applying rules for ccache")
    	set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    	set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
    	message(STATUS "Applying rules for ccache - done")
    endif(CCACHE_FOUND)
endmacro()


# -----------------------------------------------------------------------------
# Add compilers options and flags for C
# -----------------------------------------------------------------------------
macro(wsnet_add_c_compiler_options)
	# Configuration-specific C compiler settings.	
    set(CMAKE_C_FLAGS_DEBUG            "-O0 -g -DWSNET_DEBUG" CACHE STRING "The Debug flag is set" FORCE)
	set(CMAKE_C_FLAGS_MINSIZEREL       "-Os -DNDEBUG -DWSNET_RELEASE" CACHE STRING "The MinSizeRel flag is set" FORCE)
	set(CMAKE_C_FLAGS_RELEASE          "-O3 -DNDEBUG -DWSNET_RELEASE" CACHE STRING "The Release flag is set" FORCE)
	set(CMAKE_C_FLAGS_RELWITHDEBINFO   "-O2 -g -DWSNET_RELEASE" CACHE STRING "The RelWithDebInfo flag is set" FORCE)
	
    # Disable Strict Aliasing
    # TODO: Comeback to this issue 
    list(APPEND WSNET_C_COMPILER_OPTIONS -fno-strict-aliasing)

	# Enables all the warnings about constructions that some users consider questionable
	# and that are easy to avoid. 
	list(APPEND WSNET_C_COMPILER_OPTIONS -Wall)
	
	# Enable some extra warning flags that are not enabled by -Wall.
	list(APPEND WSNET_C_COMPILER_OPTIONS -Wextra)
	
	# Turn off warnings for non used parameters
	# TODO : Clean code so that is no longer needed
	list(APPEND WSNET_C_COMPILER_OPTIONS -Wno-unused-parameter)
	
	# Treat at warnings-as-errors, which forces developers to fix warnings as they arise,
	# so they don't accumulate "to be fixed later".
	list(APPEND WSNET_C_COMPILER_OPTIONS -Werror)
	
	# Change gcc mode to gnu99, fmax and other functions better supported in this mode
    list(APPEND WSNET_C_COMPILER_OPTIONS -std=gnu99)
    
    add_compile_options("$<$<COMPILE_LANGUAGE:C>:${WSNET_C_COMPILER_OPTIONS}>")	
endmacro()

# -----------------------------------------------------------------------------
# Add compilers options and flags for C++
# -----------------------------------------------------------------------------
macro(wsnet_add_cxx_compiler_options)
	# Configuration-specific C++ compiler settings.	
	set(CMAKE_CXX_FLAGS_DEBUG          "-O0 -g -DWSNET_DEBUG" CACHE STRING "The Debug flag is set" FORCE)
	set(CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -DNDEBUG -DWSNET_RELEASE" CACHE STRING "The MinSizeRel flag is set" FORCE)
	set(CMAKE_CXX_FLAGS_RELEASE        "-O3 -DNDEBUG -DWSNET_RELEASE" CACHE STRING "The Release flag is set" FORCE)
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g -DWSNET_RELEASE" CACHE STRING "The RelWithDebInfo flag is set" FORCE)
	
	# Disable Strict Aliasing
    # TODO: Comeback to this issue 
    list(APPEND WSNET_CXX_COMPILER_OPTIONS -fno-strict-aliasing)

	# Enables all the warnings about constructions that some users consider questionable
	# and that are easy to avoid. 
	list(APPEND WSNET_CXX_COMPILER_OPTIONS -Wall)
	
	# Enable some extra warning flags that are not enabled by -Wall.
	list(APPEND WSNET_CXX_COMPILER_OPTIONS -Wextra)
	
	# Treat at warnings-as-errors, which forces developers to fix warnings as they arise,
	# so they don't accumulate "to be fixed later".
	list(APPEND WSNET_CXX_COMPILER_OPTIONS -Werror)
	
	# We enforce all C++ code to follow C++14, but with an eye on C++17.
	# As soon as an official ISO version of the C++17 is realeased we plan 
	# to move toward C++17 
    list(APPEND WSNET_CXX_COMPILER_OPTIONS -std=c++14)
    
    add_compile_options("$<$<COMPILE_LANGUAGE:CXX>:${WSNET_CXX_COMPILER_OPTIONS}>")
endmacro()

# -----------------------------------------------------------------------------
# Add compilers options according to platform
# -----------------------------------------------------------------------------
macro(wsnet_add_compiler_options)
    if (("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU") OR ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang"))
    
        # Set the C compiler options and flags
        wsnet_add_c_compiler_options()

        # Set the C++ compiler options and flags
        wsnet_add_cxx_compiler_options()
    	
    	# Set a default build type for single-configuration
        # CMake generators if no build type is set.
        if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
           set(CMAKE_BUILD_TYPE Debug CACHE STRING "Default build type is Debug" FORCE)
        endif(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
    	
    # If you don't have GCC or Clang then you are on your own.  Good luck!
    else ()
    	message(WARNING "Your compiler is not tested, if you run into any issues, we'd welcome any patches.")
    endif ()    
endmacro()

# -----------------------------------------------------------------------------
# Add definitions
# -----------------------------------------------------------------------------
macro(wsnet_add_defitions)
    add_definitions(-DDATADIR="${WSNET_INSTALLED_PATH}/share")
    add_definitions(-DLIBDIR="${WSNET_INSTALLED_PATH}/lib")
    add_definitions(-DWSNET3)
endmacro()

# -----------------------------------------------------------------------------
# Add linker options according to platform
# -----------------------------------------------------------------------------
macro(wsnet_add_linker_options)
       # Add flags for shared libraries
       wsnet_add_linker_shared_library_flags()
endmacro()
# -----------------------------------------------------------------------------
# Add sanitize flags 
# -----------------------------------------------------------------------------
macro(wsnet_add_sanitize_flags)
    if (SANITIZE)
        message(STATUS "Using sanitize flags")
    	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-omit-frame-pointer -fsanitize=${SANITIZE}")
    	if (${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
    		set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -fsanitize-blacklist=${CMAKE_SOURCE_DIR}/sanitizer-blacklist.txt")
    	endif()
    endif()
endmacro()

# -----------------------------------------------------------------------------
# Add profiling flags 
# -----------------------------------------------------------------------------
macro(wsnet_add_profiling_flags)
    if (PROFILING AND (("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU") OR ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")))
    	set(CMAKE_CXX_FLAGS "-g ${CMAKE_CXX_FLAGS}")
    	set(CMAKE_C_FLAGS "-g ${CMAKE_C_FLAGS}")
    	add_definitions(-DWSNET_PROFILING_GPERF)
    	set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -lprofiler")
    #	set(CMAKE_STATIC_LINKER_FLAGS "${CMAKE_STATIC_LINKER_FLAGS} -lprofiler")
    	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lprofiler")
    endif ()
endmacro()

# -----------------------------------------------------------------------------
# Add undefined dynamic_lookup flag for shared libraries in APPLE
# -----------------------------------------------------------------------------
macro(wsnet_add_linker_shared_library_flags)
    if ("${CMAKE_C_COMPILER_ID}" MATCHES "Clang")
        set(CMAKE_SHARED_MODULE_CREATE_C_FLAGS "${CMAKE_SHARED_MODULE_CREATE_C_FLAGS} -undefined dynamic_lookup")
    endif ()
    if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
        set(CMAKE_SHARED_MODULE_CREATE_CXX_FLAGS "${CMAKE_SHARED_MODULE_CREATE_CXX_FLAGS} -undefined dynamic_lookup")
    endif ()
endmacro()
