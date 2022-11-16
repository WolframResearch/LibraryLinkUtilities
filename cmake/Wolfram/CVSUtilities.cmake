# Wolfram/CVSUtilities.cmake
#
# A collection of functions for easy checkout of CVS dependencies, specifically for paclets developed at Wolfram.
#


# Helper function to check developer machine setup; otherwise cmake produces an Extremely Cryptic Error
function(check_cvsroot)
	if(NOT DEFINED ENV{CVSROOT})
		message(FATAL_ERROR "\n[[ ~ \\/\\/\\/\\/\\/\\ ~ CVSROOT is not defined! ~ /\\/\\/\\/\\/\\/ ~ ]]\n")
	endif()
endfunction()

function(check_nexus_root)
	if(NOT DEFINED ENV{NEXUSROOT})
		message(FATAL_ERROR "\n[[ ~ \\/\\/\\/\\/\\/\\ ~ NEXUSROOT is not defined! ~ /\\/\\/\\/\\/\\/ ~ ]]\n")
	endif()
endfunction()

#Helper function to check whether given CVS module exists.
function(cvsmoduleQ MODULE WORKINGDIR RES)
	check_cvsroot()
	execute_process(
			COMMAND cvs -d $ENV{CVSROOT} rdiff -r HEAD ${MODULE}
			WORKING_DIRECTORY ${WORKINGDIR}
			RESULT_VARIABLE _RES
			OUTPUT_QUIET ERROR_QUIET
	)
	if("${_RES}" STREQUAL "0")
		set(${RES} TRUE PARENT_SCOPE)
	else()
		set(${RES} FALSE PARENT_SCOPE)
	endif()
endfunction()

# Helper function to download content from CVS.
function(download_cvs_content content_name download_path module_path DOWNLOAD_LOCATION_OUT)
	check_cvsroot()
	include(FetchContent)
	FetchContent_declare(
			${content_name}
			SOURCE_DIR "${download_path}"
			CVS_REPOSITORY $ENV{CVSROOT}
			CVS_MODULE "${module_path}"
	)
	string(TOLOWER ${content_name} lc_content_name)
	FetchContent_getproperties(${content_name})
	if(NOT ${lc_content_name}_POPULATED)
		message(STATUS "Downloading CVS module: ${module_path}")
		FetchContent_populate(${content_name})
	endif()
	# store the download location in a variable
	set(${DOWNLOAD_LOCATION_OUT} "${${lc_content_name}_SOURCE_DIR}" PARENT_SCOPE)

	if(DEFINED ENV{CVS_LOG_FILE})
		# record cvs checkout "LibraryName" "LibraryVersion" SystemID and such info in ENV{CVS_LOG_FILE}
		if(${module_path} MATCHES "^Components/.*")
			#remove prefix "Components/"
			string(SUBSTRING ${module_path} 11 -1 striped_cvs_path)
			string(REPLACE "/" "\t\t" module_info ${striped_cvs_path})
			file(APPEND $ENV{CVS_LOG_FILE} "${module_info}\n")
		endif()
	endif()
endfunction()

# Download a library from Wolfram's CVS repository and set PACKAGE_LOCATION to the download location.
# If a Source directory exists in the component root directory and DOWNLOAD_CVS_SOURCE is ON, it will be downloaded.
function(get_library_from_cvs PACKAGE_NAME PACKAGE_VERSION PACKAGE_SYSTEM_ID PACKAGE_BUILD_PLATFORM PACKAGE_LOCATION)

	message(STATUS "Looking for CVS library: ${PACKAGE_NAME} version ${PACKAGE_VERSION}")

	# Specifying false value for SystemId or BuildPlatform allows all systems or platforms to be downloaded.
	set(_PACKAGE_PATH_SUFFIX ${PACKAGE_VERSION})
	if(PACKAGE_SYSTEM_ID)
		set(_PACKAGE_PATH_SUFFIX ${_PACKAGE_PATH_SUFFIX}/${PACKAGE_SYSTEM_ID})
		if(PACKAGE_BUILD_PLATFORM)
			set(_PACKAGE_PATH_SUFFIX ${_PACKAGE_PATH_SUFFIX}/${PACKAGE_BUILD_PLATFORM})
		endif()
	endif()

	# Download component library
	download_cvs_content(${PACKAGE_NAME}_${PACKAGE_VERSION}
	                     "${${PACKAGE_LOCATION}}/${_PACKAGE_PATH_SUFFIX}"
	                     "Components/${PACKAGE_NAME}/${_PACKAGE_PATH_SUFFIX}"
	                     _PACKAGE_LOCATION
	                     )

	set(${PACKAGE_LOCATION} "${_PACKAGE_LOCATION}" PARENT_SCOPE)
	message(STATUS "${PACKAGE_NAME} downloaded to ${_PACKAGE_LOCATION}")

	if(DOWNLOAD_CVS_SOURCE)
		# Check if a Source directory exists
		cvsmoduleQ(Components/${PACKAGE_NAME}/${PACKAGE_VERSION}/Source "${${PACKAGE_LOCATION}}" HAS_SOURCE)
		if(HAS_SOURCE)
			# Download component source
			download_cvs_content(${PACKAGE_NAME}_SOURCE
			                     "${${PACKAGE_LOCATION}}/${PACKAGE_VERSION}/Source"
			                     "Components/${PACKAGE_NAME}/${PACKAGE_VERSION}/Source"
			                     _PACKAGE_SOURCE_LOCATION
			                     )
		endif()
	endif()
