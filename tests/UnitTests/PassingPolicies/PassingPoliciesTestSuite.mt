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
	lib = CCompilerDriver`CreateLibrary[FileNameJoin[{currentDirectory, "TestSources", #}]& /@ {"PoliciesTest.cpp"},
		"PassingPolicies", options];

	Get[FileNameJoin[{$LLUSharedDir, "LibraryLinkUtilities.wl"}]];

	RegisterPacletErrors[lib, <|
		"StaticTopLevelError" -> "This top-level error has a static error message.",
		"TopLevelNamedSlotsError" -> "Hi `name`! Error occurred `when`.",
		"TopLevelNumberedSlotsError" -> "Slot number one: `1`, number two: `2`."
	|>];

	Off[General::stop]; (* because we want to see all error messages from CreateLibrary *)
];


(* Compile-time errors *)
Test[
	CCompilerDriver`CreateLibrary[{FileNameJoin[{currentDirectory, "TestSources", "PoliciesCompilationErrors.cpp"}]}, "PoliciesErrors", options]
	,
	$Failed
	,
	{CreateLibrary::cmperr..}
	,
	TestID->"PassingPoliciesTestSuite-20190712-R8A2K9"
]

