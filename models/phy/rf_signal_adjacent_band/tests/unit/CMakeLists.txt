#------------------------------------------------------------------------------
# CMake file for global tests of WSNET
#
# Author: Luiz Henrique Suraty Filho
# ------------------------------------------------------------------------------

# -----------------------------------------------------------------------------
# Add unit tests
# -----------------------------------------------------------------------------
set(MODEL_TEST_SOURCES unit_tests.cc
                       )
                       
set(MODEL_TEST_INCLUDES ${WSNET_MODEL_${MOD_NAME_UPPER}_PATH}/src
                        ${WSNET_MODEL_${MOD_NAME_UPPER}_PATH}/include
                       )
                      
wsnet_add_unit_tests_model(${MODEL_TYPE}_${MODEL_NAME} "${MODEL_TEST_SOURCES}" "${MODEL_TEST_INCLUDES}")