endfunction()

# Splits comma delimited string STR and saves list to variable LIST
function(split_string_to_list STR LIST)
	string(REPLACE " " "" _STR ${STR})
	string(REPLACE "," ";" _STR ${_STR})
	set(${LIST} ${_STR} PARENT_SCOPE)
endfunction()

macro(parse_old_library_conf LIBRARY_CONF)
	message(DEPRECATION "library.conf file is now deprecated in favor of components.json")
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

	foreach(LIBRARY ${_LIBRARY_CONF_LIBRARY_LIST})
		set(LIB_SYSTEMID ${LIBRARY}_SYSTEMID)
		set(LIB_VERSION ${LIBRARY}_VERSION)
		set(LIB_BUILD_PLATFORM ${LIBRARY}_BUILD_PLATFORM)

		set(LIB_SYSTEMID_ORIG ${${LIB_SYSTEMID}})
		if(NOT LIB_SYSTEMID_ORIG)
			set(${LIB_SYSTEMID} ${SYSTEMID})
		endif()

		set(_LIBRARY_CONF_LIBRARY_STRING ${_LIBRARY_CONF_STRINGS})
		if ("${${LIB_VERSION}}")
			list(FILTER _LIBRARY_CONF_LIBRARY_STRING INCLUDE REGEX "${${LIB_SYSTEMID}}[ \t]+${LIBRARY}[ \t]+${${LIB_VERSION}}")
		else()
			list(FILTER _LIBRARY_CONF_LIBRARY_STRING INCLUDE REGEX "${${LIB_SYSTEMID}}[ \t]+${LIBRARY}")
		endif()

		if(NOT LIB_SYSTEMID_ORIG AND NOT _LIBRARY_CONF_LIBRARY_STRING)
			# If system ID was not explicitly specified and we have found nothing, try "All". This is often used for header-only libraries.
			set(${LIB_SYSTEMID} All)
			set(_LIBRARY_CONF_LIBRARY_STRING ${_LIBRARY_CONF_STRINGS})
			list(FILTER _LIBRARY_CONF_LIBRARY_STRING INCLUDE REGEX "${${LIB_SYSTEMID}}[ \t]+${LIBRARY}")
		endif()

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
		set(${LIB_SYSTEMID} ${${LIB_SYSTEMID}} PARENT_SCOPE)
		set(${LIBRARY}_BUILD_ID cvs PARENT_SCOPE)
	endforeach()
endmacro()

