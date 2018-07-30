################################################################################
######
###### LibraryLinkUtilities CMake Configuration File
######
###### Author: Rafal Chojna - rafalc@wolfram.com
#################################################################################


cmake_minimum_required(VERSION 3.8.0)

project(LLU VERSION 0.0.1 LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

include(WolframUtilities)

#project settings, overridable from the command line
set(MATHEMATICA_VERSION 12.0 CACHE STRING "Mathematica version")

#Mathematica location
if(APPLE)
	set(MATHEMATICA_INSTALL_DIR "/Applications/Mathematica.app/Contents" CACHE PATH "Mathematica installation")
elseif(WIN32)
  set(MATHEMATICA_INSTALL_DIR "C:/Program\ Files/Wolfram\ Research/Mathematica/${MATHEMATICA_VERSION}" CACHE PATH "Mathematica installation")
else()
	set(MATHEMATICA_INSTALL_DIR "/usr/local/Wolfram/Mathematica/${MATHEMATICA_VERSION}" CACHE PATH "Mathematica installation")
endif()

option(BUILD_SHARED_LIBS "Whether to build LLU as a shared library (default is static).")

# set default build type to Release
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()

#set(INSTALL_PACLET_TO_LAYOUT OFF CACHE BOOL "Copy paclet into Mathematica installation")

# verbose output - useful for debugging, but turned off by default
set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "Verbose")

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

	find_package(WolframLibrary 4 REQUIRED MODULE)

	#=============================================
	#=========== MAIN PACLET LIBRARY =============
	#=============================================

	set(LLU_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
	set(LLU_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

	# define source files
	set(LLU_SOURCE_FILES
		${LLU_SOURCE_DIR}/Containers/Image.cpp
		${LLU_SOURCE_DIR}/Containers/LibDataHolder.cpp
		${LLU_SOURCE_DIR}/LibraryLinkError.cpp
		${LLU_SOURCE_DIR}/MArgumentManager.cpp
		${LLU_SOURCE_DIR}/Containers/MArrayBase.cpp
		${LLU_SOURCE_DIR}/Containers/RawArray.cpp
		${LLU_SOURCE_DIR}/Containers/Tensor.cpp
		${LLU_SOURCE_DIR}/ML/Get.cpp
		${LLU_SOURCE_DIR}/ML/Put.cpp
		${LLU_SOURCE_DIR}/ML/Release.cpp
		${LLU_SOURCE_DIR}/ML/Strings.cpp
		${LLU_SOURCE_DIR}/ML/Utilities.cpp
	)

	#add the main library
	add_library(LLU ${LLU_SOURCE_FILES})

	#compilation options
	set_target_properties(LLU PROPERTIES
	    CXX_STANDARD 14
	    CXX_STANDARD_REQUIRED YES
	    CXX_EXTENSIONS NO
	    POSITION_INDEPENDENT_CODE YES
		#$<$<PLATFORM_ID:WIN32>>
		#PREFIX "" #in paclets we skip the usual "lib" prefix
	)
	
	if(MSVC)
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

#	if(APPLE)
#		#set the linker options to set rpath as @loader_path
#		set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-rpath,@loader_path/")
#	elseif(UNIX)
#		#set the install_rpath to be $ORIGIN so that it automatically finds the dependencies in the current folder
#		set(CMAKE_INSTALL_RPATH $ORIGIN CACHE STRING "rpath install location")
#		#mark as advanced to discourage modification
#		mark_as_advanced(CMAKE_SHARED_LINKER_FLAGS CMAKE_INSTALL_RPATH)
#	elseif(WIN32)
#		#on windows we need to specifically compile with /MT not /MD
#		set_target_properties(LLU PROPERTIES COMPILE_FLAGS "/MT")
#	endif()


	##############################################
	# Installation instructions

	include(GNUInstallDirs)
	set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/LLU)

	install(TARGETS LLU
		EXPORT LLU-targets
		LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
		ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	)

	#This is required so that the exported target has the name LLU and not jsonutils
	#set_target_properties(jsonutils PROPERTIES EXPORT_NAME LLU)

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
		${CMAKE_CURRENT_BINARY_DIR}/LLUConfig.cmake
		${CMAKE_CURRENT_BINARY_DIR}/LLUConfigVersion.cmake
		DESTINATION ${INSTALL_CONFIGDIR}
	)
endif() #end IF NOT TARGET LLU

