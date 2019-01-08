#------------------------------------------------------------------------------
# CMake file for testing WSNET Models.
#
# Author: Luiz Henrique Suraty Filho
# ------------------------------------------------------------------------------

include(WSNETTests)
wsnet_add_models_tests(${MODEL_EXTRA_NAME})