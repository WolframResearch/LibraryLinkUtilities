################################################################################
######
###### LibraryLinkUtilities CMake Configuration File
######
###### Author: Rafal Chojna - rafalc@wolfram.com
#################################################################################


cmake_minimum_required(VERSION 3.8.0)

#set minimum MacOS target version if not defined as environment variable
if(APPLE AND NOT DEFINED ENV{MACOSX_DEPLOYMENT_TARGET})
	set(CMAKE_OSX_DEPLOYMENT_TARGET "10.12" CACHE STRING "Minimum OS X deployment version")
endif()

project(LLU VERSION 2.0.0 LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

include(WolframUtilities)

#LLU is designed to be used as a static library but if you really want to try shared, swith to ON
option(BUILD_SHARED_LIBS "Whether to build LLU as a shared library (default is static).")
mark_as_advanced(BUILD_SHARED_LIBS)

#Mathematica version
set(MATHEMATICA_VERSION 12.1 CACHE STRING "Mathematica version")

#Mathematica location
if(NOT MATHEMATICA_INSTALL_DIR)
	get_default_mathematica_dir(${MATHEMATICA_VERSION} _MATHEMATICA_INSTALL_DIR)
else()
	set(_MATHEMATICA_INSTALL_DIR ${MATHEMATICA_INSTALL_DIR})
endif()
set(MATHEMATICA_INSTALL_DIR ${_MATHEMATICA_INSTALL_DIR} CACHE PATH "Mathematica installation" FORCE)

# set default build type to Release
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRINGS "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel." FORCE)
	set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS ${CMAKE_CONFIGURATION_TYPES})
endif()

# verbose output - useful for debugging, but turned off by default
set(CMAKE_VERBOSE_MAKEFILE OFF CACHE BOOL "Verbose")

