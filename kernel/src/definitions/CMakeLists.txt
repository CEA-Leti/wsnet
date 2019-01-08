#------------------------------------------------------------------------------
# CMake file for WSNET Internal Library.
#
# Author: Luiz Henrique Suraty Filho
# ------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.11.0)

# -----------------------------------------------------------------------------
# Configure the library variables
# -----------------------------------------------------------------------------

# The name of the library
set(INTERNAL_LIB_NAME definitions) 

# The extra external libraries used by the library
set(INTERNAL_LIB_EXTERNAL_LIBRARIES GMODULE2 LibXml2)

# The source files used by the library
set(INTERNAL_LIB_SOURCES ${WSNET_KERNEL_FOLDER}/src/definitions/class.c
                         ${WSNET_KERNEL_FOLDER}/src/definitions/medium.c
                         ${WSNET_KERNEL_FOLDER}/src/definitions/environment.c
                         ${WSNET_KERNEL_FOLDER}/src/definitions/nodearch.c
                         ${WSNET_KERNEL_FOLDER}/src/definitions/node.c
                         ${WSNET_KERNEL_FOLDER}/src/definitions/grouparch.c
                         ${WSNET_KERNEL_FOLDER}/src/definitions/group.c
                         ${WSNET_KERNEL_FOLDER}/src/definitions/simulation.c
                         ${WSNET_KERNEL_FOLDER}/src/definitions/packet.c
                         ${WSNET_KERNEL_FOLDER}/src/definitions/field.c
                         ${WSNET_KERNEL_FOLDER}/src/definitions/models.c
                         ${WSNET_KERNEL_FOLDER}/src/definitions/types/interval/interval.cc
                         ${WSNET_KERNEL_FOLDER}/src/definitions/types/interval/frequency_interval.cc
                         ${WSNET_KERNEL_FOLDER}/src/definitions/types/interval/time_interval.cc
                         ${WSNET_KERNEL_FOLDER}/src/definitions/types/interval/frequency_interval_waveform.cc
                         ${WSNET_KERNEL_FOLDER}/src/definitions/types/interval/frequency_interval_registered_rx_node.cc
                         ${WSNET_KERNEL_FOLDER}/src/definitions/types/interval/registered_rx_node.cc
                         ${WSNET_KERNEL_FOLDER}/src/definitions/types/interval/registered_rx_node_factory.cc
                         ${WSNET_KERNEL_FOLDER}/src/definitions/types/signal/signal.cc
                         ${WSNET_KERNEL_FOLDER}/src/definitions/types/signal/rf_signal.cc
                         ${WSNET_KERNEL_FOLDER}/src/definitions/types/waveform/waveform.cc
                         ${WSNET_KERNEL_FOLDER}/src/definitions/types/waveform/waveform_factory.cc
                         ${WSNET_KERNEL_FOLDER}/src/definitions/models/coding/coding_model.cc
                         ${WSNET_KERNEL_FOLDER}/src/definitions/models/error/error_model.cc
                         ${WSNET_KERNEL_FOLDER}/src/definitions/models/interference/interference_model.cc
                         ${WSNET_KERNEL_FOLDER}/src/definitions/models/modulator/modulator_model.cc
                         ${WSNET_KERNEL_FOLDER}/src/definitions/models/signal_tracker/signal_tracker_model.cc
                         ${WSNET_KERNEL_FOLDER}/src/definitions/models/phy/phy_model.cc
                         ${WSNET_KERNEL_FOLDER}/src/definitions/models/spectrum/spectrum_model.cc
						 ) 

# The folder(s) where your local includes (.h files) are located
set(INTERNAL_LIB_LOCAL_INCLUDES ${WSNET_KERNEL_FOLDER}/include/definitions)

