# WolframUtilities.cmake
#
# Set of short utility functions that may be helpful for Mathematica paclets that use CMake
#
# Author: Rafal Chojna - rafalc@wolfram.com

function(get_default_mathematica_dir MATHEMATICA_VERSION DEFAULT_MATHEMATICA_INSTALL_DIR)
	set(_M_INSTALL_DIR NOTFOUND)
	if(APPLE)
	 	find_path(_M_INSTALL_DIR "Contents" PATHS 
			"/Applications/Mathematica ${MATHEMATICA_VERSION}.app"
			"/Applications/Mathematica.app"
		)
		set(_M_INSTALL_DIR "${_M_INSTALL_DIR}/Contents")
	elseif(WIN32)
		set(_M_INSTALL_DIR "C:/Program\ Files/Wolfram\ Research/Mathematica/${MATHEMATICA_VERSION}")
	else()
		set(_M_INSTALL_DIR "/usr/local/Wolfram/Mathematica/${MATHEMATICA_VERSION}")
	endif()
	if(NOT IS_DIRECTORY "${_M_INSTALL_DIR}" AND IS_DIRECTORY "$ENV{MATHEMATICA_HOME}")
		set(_M_INSTALL_DIR "$ENV{MATHEMATICA_HOME}")
	endif()
	set(${DEFAULT_MATHEMATICA_INSTALL_DIR} "${_M_INSTALL_DIR}" PARENT_SCOPE)
endfunction()

function(detect_system_id DETECTED_SYSTEM_ID)
	if(NOT ${DETECTED_SYSTEM_ID})
		#set system id and build platform
		set(BITNESS 32)
		if(CMAKE_SIZEOF_VOID_P EQUAL 8)
			set(BITNESS 64)
		endif()

		set(INITIAL_SYSTEMID NOTFOUND)

		# Determine the current machine's systemid.
		if(CMAKE_C_COMPILER MATCHES "androideabi")
			set(INITIAL_SYSTEMID Android)
		elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "arm*")
			set(INITIAL_SYSTEMID Linux-ARM)
		elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND BITNESS EQUAL 64)
			set(INITIAL_SYSTEMID Linux-x86-64)
		elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND BITNESS EQUAL 32)
			set(INITIAL_SYSTEMID Linux)
		elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows" AND BITNESS EQUAL 64)
			set(INITIAL_SYSTEMID Windows-x86-64)
		elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows" AND BITNESS EQUAL 32)
			set(INITIAL_SYSTEMID Windows)
		elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin" AND BITNESS EQUAL 64)
			set(INITIAL_SYSTEMID MacOSX-x86-64)
		elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin" AND BITNESS EQUAL 32)
			set(INITIAL_SYSTEMID MacOSX-x86)
		endif()

		if(NOT INITIAL_SYSTEMID)
			message(FATAL_ERROR "Unable to determine System ID.")
		endif()

		set(${DETECTED_SYSTEM_ID} "${INITIAL_SYSTEMID}" PARENT_SCOPE)
	endif()
endfunction()

function(detect_build_platform DETECTED_BUILD_PLATFORM)
	# Determine the current machine's build platform.
	set(BUILD_PLATFORM Indeterminate)
	if(CMAKE_SYSTEM_NAME STREQUAL "Android")
		if(CMAKE_C_COMPILER_VERSION VERSION_LESS 4.9)
			set(BUILD_PLATFORM_ERROR "Android build with gcc version less than 4.9")
		else()
			set(BUILD_PLATFORM android-16-gcc4.9)
		endif()
	elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "arm*" AND CMAKE_SYSTEM_NAME STREQUAL "Linux")
		if(CMAKE_C_COMPILER_ID STREQUAL "GNU" AND CMAKE_C_COMPILER_VERSION VERSION_LESS 4.7)
			set(BUILD_PLATFORM_ERROR "Arm build with gcc less than 4.7")
		elseif(CMAKE_C_COMPILER AND NOT CMAKE_C_COMPILER_ID STREQUAL "GNU")
			set(BUILD_PLATFORM_ERROR "Arm build with non-gnu compiler")
		else()
			#at some point might be smart to dynamically construct this build platform, but
			#for now it's all we build ARM on so it should be okay
			set(BUILD_PLATFORM armv6-glibc2.19-gcc4.9)
		endif()
	elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
		if(CMAKE_C_COMPILER_ID STREQUAL "GNU" AND CMAKE_C_COMPILER_VERSION VERSION_LESS 5.2)
			set(BUILD_PLATFORM_ERROR "Linux build with gcc less than 5.2")
		elseif(CMAKE_C_COMPILER AND NOT CMAKE_C_COMPILER_ID STREQUAL "GNU")
			set(BUILD_PLATFORM_ERROR "Linux build with non-gnu compiler")
		else()
			set(BUILD_PLATFORM scientific6-gcc4.8)
		endif()
	elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
		if((NOT CMAKE_C_COMPILER) OR (NOT MSVC_VERSION LESS 1900))
			if(MSVC_VERSION EQUAL 1900)
				set(BUILD_PLATFORM vc140)
			elseif(MSVC_VERSION GREATER_EQUAL 1910)
				set(BUILD_PLATFORM vc141)
			endif()
		else()
			set(BUILD_PLATFORM_ERROR "Windows build without VS 2015 or greater.")
		endif()
	elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
		if(CMAKE_SYSTEM_VERSION VERSION_LESS 10.9)
			set(BUILD_PLATFORM_ERROR "OSX build on OSX less than 10.9")
		else()
			set(BUILD_PLATFORM libcxx-min10.9)
		endif()
	else()
		set(BUILD_PLATFORM_ERROR "Unrecognized system type.")
	endif()

	if(BUILD_PLATFORM STREQUAL "Indeterminate")
		message(FATAL_ERROR "Unable to determine Build Platform. Reason: ${BUILD_PLATFORM_ERROR}")
	endif()

	set(${DETECTED_BUILD_PLATFORM} "${BUILD_PLATFORM}" PARENT_SCOPE)
