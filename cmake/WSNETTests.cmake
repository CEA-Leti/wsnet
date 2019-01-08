#------------------------------------------------------------------------------
# WSNETTests.cmake
#
# CMake file for WSNET tests
#
# Author: Luiz Henrique Suraty Filho
#------------------------------------------------------------------------------

# -----------------------------------------------------------------------------
# WSNET configuration of GoogleTest for unit tests on Models
# -----------------------------------------------------------------------------
macro(wsnet_test_config_googletest)
	if (NOT WSNET_TEST_CONFIG_GOOGLETEST)
		# Look for your WSNET core build folder
		set(WSNET_BUILD_PATH "${WSNET_SRC_PATH}/build" CACHE PATH "The path to the wsnet build directory")
		set(WSNET_GOOGLETEST_BUILD_PATH "${WSNET_BUILD_PATH}/googletest-build" CACHE PATH "The path to the googletest build directory")
		set(WSNET_GOOGLETEST_DOWNLOAD_PATH "${WSNET_BUILD_PATH}/googletest-download" CACHE PATH "The path to the googletest download directory")
		set(WSNET_GOOGLETEST_SRC_PATH "${WSNET_BUILD_PATH}/googletest-src" CACHE PATH "The path to the googletest source directory")
	
		# Download and unpack googletest at configure time
		configure_file(${WSNET_CMAKE_PATH}/WSNETGoogleTest.CMakeLists.cmake.in googletest-download/CMakeLists.txt)
		execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
		                RESULT_VARIABLE result
		                WORKING_DIRECTORY ${WSNET_GOOGLETEST_DOWNLOAD_PATH} )
		                
		if(result)
		    message(FATAL_ERROR "CMake step for googletest failed: ${result}")
		endif()
		
		execute_process(COMMAND ${CMAKE_COMMAND} --build .
		                RESULT_VARIABLE result
		                WORKING_DIRECTORY ${WSNET_GOOGLETEST_DOWNLOAD_PATH} )
		if(result)
		    message(FATAL_ERROR "Build step for googletest failed: ${result}")
		endif()
	
		# Prevent overriding the parent project's compiler/linker
		# settings on Windows
		set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
		
		# The gtest/gtest_main targets carry header search path
		# dependencies automatically when using CMake 2.8.11 or
		# later. Otherwise we have to add them here ourselves.
		if (CMAKE_VERSION VERSION_LESS 2.8.11)
		    include_directories("${gtest_SOURCE_DIR}/include")
		endif()
	    
		set(WSNET_TEST_CONFIG_GOOGLETEST TRUE)
	endif()
		                 
endmacro()

# -----------------------------------------------------------------------------
# WSNET add GoogleTest directory
# -----------------------------------------------------------------------------
macro(wsnet_test_config_add_googletest)

    include(GoogleTest)

    if (NOT WSNET_GOOGLETEST_SRC_PATH)
    	wsnet_test_config_googletest()
    	
    	if (NOT WSNET_TEST_CONFIG_GOOGLETEST_ADDED)
    		# Add googletest directly to our build. This defines
    		# the gtest and gtest_main targets.
    		add_subdirectory(${WSNET_GOOGLETEST_SRC_PATH}
    		                 ${WSNET_GOOGLETEST_BUILD_PATH}
    		                 EXCLUDE_FROM_ALL)
    		
    		set(WSNET_TEST_CONFIG_GOOGLETEST_ADDED TRUE)
    	endif()
    	
    endif()
	
endmacro()

# -----------------------------------------------------------------------------
# WSNET global configuration of GoogleTest for unit tests
# -----------------------------------------------------------------------------
macro(wsnet_test_config_core)

    # Enable tests if not yet
    if(NOT CMAKE_TESTING_ENABLED)
        enable_testing()
        
        # create the log folder for cppcheck and valgrind
        execute_process(COMMAND mkdir - p ${WSNET_SRC_PATH}/build/log_cppcheck/
                COMMAND mkdir - p ${WSNET_SRC_PATH}/build/log_valgrind/
				OUTPUT_QUIET
                ERROR_QUIET
                )
    endif()
    
	wsnet_test_config_add_googletest()
	
endmacro()

# -----------------------------------------------------------------------------
# WSNET configuration of GoogleTest for unit tests on Models
# -----------------------------------------------------------------------------
macro(wsnet_test_config_model)

    # Enable tests if not yet
    if(NOT CMAKE_TESTING_ENABLED)
        enable_testing()
        
        # create the log folder for cppcheck and valgrind
        execute_process(COMMAND mkdir - p ${WSNET_SRC_PATH}/build/log_cppcheck/
                COMMAND mkdir - p ${WSNET_SRC_PATH}/build/log_valgrind/
				OUTPUT_QUIET
                ERROR_QUIET
                )
    endif()
	    
	wsnet_test_config_add_googletest()
