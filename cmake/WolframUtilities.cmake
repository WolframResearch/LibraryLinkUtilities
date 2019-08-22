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

# helper function for get_library_from_cvs
function(_set_package_args PACKAGE_SYSTEM_ID PACKAGE_BUILD_PLATFORM TAG VALUE)
	if(${TAG} STREQUAL SYSTEM_ID)
		set(${PACKAGE_SYSTEM_ID} ${VALUE} PARENT_SCOPE)
	elseif(${TAG} STREQUAL BUILD_PLATFORM)
		set(${PACKAGE_BUILD_PLATFORM} ${VALUE} PARENT_SCOPE)
	endif()
endfunction()

# Download a library from Wolfram's CVS repository and set PACKAGE_LOCATION to the download location.
# SystemId and BuildPlatform can be provided as optional arguments to only download a specific instance of the library.
# If a Source directory exists in the component root directory, it will be downloaded.
function(get_library_from_cvs PACKAGE_NAME PACKAGE_VERSION PACKAGE_LOCATION)

	message(STATUS "Looking for CVS library: ${PACKAGE_NAME} version ${PACKAGE_VERSION}")

	# Check optional system id and build platform
	if(ARGC GREATER_EQUAL 5)
		_set_package_args(PACKAGE_SYSTEM_ID PACKAGE_BUILD_PLATFORM ${ARGV3} ${ARGV4})
		if(ARGC GREATER_EQUAL 7)
			_set_package_args(PACKAGE_SYSTEM_ID PACKAGE_BUILD_PLATFORM ${ARGV5} ${ARGV6})
		endif()
	endif()

	set(_PACKAGE_PATH_SUFFIX ${PACKAGE_VERSION})
	if(PACKAGE_SYSTEM_ID)
		set(_PACKAGE_PATH_SUFFIX ${_PACKAGE_PATH_SUFFIX}/${PACKAGE_SYSTEM_ID})
		if(PACKAGE_BUILD_PLATFORM)
			set(_PACKAGE_PATH_SUFFIX ${_PACKAGE_PATH_SUFFIX}/${PACKAGE_BUILD_PLATFORM})
		endif()
	endif()

	# Download component library
	include(FetchContent)
	FetchContent_declare(
		${PACKAGE_NAME}
		SOURCE_DIR ${${PACKAGE_LOCATION}}/${_PACKAGE_PATH_SUFFIX}
		CVS_REPOSITORY $ENV{CVSROOT}
		CVS_MODULE "Components/${PACKAGE_NAME}/${_PACKAGE_PATH_SUFFIX}"
	)

	string(TOLOWER ${PACKAGE_NAME} lc_package_name)
	FetchContent_getproperties(${PACKAGE_NAME})
	if (NOT ${lc_package_name}_POPULATED)
		message(STATUS "Downloading CVS library: ${PACKAGE_NAME}")
		FetchContent_populate(${PACKAGE_NAME})
	endif ()

	# Check if a Source directory exists
	execute_process(
		COMMAND cvs -d $ENV{CVSROOT} rdiff -r HEAD Components/${PACKAGE_NAME}/${PACKAGE_VERSION}/Source
		WORKING_DIRECTORY ${${PACKAGE_LOCATION}}
		RESULT_VARIABLE RES
		OUTPUT_QUIET ERROR_QUIET
	)
	if("${RES}" STREQUAL "0")
		# Download component source
		FetchContent_declare(
			${PACKAGE_NAME}_SOURCE
			SOURCE_DIR ${${PACKAGE_LOCATION}}/${PACKAGE_VERSION}/Source
			CVS_REPOSITORY $ENV{CVSROOT}
			CVS_MODULE "Components/${PACKAGE_NAME}/${PACKAGE_VERSION}/Source"
		)

		FetchContent_getproperties(${PACKAGE_NAME}_SOURCE)
		if (NOT ${lc_package_name}_source_POPULATED)
			message(STATUS "Downloading CVS source: ${PACKAGE_NAME}/${PACKAGE_VERSION}/Source")
			FetchContent_populate(${PACKAGE_NAME}_SOURCE)
		endif ()
	endif()

	set(${PACKAGE_LOCATION} ${${lc_package_name}_SOURCE_DIR} PARENT_SCOPE)

	message(STATUS "${PACKAGE_NAME} downloaded to ${${PACKAGE_LOCATION}}/${_PACKAGE_PATH_SUFFIX}")

