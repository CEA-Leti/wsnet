#------------------------------------------------------------------------------
# CMake file for defining CMake policies used by WSNET.
# It must be a macro once policies have scopes
# For more info: http://www.cmake.org/cmake/help/v3.0/command/cmake_policy.html
#
# Author: Luiz Henrique Suraty Filho
# ------------------------------------------------------------------------------


macro (wsnet_policy)

	### CMP0054 ###
	# https://cmake.org/cmake/help/v3.11/policy/CMP0054.html #
	# OLD - Quoted keywords like "TARGET" will be interpreted as keywords
    # NEW - Quoted keywords like "TARGET" will no longer be interpreted as keywords
	cmake_policy(SET CMP0054 NEW)
	
	### CMP0057 ###
	# https://cmake.org/cmake/help/v3.11/policy/CMP0057.html #
	# OLD - IN_LIST will NOT be interpreted as an operator #
	# NEW - IN_LIST will be interpreted as an operator. #
	cmake_policy(SET CMP0057 NEW)

	### CMP0065 ###
	# https://cmake.org/cmake/help/v3.11/policy/CMP0065.html #
	# OLD - Always use the additional link flags when linking executables regardless of the value of the ENABLE_EXPORTS target property.
	# NEW - Only use the additional link flags when linking executables if the ENABLE_EXPORTS target property is set to True.
	cmake_policy(SET CMP0065 OLD)
	
endmacro()