find_package(Doxygen 1.8.15 REQUIRED dot)

set(DOCUMENTATION_SOURCE ${CMAKE_CURRENT_SOURCE_DIR})

set(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/doxy)
set(DOXYGEN_GENERATE_HTML NO)
set(DOXYGEN_GENERATE_XML YES)
set(DOXYGEN_GENERATE_LATEX NO)
set(DOXYGEN_OUTPUT_INDEX ${DOXYGEN_OUTPUT_DIR}/xml/index.xml)

set(GENERATED_DOXYFILE "${DOXYGEN_OUTPUT_DIR}/LibraryLinkUtilities.doxyfile")
configure_file(
		"${DOCUMENTATION_SOURCE}/LibraryLinkUtilities.doxyfile.in"
		${GENERATED_DOXYFILE}
		@ONLY)

# Custom command that when run will actually generate the documentation
add_custom_command(
		OUTPUT ${DOXYGEN_OUTPUT_INDEX}
		# Generate doxygen documentation
		COMMAND Doxygen::doxygen ${GENERATED_DOXYFILE}
		# We need to work where the original doxyfile resides because source paths in doxyfiles are often relative
		WORKING_DIRECTORY ${DOCUMENTATION_SOURCE}
		COMMENT "Generating documentation with Doxygen"
		VERBATIM)

# Custom target for generating documentation
add_custom_target(doxygen ALL DEPENDS ${DOXYGEN_OUTPUT_INDEX})


find_package(Python REQUIRED COMPONENTS Interpreter)
find_package(Sphinx REQUIRED)

# find the Breathe module
if (NOT BREATHE_MODULE_LOCATION)
	execute_process(
			COMMAND "${Python_EXECUTABLE}" "-c"
			"from __future__ import print_function; import re, breathe; print(re.compile('/__init__.py.*').sub('', breathe.__file__))"
			RESULT_VARIABLE _BREATHE_STATUS
			OUTPUT_VARIABLE _BREATHE_LOCATION
			ERROR_QUIET
			OUTPUT_STRIP_TRAILING_WHITESPACE)
	if (NOT _BREATHE_STATUS)
		set(BREATHE_MODULE_LOCATION ${_BREATHE_LOCATION} CACHE STRING "Location of Breathe")
		mark_as_advanced(BREATHE_MODULE_LOCATION)
	endif ()
endif ()
find_package_handle_standard_args(Breathe DEFAULT_MSG BREATHE_MODULE_LOCATION)

set(SPHINX_CONF_DIR ${DOCUMENTATION_SOURCE})

if (SPHINX_CUSTOM_THEME)
	set(SPHINX_THEME ${SPHINX_CUSTOM_THEME})
	set(SPHINX_THEME_PATH "sphinx_theme.get_html_theme_path('${SPHINX_THEME}')")
elseif(NOT SPHINX_THEME)
	set(SPHINX_THEME bizstyle)
endif()

configure_file(
		"${SPHINX_CONF_DIR}/conf.py.in"
		"${CMAKE_CURRENT_BINARY_DIR}/conf.py"
		@ONLY)

set(SPHINX_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/LLU)

add_custom_target(
		docs
		DEPENDS
			${DOXYGEN_OUTPUT_INDEX}
		COMMAND
			${SPHINX_EXECUTABLE}
				-q
				-b html
				-c ${CMAKE_CURRENT_BINARY_DIR}
				-j auto
				${DOCUMENTATION_SOURCE}
				${SPHINX_OUTPUT}
		WORKING_DIRECTORY
			${DOCUMENTATION_SOURCE}
		COMMENT
			"Generating HTML documentation with Doxygen + Breathe + Sphinx"
		VERBATIM)