# The local headers used by the library
set(INTERNAL_LIB_LOCAL_HEADERS ${WSNET_KERNEL_FOLDER}/include/definitions/class.h
                               ${WSNET_KERNEL_FOLDER}/include/definitions/medium.h
                               ${WSNET_KERNEL_FOLDER}/include/definitions/environment.h
                               ${WSNET_KERNEL_FOLDER}/include/definitions/nodearch.h
                               ${WSNET_KERNEL_FOLDER}/include/definitions/node.h
                               ${WSNET_KERNEL_FOLDER}/include/definitions/grouparch.h
                               ${WSNET_KERNEL_FOLDER}/include/definitions/group.h
                               ${WSNET_KERNEL_FOLDER}/include/definitions/simulation.h
                               ${WSNET_KERNEL_FOLDER}/include/definitions/packet.h
                               ${WSNET_KERNEL_FOLDER}/include/definitions/field.h
                               ${WSNET_KERNEL_FOLDER}/include/definitions/models.h
                               ${WSNET_KERNEL_FOLDER}/include/definitions/types/interval/interval.h
                               ${WSNET_KERNEL_FOLDER}/include/definitions/types/interval/frequency_interval.h
                               ${WSNET_KERNEL_FOLDER}/include/definitions/types/interval/time_interval.h
                               ${WSNET_KERNEL_FOLDER}/include/definitions/types/interval/frequency_interval_waveform.h
                               ${WSNET_KERNEL_FOLDER}/include/definitions/types/interval/frequency_interval_registered_rx_node.h
                               ${WSNET_KERNEL_FOLDER}/include/definitions/types/interval/registered_rx_node.h
                               ${WSNET_KERNEL_FOLDER}/include/definitions/types/interval/registered_rx_node_factory.h
                               ${WSNET_KERNEL_FOLDER}/include/definitions/types/signal/signal.h
                               ${WSNET_KERNEL_FOLDER}/include/definitions/types/signal/rf_signal.h
                               ${WSNET_KERNEL_FOLDER}/include/definitions/types/waveform/waveform.h
                               ${WSNET_KERNEL_FOLDER}/include/definitions/types/waveform/waveform_factory.h
                               ${WSNET_KERNEL_FOLDER}/include/definitions/models/coding/coding_model.h
                               ${WSNET_KERNEL_FOLDER}/include/definitions/models/error/error_model.h
                               ${WSNET_KERNEL_FOLDER}/include/definitions/models/interference/interference_model.h
                               ${WSNET_KERNEL_FOLDER}/include/definitions/models/modulator/modulator_model.h
                               ${WSNET_KERNEL_FOLDER}/include/definitions/models/signal_tracker/signal_tracker_model.h
                               ${WSNET_KERNEL_FOLDER}/include/definitions/models/phy/phy_model.h
                               ${WSNET_KERNEL_FOLDER}/include/definitions/models/spectrum/spectrum_model.h
							   ) 

# The WSNET libraries used by the library
set(INTERNAL_LIB_LOCAL_LINK model_handlers)

# -----------------------------------------------------------------------------
# Add the library
# -----------------------------------------------------------------------------
set(INTERNAL_LIB_ALL_SOURCES ${INTERNAL_LIB_SOURCES} ${INTERNAL_LIB_LOCAL_HEADERS})
wsnet_add_internal_library(${INTERNAL_LIB_NAME} "${INTERNAL_LIB_ALL_SOURCES}")

# -----------------------------------------------------------------------------
# Include all external and internal libs needed
# -----------------------------------------------------------------------------
wsnet_include_all_internal_libs()

if(INTERNAL_LIB_EXTERNAL_LIBRARIES)
    wsnet_find_external_libs(${INTERNAL_LIB_NAME} "${INTERNAL_LIB_EXTERNAL_LIBRARIES}")
endif()

if(INTERNAL_LIB_LOCAL_INCLUDES)
    target_include_directories(${INTERNAL_LIB_NAME} PRIVATE "${INTERNAL_LIB_LOCAL_INCLUDES}")
endif()

if(INTERNAL_LIB_LOCAL_LINK)
    target_link_libraries(${INTERNAL_LIB_NAME} "${INTERNAL_LIB_LOCAL_LINK}")
endif()