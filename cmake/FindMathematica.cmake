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

function(parse_mathematica_version M_DIRECTORY VERSION)
	find_file(VERSION_ID_FILE .VersionID ${M_DIRECTORY})
	if(NOT VERSION_ID_FILE)
		set(${VERSION} "${VERSION}-NOTFOUND" PARENT_SCOPE)
		return()
	endif()
	file(STRINGS ${VERSION_ID_FILE} VERSION_ID_STRING)
	set(${VERSION} ${VERSION_ID_STRING} PARENT_SCOPE)
endfunction()

if (NOT Mathematica_ROOT AND MATHEMATICA_DIR)
	set(Mathematica_ROOT ${MATHEMATICA_DIR})
endif()

cmake_print_variables(Mathematica_ROOT)
find_program(_MATHEMATICA_EXE
		NAMES
			Mathematica mathematica
		HINTS
			${Mathematica_ROOT}
		PATH_SUFFIXES
			Executables
		DOC
			"Location of Mathematica executable")

cmake_print_variables(_MATHEMATICA_EXE)

if (_MATHEMATICA_EXE)
	get_filename_component(_MATHEMATICA_EXE_REALPATH ${_MATHEMATICA_EXE} REALPATH)

	cmake_print_variables(_MATHEMATICA_EXE_REALPATH)

	get_filename_component(_MATHEMATICA_EXE_DIRECTORY ${_MATHEMATICA_EXE_REALPATH} DIRECTORY)
	get_filename_component(_MATHEMATICA_DIRECTORY ${_MATHEMATICA_EXE_DIRECTORY} DIRECTORY)

	cmake_print_variables(_MATHEMATICA_DIRECTORY)

	parse_mathematica_version(${_MATHEMATICA_DIRECTORY} MMA_VERSION_STRING)

	cmake_print_variables(MMA_VERSION_STRING)

	cmake_print_variables(Mathematica_FIND_COMPONENTS)

	set(MATHEMATICA_INSTALL_DIR ${_MATHEMATICA_DIRECTORY})
endif()

foreach(_COMP IN LISTS Mathematica_FIND_COMPONENTS)
	if(_COMP STREQUAL "WolframLibrary")
		find_package(WolframLibrary ${Mathematica_WOLFRAMLIBRARY_VERSION_REQUIRED} QUIET)
		set(Mathematica_${_COMP}_FOUND ${WolframLibrary_FOUND})
	elseif(_COMP STREQUAL "WSTP")
		find_package(WSTP ${Mathematica_WSTP_VERSION_REQUIRED} QUIET)
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