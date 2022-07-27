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
	"GenericContainers"
	"Image"
	"ManagedExpressions"
	"MArgumentManager"
	"NumericArray"
	"Scalar"
	"SparseArray"
	"String"
	"Tensor"
	"Utilities"
	"WSTP"
	)


find_package(WolframLanguage 12.0 COMPONENTS wolframscript)
if(WolframLanguage_wolframscript_EXE)
	message(STATUS "Unit tests will be run with: ${WolframLanguage_wolframscript_EXE}")
	set(_WL_KERNEL_EXE ${WolframLanguage_wolframscript_EXE})
	set(_WL_KERNEL_OPTION "-code")
elseif(WolframLanguage_EXE)
	message(STATUS "Unit tests will be run with: ${WolframLanguage_EXE}")
	set(_WL_KERNEL_EXE ${WolframLanguage_EXE})
	set(_WL_KERNEL_OPTION "-run")
else()
	message(STATUS "Could not find any instance of the WolframLanguage Kernel. Unit tests target will not be generated.")
	return()
endif()

get_target_property(LLU_CRT_LINKING LLU MSVC_RUNTIME_LIBRARY)
if(NOT LLU_CRT_LINKING)
	set(LLU_CRT_LINKING ${CMAKE_MSVC_RUNTIME_LIBRARY})
endif()

foreach(UnitTest ${TEST_MODULES})
	set(MODULE_CODE
		[===[
			Needs["MUnit`"];
			$LLUInstallDir = "${CMAKE_INSTALL_PREFIX}";
			$LinkToCRTStaticallyQ = "${LLU_CRT_LINKING}";
			report = Catch[
				TestReport["${UnitTest}TestSuite.mt", MemoryConstraint -> Quantity[500, "MB"], TimeConstraint -> 30],
				_String,
				Function[{value, tag}, Print["ERROR: " <> ToString[value]]; Exit[1]]
			];
			indexKeyword = If[$VersionNumber >= 13, "Keys", "Indices"];
			propertiesToPrint = {"Title", "Aborted"};
			failedCount = 0;
			Scan[
				If[report["TestsFailed" <> # <> indexKeyword] != {},
					AppendTo[propertiesToPrint, "TestsFailed" <> # <> indexKeyword];
					failedCount += Length @ report["TestsFailed" <> # <> indexKeyword];
				]&, {"WrongResults", "WithMessages", "WithErrors"}
			];
			If[failedCount > 0,
				If[#["Outcome"] =!= "Success",
					Print @ TextString[#, AssociationFormat -> {"{\n\t", ",\n\t", "\n}", ": "}]
				]& /@ report["TestResults"];
			];
			Print[TextString[AssociationMap[report, propertiesToPrint], AssociationFormat -> {"{\n\t", ",\n\t", "\n}", ": "}]];
			exitCode = Boole[failedCount > 0];
			Exit[exitCode]
		]===])
	string(REGEX REPLACE "[\t\r\n]+" "" MODULE_CODE "${MODULE_CODE}")
	string(CONFIGURE "${MODULE_CODE}" MODULE_CODE)

	add_test(NAME ${UnitTest}
		WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/${UnitTest}"
		COMMAND ${_WL_KERNEL_EXE} ${_WL_KERNEL_OPTION} "${MODULE_CODE}"
		)
endforeach()