endfunction()


# Splits comma delimited string STR and saves list to variable LIST
function(split_string_to_list STR LIST)
	string(REPLACE " " "" _STR ${STR})
	string(REPLACE "," ";" _STR ${_STR})
	set(${LIST} ${_STR} PARENT_SCOPE)
endfunction()

# Finds library.conf and for each library therein sets:
# ${LIBRARY_NAME}_SYSTEMID
# ${LIBRARY_NAME}_VERSION
# ${LIBRARY_NAME}_BUILD_PLATFORM
function(find_and_parse_library_conf)
	if(ARGC GREATER_EQUAL 1)
		set(LIBRARY_CONF "${ARGV0}")
	else()
		set(LIBRARY_CONF "${CMAKE_CURRENT_SOURCE_DIR}/scripts/library.conf")
	endif()
	if(NOT EXISTS ${LIBRARY_CONF})
		message(FATAL_ERROR "Unable to find ${LIBRARY_CONF}")
	endif()

	file(STRINGS ${LIBRARY_CONF} _LIBRARY_CONF_STRINGS)
	# lines beginning with '#' shall be ignored.
	list(FILTER _LIBRARY_CONF_STRINGS EXCLUDE REGEX "^#")

	set(_LIBRARY_CONF_LIBRARY_LIST ${_LIBRARY_CONF_STRINGS})
	list(FILTER _LIBRARY_CONF_LIBRARY_LIST INCLUDE REGEX "\\[Library\\]")

	string(REGEX REPLACE
		"\\[Library\\][ \t]+(.*)" "\\1" 
		_LIBRARY_CONF_LIBRARY_LIST "${_LIBRARY_CONF_LIBRARY_LIST}"
	)
	split_string_to_list(${_LIBRARY_CONF_LIBRARY_LIST} _LIBRARY_CONF_LIBRARY_LIST)

	detect_system_id(SYSTEMID)

	foreach(LIBRARY ${_LIBRARY_CONF_LIBRARY_LIST})
		string(TOUPPER ${LIBRARY} _LIBRARY)

		set(LIB_SYSTEMID ${_LIBRARY}_SYSTEMID)
		set(LIB_VERSION ${_LIBRARY}_VERSION)
		set(LIB_BUILD_PLATFORM ${_LIBRARY}_BUILD_PLATFORM)

		if(NOT ${LIB_SYSTEMID})
			set(${LIB_SYSTEMID} ${SYSTEMID})
			set(${LIB_SYSTEMID} ${SYSTEMID} PARENT_SCOPE)
		endif()

		set(_LIBRARY_CONF_LIBRARY_STRING ${_LIBRARY_CONF_STRINGS})
		list(FILTER _LIBRARY_CONF_LIBRARY_STRING INCLUDE REGEX "${${LIB_SYSTEMID}}[ \t]+${LIBRARY}")

		if(NOT _LIBRARY_CONF_LIBRARY_STRING)
			list(APPEND UNUSED_LIBRARIES ${LIBRARY})
			message(STATUS "Skipping library ${LIBRARY}")
			continue()
		endif()

		string(REGEX REPLACE
			"${${LIB_SYSTEMID}}[ \t]+${LIBRARY}[ \t]+([A-Za-z0-9.]+)[ \t]+([A-Za-z0-9_\\-]+)" "\\1;\\2"
			_LIB_VERSION_BUILD_PLATFORM "${_LIBRARY_CONF_LIBRARY_STRING}"
		)

		list(GET _LIB_VERSION_BUILD_PLATFORM 0 _LIB_VERSION)
		list(GET _LIB_VERSION_BUILD_PLATFORM 1 _LIB_BUILD_PLATFORM)

		set(${LIB_VERSION} ${_LIB_VERSION} PARENT_SCOPE)

		set(${LIB_BUILD_PLATFORM} ${_LIB_BUILD_PLATFORM} PARENT_SCOPE)
	endforeach()
