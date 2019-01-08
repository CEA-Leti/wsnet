#------------------------------------------------------------------------------
# WSNETModels.cmake
#
# CMake file for WSNET models which specifies the library and installation
# settings for each supported platform and build configuration.
#
# Author: Luiz Henrique Suraty Filho
#------------------------------------------------------------------------------

# -----------------------------------------------------------------------------
# Add a model
# -----------------------------------------------------------------------------
macro(wsnet_add_model)

    # Verify the model type
    wsnet_check_model_type(${MODEL_TYPE})
    
    # Verify if the target name exists
    if(NOT MODEL_NAME)
        message(FATAL_ERROR "You must define a name for your model (Check CMakeLists.txt)")
    endif()
    
    # Verify if it is using extra name
    if(MODEL_EXTRA_NAME)
        set(MODEL_USED_NAME ${MODEL_TYPE}_${MODEL_NAME}_${MODEL_EXTRA_NAME})
    else()
        set(MODEL_USED_NAME ${MODEL_TYPE}_${MODEL_NAME})
    endif()
        
    # Verify if it is an external model
    if(${ARGC} GREATER 0)
        set(MODEL_INSTALL_PATH "$ENV{WSNET_MODDIR}")
        include(WSNETCompilerSettings)
        wsnet_add_compiler_options()
        # Set the correct CMake policies
    	include(WSNETPolicy)
    	wsnet_policy()
    else()
        set(MODEL_INSTALL_PATH "${WSNET_INSTALLED_PATH}/lib")
    endif()
    
	# We need to set the MODEL_USED_NAME for the dependency to be 
	# included properly by wsnet_add_model_subdir_and_install 
	get_directory_property(MODEL_HAS_PARENT PARENT_DIRECTORY)
	if(MODEL_HAS_PARENT)
		# Set the used name for the lib in the parent scope
   		set(MODEL_USED_NAME ${MODEL_USED_NAME} PARENT_SCOPE)    
    endif()
    
    include(WSNETTests)
	wsnet_test_config_model()
   
    # Config paths to auxiliary files 
    string(TOUPPER ${MODEL_USED_NAME} MOD_NAME_UPPER)
    set(WSNET_MODEL_${MOD_NAME_UPPER}_PATH "${CMAKE_CURRENT_LIST_DIR}" CACHE PATH "The path to the model source directory")
    
    include(WSNETSystemConfig)
    include(WSNETCompilerSettings)
    include(WSNETDependencies)
    
    # Declare the name of the project
    project(${MODEL_USED_NAME})

    # Add the library
    set(MODEL_ALL_SOURCES ${MODEL_SOURCES} ${MODEL_LOCAL_HEADERS})
    add_library(${MODEL_USED_NAME} MODULE "${MODEL_ALL_SOURCES}")
    install(TARGETS ${MODEL_USED_NAME} DESTINATION "${MODEL_INSTALL_PATH}" COMPONENT ${MODEL_USED_NAME})
   
    if (TARGET ${MODEL_TYPE})
        if(MODEL_EXTRA_NAME)
            add_dependencies(${MODEL_TYPE}_${MODEL_NAME} ${MODEL_TYPE}_${MODEL_NAME}_${MODEL_EXTRA_NAME})
        endif()
        add_dependencies(${MODEL_TYPE} ${MODEL_USED_NAME})
    endif()
    
    # Include all external and internal libs needed
    wsnet_include_all_internal_libs()
    
    if(MODEL_EXTERNAL_LIBRARIES)
        wsnet_find_external_libs(${MODEL_USED_NAME} "${MODEL_EXTERNAL_LIBRARIES}")
    endif()
    
    if(MODEL_LOCAL_INCLUDES)
        target_include_directories(${MODEL_USED_NAME} PRIVATE "${MODEL_LOCAL_INCLUDES}")
    endif()
    
    if(MODEL_LIB_LOCAL_LINK)
        target_link_libraries(${MODEL_USED_NAME} "${MODEL_LIB_LOCAL_LINK}")
    endif()
    
    if(MODEL_DEFINES)
        target_compile_definitions(${MODEL_USED_NAME} PRIVATE "${MODEL_DEFINES}")
    endif()
    
    if(EXISTS "${WSNET_MODEL_${MOD_NAME_UPPER}_PATH}/tests")
        add_subdirectory(tests)
    endif() 
         
    # Add target for installation of type of model
    set(MODEL_INSTALL_COMMAND ${CMAKE_COMMAND} -DCMAKE_INSTALL_COMPONENT=${MODEL_USED_NAME} -P ${CMAKE_BINARY_DIR}/cmake_install.cmake)
    
    add_custom_target(${MODEL_USED_NAME}-install
      DEPENDS ${MODEL_USED_NAME}
      COMMAND 
              ${MODEL_INSTALL_COMMAND}
    )
