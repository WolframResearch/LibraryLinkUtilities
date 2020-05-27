# FindMathematica.cmake
#
# Finds Mathematica executable together with other components of the installation, if requested. The following locations are searched:
#   - location specified via Mathematica_ROOT or Mathematica_INSTALL_DIR
#   - default install directory for given OS
#   - system path
#
# Supported components are:
#   - WolframLibrary
#   - WSTP
#   - wolframscript
#
# Components are searched for in the same installation where Mathematica executable was found but this can be overriden by specifying WOLFRAM_LIBRARY_PATH
# or WSTP_PATH as custom locations of Wolfram Library and WSTP, respectively.
#
# This module will define the following variables
#
#    Mathematica_FOUND
#    Mathematica_VERSION
#    Mathematica_INSTALL_DIR
#    Mathematica_EXE
#
# And for specific components, if they were requested:
#
# WolframLibrary:
#    Mathematica_WolframLibrary_FOUND
#    WolframLibrary_FOUND
#    WolframLibrary_INCLUDE_DIRS
#    WolframLibrary_VERSION
#
#    and the imported target WolframLibrary::WolframLibrary
#
# WSTP:
#    Mathematica_WSTP_FOUND
#    WSTP_FOUND
#    WSTP_INCLUDE_DIRS
#    WSTP_LIBRARIES
#    WSTP_VERSION
#
#    and the imported target WSTP::WSTP
#
# wolframscript:
#    Mathematica_wolframscript_FOUND
#    Mathematica_wolframscript_EXE
#
# Author: Rafal Chojna - rafalc@wolfram.com


include("${CMAKE_CURRENT_LIST_DIR}/Wolfram/Common.cmake")

set(_MMA_CONSIDERED_VERSIONS 12.2;12.1;12.0) # LLU requires Mathematica 12.0+, so we do not look for older versions
set(_MMA_FIND_NAMES Mathematica mathematica)
set(_MMA_FIND_SUFFIXES Executables MacOS Contents/MacOS)
set(_MMA_FIND_DOC "Location of Mathematica executable")

###############################################################################
# Helper functions and macros
###############################################################################

function(parse_mathematica_version M_DIRECTORY VERSION)
	if(APPLE)
		find_file(VERSION_FILE Info.plist PATHS ${M_DIRECTORY} PATH_SUFFIXES Contents NO_DEFAULT_PATH)
	else()
		find_file(VERSION_FILE .VersionID ${M_DIRECTORY})
	endif()
	if(NOT VERSION_FILE)
		set(${VERSION} "${VERSION}-NOTFOUND" PARENT_SCOPE)
		return()
	endif()
	if(APPLE)
		file(READ ${VERSION_FILE} _VERSION_INFO)
		string(REGEX REPLACE ".*<key>CFBundleVersion</key>[ \n\t\r]+<string>([0-9\\.]+)</string>.*" "\\1" VERSION_ID_STRING "${_VERSION_INFO}")
	else()
		file(STRINGS ${VERSION_FILE} VERSION_ID_STRING)
	endif()
	set(${VERSION} ${VERSION_ID_STRING} PARENT_SCOPE)
endfunction()

macro(find_mathematica_from_hint)
	if(Mathematica_ROOT OR Mathematica_INSTALL_DIR)
		find_program(Mathematica_EXE
			NAMES ${_MMA_FIND_NAMES}
			HINTS ${Mathematica_ROOT} ${Mathematica_INSTALL_DIR}
			PATH_SUFFIXES ${_MMA_FIND_SUFFIXES}
			DOC ${_MMA_FIND_DOC}
			NO_DEFAULT_PATH)

		if(NOT Mathematica_EXE)
			message(WARNING
				"Could not find Mathematica in requested location \n${Mathematica_ROOT}${Mathematica_INSTALL_DIR}\n"
				"Looking in default directories...")
		endif()
	endif()
endmacro()

macro(find_mathematica_on_path)
	find_program(Mathematica_EXE
		NAMES ${_MMA_FIND_NAMES}
		PATH_SUFFIXES ${_MMA_FIND_SUFFIXES}
		DOC ${_MMA_FIND_DOC})
endmacro()

function(find_mathematica_in_default_dir MATHEMATICA_VERSION)
	get_default_mathematica_dir(${MATHEMATICA_VERSION} _DEFAULT_DIR)
	find_program(Mathematica_EXE
		NAMES ${_MMA_FIND_NAMES}
		HINTS ${_DEFAULT_DIR}
		PATH_SUFFIXES ${_MMA_FIND_SUFFIXES}
		DOC ${_MMA_FIND_DOC}
		NO_DEFAULT_PATH)
endfunction()

# Locate wolframscript executable, preferably within Mathematica_INSTALL_DIR, if defined
function(find_wolframscript)
	set(CMAKE_FIND_APPBUNDLE NEVER)

	find_program(Mathematica_wolframscript_EXE
		NAMES wolframscript
		HINTS ${Mathematica_INSTALL_DIR}
		PATH_SUFFIXES ${_MMA_FIND_SUFFIXES}
		DOC "Path to wolframscript executable."
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

if (Mathematica_EXE)
	get_filename_component(Mathematica_EXE_REALPATH ${Mathematica_EXE} REALPATH)

	get_filename_component(Mathematica_EXE_DIRECTORY ${Mathematica_EXE_REALPATH} DIRECTORY)

	if(WIN32)
		# On Windows executables are in the installation directory
		set(_MATHEMATICA_DIRECTORY ${Mathematica_EXE_DIRECTORY})
	else()
		# Jump one level up from the Executables directory
		get_filename_component(_MATHEMATICA_DIRECTORY ${Mathematica_EXE_DIRECTORY} DIRECTORY)
	endif()

	parse_mathematica_version(${_MATHEMATICA_DIRECTORY} Mathematica_Version)

	set(Mathematica_INSTALL_DIR ${_MATHEMATICA_DIRECTORY} CACHE PATH "Path to the root folder of Mathematica installation.")
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
			Mathematica_EXE
		VERSION_VAR
			Mathematica_Version
		FAIL_MESSAGE
			"Could not find Mathematica, please set the path to Mathematica installation folder in the variable Mathematica_INSTALL_DIR"
		HANDLE_COMPONENTS)