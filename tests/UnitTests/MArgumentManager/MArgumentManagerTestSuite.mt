(***************************************************************************************************************************************)
(*
	Set of test cases to test LLU functionality related to MArgumentManager and MArgument passing to and fro LibraryLink
*)
(***************************************************************************************************************************************)
TestExecute[
	Needs["CCompilerDriver`"];
	currentDirectory = DirectoryName[$TestFileName];

	(* Get configuration (path to LLU sources, compilation options, etc.) *)
	Get[FileNameJoin[{ParentDirectory[currentDirectory], "TestConfig.wl"}]];

	(* Compile the test library *)
	lib = CCompilerDriver`CreateLibrary[
		FileNameJoin[{currentDirectory, "TestSources", #}]& /@ {"MArgumentManagerTest.cpp"},
		"MArgumentManagerTest",
		options
	];

	Get[FileNameJoin[{$LLUSharedDir, "LibraryLinkUtilities.wl"}]];

	`LLU`InitializePacletLibrary[lib];

	(* Person expression takes 3 arguments: name, age and height e.g. Person["James", 20, 1.75],
	 * so it decays to 3 LibraryLink arguments: String, Integer and Real. *)
	`LLU`MArgumentType[Person, {String, Integer, Real}, (Sequence @@ #) &];
	`LLU`MArgumentType[
		Couple,
		`LLU`MArgumentCustomType /@ {Person, Person},
		(Sequence @@ (`LLU`MArgumentTransform[Person] /@ #)) &
	];

	$DescribePerson = `LLU`PacletFunctionLoad["DescribePerson", {String, Integer, Real}, String];
	$DescribePerson2 = `LLU`PacletFunctionLoad["DescribePerson2", {Person}, String];
	$ComparePeople = `LLU`PacletFunctionLoad["ComparePeople", {Couple}, String];

	$RepeatString = `LLU`PacletFunctionLoad["LL_repeatString", {String, Integer}, String];
	$DoNothing = `LLU`PacletFunctionLoad["LL_doNothing", {}, "Void"];

	$GetPersonDescription = `LLU`PacletFunctionLoad["GetPersonDescription", {Person}, String];

	john = Person["John", 42, 1.83];
	james = Person["James", 43, 1.73];
	alicia = Person["Alicia", 27, 1.74];

	`LLU`MResultType[Person, "DataStore", (Person @@ #)&];
	$PredictChild = `LLU`PacletFunctionLoad["PredictChild", {Couple}, Person];
];


(* Compile-time errors *)
TestMatch[
	Off[General::stop];
	CCompilerDriver`CreateLibrary[{FileNameJoin[{currentDirectory, "TestSources", "MArgumentManagerCompilationErrors.cpp"}]}, "MArgumentManagerErrors", options]
	,
	$Failed
	,
	{Repeated[CreateLibrary::cmperr, {1, 5}]} (* different compilers may stop after different number of errors, there must be at least 1 *)
	,
	TestID -> "MArgumentManagerTestSuite-20180903-Y8Z5P1"
];

(* Basic tests *)

TestCreate[
	`LLU`Private`SelectSpecialArgs[{`LLU`Managed[x], String, "UTF8String"}]
	,
	<|1 -> `LLU`Managed[x]|>
	,
	TestID -> "MArgumentManagerTestSuite-20191221-I2C5M4"
];

TestCreate[
	`LLU`Private`SelectSpecialArgs[{String, Person, {_, _}, NotRegisteredSoNotSpecialArg, Couple}]
	,
	<|2 -> Person, 5 -> Couple|>
	,
	TestID -> "MArgumentManagerTestSuite-20200309-T6P9I5"
];

TestCreate[
	args = `LLU`Private`SelectSpecialArgs[{String, Person, {_, _}, Couple}];
	{`LLU`Private`ArgumentParser[args]["hello", john, Range[7], Couple[james, john]]}
	,
	{"hello", "John", 42, 1.83, {1, 2, 3, 4, 5, 6, 7}, "James", 43, 1.73, "John", 42, 1.83}
	,
	TestID -> "MArgumentManagerTestSuite-20200309-P1B5S5"
];