endfunction()


# Resolve full path to a CVS dependency, downloading if necessary
# Prioritize ${LIB_NAME}_DIR, ${LIB_NAME}_LOCATION, CVS_COMPONENTS_DIR, then CVS download
# Do not download if ${LIB_NAME}_DIR or ${LIB_NAME}_LOCATION are set
# ${LIB_NAME}_VERSION|SYSTEMID|BUILD_PLATFORM are expected to be previously set
function(find_cvs_dependency LIB_NAME)

	# helper variables
	string(TOUPPER ${LIB_NAME} _LIB_NAME)
	set(LIB_DIR "${${_LIB_NAME}_DIR}")
	set(LIB_LOCATION "${${_LIB_NAME}_LOCATION}")
	set(LIB_VERSION ${${_LIB_NAME}_VERSION})
	set(LIB_SYSTEMID ${${_LIB_NAME}_SYSTEMID})
	set(LIB_BUILD_PLATFORM ${${_LIB_NAME}_BUILD_PLATFORM})
	set(_LIB_DIR_SUFFIX ${LIB_VERSION}/${LIB_SYSTEMID}/${LIB_BUILD_PLATFORM})

	# Check if there is a full path to the dependency with version, system id and build platform.
	if(NOT ${LIB_DIR} STREQUAL "")
		if(NOT EXISTS ${LIB_DIR})
			message(FATAL_ERROR "Specified full path to Lib does not exist: ${LIB_DIR}")
		endif()
		return()
	endif()

	# Check if there is a path to the Lib component
	if(NOT ${LIB_LOCATION} STREQUAL "")
		if(NOT EXISTS ${LIB_LOCATION})
			message(FATAL_ERROR "Specified location of Lib does not exist: ${LIB_LOCATION}")
		elseif(EXISTS ${LIB_LOCATION}/${_LIB_DIR_SUFFIX})
			set(${_LIB_NAME}_DIR ${LIB_LOCATION}/${_LIB_DIR_SUFFIX} PARENT_SCOPE)
			return()
		endif()
	endif()

	# Check if there is a path to CVS modules
	if(CVS_COMPONENTS_DIR)
		set(_CVS_COMPONENTS_DIR ${CVS_COMPONENTS_DIR})
	elseif(DEFINED ENV{CVS_COMPONENTS_DIR})
		set(_CVS_COMPONENTS_DIR $ENV{CVS_COMPONENTS_DIR})
	endif()

	if(_CVS_COMPONENTS_DIR)
		if(NOT EXISTS ${_CVS_COMPONENTS_DIR})
			message(FATAL_ERROR "Specified location of CVS components does not exist: ${_CVS_COMPONENTS_DIR}")
		elseif(EXISTS ${_CVS_COMPONENTS_DIR}/${LIB_NAME}/${_LIB_DIR_SUFFIX})
			set(${_LIB_NAME}_DIR ${_CVS_COMPONENTS_DIR}/${LIB_NAME}/${_LIB_DIR_SUFFIX} PARENT_SCOPE)
			return()
		endif()
	endif()

	# Finally download component from cvs
	# Set location of library sources checked out from cvs
	set(LIB_LOCATION "${CMAKE_BINARY_DIR}/Components/${LIB_NAME}")
	set(${_LIB_NAME}_LOCATION ${LIB_LOCATION} CACHE PATH "Location of ${LIB_NAME} root directory.")

	get_library_from_cvs(${LIB_NAME} ${LIB_VERSION} LIB_LOCATION
		SYSTEM_ID ${LIB_SYSTEMID}
		BUILD_PLATFORM ${LIB_BUILD_PLATFORM}
	)
	set(${_LIB_NAME}_DIR ${LIB_LOCATION} PARENT_SCOPE)

