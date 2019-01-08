#------------------------------------------------------------------------------
# WSNETTestsWrap.cmake
#
# CMake file for using wraps for tests
#
# Author: Luiz Henrique Suraty Filho
#------------------------------------------------------------------------------

# -----------------------------------------------------------------------------
# Get all wrap symbols for a given include
# -----------------------------------------------------------------------------
macro(wsnet_get_wrapped_symbols TEST_INCLUDE_FILE)

    # The following command works correctly on command line, however
    # CMake is not able to correctly parse it and produces errors
    # or wrong results. We need to comeback to this issue, for the 
    # moment, we stick with the less clean solution below
    # set(SED_COMMAND "sed -n 's%\.*__wrap_\(.*\)(.*%\1%p")
    execute_process(COMMAND cat "${TEST_INCLUDE_FILE}"
                    COMMAND sed -n "s%.*__wrap_%%p"
                    COMMAND sed -n "s%(.*%%p"
                    OUTPUT_VARIABLE PROCESS_OUTPUT 
                    ERROR_QUIET
                    )
                    
    string(STRIP ${PROCESS_OUTPUT} PROCESS_OUTPUT )
    string(REPLACE "\n" ";" WRAPPED_SYMBOLS ${PROCESS_OUTPUT})
    
endmacro()

# -----------------------------------------------------------------------------
# Get test includes used by the source file and get all wrapped symbols on them
# -----------------------------------------------------------------------------
macro(wsnet_get_all_wrapped_symbols UNIT_TEST_SOURCE)

    # The following command works correctly on command line, however
    # CMake is not able to correctly parse it and produces errors
    # or wrong results. We need to comeback to this issue, for the 
    # moment, we stick with the less clean solution below
    # set(SED_COMMAND "sed -n 's%.*<\(tests\/include\/fakes\/.*\.h\)>.*%\1%p'")
    execute_process(COMMAND cat "${UNIT_TEST_SOURCE}"
                    COMMAND grep "#include <tests\/include\/fakes"
                    COMMAND sed -n "s%#include <%%p"
                    COMMAND sed -n "s%>%%p"
                    OUTPUT_VARIABLE PROCESS_OUTPUT 
                    ERROR_QUIET
                    )
    
    string(STRIP ${PROCESS_OUTPUT} PROCESS_OUTPUT )
    string(REPLACE "\n" ";" TEST_INCLUDES_LIST ${PROCESS_OUTPUT})
    
    foreach(TEST_INCLUDE_FILE ${TEST_INCLUDES_LIST})
        wsnet_get_wrapped_symbols(${WSNET_SRC_PATH}/${TEST_INCLUDE_FILE})
        list(APPEND WRAPPED_SYMBOLS_LIST ${WRAPPED_SYMBOLS})
    endforeach()
    
endmacro()

# -----------------------------------------------------------------------------
# Replace symbols to the unit test
# -----------------------------------------------------------------------------
macro(wsnet_replace_wrapped_functions UNIT_TEST_NAME UNIT_TEST_SOURCE)

    wsnet_get_all_wrapped_symbols(${CMAKE_CURRENT_LIST_DIR}/${UNIT_TEST_SOURCE})
    
    if(APPLE)
        set(WRAPPER_COMMAND -Wl,--alias,)
    else()
        set(WRAPPER_COMMAND -Wl,--wrap,)
    endif()
    
    # Maybe it is better (faster) to transform the list into string (properly concatenated with WRAPPER_COMMAND)
    # and use a single target_link_libraries command    
    foreach(WRAPPED_SYMBOL ${WRAPPED_SYMBOLS_LIST})
        target_link_libraries(${UNIT_TEST_NAME} ${WRAPPER_COMMAND}${WRAPPED_SYMBOL})
    endforeach()

endmacro()