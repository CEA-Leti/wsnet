#------------------------------------------------------------------------------
# CMake file for testing WSNET Models.
#
# Author: Luiz Henrique Suraty Filho
# ------------------------------------------------------------------------------

get_filename_component(TEST_TYPE ${CMAKE_CURRENT_SOURCE_DIR} NAME)

wsnet_add_standard_integration_tests(${TEST_TYPE} ${MODEL_TYPE}_${MODEL_NAME} "${CMAKE_CURRENT_LIST_DIR}/xml/${MODEL_NAME}.xml")
             
wsnet_add_valgrind_tests(${TEST_TYPE} ${MODEL_TYPE}_${MODEL_NAME} "${CMAKE_CURRENT_LIST_DIR}/xml/${MODEL_NAME}.xml")    