(* Wolfram Language Test file *)
TestRequirement[$VersionNumber >= 12.0];
(***************************************************************************************************************************************)
(*
	Set of test cases to test LLU functionality related to Managed Library Expressions
*)
(***************************************************************************************************************************************)
TestExecute[
	Needs["CCompilerDriver`"];
	currentDirectory = DirectoryName[$CurrentFile];

	(* Get configuration (path to LLU sources, compilation options, etc.) *)
	Get[FileNameJoin[{ParentDirectory[currentDirectory], "TestConfig.wl"}]];

	(* Compile the test library *)
	lib = CCompilerDriver`CreateLibrary[
		FileNameJoin[{currentDirectory, "TestSources", #}]& /@ {"ManagedExprTest.cpp"},
		"ManagedExpressions",
		options,
		"Defines" -> {"LLU_LOG_DEBUG"}
	];

	Get[FileNameJoin[{$LLUSharedDir, "LibraryLinkUtilities.wl"}]];

	RegisterPacletErrors[lib, <||>];
	
	`LLU`Logger`PrintLogFunctionSelector := Block[{`LLU`Logger`FormattedLog = `LLU`Logger`LogToShortString},
		`LLU`Logger`PrintLogToSymbol[LogSymbol][##]
	]&;

	(* Register a constructor for new Managed Expression. This step could be more automated if we agree that for each class X that shall be managed there is
	an interface function "OpenManagedX" defined in the library. *)
	`LLU`Constructor[MyExpression] = SafeLibraryFunction["OpenManagedMyExpression", {`LLU`Managed[MyExpression], String}, "Void"];

	(* Load a library function that operates on MyExpression which is a registered Managed Expression *)
	getText = LibraryMemberFunction[MyExpression]["GetText", {}, String];

	(* Create new instance of MyExpression *)
	globalExpr = `LLU`NewManagedExpression[MyExpression]["I will live through all tests"];
];

Test[
	LogSymbol
	,
	{
		"[Debug] ManagedExprTest.cpp:15 (MyExpression): MyExpression[1] created."
	}
	,
	TestID -> "ManagedExpressionsTestSuite-20190718-I7S1K0"
];

TestExecute[
	Clear[LogSymbol];
];

Test[
	Clear[LogSymbol];
	e = `LLU`NewManagedExpression[MyExpression]["I will die when this test ends"];
	getText[e]
	,
	"I will die when this test ends"
	,
	TestID->"ManagedExpressionsTestSuite-20190718-O7N0B8"
];

Test[
	LogSymbol
	,
	{
		"[Debug] ManagedExprTest.cpp:15 (MyExpression): MyExpression[2] created.",
		"[Debug] ManagedExprTest.cpp:18 (~MyExpression): MyExpression[2] is dying now."
	}
	,
	TestID->"ManagedExpressionsTestSuite-20190718-J5J8Q0"
];

Test[
	getText[globalExpr]
	,
	"I will live through all tests"
	,
	TestID -> "ManagedExpressionsTestSuite-20190718-A4Y1E1"
];

Test[
	Catch @ getText[MyExpression[500]]
	,
	Failure["InvalidManagedExpressionID", 
		<|
			"MessageTemplate" -> "`expr` is not a valid ManagedExpression.", 
			"MessageParameters" -> <|"expr" -> MyExpression[500]|>, 
			"ErrorCode" -> 25, 
			"Parameters" -> {}
		|>
	]
	,
	TestID->"ManagedExpressionsTestSuite-20190718-B1C1K7"
];

Test[
	Catch @ getText[NotMyExpression[1]]
	,
	Failure["UnexpectedManagedExpression", 
		<|
			"MessageTemplate" -> "Expected managed `expected`, got `actual`.", 
			"MessageParameters" -> <|"expected" -> MyExpression, "actual" -> NotMyExpression[1]|>, 
			"ErrorCode" -> 26, 
			"Parameters" -> {}
		|>
	]
	,
	TestID->"ManagedExpressionsTestSuite-20190718-E0M9X3"
];