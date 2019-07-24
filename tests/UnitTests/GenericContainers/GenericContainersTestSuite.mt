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

	RegisterPacletErrors[lib, <|
		"StaticTopLevelError" -> "This top-level error has a static error message.",
		"TopLevelNamedSlotsError" -> "Hi `name`! Error occurred `when`.",
		"TopLevelNumberedSlotsError" -> "Slot number one: `1`, number two: `2`."
	|>];

	Off[General::stop]; (* because we want to see all error messages from CreateLibrary *)

	IsOwnerAutomatic = SafeLibraryFunction["IsOwnerAutomatic", {Image}, "Boolean"];
	IsOwnerManual = SafeLibraryFunction["IsOwnerManual", {{Integer, _, "Manual"}}, "Boolean"];
	IsOwnerShared = SafeLibraryFunction["IsOwnerShared", {{NumericArray, "Shared"}}, "Boolean"];

	CloneAutomatic = SafeLibraryFunction["CloneAutomatic", {Image}, Image];
	CloneManual = SafeLibraryFunction["CloneManual", {{Integer, _, "Manual"}}, {Integer, _}];
	CloneShared = SafeLibraryFunction["CloneShared", {{NumericArray, "Shared"}}, NumericArray];

	MoveAutomatic = SafeLibraryFunction["MoveAutomatic", {Image}, Image];
	MoveManual = SafeLibraryFunction["MoveManual", {{Integer, _, "Manual"}}, {Integer, _}];
	MoveShared = SafeLibraryFunction["MoveShared", {{NumericArray, "Shared"}}, NumericArray];

	img = RandomImage[];
	tensor = {1, 2, 3, 4, 5};
	na = NumericArray[{5, 4, 3, 2, 1}, "UnsignedInteger16"];
	ds = Developer`DataStore["x" -> img, "y" -> 3];
];


(* Compile-time errors *)
Test[
	CCompilerDriver`CreateLibrary[{FileNameJoin[{currentDirectory, "TestSources", "PoliciesCompilationErrors.cpp"}]}, "PoliciesErrors", options]
	,
	$Failed
	,
	{CreateLibrary::cmperr..}
	,
	TestID->"GenericContainersTestSuite-20190712-R8A2K9"
];

ExactTest[
	IsOwnerAutomatic[img]
	,
	False
	,
	TestID->"GenericContainersTestSuite-20190724-V1C6L5"
];

ExactTest[
	IsOwnerManual[tensor]
	,
	True
	,
	TestID->"GenericContainersTestSuite-20190724-V7L9Q7"
];

ExactTest[
	IsOwnerShared[na]
	,
	True
	,
	TestID->"GenericContainersTestSuite-20190724-D2Q7F2"
];

Test[
	CloneAutomatic[img]
	,
	img
	,
	TestID->"GenericContainersTestSuite-20190724-L8K9W7"
];

Test[
	CloneManual[tensor]
	,
	tensor
	,
	TestID->"GenericContainersTestSuite-20190724-H0H3Y5"
];

Test[
	CloneShared[na]
	,
	na
	,
	TestID->"GenericContainersTestSuite-20190724-E8P2L4"
];

Test[
	MoveAutomatic[img]
	,
	img
	,
	TestID->"GenericContainersTestSuite-20190724-H0P0K2"
];

Test[
	MoveManual[tensor]
	,
	{-324, 2, 3, 4, 5}
	,
	TestID->"GenericContainersTestSuite-20190724-X7Z5N5"
];

Test[
	MoveShared[na]
	,
	na
	,
	TestID->"GenericContainersTestSuite-20190724-E4T3F7"
];