endmacro()

# -----------------------------------------------------------------------------
# Add a model dir
# -----------------------------------------------------------------------------
macro(wsnet_add_model_subdir_and_install MOD_TYPE MOD_NAME_LIST)
    add_custom_target(${MOD_TYPE}-install)
    
    foreach(MOD_NAME ${MOD_NAME_LIST})
        add_subdirectory(${MOD_NAME})
        if(NOT MODEL_USED_NAME)
        	add_dependencies(${MOD_TYPE}-install ${MOD_TYPE}_${MOD_NAME}-install)
        else()
        	add_dependencies(${MOD_TYPE}-install ${MODEL_USED_NAME}-install)
        endif()
        
    endforeach()
endmacro()

# -----------------------------------------------------------------------------
# Check model type
# -----------------------------------------------------------------------------
function(wsnet_check_model_type MOD_TYPE)

    list(APPEND WSNET_MODEL_TYPE_LIST    application 
                                         energy
                                         fading
                                         global_map
                                         interface
                                         interferences
                                         intermodulation
                                         link
                                         mac
                                         map
                                         mobility
                                         modulations
                                         monitor
                                         noise
                                         spectrum
                                         pathloss
                                         physical
                                         routing
                                         sensor
                                         shadowing
                                         transceiver
                                         shared
                                         phy
                                         signal_tracker
                                         error
                                         coding
                                         interference
                                         modulator
                                         )
    
    list(FIND WSNET_MODEL_TYPE_LIST ${MOD_TYPE} IS_MODEL_TYPE_ON_LIST)

	
    if(${IS_MODEL_TYPE_ON_LIST} LESS 0)
    	set(IS_A_MODEL_TYPE FALSE PARENT_SCOPE)
    	if(${ARGC} EQUAL 1)
        	message(FATAL_ERROR "The model type is incorrect, please check the CMakeLists.txt file")
    	endif()
	else()
		set(IS_A_MODEL_TYPE TRUE PARENT_SCOPE)
	endif()
	
endfunction(wsnet_check_model_type)

