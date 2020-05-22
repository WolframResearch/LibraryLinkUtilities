################################################################################
######
###### LLU Demo Project Configuration File
######
################################################################################

cmake_minimum_required(VERSION 3.12.0)

project(Demo
		VERSION     0.0.1
		DESCRIPTION "Demo paclet that shows how to use LibraryLinkUtilities."
		LANGUAGES   CXX
)

# By default install to the build directory
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
	set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_BINARY_DIR}" CACHE PATH "Demo paclet install prefix" FORCE)
endif ()

#=============================================
#=============== FIND LLU ====================
#=============================================

set(LLU_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../install" CACHE PATH "Location of LLU installation.")

set(CMAKE_FIND_DEBUG_MODE ON)
find_package(LLU 2.2.0 EXACT REQUIRED NO_MODULE PATH_SUFFIXES LLU)

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

# We need to link statically to runtime library on Windows because this is how LLU is built
set_windows_static_runtime()

# link to LLU
target_link_libraries(Demo PRIVATE LLU::LLU)

#=============================================
#=========== INSTALL PACLET ==================
#=============================================

install_paclet_files(
		TARGET Demo
		LLU_LOCATION ${LLU_ROOT}
)

add_paclet_target(paclet
		NAME Demo
		VERIFY
		INSTALL
		TEST_FILE Tests/test.wl
)

