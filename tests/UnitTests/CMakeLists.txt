################################################################################
######
###### LLU unit tests CMake Configuration File
######
###### Author: Rafal Chojna - rafalc@wolfram.com
#################################################################################

message(STATUS "Creating test targets.")

# Unit tests are split into independent modules. You can run them separately, for example:
#
#   ctest -R WSTP
#
# or all at once:
#
#   ctest (or make test)
#
# One helpful option for ctest is --verbose. It shows more output from wolframscript.

set(TEST_MODULES
	"Async"
	"DataList"
	"ErrorReporting"
	"Image"
	"ManagedExpressions"
	"MArgumentManager"
	"WSTP"
	"NumericArray"
	"GenericContainers"
	"Scalar"
	"String"
	"Tensor"
	"Utilities"
	)


find_package(Mathematica 12.0 COMPONENTS wolframscript)
if(Mathematica_wolframscript_EXE)
	message(STATUS "Unit tests will be run with: ${Mathematica_wolframscript_EXE}")

	foreach(UnitTest ${TEST_MODULES})
		set(MODULE_CODE
			[===[
				Needs["MUnit`"];
				report = TestReport["${UnitTest}TestSuite.mt", MemoryConstraint -> Quantity[500, "MB"], TimeConstraint -> 30];
				report = report /. resObj_TestResultObject :> RuleCondition[TextString[resObj[{
					"TestID",
					"ExpectedOutput",
					"ActualOutput",
					"ExpectedMessages",
					"ActualMessages",
					"AbsoluteTimeUsed",
					"MemoryUsed"
				}], AssociationFormat -> {"{\n\t\t", ",\n\t\t", "\n\t}", " -> "}]];
				propertiesToPrint = {"Title", "TimeElapsed", "Aborted", "TestsSucceededCount", "TestsFailedCount"};
				failedCount = report["TestsFailedCount"];
				If[failedCount > 0,
					AppendTo[propertiesToPrint, "TestsFailedIndices"];
					Scan[If[report["TestsFailed" <> # <> "Count"] > 0,
						propertiesToPrint = Join[propertiesToPrint, {"TestsFailed" <> # <> "Count", "TestsFailed" <> # <> "Indices", "TestsFailed" <> #}];
					]&, {"WrongResults", "WithMessages", "WithErrors"}];
				];
				Print[TextString[AssociationMap[report, propertiesToPrint], AssociationFormat -> {"{\n\t", ",\n\t", "\n}", ": "}]];
				exitCode = 1 - Boole[report["AllTestsSucceeded"]];
				Exit[exitCode]
			]===])
		string(REGEX REPLACE "[\t\r\n]+" "" MODULE_CODE "${MODULE_CODE}")
		string(CONFIGURE "${MODULE_CODE}" MODULE_CODE)

		add_test(NAME ${UnitTest}
			WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/${UnitTest}"
			COMMAND ${Mathematica_wolframscript_EXE} -code "${MODULE_CODE}"
			)
	endforeach()

else()
	message(STATUS "Could not find wolframscript. Unit tests target will not be generated.")
endif()