# Reads components.json and for each library therein sets:
# ${LIBRARY_NAME}_SYSTEMID
# ${LIBRARY_NAME}_VERSION
# ${LIBRARY_NAME}_BUILD_PLATFORM
# Also sets DOWNLOAD_CVS_SOURCE variable to control Source download (default is OFF for Release config, ON otherwise).
function(parse_components_config)
	if(NOT DEFINED DOWNLOAD_CVS_SOURCE)
		if("${CMAKE_BUILD_TYPE}" STREQUAL Release)
			set(DOWNLOAD_CVS_SOURCE OFF CACHE BOOL "Download CVS Source directory for all dependencies if it exists.")
		else()
			set(DOWNLOAD_CVS_SOURCE ON CACHE BOOL "Download CVS Source directory for all dependencies if it exists.")
		endif()
	endif()

	# path to components.json. Located in scripts directory by default, but custom location and file name can be passed in.
	if(ARGC GREATER_EQUAL 1)
		set(LIBRARY_CONF "${ARGV0}")
	else()
		set(LIBRARY_CONF "${CMAKE_CURRENT_SOURCE_DIR}/scripts/components.json")
		if(NOT EXISTS ${LIBRARY_CONF})
			set(LIBRARY_CONF "${CMAKE_CURRENT_SOURCE_DIR}/scripts/library.conf") # for better backwards compatibility
		endif()
	endif()
	if(NOT EXISTS ${LIBRARY_CONF})
		message(FATAL_ERROR "Unable to find ${LIBRARY_CONF}")
	endif()

	detect_system_id(SYSTEMID)

	if (LIBRARY_CONF MATCHES .*\\.json)
		file(READ ${LIBRARY_CONF} LIBRARY_CONF_JSON)

		string(JSON _LIBRARY_LIST GET ${LIBRARY_CONF_JSON} ${SYSTEMID})
		string(JSON _LIBRARY_LIST_LENGTH LENGTH ${_LIBRARY_LIST})
		math(EXPR _MAX_LIBRARY_INDEX "${_LIBRARY_LIST_LENGTH} - 1")
		foreach(INDEX RANGE ${_MAX_LIBRARY_INDEX})
			string(JSON _LIB_OBJECT GET ${_LIBRARY_LIST} ${INDEX})
			string(JSON _LIB_NAME GET ${_LIB_OBJECT} "Library")
			string(JSON _LIB_VERSION GET ${_LIB_OBJECT} "Version")
			string(JSON _LIB_BUILD_PLATFORM ERROR_VARIABLE JSON_ERROR GET ${_LIB_OBJECT} "Platform")
			string(JSON _LIB_BUILD_ID ERROR_VARIABLE JSON_ERROR GET ${_LIB_OBJECT} "BuildID")
			string(JSON _LIB_CHECKSUM_MD5 ERROR_VARIABLE JSON_ERROR GET ${_LIB_OBJECT} "MD5")

			set(${_LIB_NAME}_SYSTEMID ${SYSTEMID} PARENT_SCOPE)
			set(${_LIB_NAME}_VERSION ${_LIB_VERSION} PARENT_SCOPE)
			set(${_LIB_NAME}_BUILD_PLATFORM ${_LIB_BUILD_PLATFORM} PARENT_SCOPE)
			set(${_LIB_NAME}_BUILD_ID ${_LIB_BUILD_ID} PARENT_SCOPE)
			set(${_LIB_NAME}_CHECKSUM_MD5 ${_LIB_CHECKSUM_MD5} PARENT_SCOPE)
		endforeach()
	else()
		parse_old_library_conf(${LIBRARY_CONF})
	endif()
endfunction()


function(fetch_dependency_from_nexus LIB_NAME LIB_VERSION LIB_SYSTEMID LIB_BUILD_PLATFORM LIB_BUILD_ID LIB_CHECKSUM_MD5 DOWNLOAD_LOCATION_OUT)
	check_nexus_root()
	if(NOT LIB_BUILD_ID)
		set(LIB_BUILD_ID "")
	endif()
	set(NXS_ASSET_PATH "${LIB_NAME}/${LIB_VERSION}/${LIB_SYSTEMID}/${LIB_BUILD_PLATFORM}/${LIB_NAME}-${LIB_BUILD_ID}*")

	# This search must return a single result, so including the Build ID is a good idea
	set(NXS_SEARCH_URL "$ENV{NEXUSROOT}/service/rest/v1/search?sort=name&repository=re-components&name=${NXS_ASSET_PATH}")
	file(DOWNLOAD
		${NXS_SEARCH_URL}
		nexus_assets.json
		STATUS NXS_ASSETS_STATUS)

	list(GET NXS_ASSETS_STATUS 0 STATUS_CODE)
	if(NOT STATUS_CODE EQUAL 0)
		message(FATAL_ERROR "Invalid Nexus search request.")
	endif()

	file(READ "${CMAKE_CURRENT_BINARY_DIR}/nexus_assets.json" NXS_COMPONENTS_JSON)

	string(JSON ALL_COMPONENTS GET ${NXS_COMPONENTS_JSON} "items")
	string(JSON COMPONENT_COUNT LENGTH ${ALL_COMPONENTS})
	if(COMPONENT_COUNT LESS_EQUAL 0)
		if(NOT LIB_BUILD_ID OR LIB_BUILD_ID STREQUAL "*")
			message(STATUS "No matching components found in Nexus for ${LIB_NAME} version ${LIB_VERSION}. Attempting CVS checkout...")
			return()
		else()
			message(FATAL_ERROR "Could not find ${LIB_NAME} version ${LIB_VERSION} with build id ${LIB_BUILD_ID} in Nexus.")
		endif()
	endif()
	math(EXPR MAX_COMPONENT_INDEX "${COMPONENT_COUNT} - 1")
	string(JSON LAST_COMPONENT GET ${ALL_COMPONENTS} ${MAX_COMPONENT_INDEX})
	string(JSON REQUESTED_ASSET GET ${LAST_COMPONENT} "assets" 0)
	string(JSON ASSET_DOWNLOAD_URL GET ${REQUESTED_ASSET} "downloadUrl")
	string(JSON ASSET_MD5 GET ${REQUESTED_ASSET} "checksum" "md5")
	if(LIB_CHECKSUM_MD5 AND NOT (ASSET_MD5 STREQUAL LIB_CHECKSUM_MD5))
		message(FATAL_ERROR "Expected asset with MD5 ${LIB_CHECKSUM_MD5} but server has ${ASSET_MD5}.")
	endif()
	message(STATUS "Attempting ${LIB_NAME} checkout from Nexus: ${ASSET_DOWNLOAD_URL}")
	include(FetchContent)
	FetchContent_declare(
		${LIB_NAME}
		URL ${ASSET_DOWNLOAD_URL}
		URL_HASH MD5=${ASSET_MD5}
	)
	FetchContent_MakeAvailable(${LIB_NAME})
	set(${DOWNLOAD_LOCATION_OUT} "${${LIB_NAME}_SOURCE_DIR}/${LIB_BUILD_PLATFORM}" PARENT_SCOPE)
