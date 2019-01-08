#------------------------------------------------------------------------------
# CMake file for global tests of WSNET
#
# Author: Luiz Henrique Suraty Filho
# ------------------------------------------------------------------------------

# -----------------------------------------------------------------------------
# Add unit tests
# -----------------------------------------------------------------------------
set(SCHEDULER_UNIT_TEST_SOURCES scheduler_unit_test.cc
                       			)
                       
set(SCHEDULER_UNIT_TEST_INCLUDES ${WSNET_SRC_FOLDER}/kernel/scheduler/include
                       			 )
                       
set(SCHEDULER_UNIT_LIB_LINK scheduler
							model_handlers
							definitions
							list
							hashtable
							heap
							mem_fs
                       		)
                      
wsnet_add_unit_tests(kernel_scheduler "${SCHEDULER_UNIT_TEST_SOURCES}" "${SCHEDULER_UNIT_TEST_INCLUDES}" "${SCHEDULER_UNIT_LIB_LINK}")
