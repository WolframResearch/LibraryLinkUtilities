(* Wolfram Language Test file *)
TestRequirement[$VersionNumber > 10.3];
(***************************************************************************************************************************************)
(*
	Set of test cases to test LLU functionality related to passing modes of different containers
*)
(***************************************************************************************************************************************)
TestExecute[
	Needs["CCompilerDriver`"];
	currentDirectory = DirectoryName[$CurrentFile];

	(* Get configuration (path to LLU sources, compilation options, etc.) *)
	Get[FileNameJoin[{ParentDirectory[currentDirectory], "TestConfig.wl"}]];

	(* Compile the test library *)
	lib = CCompilerDriver`CreateLibrary[FileNameJoin[{currentDirectory, "TestSources", #}]& /@ {"GenericContainersTest.cpp"},
		"GenericContainers", options, "Defines" -> {"LLU_LOG_DEBUG"}];

	Get[FileNameJoin[{$LLUSharedDir, "LibraryLinkUtilities.wl"}]];

	`LLU`InitializePacletLibrary[lib];

	Off[General::stop]; (* because we want to see all error messages from CreateLibrary *)

	IsOwnerAutomatic = `LLU`PacletFunctionLoad["IsOwnerAutomatic", {Image}, "Boolean"];
	IsOwnerManual = `LLU`PacletFunctionLoad["IsOwnerManual", {{Integer, _, "Manual"}}, "Boolean"];
	IsOwnerShared = `LLU`PacletFunctionLoad["IsOwnerShared", {{NumericArray, "Shared"}}, "Boolean"];

	CloneAutomatic = `LLU`PacletFunctionLoad["CloneAutomatic", {Image}, Image];
	CloneManual = `LLU`PacletFunctionLoad["CloneManual", {{Integer, _, "Manual"}}, {Integer, _}];
	CloneShared = `LLU`PacletFunctionLoad["CloneShared", {{NumericArray, "Shared"}}, NumericArray];

	MoveAutomatic = `LLU`PacletFunctionLoad["MoveAutomatic", {Image}, Image];
	MoveManual = `LLU`PacletFunctionLoad["MoveManual", {{Integer, _, "Manual"}}, {Integer, _}];
	MoveShared = `LLU`PacletFunctionLoad["MoveShared", {{NumericArray, "Shared"}}, NumericArray];

	img = RandomImage[];
	tensor = {1, 2, 3, 4, 5};
	na = NumericArray[{5, 4, 3, 2, 1}, "UnsignedInteger16"];
	ds = Developer`DataStore["x" -> img, "y" -> 3];

	ClearAll[TestLogSymbol];
	`LLU`Logger`PrintLogFunctionSelector := Block[{`LLU`Logger`FormattedLog = `LLU`Logger`LogToShortString},
		`LLU`Logger`PrintLogToSymbol[TestLogSymbol][##]
	]&;
];


(* Compile-time errors *)
Test[
	CCompilerDriver`CreateLibrary[{FileNameJoin[{currentDirectory, "TestSources", "PoliciesCompilationErrors.cpp"}]}, "PoliciesErrors", options]
	,
	$Failed
	,
	{CreateLibrary::cmperr..}
	,
	TestID -> "GenericContainersTestSuite-20190712-R8A2K9"
];

ExactTest[
	IsOwnerAutomatic[img]
	,
	False
	,
	TestID -> "GenericContainersTestSuite-20190724-V1C6L5"
];

ExactTest[
	IsOwnerManual[tensor]
	,
	True
	,
	TestID -> "GenericContainersTestSuite-20190724-V7L9Q7"
];

ExactTest[
	IsOwnerShared[na]
	,
	True
	,
	TestID -> "GenericContainersTestSuite-20190724-D2Q7F2"
];

Test[
	CloneAutomatic[img]
	,
	img
	,
	TestID -> "GenericContainersTestSuite-20190724-L8K9W7"
];

Test[
	CloneManual[tensor]
	,
	tensor
	,
	TestID -> "GenericContainersTestSuite-20190724-H0H3Y5"
];

Test[
	CloneShared[na]
	,
	na
	,
	TestID -> "GenericContainersTestSuite-20190724-E8P2L4"
];

Test[
	MoveAutomatic[img]
	,
	img
	,
	TestID -> "GenericContainersTestSuite-20190724-H0P0K2"
];

Test[
	MoveManual[tensor]
	,
	{-324, 2, 3, 4, 5}
	,
	TestID -> "GenericContainersTestSuite-20190724-X7Z5N5"
];

Test[
	MoveShared[na]
	,
	na
	,
	TestID -> "GenericContainersTestSuite-20190724-E4T3F7"
];

Test[
	TestLogSymbol
	,
	{
		"[Debug] GenericContainersTest.cpp:68 (MoveAutomatic): Automatic arg is owner: False",
		"[Debug] GenericContainersTest.cpp:70 (MoveAutomatic): Automatic arg is owner: False, clone is owner: False",
		"[Debug] GenericContainersTest.cpp:72 (MoveAutomatic): Automatic arg is owner: False, clone is owner: False",
		"[Debug] GenericContainersTest.cpp:79 (MoveManual): Manual arg is owner: True",
		"[Debug] GenericContainersTest.cpp:83 (MoveManual): Manual arg is owner: False, clone is owner: True",
		"[Debug] GenericContainersTest.cpp:85 (MoveManual): Manual arg is owner: False, clone is owner: False",
		"[Debug] GenericContainersTest.cpp:92 (MoveShared): Shared arg is owner: True",
		"[Debug] GenericContainersTest.cpp:94 (MoveShared): Shared arg is owner: False, clone is owner: True",
		"[Debug] GenericContainersTest.cpp:96 (MoveShared): Shared arg is owner: False, clone is owner: True"
	}
	,
	TestID -> "GenericContainersTestSuite-20190906-W5T3O4"
];