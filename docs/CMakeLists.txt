find_package(Doxygen 1.8.15 REQUIRED dot)

set(DOCUMENTATION_SOURCE ${CMAKE_CURRENT_SOURCE_DIR})

# Set the location for generated doxygen documentation
set(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/doxy)

# Additional doxygen options
set(DOXYGEN_GENERATE_HTML NO)
set(DOXYGEN_GENERATE_XML YES)
set(DOXYGEN_GENERATE_LATEX NO)

# Go through LibraryLinkUtilities.doxyfile.in substituting custom values
set(GENERATED_DOXYFILE "${DOXYGEN_OUTPUT_DIR}/LibraryLinkUtilities.doxyfile")
configure_file(
		"${DOCUMENTATION_SOURCE}/LibraryLinkUtilities.doxyfile.in"
		${GENERATED_DOXYFILE}
		@ONLY)

# Doxygen generation output
set(DOXYGEN_OUTPUT_INDEX ${DOXYGEN_OUTPUT_DIR}/xml/index.xml)

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
add_custom_target(doxygen DEPENDS ${DOXYGEN_OUTPUT_INDEX})


find_package(Python REQUIRED COMPONENTS Interpreter)
find_package(Sphinx REQUIRED)

# Find the Breathe module
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

# Determine Sphinx theme for the docs. Use the following variables:
#   SPHINX_THEME - this should be one of the built-in Sphinx themes, see https://www.sphinx-doc.org/en/master/usage/theming.html
#   SPHINX_CUSTOM_THEME - if you don't want a built-in theme, use custom one that you have installed, this takes precedence over SPHINX_THEME
# When you use a custom theme, you will also need to specify SPHINX_THEME_PATH and possibly SPHINX_CUSTOM_THEME_INIT which allow Sphinx to find your theme.
# For example, imagine I want to use a neo_rtd_theme from https://github.com/LinxiFan/Sphinx-theme. To do this, I need to pass:
#   -DSPHINX_CUSTOM_THEME=neo_rtd_theme
#   -DSPHINX_CUSTOM_THEME_INIT="import sphinx_theme"
#   -DSPHINX_THEME_PATH="sphinx_theme.get_html_theme_path('neo_rtd_theme')"
# The default theme is "bizstyle" from built-in Sphinx themes.
if (SPHINX_CUSTOM_THEME)
	set(SPHINX_THEME ${SPHINX_CUSTOM_THEME})
elseif(NOT SPHINX_THEME)
	set(SPHINX_THEME bizstyle)
endif()

# Go through conf.py.in substituting custom values
set(SPHINX_CONF_DIR ${DOCUMENTATION_SOURCE})
configure_file(
		"${SPHINX_CONF_DIR}/conf.py.in"
		"${CMAKE_CURRENT_BINARY_DIR}/conf.py"
		@ONLY)

# Set the location for the generated Sphinx docs
set(SPHINX_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/LLU)

# Create a target that runs Breathe and Sphinx on the doxygen output. This depends on the doxygen target.
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