endfunction()

# Resolve full path to an external dependency, downloading if necessary
# Prioritize ${LIB_NAME}_DIR, ${LIB_NAME}_LOCATION, CVS_COMPONENTS_DIR, then Nexus download then CVS download
# Do not download if ${LIB_NAME}_DIR or ${LIB_NAME}_LOCATION are set
# ${LIB_NAME}_VERSION|SYSTEMID|BUILD_PLATFORM are expected to be previously set
function(find_component LIB_NAME)

	# helper variables
	set(LIB_DIR "${${LIB_NAME}_DIR}")
	set(LIB_LOCATION "${${LIB_NAME}_LOCATION}")
	set(LIB_VERSION ${${LIB_NAME}_VERSION})
	set(LIB_SYSTEMID ${${LIB_NAME}_SYSTEMID})
	set(LIB_BUILD_PLATFORM ${${LIB_NAME}_BUILD_PLATFORM})
	set(LIB_BUILD_ID ${${LIB_NAME}_BUILD_ID})
	set(LIB_CHECKSUM_MD5 ${${LIB_NAME}_CHECKSUM_MD5})
	set(_LIB_DIR_SUFFIX ${LIB_VERSION}/${LIB_SYSTEMID}/${LIB_BUILD_PLATFORM})

	if(NOT LIB_SYSTEMID)
		message(STATUS "[find_component] ${LIB_NAME}_SYSTEMID not defined. Returning.")
		return()
	endif()

	# Check if there is a full path to the dependency with version, system id and build platform.
	if(NOT ${LIB_DIR} STREQUAL "")
		if(NOT EXISTS ${LIB_DIR})
			message(FATAL_ERROR "Specified full path to ${LIB_NAME} does not exist: ${LIB_DIR}")
		endif()
		return()
	endif()

	# Check if there is a path to the Lib component
	if(NOT ${LIB_LOCATION} STREQUAL "")
		if(NOT EXISTS ${LIB_LOCATION})
			message(FATAL_ERROR "Specified location of ${LIB_NAME} does not exist: ${LIB_LOCATION}")
		elseif(EXISTS ${LIB_LOCATION}/${_LIB_DIR_SUFFIX})
			set(${LIB_NAME}_DIR ${LIB_LOCATION}/${_LIB_DIR_SUFFIX} PARENT_SCOPE)
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
			set(${LIB_NAME}_DIR ${_CVS_COMPONENTS_DIR}/${LIB_NAME}/${_LIB_DIR_SUFFIX} PARENT_SCOPE)
			return()
		endif()
	endif()

	# The library is not on a local drive so we need to download it. Unless cvs checkout is explicitly requested try Nexus first:
	if(NOT LIB_BUILD_ID STREQUAL "cvs")
		fetch_dependency_from_nexus(${LIB_NAME} ${LIB_VERSION} ${LIB_SYSTEMID} ${LIB_BUILD_PLATFORM} ${LIB_BUILD_ID} ${LIB_CHECKSUM_MD5} LIB_LOCATION)
	endif()
	if (NOT LIB_LOCATION)
		# Set location for artifacts to be downloaded:
		set(LIB_LOCATION "${CMAKE_BINARY_DIR}/Components/${LIB_NAME}")
		get_library_from_cvs(${LIB_NAME} ${LIB_VERSION} ${LIB_SYSTEMID} ${LIB_BUILD_PLATFORM} LIB_LOCATION)
	endif()

	set(${LIB_NAME}_LOCATION ${LIB_LOCATION} CACHE PATH "Location of ${LIB_NAME} root directory.")
	set(${LIB_NAME}_DIR ${LIB_LOCATION} PARENT_SCOPE)
endfunction()