endfunction()

#set MathLink library name depending on the platform
function(get_mathlink_library_name MATHLINK_INTERFACE_VERSION MATHLINK_LIB_NAME)

	detect_system_id(SYSTEM_ID)

	set(MATHLINK_LIBRARY NOTFOUND)
	if(SYSTEM_ID STREQUAL "MacOSX-x86-64")
		set(MATHLINK_LIBRARY "MLi${MATHLINK_INTERFACE_VERSION}")
	elseif(SYSTEM_ID STREQUAL "Linux" OR SYSTEM_ID STREQUAL "Linux-ARM" OR SYSTEM_ID STREQUAL "Windows")
		set(MATHLINK_LIBRARY "ML32i${MATHLINK_INTERFACE_VERSION}")
	elseif(SYSTEM_ID STREQUAL "Linux-x86-64" OR SYSTEM_ID STREQUAL "Windows-x86-64")
		set(MATHLINK_LIBRARY "ML64i${MATHLINK_INTERFACE_VERSION}")
	endif()

	if(NOT MATHLINK_LIBRARY)
		message(FATAL_ERROR "Unable to determine MathLink library name for system: ${SYSTEM_ID}")
	endif()

	set(${MATHLINK_LIB_NAME} "${MATHLINK_LIBRARY}" PARENT_SCOPE)
endfunction()

# not sure if this one is needed, keep it just in case
function(additional_paclet_dependencies SYSTEM_ID EXTRA_LIBS)
	if (${SYSTEM_ID} STREQUAL "MacOSX-x86-64")
		set(EXTRA_LIBS "c++" "-framework Foundation" PARENT_SCOPE)
	elseif (${SYSTEM_ID} STREQUAL "Linux")
		# nothing for now
	elseif (${SYSTEM_ID} STREQUAL "Linux-x86-64")
		# nothing for now
	elseif (${SYSTEM_ID} STREQUAL "Linux-ARM")
		# nothing for now
	elseif (${SYSTEM_ID} STREQUAL "Windows")
		# nothing for now
	elseif (${SYSTEM_ID} STREQUAL "Windows-x86-64")
		# nothing for now
	endif ()
endfunction()

# set machine bitness flags for given target
function(set_machine_flags TARGET_NAME)
	detect_system_id(SYSTEM_ID)

	if(SYSTEM_ID MATCHES "-x86-64")
		if(MSVC)
			set_target_properties(${TARGET_NAME} PROPERTIES LINK_FLAGS "/MACHINE:x64")
		else()
			set_target_properties(${TARGET_NAME} PROPERTIES COMPILE_FLAGS "-m64" LINK_FLAGS "-m64")
		endif()
	elseif(SYSTEM_ID MATCHES "Linux-ARM")
		target_compile_definitions(${TARGET_NAME} PUBLIC MINT_32)
		set_target_properties(${TARGET_NAME} PROPERTIES COMPILE_FLAGS "-marm -march=armv6" LINK_FLAGS "-marm -march=armv6")
	else()
		target_compile_definitions(${TARGET_NAME} PUBLIC MINT_32)
		if(MSVC)
			set_target_properties(${TARGET_NAME} PROPERTIES LINK_FLAGS "/MACHINE:x86")
		else()
			set_target_properties(${TARGET_NAME} PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
		endif()
	endif()
endfunction()

# Sets rpath for a target. If second argument is false then "Wolfram-default" rpath is set:
# - $ORIGIN on Linux
# - @loader_path on Mac
# On Windows rpath does not make sense.
function(set_rpath TARGET_NAME NEW_RPATH)
	if(NOT NEW_RPATH)
		if(APPLE)
			#set the linker options to set rpath as @loader_path
			set(NEW_RPATH "@loader_path/")
		elseif(UNIX)
			#set the install_rpath to be $ORIGIN so that it automatically finds the dependencies in the current folder
			set(NEW_RPATH $ORIGIN)
		endif()
	endif ()
	set_target_properties(${TARGET_NAME} PROPERTIES INSTALL_RPATH ${NEW_RPATH})
endfunction()

# Download a library from Wolfram's CVS repository.
function(get_library_from_cvs PACKAGE_NAME PACKAGE_VERSION PACKAGE_LOCATION)

	message(STATUS "Looking for CVS library: ${PACKAGE_NAME} version ${PACKAGE_VERSION}")

	include(FetchContent)
	FetchContent_declare(
		${PACKAGE_NAME}
		SOURCE_DIR ${PACKAGE_LOCATION}/${PACKAGE_VERSION}
		CVS_REPOSITORY $ENV{CVSROOT}
		CVS_MODULE "Components/${PACKAGE_NAME}/${PACKAGE_VERSION}"
	)

	FetchContent_getproperties(${PACKAGE_NAME})
	if (NOT ${PACKAGE_NAME}_POPULATED)
		message(STATUS "Downloading CVS library: ${PACKAGE_NAME}")
		FetchContent_populate(${PACKAGE_NAME})
	endif ()

	set(${PACKAGE_LOCATION} ${${PACKAGE_NAME}_SOURCE_DIR} PARENT_SCOPE)

	message(STATUS "${PACKAGE_NAME} downloaded to ${PACKAGE_LOCATION}/${PACKAGE_VERSION}")

endfunction()