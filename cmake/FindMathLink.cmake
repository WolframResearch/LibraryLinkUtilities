# FindMathLink.cmake
#
# Finds the MathLink installation
#
# This will define the following variables
#
#    MathLink_FOUND
#    MathLink_INCLUDE_DIRS
#    MathLink_LIBRARIES
#    MathLink_VERSION
#
# and the following imported target
#
#     MathLink::MathLink
#
# You can specify custom location to search for MathLink either by specifying WOLFRAM_MATHLINK_PATH explicitly,
# or if that variable is not set, by providing MATHEMATICA_INSTALL_DIR variable with a path to Mathematica installation.
#
# Author: Rafal Chojna - rafalc@wolfram.com

include(WolframUtilities)

detect_system_id(_WOLFMLINK_SYSTEMID)

if(WOLFRAM_MATHLINK_PATH)
	set(_WOLFMLINK_LIBRARY_PATH "${WOLFRAM_MATHLINK_PATH}")
else()
	set(_WOLFMLINK_LIBRARY_PATH "$ENV{WOLFRAM_MATHLINK_PATH}")
endif()

if(NOT _WOLFMLINK_LIBRARY_PATH AND MATHEMATICA_INSTALL_DIR)
	set(_WOLFMLINK_LIBRARY_PATH "${MATHEMATICA_INSTALL_DIR}/SystemFiles/Links/MathLink/DeveloperKit/${_WOLFMLINK_SYSTEMID}/CompilerAdditions")
endif()

if(_WOLFMLINK_LIBRARY_PATH)
	set(_WOLFMLINK_SEARCH_OPTS NO_DEFAULT_PATH)
else()
	set(_WOLFMLINK_SEARCH_OPTS)
endif()

find_path(MathLink_INCLUDE_DIR
	NAMES mathlink.h
	PATHS "${_WOLFMLINK_LIBRARY_PATH}"
	${_WOLFMLINK_SEARCH_OPTS}
	DOC "Path to the mathlink.h"
)

if(MathLink_INCLUDE_DIR)
	file(STRINGS "${MathLink_INCLUDE_DIR}/mathlink.h" _WOLFMLINK_HEADER_CONTENTS REGEX "#define ML[A-Z]+ ")
	string(REGEX REPLACE ".*#define MLINTERFACE ([0-9]+).*" "\\1" MathLink_VERSION_MAJOR "${_WOLFMLINK_HEADER_CONTENTS}")
	string(REGEX REPLACE ".*#define MLREVISION ([0-9]+).*" "\\1" MathLink_VERSION_MINOR "${_WOLFMLINK_HEADER_CONTENTS}")

	set(MathLink_VERSION_STRING "${MathLink_VERSION_MAJOR}.${MathLink_VERSION_MINOR}")

	include(WolframUtilities)
	get_mathlink_library_name(${MathLink_VERSION_MAJOR} _WOLFMLINK_LIB_NAME)

	message(STATUS "Looking for ${_WOLFMLINK_LIB_NAME} in ${_WOLFMLINK_LIBRARY_PATH}")

	find_library(MathLink_LIBRARY
		NAMES ${_WOLFMLINK_LIB_NAME}
		PATHS "${_WOLFMLINK_LIBRARY_PATH}"
		${_WOLFMLINK_SEARCH_OPTS}
		DOC "Path to the MathLink library"
	)

	mark_as_advanced(MathLink_FOUND MathLink_INCLUDE_DIR MathLink_VERSION_MAJOR MathLink_VERSION_MINOR MathLink_VERSION_STRING)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(MathLink
	REQUIRED_VARS MathLink_LIBRARY MathLink_INCLUDE_DIR
	VERSION_VAR MathLink_VERSION_STRING
)

if (MathLink_FOUND)
	set(MathLink_INCLUDE_DIRS ${MathLink_INCLUDE_DIR})
	set(MathLink_LIBRARIES ${MathLink_LIBRARY})
endif()

if(MathLink_FOUND AND NOT TARGET MathLink::MathLink)
	add_library(MathLink::MathLink SHARED IMPORTED)
	set_target_properties(MathLink::MathLink PROPERTIES
		INTERFACE_INCLUDE_DIRECTORIES "${MathLink_INCLUDE_DIR}"
		IMPORTED_LOCATION "${MathLink_LIBRARIES}"
	)
endif()