# -----------------------------------------------------------------------------
# Creates a new target on the Makefile
# This target "create_new_module" is used to create a new module
# Usage: make create_new_module MODEL_TYPE MODEL_NAME
# -----------------------------------------------------------------------------
macro(wsnet_create_new_model_target)
    if(UNIX)
        if (NOT TARGET create_new_module)
            include(WSNETSystemConfig)
            
            set(WSNET_EXTERNAL_MODULE_DIR "$ENV{WSNET_EXTERNAL_MOD_PATH}")
            
            if(NOT (NEW_MODULE_TYPE AND NEW_MODULE_NAME))
                message(FATAL_ERROR "You must define a name and type for your module using options -DNEW_MODULE_TYPE=desired_type -DNEW_MODULE_NAME=desired_name")
            endif()
            
            wsnet_check_model_type(${NEW_MODULE_TYPE})
            
            wsnet_get_author_name()
           
            string(TIMESTAMP CREATION_DATE %d-%m-%Y)
            
            string(TOUPPER "WSNET_EXTERNAL_MODELS_${NEW_MODULE_TYPE}_${NEW_MODULE_NAME}" NEW_MODULE_GUARD)
            
            add_custom_target (create_new_module COMMENT "Creating new module with type ${NEW_MODULE_NAME} and name ${NEW_MODULE_TYPE}")
            
            # Module path is set to external_models/name_type
            set(NEW_MODULE_PATH "${WSNET_EXTERNAL_MODULE_DIR}/${NEW_MODULE_NAME}_${NEW_MODULE_TYPE}")
            # Module path is set to external_models/type/name
            #set(NEW_MODULE_PATH "${WSNET_EXTERNAL_MODULE_DIR}/${NEW_MODULE_TYPE}/${NEW_MODULE_NAME}")
            
            # If external model folder is being organized by type
            if(ORGANIZE_BY_TYPE)
                # Module path is set to external_models/type/name
                set(NEW_MODULE_PATH "${WSNET_EXTERNAL_MODULE_DIR}/${NEW_MODULE_TYPE}/${NEW_MODULE_NAME}")
                # Module filename is set to name_type
                set(NEW_MODULE_FILENAME "${NEW_MODULE_NAME}_${NEW_MODULE_TYPE}")
                # Module filename is set to name
                set(NEW_MODULE_TYPE_CMAKE_FILE "${WSNET_EXTERNAL_MODULE_DIR}/${NEW_MODULE_TYPE}/CMakeLists.txt")
            else()
                # Module path is set to external_models/name_type
                set(NEW_MODULE_PATH "${WSNET_EXTERNAL_MODULE_DIR}/${NEW_MODULE_NAME}_${NEW_MODULE_TYPE}")
                # Module filename is set to name_type
                set(NEW_MODULE_FILENAME "${NEW_MODULE_NAME}_${NEW_MODULE_TYPE}")
                # Module filename is set to name
                set(NEW_MODULE_TYPE_CMAKE_FILE "/dev/null")
            endif()
            
            # Clear the cache entry for variable
            unset(ORGANIZE_BY_TYPE CACHE)
            
            add_custom_command(
                COMMENT "Creating new module with type ${NEW_MODULE_TYPE} and name ${NEW_MODULE_NAME}"
                # The following command would give to the make command the option
                # to choose the name and type of the model, by simply using make (instead of cmake + make) as in:
                # make create_new_module NEW_MODULE_TYPE=mac NEW_MODULE_NAME=test
                # However, this would imply in a rather complicated verification of the module type existence
                # inside the Makefile (CMake seems to force a tab in the begining of each new line. As we need an "if", the "if" requires to be properly indented)
                # This is something to come back later.
                #COMMAND mkdir $(NEW_MODULE_TYPE)_$(NEW_MODULE_NAME)
                COMMAND echo "Creating folder ${NEW_MODULE_PATH} ..."
                COMMAND mkdir -p ${NEW_MODULE_PATH}
                
                COMMAND echo "Creating folders  ..."
                # Although the command works perfectly when used in command line, it fails when executed by make. (Need to comeback here later for verifying the issue)
                # You can check by uncommenting the next line and then copying and executing the content from generated file: CMakeFiles/create_new_module.dir/build.make
                #COMMAND find  ${WSNET_EXTERNAL_MODULE_DIR}/user_models/ -type d -links 2 -exec mkdir -p "${WSNET_EXTERNAL_MODULE_DIR}/${NEW_MODULE_NAME}_${NEW_MODULE_TYPE}/" \;
                COMMAND mkdir ${NEW_MODULE_PATH}/doc
                COMMAND mkdir ${NEW_MODULE_PATH}/include
                COMMAND mkdir ${NEW_MODULE_PATH}/json
                COMMAND mkdir ${NEW_MODULE_PATH}/src
                COMMAND mkdir ${NEW_MODULE_PATH}/tests
                COMMAND mkdir ${NEW_MODULE_PATH}/tests/integration
                COMMAND mkdir ${NEW_MODULE_PATH}/tests/integration/xml
                COMMAND mkdir ${NEW_MODULE_PATH}/xml
                
                COMMAND echo "Copying contents from standard files"
                
                # Create markdown (.md) file               
                COMMAND cp ${WSNET_EXTERNAL_MODULE_DIR}/user_models/doc/model.md ${NEW_MODULE_PATH}/doc/${NEW_MODULE_FILENAME}.md
                # Create header (.h) file
                COMMAND cat ${WSNET_EXTERNAL_MODULE_DIR}/user_models/include/model.h | sed 's%USER_MODEL_NAME%${NEW_MODULE_NAME}_${NEW_MODULE_TYPE}%g' | sed 's%AUTHOR_NAME%${AUTHOR_NAME}%g' | sed 's%DATE%${CREATION_DATE}%g' | sed 's%USER_MODEL_GUARD%${NEW_MODULE_GUARD}%g' > ${NEW_MODULE_PATH}/include/${NEW_MODULE_FILENAME}.h
                # Create json (.json) file
                COMMAND cp ${WSNET_EXTERNAL_MODULE_DIR}/user_models/json/model.json ${NEW_MODULE_PATH}/json/${NEW_MODULE_FILENAME}.json
                # Create source (.c) file
                COMMAND cat ${WSNET_EXTERNAL_MODULE_DIR}/user_models/src/${NEW_MODULE_TYPE}_model.c | sed 's%USER_MODEL_NAME%${NEW_MODULE_NAME}_${NEW_MODULE_TYPE}%g' | sed 's%AUTHOR_NAME%${AUTHOR_NAME}%g' | sed 's%DATE%${CREATION_DATE}%g' > ${NEW_MODULE_PATH}/src/${NEW_MODULE_FILENAME}.c
                # Create test source (.c) file
                COMMAND cat ${WSNET_EXTERNAL_MODULE_DIR}/user_models/tests/model.c | sed 's%USER_MODEL_NAME%${NEW_MODULE_NAME}_${NEW_MODULE_TYPE}%g' | sed 's%AUTHOR_NAME%${AUTHOR_NAME}%g' | sed 's%DATE%${CREATION_DATE}%g' > ${NEW_MODULE_PATH}/tests/${NEW_MODULE_FILENAME}_test.c
                # Create test CMake (CMakeLists.txt) file
                COMMAND cp ${WSNET_EXTERNAL_MODULE_DIR}/user_models/tests/CMakeLists.txt ${NEW_MODULE_PATH}/tests/CMakeLists.txt 
                # Create test integration CMake (CMakeLists.txt) file
                COMMAND cp ${WSNET_EXTERNAL_MODULE_DIR}/user_models/tests/integration/CMakeLists.txt ${NEW_MODULE_PATH}/tests/integration/CMakeLists.txt
                # Create test integration XML (.xml) file
                COMMAND cp ${WSNET_EXTERNAL_MODULE_DIR}/user_models/tests/integration/xml/model.xml ${NEW_MODULE_PATH}/tests/integration/xml/${NEW_MODULE_FILENAME}.xml
                # Create XML (.xml) file
                COMMAND cp ${WSNET_EXTERNAL_MODULE_DIR}/user_models/xml/model.xml ${NEW_MODULE_PATH}/xml/${NEW_MODULE_FILENAME}.xml
                # Create .gitignore file
                COMMAND cp ${WSNET_EXTERNAL_MODULE_DIR}/user_models/gitignore ${NEW_MODULE_PATH}/.gitignore
                
                COMMAND mkdir ${NEW_MODULE_PATH}/build
                
                # Create main CMake (CMakeLists.txt) file
                COMMAND echo "Updating information on CMakeLists.txt. You need to edit it if you create extra source files, use specific libraries, etc ..."
                COMMAND cat ${WSNET_EXTERNAL_MODULE_DIR}/user_models/CMakeLists_model.txt | sed 's%user_model_type%${NEW_MODULE_TYPE}%g' | sed 's%user_model_name%${NEW_MODULE_NAME}%g' > ${NEW_MODULE_PATH}/CMakeLists.txt
                
                # It still copies the content at every iteration
                # TODO: comeback to this issue
                COMMAND cat ${WSNET_EXTERNAL_MODULE_DIR}/user_models/CMakeLists_type.txt > ${NEW_MODULE_TYPE_CMAKE_FILE}
                
                COMMAND printf \"\\033[1;32mYour model folder is ready to be used, please visit the following folder:\\033[0;39m \\n\"
                COMMAND tree ${NEW_MODULE_PATH}
                TARGET  create_new_module
            )
        endif()
    endif(UNIX) 
endmacro()
