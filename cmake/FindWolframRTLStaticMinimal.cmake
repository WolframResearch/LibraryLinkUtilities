# FindWolframRTLStaticMinimal.cmake
#
# Finds the WolframRTLStaticMinimal installation
#
# This will define the following variables
#
#    WolframRTLStaticMinimal_FOUND
#
# and the following imported target
#
#     WolframRTLStaticMinimal::WolframRTLStaticMinimal
#
# You can specify custom location to search for WolframRTLStaticMinimal by specifying WRTL_STATIC_MINIMAL_DIR either as regular or env variable.
# By default it is searched for in MATHEMATICA_INSTALL_DIR/SystemFiles/Libraries.
#
# You can specify custom location to search for header files by either specifying KERNEL_BANK_DIR as regular or env variable (path to a Kernel Bank),
# or by specifying individual components of the Kernel Bank: WOLFRAM_LIBRARY_PATH (path to SHAR/WolframRTL) and KERNEL_CMAKE_PATH (path to KERN/CMake).
#
# The version is not checked. Woe if it does not match the WolframLibrary version used elsewhere!
# The configuration is not checked. Woe if it does not match the paclet build configuration!

set_from_env(WRTL_STATIC_MINIMAL_DIR _WRTL_STATIC_MINIMAL_DIR)

if(NOT _WRTL_STATIC_MINIMAL_DIR)
	if(MATHEMATICA_INSTALL_DIR)
		detect_system_id(SYSTEMID)
		set(_WRTL_STATIC_MINIMAL_DIR ${MATHEMATICA_INSTALL_DIR}/SystemFiles/Libraries/${SYSTEMID})
	endif()
endif()

message(STATUS "Looking for WolframRTLStaticMinimal in ${_WRTL_STATIC_MINIMAL_DIR}")

set_search_opts_from_path(_WRTL_STATIC_MINIMAL_DIR _WRTL_SEARCH_OPTS)

find_library(WolframRTLStaticMinimal_LIBRARY
	NAMES WolframRTL_Static_Minimal
	PATHS "${_WRTL_STATIC_MINIMAL_DIR}"
	${_WRTL_SEARCH_OPTS}
	DOC "WolframRTL_Static_Minimal library."
)
fail_if_notfound(WolframRTLStaticMinimal_LIBRARY "Failed to find WolframRTLStaticMinimal library.")

# Include directories for os-thread.h and types.h (required by os-thread.h)
if(KERNEL_BANK_DIR)
	set(_WRTL_INCLUDE_DIR ${KERNEL_BANK_DIR}/WolframRTL/Includes)
	set(_EXPORTS_INCLUDE_DIR ${KERNEL_BANK_DIR}/CMake/SystemConfiguration/Exports)
elseif(DEFINED ENV{KERNEL_BANK_DIR})
	set(_WRTL_INCLUDE_DIR $ENV{KERNEL_BANK_DIR}/WolframRTL/Includes)
	set(_EXPORTS_INCLUDE_DIR $ENV{KERNEL_BANK_DIR}/CMake/SystemConfiguration/Exports)
else()
	if(WOLFRAM_LIBRARY_PATH)
		set(_WRTL_INCLUDE_DIR ${WOLFRAM_LIBRARY_PATH}/Includes)
	endif()
	if(KERNEL_CMAKE_PATH)
		set(_EXPORTS_INCLUDE_DIR ${KERNEL_CMAKE_PATH}/SystemConfiguration/Exports)
	endif()
endif()

set_search_opts_from_path(_WRTL_INCLUDE_DIR _WRTL_SEARCH_OPTS)

find_path(WRTL_INCLUDE_DIR
	NAMES os-thread.h
	PATHS "${_WRTL_INCLUDE_DIR}"
	${_WRTL_SEARCH_OPTS}
	DOC "WolframRTL/Include directory."
)
fail_if_notfound(WRTL_INCLUDE_DIR "Failed to find WolframRTL/Include/os-thread.h.")

set_search_opts_from_path(_EXPORTS_INCLUDE_DIR _WRTL_SEARCH_OPTS)

find_path(EXPORTS_INCLUDE_DIR
	NAMES types.h
	PATHS "${_EXPORTS_INCLUDE_DIR}"
	${_WRTL_SEARCH_OPTS}
	DOC "CMake/SystemConfiguration/Exports directory."
)
fail_if_notfound(EXPORTS_INCLUDE_DIR "Failed to find CMake/SystemConfiguration/Exports/types.h.")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(WolframRTLStaticMinimal
	REQUIRED_VARS WolframRTLStaticMinimal_LIBRARY WRTL_INCLUDE_DIR EXPORTS_INCLUDE_DIR
)

if(WolframRTLStaticMinimal_FOUND AND ${WolframRTLStaticMinimal_FOUND})
	if(NOT TARGET WolframRTLStaticMinimal::WolframRTLStaticMinimal)
		fail_if_dne(${WolframRTLStaticMinimal_LIBRARY})
		add_library(WolframRTLStaticMinimal::WolframRTLStaticMinimal STATIC IMPORTED)
		set_target_properties(WolframRTLStaticMinimal::WolframRTLStaticMinimal PROPERTIES
			IMPORTED_LOCATION ${WolframRTLStaticMinimal_LIBRARY}
			IMPORTED_IMPLIB ${WolframRTLStaticMinimal_LIBRARY}
			INTERFACE_INCLUDE_DIRECTORIES "${_WRTL_INCLUDE_DIR};${_EXPORTS_INCLUDE_DIR}"
		)
	endif()
endif()