VerificationTest[
	$AsFloat = `LLU`PacletFunctionLoad["AsFloat", {Real}, Real];
	Abs[$AsFloat[1.23456] - 1.23456] < 10^-7
	,
	TestID -> "MArgumentManagerTestSuite-20200309-R1Y5I7"
];

VerificationTest[
	$Transform = `LLU`PacletFunctionLoad["Transform", {Real, Integer, Real}, Real];
	a = 3.14;
	n = 42;
	b = 56.789;
	Abs[$Transform[a, n, b] - (a * n + b)] < 10^-5
	,
	TestID -> "MArgumentManagerTestSuite-20200406-S1A5I1"
];

TestCreate[
	$DescribePerson["John", 42, 1.83]
	,
	"John is 42 years old and 1.830000m tall."
	,
	TestID -> "MArgumentManagerTestSuite-20200405-W6E2K7"
];

TestMatch[
	Catch[
		$DescribePerson[james], (* should fail because this function is not registered to handle Person expression *)
		"LLUExceptionTag"
	]
	,
	_?FailureQ(*unevaluated LibraryFunction*)
	,
	{Message[LibraryFunction::cfct, HoldCompleteForm[1],  HoldCompleteForm[3]]}
	,
	TestID -> "MArgumentManagerTestSuite-20200309-T0B1C4"
];

TestCreate[
	$DescribePerson2[james]
	,
	"James is 43 years old and 1.730000m tall."
	,
	TestID -> "MArgumentManagerTestSuite-20200307-Y7N4C9"
];

TestCreate[
	$DescribePerson2["John", 42, 1.83]
	(* this only works because MArgumentTransform[Person] does not validate if the input expression is actually a Person *)
	,
	"John is 42 years old and 1.830000m tall."
	,
	TestID -> "MArgumentManagerTestSuite-20200309-D8Q9D8"
];

TestCreate[
	c = Couple[john, Person["Alicia", 27, 1.74]];
	$ComparePeople @ c
	,
	"John is taller than Alicia."
	,
	TestID -> "MArgumentManagerTestSuite-20200307-F6Z9C4"
];

TestCreate[
	$RepeatString["na", 8]
	,
	"nananananananana"
	,
	TestID -> "MArgumentManagerTestSuite-20200307-T9U0L4"
];

TestCreate[
	$DoNothing[]
	,
	Null
	,
	TestID -> "MArgumentManagerTestSuite-20200307-F1B8G7"
];

TestCreate[
	$GetPersonDescription @ john
	,
	"John is 42 years old and 1.830000m tall."
	,
	TestID -> "MArgumentManagerTestSuite-20200307-B4B3M7"
];

TestCreate[
	$PredictChild @ Couple[john, alicia]
	,
	Person["John Junior", 0, (john[[3]] + alicia[[3]]) / 2]
	,
	TestID -> "MArgumentManagerTestSuite-20200312-D9T9H8"
];

TestExecute[
	$GetTallest = `LLU`PacletFunctionLoad["GetTallest", {"DataStore"}, String];
	$Sort = `LLU`PacletFunctionLoad["Sort", {{NumericArray, "Constant"}, {NumericArray, "Constant"}}, NumericArray];
];

TestCreate[
	people = Apply[Developer`DataStore, {john, james, alicia}, {0, 1}];
	$GetTallest @ people
	,
	"John"
	,
	TestID -> "MArgumentManagerTestSuite-20200406-B7I2M4"
];

TestExecute[
	v1 = Range[10];
	v2 = Range[20];
];

TestCreate[
	Normal @ $Sort[NumericArray[v1, "Integer32"], NumericArray[v2, "Integer64"]]
	,
	Sort[v2, Greater] ~Join~ Sort[v1, Less]
	,
	TestID -> "MArgumentManagerTestSuite-20200406-A1Y8O9"
];