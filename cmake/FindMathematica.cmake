# FindMathematica.cmake
#
# Finds an installation of Mathematica. By "installation of Mathematica" we mean a directory DIR such that:
#
# - DIR/.VersionID is a text file containing a version number of the form X.Y.Z.A
# - DIR/Executables/Mathematica is an executable file
# - DIR/Executables/WolframKernel is an executable file
#
# This module will define the following variables
#
#    Mathematica_FOUND
#    Mathematica_VERSION
#    Mathematica_INSTALL_DIR
#    Mathematica_EXE
#    Mathematica_KERNEL_EXE
#
# You can specify custom location to search for Wolfram Library either by specifying WOLFRAM_LIBRARY_PATH explicitly,
# or if that variable is not set, by providing MATHEMATICA_INSTALL_DIR variable with a path to Mathematica installation.
#
# Author: Rafal Chojna - rafalc@wolfram.com

include(CMakePrintHelpers)
include("${CMAKE_CURRENT_LIST_DIR}/Wolfram/Common.cmake")

set(_MMA_CONSIDERED_VERSIONS 12.2;12.1;12.0) # LLU requires Mathematica 12.0+, so we do not look for older versions
set(_MMA_FIND_NAMES Mathematica mathematica)
set(_MMA_FIND_SUFFIXES Executables)
set(_MMA_FIND_DOC "Location of Mathematica executable")

###############################################################################
# Helper functions and macros
###############################################################################

function(parse_mathematica_version M_DIRECTORY VERSION)
	find_file(VERSION_ID_FILE .VersionID ${M_DIRECTORY})
	if(NOT VERSION_ID_FILE)
		set(${VERSION} "${VERSION}-NOTFOUND" PARENT_SCOPE)
		return()
	endif()
	file(STRINGS ${VERSION_ID_FILE} VERSION_ID_STRING)
	set(${VERSION} ${VERSION_ID_STRING} PARENT_SCOPE)
endfunction()

macro(find_mathematica_from_hint)
	if (NOT Mathematica_ROOT AND MATHEMATICA_DIR)
		set(Mathematica_ROOT ${MATHEMATICA_DIR})
	endif()

	cmake_print_variables(Mathematica_ROOT)


	if(Mathematica_ROOT OR MATHEMATICA_DIR OR MATHEMATICA_INSTALL_DIR)
		find_program(_MATHEMATICA_EXE
			NAMES ${_MMA_FIND_NAMES}
			HINTS ${Mathematica_ROOT} ${MATHEMATICA_DIR} ${MATHEMATICA_INSTALL_DIR}
			PATH_SUFFIXES ${_MMA_FIND_SUFFIXES}
			DOC ${_MMA_FIND_DOC}
			NO_DEFAULT_PATH)

		if(NOT _MATHEMATICA_EXE)
			message(WARNING
				"Could not find Mathematica in requested location \n${Mathematica_ROOT}${MATHEMATICA_DIR}${MATHEMATICA_INSTALL_DIR}\n"
				"Looking in default directories...")
		endif()
	endif()
endmacro()

macro(find_mathematica_on_path)
	find_program(_MATHEMATICA_EXE
		NAMES ${_MMA_FIND_NAMES}
		PATH_SUFFIXES ${_MMA_FIND_SUFFIXES}
		DOC ${_MMA_FIND_DOC})
endmacro()

function(find_mathematica_in_default_dir MATHEMATICA_VERSION)
	get_default_mathematica_dir(${MATHEMATICA_VERSION} _DEFAULT_DIR)
	find_program(_MATHEMATICA_EXE
		NAMES ${_MMA_FIND_NAMES}
		HINTS ${_DEFAULT_DIR}
		PATH_SUFFIXES ${_MMA_FIND_SUFFIXES}
		DOC ${_MMA_FIND_DOC}
		NO_DEFAULT_PATH)
endfunction()

###############################################################################
# Action starts here
###############################################################################

# First, respect user-provided hints
find_mathematica_from_hint()

# If no hint provided, search default installation directories
foreach(_MMA_VER IN LISTS _MMA_CONSIDERED_VERSIONS)
	find_mathematica_in_default_dir(${_MMA_VER})
endforeach()

# Finally, try looking for Mathematica on the system path and wherever CMake looks by default
find_mathematica_on_path()

cmake_print_variables(_MATHEMATICA_EXE)

if (_MATHEMATICA_EXE)
	get_filename_component(_MATHEMATICA_EXE_REALPATH ${_MATHEMATICA_EXE} REALPATH)

	cmake_print_variables(_MATHEMATICA_EXE_REALPATH)

	get_filename_component(_MATHEMATICA_EXE_DIRECTORY ${_MATHEMATICA_EXE_REALPATH} DIRECTORY)

	if(WIN32)
		# On Windows executables are in the installation directory
		set(_MATHEMATICA_DIRECTORY ${_MATHEMATICA_EXE_DIRECTORY})
	else()
		# Jump one level up from the Executables directory
		get_filename_component(_MATHEMATICA_DIRECTORY ${_MATHEMATICA_EXE_DIRECTORY} DIRECTORY)
	endif()

	cmake_print_variables(_MATHEMATICA_DIRECTORY)

	parse_mathematica_version(${_MATHEMATICA_DIRECTORY} MMA_VERSION_STRING)

	cmake_print_variables(MMA_VERSION_STRING)

	cmake_print_variables(Mathematica_FIND_COMPONENTS)

	set(MATHEMATICA_INSTALL_DIR ${_MATHEMATICA_DIRECTORY})
endif()

foreach(_COMP IN LISTS Mathematica_FIND_COMPONENTS)
	if(_COMP STREQUAL "WolframLibrary")
		find_package(WolframLibrary)
		set(Mathematica_${_COMP}_FOUND ${WolframLibrary_FOUND})
	elseif(_COMP STREQUAL "WSTP")
		find_package(WSTP)
		set(Mathematica_${_COMP}_FOUND ${WSTP_FOUND})
	elseif(_COMP STREQUAL "wolframscript")
		find_wolframscript(Mathematica_wolframscript_EXE)
		if(EXISTS ${Mathematica_wolframscript_EXE})
			set(Mathematica_${_COMP}_FOUND TRUE)
		else()
			set(Mathematica_${_COMP}_FOUND FALSE)
		endif()
	else()
		message(WARNING "Unknown Mathematica component \"${_COMP}\" requested.")
		set(Mathematica_${_COMP}_FOUND FALSE)
	endif()
endforeach()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Mathematica
		REQUIRED_VARS
			_MATHEMATICA_EXE
		VERSION_VAR
			MMA_VERSION_STRING
		FAIL_MESSAGE
			"Could NOT find Mathematica, please set the path to Mathematica installation folder in the variable MATHEMATICA_DIR"
		HANDLE_COMPONENTS)