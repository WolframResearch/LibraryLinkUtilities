################################################################################
######
###### LLU unit tests CMake Configuration File
######
###### Author: Rafal Chojna - rafalc@wolfram.com
#################################################################################

message(STATUS "Creating test targets.")

# Unit tests are split into independent modules. You can run them separately, for example:
#
#   ctest -R MathLink
#
# or all at once:
#
#   ctest (or make test)
#
# One helpful option for ctest is --verbose. It shows more output from wolframscript.

set(TEST_MODULES
        "DataList"
        "ErrorReporting"
        "Image"
        "ManagedExpressions"
        "MArgumentManager"
        "MathLink"
        "NumericArray"
        "GenericContainers"
        "Scalar"
        "String"
        "Tensor"
        "Utilities"
        )

set(CMAKE_FIND_APPBUNDLE NEVER)

find_program(WOLFRAMSCRIPT_EXE
        NAMES wolframscript
        HINTS ${MATHEMATICA_INSTALL_DIR}
        PATH_SUFFIXES Executables MacOS
        DOC "Path to wolframscript executable."
)

message(STATUS "Unit tests will be run with: ${WOLFRAMSCRIPT_EXE}")

foreach (UnitTest ${TEST_MODULES})
    set(MODULE_CODE "
			Needs[\"MUnit`\"]\;
			allPassed = MUnit`TestRun[\"${UnitTest}TestSuite.mt\"]
			Quit[1 - Boole[allPassed]]
		")
    add_test(NAME ${UnitTest}
            WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/${UnitTest}"
            COMMAND ${WOLFRAMSCRIPT_EXE} -code ${MODULE_CODE}
            )
endforeach ()