endfunction()

# Sets SEARCH_OPTS depending on whether the variable PATH has a value.
function(set_search_opts_from_path PATH SEARCH_OPTS)
	if(${PATH})
		set(${SEARCH_OPTS} NO_DEFAULT_PATH PARENT_SCOPE)
	else()
		set(${SEARCH_OPTS} PARENT_SCOPE)
	endif()
endfunction()

# Detects whether a library is shared or static. Setting type in add_library() ensures target TYPE property will be available later.
function(detect_library_type LIBRARY TYPE_VAR)
	get_filename_component(_EXT ${LIBRARY} EXT)
	if("${_EXT}" STREQUAL "${CMAKE_SHARED_LIBRARY_SUFFIX}")
		set(${TYPE_VAR} SHARED PARENT_SCOPE)
	elseif("${_EXT}" STREQUAL "${CMAKE_STATIC_LIBRARY_SUFFIX}")
		set(${TYPE_VAR} STATIC PARENT_SCOPE)
	else()
		set(${TYPE_VAR} UNKNOWN PARENT_SCOPE)
	endif()
endfunction()

# Copies dependency libraries into paclet layout if the library type is SHARED (always copies on Windows).
# If the optional 3rd argument is not specified (the libraries to copy), defaults to main target file.
function(install_dependency_files PACLET_NAME DEP_TARGET_NAME)
	get_target_property(_DEP_TYPE ${DEP_TARGET_NAME} TYPE)
	if(MSVC OR "${_DEP_TYPE}" STREQUAL SHARED_LIBRARY)
		if(ARGC GREATER_EQUAL 3)
			set(DEP_LIBS ${ARGV2})
		else()
			set(DEP_LIBS $<TARGET_FILE:${DEP_TARGET_NAME}>)
		endif()
		string(REPLACE ".lib" ".dll" DEP_LIBS_DLL "${DEP_LIBS}")
		detect_system_id(SYSTEMID)
		install(FILES
			${DEP_LIBS_DLL}
			DESTINATION ${PACLET_NAME}/LibraryResources/${SYSTEMID}
		)
	endif()
endfunction()

# On MacOS, changes the loader path for a dependency library to point to the library's location in a Mathematica layout.
# This is used when linking against a dependency already included in another paclet (for example, CURLLink's libcurl).
function(change_loader_path_for_layout TARGETNAME DEP_TARGETNAME DEP_PACLETNAME)
	if(APPLE)
		add_custom_command(TARGET ${TARGETNAME} POST_BUILD
			COMMAND "install_name_tool" ARGS
				"-change"
				"@loader_path/$<TARGET_FILE_NAME:${DEP_TARGETNAME}>"
				"@executable_path/../SystemFiles/Links/${DEP_PACLETNAME}/LibraryResources/${SYSTEMID}/$<TARGET_FILE_NAME:${DEP_TARGETNAME}>"
				"$<TARGET_FILE:${TARGETNAME}>"
		)
	endif()
endfunction()