#guard to prevent this file from running twice
if(NOT TARGET LLU)

	detect_system_id(DEFAULT_SYSTEMID)
	message(STATUS "Detected SystemID: " "${DEFAULT_SYSTEMID}")
	set(SYSTEMID ${DEFAULT_SYSTEMID} CACHE STRING "Wolfram System ID")

	#=============================================
	#=========== WOLFRAM MATHLINK ================
	#=============================================

	find_package(MathLink 4.25 REQUIRED MODULE)

	#=============================================
	#=============== LIBRARYLINK =================
	#=============================================

	find_package(WolframLibrary 5 REQUIRED MODULE)

	#=============================================
	#=========== MAIN PACLET LIBRARY =============
	#=============================================

	set(LLU_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
	set(LLU_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

	# define source files
	set(LLU_SOURCE_FILES
		${LLU_SOURCE_DIR}/Containers/Image.cpp
		${LLU_SOURCE_DIR}/LibraryData.cpp
		${LLU_SOURCE_DIR}/ErrorLog/LibraryLinkError.cpp
		${LLU_SOURCE_DIR}/MArgumentManager.cpp
		${LLU_SOURCE_DIR}/Containers/MArrayDimensions.cpp
		${LLU_SOURCE_DIR}/Containers/NumericArray.cpp
		${LLU_SOURCE_DIR}/Containers/Tensor.cpp
		${LLU_SOURCE_DIR}/ML/Get.cpp
		${LLU_SOURCE_DIR}/ML/Put.cpp
		${LLU_SOURCE_DIR}/ML/Release.cpp
		${LLU_SOURCE_DIR}/ML/Strings.cpp
		${LLU_SOURCE_DIR}/ML/Utilities.cpp
		${LLU_SOURCE_DIR}/MArgument.cpp
		${LLU_SOURCE_DIR}/ProgressMonitor.cpp
        ${LLU_SOURCE_DIR}/ErrorLog/ErrorManager.cpp
		${LLU_SOURCE_DIR}/ErrorLog/Errors.cpp
		${LLU_SOURCE_DIR}/ErrorLog/Logger.cpp
	)

	#add the main library
	add_library(LLU ${LLU_SOURCE_FILES})

	#compilation options
	set_target_properties(LLU PROPERTIES
	    CXX_STANDARD 14
	    CXX_STANDARD_REQUIRED YES
	    CXX_EXTENSIONS NO
		CXX_VISIBILITY_PRESET hidden
	    POSITION_INDEPENDENT_CODE YES
	)

	if(MSVC)
		# Remove default warning level. It will be set later.
		string(REGEX REPLACE "/W[0-4]" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

		# Force static runtime on Windows. See https://gitlab.kitware.com/cmake/community/wikis/FAQ#dynamic-replace
		foreach(flag_var CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
			if(${flag_var} MATCHES "/MD")
				string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
			endif()
		endforeach()

		target_compile_options(LLU
			PRIVATE "/W4"
			PRIVATE "/EHsc"
			PRIVATE "$<$<CONFIG:Debug>:/Zi>"
			PRIVATE "$<$<NOT:$<CONFIG:Debug>>:/O2>"
		)
	else()
		target_compile_options(LLU
			PRIVATE "-Wall"
			PRIVATE "-Wextra"
			PRIVATE "-pedantic"
			PRIVATE "-Werror-implicit-function-declaration"
			PRIVATE "$<$<NOT:$<CONFIG:Debug>>:-O3>"
		)
	endif()

	# tell all targets importing LLU that public part of LLU uses C++14 features
	target_compile_features(LLU
		PUBLIC
			cxx_std_14
	)

	#add the include directory for the project
	target_include_directories(LLU
		PUBLIC
			$<INSTALL_INTERFACE:include>
			$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		PRIVATE
			${CMAKE_CURRENT_SOURCE_DIR}/src
	)

	#link libraries
	target_link_libraries(LLU
		PUBLIC
			MathLink::MathLink
			WolframLibrary::WolframLibrary
	)

	if (NOT WIN32)
		set_rpath(LLU "")
	endif ()
	set_machine_flags(LLU)

	##############################################
	# Installation instructions

	# By default install to "install" directory
	if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
		set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/install" CACHE PATH "Library install prefix" FORCE)
	endif ()

	include(GNUInstallDirs)
	set(INSTALL_CONFIGDIR cmake/LLU)

	install(TARGETS LLU
		EXPORT LLU-targets
		LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
		ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	)

	install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

	#Export the targets to a script
	install(EXPORT LLU-targets
		FILE
			LLUTargets.cmake
		NAMESPACE
			LLU::
		DESTINATION
			${INSTALL_CONFIGDIR}
	)

	#Create a ConfigVersion.cmake file
	include(CMakePackageConfigHelpers)
	write_basic_package_version_file(
		${CMAKE_CURRENT_BINARY_DIR}/LLUConfigVersion.cmake
		VERSION ${PROJECT_VERSION}
		COMPATIBILITY AnyNewerVersion
	)

	configure_package_config_file(${CMAKE_CURRENT_LIST_DIR}/cmake/LLUConfig.cmake.in
		${CMAKE_CURRENT_BINARY_DIR}/LLUConfig.cmake
		INSTALL_DESTINATION ${INSTALL_CONFIGDIR}
	)

	#Install the config, configversion and custom find modules
	install(FILES
		${CMAKE_CURRENT_LIST_DIR}/cmake/FindWolframLibrary.cmake
		${CMAKE_CURRENT_LIST_DIR}/cmake/FindMathLink.cmake
		${CMAKE_CURRENT_LIST_DIR}/cmake/WolframUtilities.cmake
		${CMAKE_CURRENT_BINARY_DIR}/LLUConfig.cmake
		${CMAKE_CURRENT_BINARY_DIR}/LLUConfigVersion.cmake
		DESTINATION ${INSTALL_CONFIGDIR}
	)

	install(FILES
		"${CMAKE_CURRENT_LIST_DIR}/LibraryLinkUtilities.wl"
		DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}
	)

	##############################################
	## Exporting from the build tree

	configure_file(${CMAKE_CURRENT_LIST_DIR}/cmake/WolframUtilities.cmake ${CMAKE_CURRENT_BINARY_DIR}/WolframUtilities.cmake COPYONLY)
	configure_file(${CMAKE_CURRENT_LIST_DIR}/cmake/FindWolframLibrary.cmake ${CMAKE_CURRENT_BINARY_DIR}/FindWolframLibrary.cmake COPYONLY)
	configure_file(${CMAKE_CURRENT_LIST_DIR}/cmake/FindMathLink.cmake ${CMAKE_CURRENT_BINARY_DIR}/FindMathLink.cmake COPYONLY)

	export(EXPORT LLU-targets
		FILE ${CMAKE_CURRENT_BINARY_DIR}/LLUTargets.cmake
		NAMESPACE LLU::
	)

	#Register package in user's package registry
	export(PACKAGE LLU)

	##############################################
	## Create target for unit tests

	enable_testing()

	add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/tests/UnitTests")

	add_custom_target(TestWithOutputOnFailure
		COMMAND
			"${CMAKE_COMMAND}" -E env CTEST_OUTPUT_ON_FAILURE=1 ${CMAKE_CTEST_COMMAND} -C ${CMAKE_BUILD_TYPE}
	)
endif() #end IF NOT TARGET LLU

