
macro(set_if_undefined VAR VALUE)
	if(NOT ${VAR})
		set(${VAR} ${VALUE})
	endif()
endmacro()

macro(required_arg VAR MESSAGE)
	if(NOT ${VAR})
		message(FATAL_ERROR ${MESSAGE})
	endif()
endmacro()

macro(find_paclet_info DIR PACLET_INFO_LOCATION)
	find_file(${PACLET_INFO_LOCATION}
			NAMES PacletInfo.wl PacletInfo.wl.in PacletInfo.m PacletInfo.m.in
			HINTS ${DIR}
			DOC "Path to the PacletInfo file"
			NO_DEFAULT_PATH)
endmacro()

# Copies paclet files to install location (CMAKE_INSTALL_PREFIX should be set appropriately before calling this).
# Optional 3rd arg is PacletName (defaults to TARGET_NAME). Optional 4th arg is paclet location (defaults to CMAKE_CURRENT_SOURCE_DIR/PacletName).
# "Old-style" (non-updateable) paclet layout (with PacletInfo.m in git root directory) is not supported.
function(install_paclet_files)
	set(OPTIONS INSTALL_TO_LAYOUT)
	set(ONE_VALUE_ARGS TARGET LLU_LOCATION PACLET_NAME PACLET_FILES_LOCATION)
	set(MULTI_VALUE_ARGS)
	cmake_parse_arguments(INSTALL_PACLET "${OPTIONS}" "${ONE_VALUE_ARGS}" "${MULTI_VALUE_ARGS}" ${ARGN})

	required_arg(INSTALL_PACLET_TARGET "Target must be specified.")
	set_if_undefined(INSTALL_PACLET_PACLET_NAME ${INSTALL_PACLET_TARGET})
	set_if_undefined(INSTALL_PACLET_PACLET_FILES_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/${INSTALL_PACLET_PACLET_NAME})

	find_paclet_info(${INSTALL_PACLET_PACLET_FILES_LOCATION} PACLET_INFO)
	if(NOT PACLET_INFO)
		message(WARNING "PacletInfo file could not be found. Paclet might be broken.")
	else()
		configure_file(${PACLET_INFO} ${CMAKE_CURRENT_BINARY_DIR}/PacletInfo.wl @ONLY) # enforce modern .wl extension
	endif()

	#copy over the paclet directory - i.e. .wl sources and other files except PacletInfo
	install(DIRECTORY ${INSTALL_PACLET_PACLET_FILES_LOCATION}
			DESTINATION ${CMAKE_INSTALL_PREFIX}
			PATTERN ".DS_Store" EXCLUDE
			PATTERN REGEX ${PACLET_INFO} EXCLUDE)

	# install generated PacletInfo
	install(FILES ${CMAKE_CURRENT_BINARY_DIR}/PacletInfo.wl
			DESTINATION ${CMAKE_INSTALL_PREFIX}/${INSTALL_PACLET_PACLET_NAME})

	#copy the library produced into LibraryResources/$SystemID
	set(LIB_RESOURCES_DIR "${INSTALL_PACLET_PACLET_NAME}/LibraryResources")
	detect_system_id(SYSTEMID)
	install(TARGETS ${INSTALL_PACLET_TARGET}
			LIBRARY DESTINATION ${LIB_RESOURCES_DIR}/${SYSTEMID}
			RUNTIME DESTINATION ${LIB_RESOURCES_DIR}/${SYSTEMID}
			)

	# copy LLU top-level code
	if(NOT INSTALL_PACLET_LLU_LOCATION)
		message(WARNING "*** LLU_LOCATION was not specified. This may be OK if the paclet is not using LLU. ***")
	else()
		install(FILES "${INSTALL_PACLET_LLU_LOCATION}/share/LibraryLinkUtilities.wl"
				DESTINATION ${LIB_RESOURCES_DIR}
				)
	endif()

	if(INSTALL_PACLET_INSTALL_TO_LAYOUT)
		install_paclet_to_layout(${INSTALL_PACLET_PACLET_NAME} TRUE)
	endif()
endfunction()

# Installs paclet into a Mathematica layout if requested.
macro(install_paclet_to_layout PACLET_NAME INSTALLQ)
	if(${INSTALLQ})
		if(EXISTS "${MATHEMATICA_INSTALL_DIR}")
			install(DIRECTORY "${CMAKE_INSTALL_PREFIX}/${PACLET_NAME}"
					DESTINATION "${MATHEMATICA_INSTALL_DIR}/SystemFiles/Links"
					)
		else()
			message(WARNING "Failed to install paclet to layout: \"${MATHEMATICA_INSTALL_DIR}\" does not exist.")
		endif()
	endif()
endmacro()

# Creates a custom 'zip' target for a paclet.
# CMAKE_INSTALL_PREFIX should be set appropriately before calling this.
function(create_zip_target PACLET_NAME)
	message(DEPRECATION "Distributing paclets in .zip archives is deprecated in favor of .paclet format. Consider using pack_paclet() function instead.")
	add_custom_target(zip
			COMMAND ${CMAKE_COMMAND} -E tar "cfv" "${CMAKE_INSTALL_PREFIX}/${PACLET_NAME}.zip" --format=zip "${CMAKE_INSTALL_PREFIX}/${PACLET_NAME}"
			COMMENT "Creating zip..."
			)
endfunction()


function(pack_paclet)
	set(OPTIONS VERIFY)
	set(ONE_VALUE_ARGS NAME)
	set(MULTI_VALUE_ARGS)
	cmake_parse_arguments(PACK_PACLET "${OPTIONS}" "${ONE_VALUE_ARGS}" "${MULTI_VALUE_ARGS}" ${ARGN})
	required_arg(PACK_PACLET_NAME "Paclet name must be provided.")

	find_wolframscript(WOLFRAMSCRIPT)
	if(NOT WOLFRAMSCRIPT)
		message(WARNING "Could not find wolframscript. \"paclet\" target will not be created.")
		return()
	endif()

	if(PACK_PACLET_VERIFY)
		set(VERIFICATION_MESSAGE " and verifying PacletInfo contents.")
		set(VERIFICATION_CODE "
			If[Not @ PacletManager`VerifyPaclet[paclet],
				Print[\"Paclet verification failed! Check the structure of your project and the contents of PacletInfo file\".];
				Return[$Failed];
			];
			hasLLExt = MemberQ[First[paclet][\"Extensions\"], {\"LibraryLink\", ___}];
			If[Not @ hasLLExt,
				Print[\"Paclet does not contain the \"LibraryLink\" extension which may potentially lead to loading issues.\"];
			];
		")
	endif()

	set(WL_CODE "
			pacDir = \"${PACK_PACLET_NAME}\";
			If[Not @ DirectoryQ[pacDir],
				Print[];
			];
			paclet = CreatePacletArchive[pacDir];
		")
	add_custom_target(paclet
			COMMAND ${WOLFRAMSCRIPT} -code ${WL_CODE}${VERIFICATION_CODE}
			WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}
			COMMENT "Creating .paclet file${VERIFICATION_MESSAGE}..."
			VERBATIM
			)
endfunction()