# Forces static runtime on Windows. See https://gitlab.kitware.com/cmake/community/wikis/FAQ#dynamic-replace
function(set_windows_static_runtime)
	if(WIN32)
		foreach(flag_var CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
			if(${flag_var} MATCHES "/MD")
				string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
			endif()
		endforeach()
	endif()
endfunction()

# Helper function for copying paclet to install location
function(_copy_paclet_files OLDSTYLE_Q TARGET_NAME LLU_INSTALL_DIR)
	if(ARGC GREATER_EQUAL 4)
		set(PACLET_NAME ${ARGV3})
	else()
		set(PACLET_NAME ${TARGET_NAME})
	endif()
	if(ARGC GREATER_EQUAL 5)
		set(PACLET_DIRECTORY ${ARGV4})
	else()
		set(PACLET_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${PACLET_NAME})
	endif()
	#copy over the paclet directory - i.e. the main .m file, the Kernel directory, etc.
	install(DIRECTORY ${PACLET_DIRECTORY}
		DESTINATION ${CMAKE_INSTALL_PREFIX}
		PATTERN ".DS_Store" EXCLUDE
	)
	#copy the library produced into LibraryResources/$SystemID
	detect_system_id(SYSTEMID)
	install(TARGETS ${TARGET_NAME}
		LIBRARY DESTINATION ${PACLET_NAME}/LibraryResources/${SYSTEMID}
		RUNTIME DESTINATION ${PACLET_NAME}/LibraryResources/${SYSTEMID}
	)
	# copy LLU top-level code
	if(NOT "${LLU_INSTALL_DIR}" STREQUAL "")
		install(FILES "${LLU_INSTALL_DIR}/share/LibraryLinkUtilities.wl"
			DESTINATION "${PACLET_NAME}/LibraryResources"
		)
	else()
		message(WARNING "*** Specified variable LLU_INSTALL_DIR is empty. This may be OK if the paclet is not using LLU. ***")
	endif()
	#copy PacletInfo.m
	if(${OLDSTYLE_Q})
		set(PACLET_INFO_M PacletInfo.m)
	else()
		set(PACLET_INFO_M ${PACLET_NAME}/PacletInfo.m)
	endif()
	install(FILES ${PACLET_INFO_M}
		DESTINATION "${PACLET_NAME}"
	)
endfunction()

# The following two functions copy paclet files (either updateable or oldstyle layout) to install location.
# Optional 3rd arg is PacletName (defaults to TARGET_NAME). Optional 4th arg is paclet location (defaults to CMAKE_CURRENT_SOURCE_DIR/PacletName).
# CMAKE_INSTALL_PREFIX should be set appropriately before calling this.
function(copy_updateable_paclet_files TARGET_NAME LLU_INSTALL_DIR)
	_copy_paclet_files(FALSE ${TARGET_NAME} "${LLU_INSTALL_DIR}" ${ARGN})
endfunction()

function(copy_oldstyle_paclet_files TARGET_NAME LLU_INSTALL_DIR)
	_copy_paclet_files(TRUE ${TARGET_NAME} "${LLU_INSTALL_DIR}" ${ARGN})
endfunction()

# Installs paclet into a Mathematica layout if requested.
function(install_paclet_to_layout PACLET_NAME INSTALLQ)
	if(${INSTALLQ})
		if(EXISTS "${MATHEMATICA_INSTALL_DIR}")
			install(DIRECTORY "${CMAKE_INSTALL_PREFIX}/${PACLET_NAME}"
				DESTINATION "${MATHEMATICA_INSTALL_DIR}/SystemFiles/Links"
			)
		else()
			message(WARNING "Failed to install paclet to layout: \"${MATHEMATICA_INSTALL_DIR}\" does not exist.")
		endif()
	endif()
endfunction()

# Creates a custom 'zip' target for a paclet.
# CMAKE_INSTALL_PREFIX should be set appropriately before calling this.
function(create_zip_target PACLET_NAME)
	add_custom_target(zip
		COMMAND ${CMAKE_COMMAND} -E tar "cfv" "${CMAKE_INSTALL_PREFIX}/${PACLET_NAME}.zip" --format=zip "${CMAKE_INSTALL_PREFIX}/${PACLET_NAME}"
		COMMENT "Creating zip..."
	)
endfunction()

# Append a cmake variable to a list of options
macro(append_def OPTS VAR)
	if(${VAR})
		list(APPEND ${OPTS} "-D${VAR}=${${VAR}}")
	endif()
endmacro()

# Append a cmake flag to a list of options
macro(append_opt OPTS FLAG VAR)
	if(${VAR})
		list(APPEND ${OPTS} ${FLAG} "${${VAR}}")
	endif()
endmacro()