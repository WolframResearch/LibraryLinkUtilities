(* Wolfram Language Test file *)
TestRequirement[$VersionNumber >= 12.0];
(***************************************************************************************************************************************)
(*
	Set of test cases to test LLU functionality related to MArgumentManager and MArgument passing to and fro LibraryLink
*)
(***************************************************************************************************************************************)
TestExecute[
	Needs["CCompilerDriver`"];
	currentDirectory = DirectoryName[$CurrentFile];

	(* Get configuration (path to LLU sources, compilation options, etc.) *)
	Get[FileNameJoin[{ParentDirectory[currentDirectory], "TestConfig.wl"}]];

	$CppVersion = "c++17";

	(* Compile the test library *)
	lib = CCompilerDriver`CreateLibrary[
		FileNameJoin[{currentDirectory, "TestSources", #}]& /@ {"MArgumentManagerTest.cpp"},
		"MArgumentManagerTest",
		options
	];

	Get[FileNameJoin[{$LLUSharedDir, "LibraryLinkUtilities.wl"}]];

	`LLU`RegisterPacletErrors[lib, <||>];

	$DescribePerson = `LLU`SafeLibraryFunction["DescribePerson", {String, Integer, Real}, String];
	$DescribePerson2 = `LLU`SafeLibraryFunction["DescribePerson2", {String, Integer, Real}, String];
	$ComparePeople = `LLU`SafeLibraryFunction["ComparePeople", {String, Integer, Real, String, Integer, Real}, String];

	$RepeatString = `LLU`SafeLibraryFunction["LL_repeatString", {String, Integer}, String];
	$DoNothing = `LLU`SafeLibraryFunction["LL_doNothing", {}, "Void"];

	$GetPersonDescription = `LLU`SafeLibraryFunction["GetPersonDescription", {String, Integer, Real}, String];

	LLU`CustomArgument[Person] = {String, Integer, Real};
];


(* Compile-time errors *)
(*Test[*)
(*	CCompilerDriver`CreateLibrary[{FileNameJoin[{currentDirectory, "TestSources", "MArgumentManagerCompilationErrors.cpp"}]}, "MArgumentManagerErrors", options]*)
(*	,*)
(*	$Failed*)
(*	,*)
(*	{CreateLibrary::cmperr..} *)(* On Linux there should be 6 errors, but MSVC does not like generic lambdas so it spits out more errors *)
(*	,*)
(*	TestID -> "MArgumentManagerTestSuite-20180903-Y8Z5P1"*)
(*];*)

(* Basic tests *)

Test[
	$DescribePerson["John", 42, 1.83]
	,
	"John is 42 years old and 1.830000m tall."
	,
	TestID -> "MArgumentManagerTestSuite-20191221-I2C5M4"
];

Test[
	$DescribePerson2["James", 43, 1.83]
	,
	"James is 43 years old and 1.830000m tall."
	,
	TestID -> "MArgumentManagerTestSuite-20200307-Y7N4C9"
];

Test[
	$ComparePeople["John", 42, 1.73, "James", 43, 1.83]
	,
	"John is not taller than James."
	,
	TestID -> "MArgumentManagerTestSuite-20200307-F6Z9C4"
];

Test[
	$RepeatString["na", 8]
	,
	"nananananananana"
	,
	TestID -> "MArgumentManagerTestSuite-20200307-T9U0L4"
];

Test[
	$DoNothing[]
	,
	Null
	,
	TestID -> "MArgumentManagerTestSuite-20200307-F1B8G7"
];

Test[
	$GetPersonDescription["John", 42, 1.83]
	,
	"John is 42 years old and 1.830000m tall."
	,
	TestID -> "MArgumentManagerTestSuite-20200307-B4B3M7"
];

(*Test[*)
(*	MemoryLeakTest[PassDataStore[ds3, #]] & /@ {False, True}*)
(*	,*)
(*	{ 0, n_ } /; n > 0*)
(*	,*)
(*	TestID -> "MArgumentManagerTestSuite-20180908-Z1Y1Q5"*)
(*	,*)
(*	SameTest -> MatchQ*)
(*];*)