#------------------------------------------------------------------------------
# WSNETCommons.cmake
#
# CMake file for common functions used by Cmake under WSNET
#
# Author: Luiz Henrique Suraty Filho
#------------------------------------------------------------------------------

# -----------------------------------------------------------------------------
# List all subdirectories (with CMake files) of a folder
# -----------------------------------------------------------------------------
macro(subdirlist RESULT CURRENT_DIR)
    file(GLOB CHILDREN RELATIVE ${CURRENT_DIR} ${CURRENT_DIR}/*)
    set(DIRLIST "")
    foreach(CHILD ${CHILDREN})
        if(EXISTS ${CURRENT_DIR}/${CHILD}/CMakeLists.txt)
            list(APPEND DIRLIST ${CHILD})
        endif()
    endforeach()
    set(${RESULT} ${DIRLIST})
endmacro()
