#------------------------------------------------------------------------------
# CMake file for global tests of WSNET
#
# Author: Luiz Henrique Suraty Filho
# ------------------------------------------------------------------------------

# -----------------------------------------------------------------------------
# Add unit tests
# -----------------------------------------------------------------------------
set(MODEL_TEST_SOURCES linear_battery_energy_unit_tests.cc
                       )
                       
set(MODEL_TEST_INCLUDES ${WSNET_MODELS_FOLDER}/${MODEL_TYPE}/${MODEL_NAME}/src
                        ${WSNET_MODELS_FOLDER}/${MODEL_TYPE}/${MODEL_NAME}/include
                       )
                      
wsnet_add_unit_tests_model(${MODEL_TYPE}_${MODEL_NAME} "${MODEL_TEST_SOURCES}" "${MODEL_TEST_INCLUDES}")
