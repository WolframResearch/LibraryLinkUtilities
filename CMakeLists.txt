################################################################################
######
###### LibraryLinkUtilities CMake Configuration File
######
#################################################################################


cmake_minimum_required(VERSION 3.0.0)


project(LLU)

#project settings, overridable from the command line
set(PACLET_NAME LLU)
set(PACLET_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${PACLET_NAME})

#set(MATHEMATICA_VERSION 11.2 CACHE STRING "Mathematica version")
#
# Mathematica location
#if(APPLE)
#	set(MATHEMATICA_INSTALL_DIR "/Applications/Mathematica.app/Contents" CACHE PATH "Mathematica installation")
#elseif(WIN32)
#  set(MATHEMATICA_INSTALL_DIR "C:/Program\ Files/Wolfram\ Research/Mathematica/${MATHEMATICA_VERSION}" CACHE PATH "Mathematica installation")
#else()
#	set(MATHEMATICA_INSTALL_DIR "/usr/local/Wolfram/Mathematica/${MATHEMATICA_VERSION}" CACHE PATH "Mathematica installation")
#endif()
#
#if(NOT CMAKE_BUILD_TYPE)
#  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel." FORCE)
#endif()
#
#set(INSTALL_PACLET_TO_LAYOUT OFF CACHE BOOL "Copy paclet into Mathematica installation")
#
# verbose output - useful for debugging, but turned off by default
#set(CMAKE_VERBOSE_MAKEFILE OFF CACHE BOOL "Verbose")

#guard to prevent this file from running twice
if(NOT TARGET LLU)

	#=============================================
	#========== LIBRARYLINK UTILITIES ============
	#=============================================

	#set(LLU_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/CPPSource/LibraryLinkUtilities CACHE PATH "Location of LibraryLink Utilities")
	set(LLU_INCLUDE_DIR ${LLU_LOCATION}/src)
	get_filename_component(LLU_INCLUDE_DIR_PARENT ${LLU_LOCATION} DIRECTORY)
	#file(GLOB LLU_SOURCE_FILES ${LLU_LOCATION}/src/*.cpp)

	#=============================================
	#=========== MAIN PACLET LIBRARY =============
	#=============================================

	# define source files
	file(GLOB LLU_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp ${LLU_SOURCE_FILES})

	#add the main library
	add_library(LLU OBJECT ${LLU_SOURCE_FILES})

	#compilation options
	set_target_properties(LLU PROPERTIES
	    CXX_STANDARD 14
	    CXX_STANDARD_REQUIRED YES
	    CXX_EXTENSIONS NO
	    POSITION_INDEPENDENT_CODE YES
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
#	
#	
#	if(APPLE)
		#set the linker options to set rpath as @loader_path
#		set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-rpath,@loader_path/")
#	elseif(UNIX)
		#set the install_rpath to be $ORIGIN so that it automatically finds the dependencies in the current folder
#		set(CMAKE_INSTALL_RPATH $ORIGIN CACHE STRING "rpath install location")
		#mark as advanced to discourage modification
#		mark_as_advanced(CMAKE_SHARED_LINKER_FLAGS CMAKE_INSTALL_RPATH)
#	elseif(WIN32)
		#on windows we need to specifically compile with /MT not /MD
#		set_target_properties(LLU PROPERTIES COMPILE_FLAGS "/MT")
#	endif()
#
#
#	if(${SYSTEMID} STREQUAL "MacOSX-x86-64")
#		set(LLU_EXTRA_LIBRARIES "c++" "-framework Foundation")
#	elseif(${SYSTEMID} STREQUAL "Linux")
		#nothing for now
#	elseif(${SYSTEMID} STREQUAL "Linux-x86-64")
		#nothing for now
#	elseif(${SYSTEMID} STREQUAL "Linux-ARM")
		#nothing for now
#	elseif(${SYSTEMID} STREQUAL "Windows")
		#nothing for now
#	elseif(${SYSTEMID} STREQUAL "Windows-x86-64")
		#nothing for now
#	endif()
#	
	#in paclets we skip the usual "lib" prefix
#	set_property(TARGET LLU PROPERTY PREFIX "")
#
	#link libraries
#	target_link_libraries(LLU PRIVATE 
#		${WOLFRAM_MATHLINK_LIBRARIES} 
#		${LLU_EXTRA_LIBRARIES})

	#add the include directory for the project
	target_include_directories(LLU PUBLIC
		${WOLFRAM_MATHLINK_INCLUDE_DIR} 
		${LIBRARYLINK_INCLUDE_DIR}
		${LLU_INCLUDE_DIR}
		${LLU_INCLUDE_DIR_PARENT}
	)


endif() #end IF NOT TARGET LLU