endmacro()

# -----------------------------------------------------------------------------
# WSNET add unit tests
# -----------------------------------------------------------------------------
function(wsnet_add_unit_tests UNIT_TEST_NAME UNIT_TEST_SOURCES UNIT_TEST_INCLUDES)
    set(UNIT_TEST_NAME "unit_test_${UNIT_TEST_NAME}")
    
    wsnet_include_all_internal_libs()
       
    add_executable(${UNIT_TEST_NAME} ${UNIT_TEST_SOURCES})
    
    wsnet_find_external_libs(${UNIT_TEST_NAME} GMODULE2)
    
    # Verify if it is has lib links
    if(${ARGC} GREATER 0)
        set(UNIT_TEST_LIB_LINKS ${ARGN})
    endif()
    
    if(UNIT_TEST_INCLUDES)
        target_include_directories(${UNIT_TEST_NAME} PRIVATE "${UNIT_TEST_INCLUDES}")
    endif()
    
    if (UNIT_TEST_LIB_LINKS)
        target_link_libraries(${UNIT_TEST_NAME} gmock_main 
                                            -Wl,--start-group ${UNIT_TEST_LIB_LINKS} -Wl,--end-group
                                            )
    else()
        target_link_libraries(${UNIT_TEST_NAME} gmock_main)
    endif()
    
    include(WSNETTestsWrap)
    wsnet_replace_wrapped_functions(${UNIT_TEST_NAME} ${UNIT_TEST_SOURCES}) 
    
    gtest_add_tests(TARGET ${UNIT_TEST_NAME}
                    TEST_PREFIX unit_test_
                    TEST_LIST noArgsTest)
endfunction()

# -----------------------------------------------------------------------------
# WSNET add unit tests for models
# -----------------------------------------------------------------------------
macro(wsnet_add_unit_tests_model_link_libs UNIT_TEST_NAME)
    set(UNIT_TEST_EXTERNAL_LIBS  GMODULE2 LibXml2)
    wsnet_find_external_libs(${UNIT_TEST_NAME} "${UNIT_TEST_EXTERNAL_LIBS}")

    set(UNIT_TEST_LIB_LINKS configuration_parser
                        circ_array
                        hashtable
                        heap
                        interval_tree
                        list
                        mem_fs
                        sliding_window
                        definitions
                        tools_math_rng
                        model_handlers
                        scheduler
                        )
    
    foreach(UNIT_TEST_INTERNAL_LIB ${UNIT_TEST_LIB_LINKS})
    	if (NOT TARGET ${UNIT_TEST_INTERNAL_LIB})
    		add_library(${UNIT_TEST_INTERNAL_LIB} UNKNOWN IMPORTED)
    		set_property(TARGET ${UNIT_TEST_INTERNAL_LIB} PROPERTY IMPORTED_LOCATION "${WSNET_INSTALLED_PATH}/lib/lib${UNIT_TEST_INTERNAL_LIB}.a")
    	endif()
    endforeach()

endmacro()


# -----------------------------------------------------------------------------
# WSNET add unit tests for models
# -----------------------------------------------------------------------------
function(wsnet_add_unit_tests_model UNIT_TEST_NAME UNIT_TEST_SOURCES UNIT_TEST_INCLUDES)
    set(UNIT_TEST_NAME "unit_test_${UNIT_TEST_NAME}")
    
    wsnet_include_all_internal_libs()
    
    add_executable(${UNIT_TEST_NAME} ${UNIT_TEST_SOURCES})
    
    wsnet_add_unit_tests_model_link_libs(${UNIT_TEST_NAME})
    
    if(UNIT_TEST_INCLUDES)
        target_include_directories(${UNIT_TEST_NAME} PRIVATE "${UNIT_TEST_INCLUDES}")
    endif()
    
    target_link_libraries(${UNIT_TEST_NAME} gmock_main 
                                            -Wl,--start-group ${UNIT_TEST_LIB_LINKS} -Wl,--end-group
                                            )
                                            
    include(WSNETTestsWrap)
    wsnet_replace_wrapped_functions(${UNIT_TEST_NAME} ${UNIT_TEST_SOURCES})          
                                      
    gtest_add_tests(TARGET ${UNIT_TEST_NAME}
                    TEST_PREFIX unit_test_
                    TEST_LIST noArgsTest)
endfunction()


