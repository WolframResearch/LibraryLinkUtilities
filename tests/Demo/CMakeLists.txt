################################################################################
######
###### LLU Demo Project Configuration File
######
################################################################################

cmake_minimum_required(VERSION 3.11.0)

project(Demo
		VERSION     0.0.1
		DESCRIPTION "Demo paclet that shows how to use LibraryLinkUtilities."
		LANGUAGES   CXX
)

# By default install to the build directory
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
	set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_BINARY_DIR}" CACHE PATH "Demo paclet install prefix" FORCE)
endif ()

# Optionally, install paclet to Mathematica layout (set MATHEMATICA_INSTALL_DIR to choose which one)
set(INSTALL_PACLET_TO_LAYOUT OFF CACHE BOOL "Copy paclet into Mathematica installation")

#=============================================
#=============== FIND LLU ====================
#=============================================

set(LLU_INSTALL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../install" CACHE PATH "Location of LLU installation.")

# Add cmake scripts distributed with LLU to cmake prefix path
set(CMAKE_MODULE_PATH "${LLU_INSTALL_DIR}/cmake/LLU")

#find_package(Mathematica 11.3 REQUIRED COMPONENTS WolframLibrary WSTP OPTIONAL_COMPONENTS wolframscript)

# Include cmake script with utility functions
include(${LLU_INSTALL_DIR}/cmake/LLU/Wolfram/Common.cmake)

# Figure out default Mathematica location
#get_default_mathematica_dir("12.1" DEFAULT_MATHEMATICA_INSTALL_DIR)
#set(MATHEMATICA_INSTALL_DIR "${DEFAULT_MATHEMATICA_INSTALL_DIR}" CACHE PATH "Mathematica installation")

find_package(LLU 2.2.0 EXACT REQUIRED)

#=============================================
#=========== MAIN PACLET LIBRARY =============
#=============================================

# define source files
set(DEMO_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Sources/)
set(DEMO_SOURCE_FILES ${DEMO_SOURCE_DIR}/demo.cpp)

#add the main library
add_library(Demo SHARED ${DEMO_SOURCE_FILES})

set_target_properties(Demo PROPERTIES
		CXX_STANDARD 17
		CXX_STANDARD_REQUIRED YES
		CXX_EXTENSIONS NO
		CXX_VISIBILITY_PRESET hidden
)

# set compile options
# here we use a utility function from cmake/Wolfram/Common.cmake which sets a basic set of compiler flags in a cross-platform way
set_default_compile_options(Demo O2)

# link to LLU
target_link_libraries(Demo PRIVATE LLU::LLU)

#=============================================
#=========== INSTALL PACLET ==================
#=============================================

# Include cmake script with utility functions
#include(${LLU_INSTALL_DIR}/cmake/LLU/Wolfram/PacletUtilities.cmake)

install_paclet_files(
		TARGET Demo
		LLU_LOCATION ${LLU_INSTALL_DIR}
)

add_paclet_target(paclet
		NAME Demo
		VERIFY
		INSTALL
)

