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
		"Image"
		"MathLink"
		"RawArray"
		"Scalar"
		"String"
		"Tensor"
	)

	foreach(UnitTest ${TEST_MODULES})
		set(MODULE_CODE "
			Needs[\"MUnit`\"]\;
			allPassed = MUnit`TestRun[\"${UnitTest}TestSuite.mt\"]
			Quit[1 - Boole[allPassed]]
		")
		add_test(NAME ${UnitTest}
			WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/${UnitTest}"
			COMMAND wolframscript -code ${MODULE_CODE} # we assume here that wolframscript is installed and on the PATH
		)
	endforeach()