# -----------------------------------------------------------------------------
# WSNET add cppcheck tests
# -----------------------------------------------------------------------------
function(wsnet_add_cppcheck_tests CPPCHECK_TEST_NAME CPPCHECK_TEST_PATH)

    find_program(CPPCHECK_EXECUTABLE NAMES cppcheck HINT $ENV{CPPCHECK_EXECUTABLE})
    
    add_test(NAME memcheck_cppcheck_${CPPCHECK_TEST_NAME}
             COMMAND sh -c "${CPPCHECK_EXECUTABLE} --enable=warning,performance --xml-version=2  --error-exitcode=10 ${CPPCHECK_TEST_PATH} -i${CPPCHECK_TEST_PATH}/build  2> ${WSNET_SRC_PATH}/build/log_cppcheck/cppcheck-result-${CPPCHECK_TEST_NAME}.xml")
endfunction()

# -----------------------------------------------------------------------------
# WSNET add valgrind tests
# -----------------------------------------------------------------------------
function(wsnet_add_valgrind_tests VALGRIND_TEST_TYPE VALGRIND_TEST_NAME VALGRIND_TEST_PATH)

    find_program(VALGRIND_EXECUTABLE NAMES valgrind HINT $ENV{VALGRIND_EXECUTABLE})
    
    add_test(NAME ${VALGRIND_TEST_TYPE}_memcheck_valgrind_${VALGRIND_TEST_NAME}
             COMMAND sh -c "${VALGRIND_EXECUTABLE} --leak-check=full --track-origins=yes --xml=yes --xml-file=${WSNET_SRC_PATH}/build/log_valgrind/valgrind-result-${VALGRIND_TEST_NAME}.xml --error-exitcode=4 ${WSNET_SRC_PATH}/build/kernel/src/wsnet -c ${VALGRIND_TEST_PATH}" )
endfunction()

# -----------------------------------------------------------------------------
# WSNET add standard integration tests
# -----------------------------------------------------------------------------
function(wsnet_add_standard_integration_tests TEST_TYPE TEST_NAME TEST_PATH)
   
    add_test(NAME integration_${TEST_NAME}
             COMMAND sh -c "${WSNET_SRC_PATH}/build/kernel/src/wsnet -c ${TEST_PATH}")
endfunction()

# -----------------------------------------------------------------------------
# WSNET add simple end-to-end tests
# The simple tests are basically a verification that the following outputs
# are not equal to zero :
# - RADIO_RXING duration
# - RADIO_TX duration
# -----------------------------------------------------------------------------
function(wsnet_add_simple_end_to_end_tests TEST_TYPE TEST_NAME TEST_PATH)

    set(RADIO_RXING_TEST "RXING")
    set(RADIO_TX_TEST "TX")
    set(TEST_REGEX )
    
    if(TEST_TYPE MATCHES ${RADIO_RXING_TEST})
        string(APPEND TEST_REGEX "/RXING during 0.00/ ")
    endif()
    
    if(TEST_TYPE MATCHES ${RADIO_TX_TEST})
        if(TEST_REGEX)
            string(APPEND TEST_REGEX "|| ")
        endif()
        string(APPEND TEST_REGEX "/TX during 0.00/ ")
    endif()
   
    add_test(NAME end_to_end_simple_${TEST_NAME}
             COMMAND sh -c "${WSNET_SRC_PATH}/build/kernel/wsnet -c ${TEST_PATH} | awk '{print $0} ${TEST_REGEX} {err=1; next} END {exit err}' ")
endfunction()

# -----------------------------------------------------------------------------
# WSNET add subdir tests
# -----------------------------------------------------------------------------
macro(wsnet_add_subdir_tests)
    include(WSNETCommons)
    subdirlist(WSNET_SUBDIR_TEST_TYPES "${CMAKE_CURRENT_LIST_DIR}")
    foreach(WSNET_SUBDIR_TEST ${WSNET_SUBDIR_TEST_TYPES} )
        add_subdirectory(${WSNET_SUBDIR_TEST})
    endforeach()
endmacro()

# -----------------------------------------------------------------------------
# WSNET add models tests
# -----------------------------------------------------------------------------
macro(wsnet_add_models_tests)
    
    wsnet_add_subdir_tests()
    
    find_program(CPPCHECK_EXECUTABLE NAMES cppcheck HINT $ENV{CPPCHECK_EXECUTABLE})
  
    if(${ARGC} GREATER 0)
        set(CPPCHECK_DIR_TO_CHECK "${WSNET_SRC_PATH}/models/${MODEL_TYPE}/${MODEL_NAME}")
        set(TEST_NAME ${MODEL_TYPE}_${MODEL_NAME}_${ARGV0})
    else()
        get_filename_component(CPPCHECK_DIR_TO_CHECK "${CMAKE_CURRENT_LIST_DIR}" DIRECTORY)
        set(TEST_NAME ${MODEL_TYPE}_${MODEL_NAME})
    endif()
    
    wsnet_add_cppcheck_tests(${TEST_NAME} "${CPPCHECK_DIR_TO_CHECK}")

endmacro()

