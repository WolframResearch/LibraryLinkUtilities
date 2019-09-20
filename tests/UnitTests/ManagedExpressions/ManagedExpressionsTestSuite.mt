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


	$CreateNewMyExpression = SafeLibraryFunction["OpenManagedMyExpression", {`LLU`Managed[MyExpression], String}, "Void"];
	$CreateNewMyChildExpression = SafeLibraryFunction["OpenManagedMyChildExpression", {`LLU`Managed[MyExpression], String}, "Void"];

	(* Register a constructor for new Managed Expression. This step could be more automated if we agree that for each class X that shall be managed there is
	 * an interface function "OpenManagedX" defined in the library.
	 * It gets more complicated if you want to use a class hierarchy as MLE as you need to decide which class will be instantiated at WL level.
	 *)
	CreateMyExpression[instance_, text_?StringQ, createChildQ_ : False] := If[createChildQ, $CreateNewMyChildExpression, $CreateNewMyExpression][instance, text];
	`LLU`Constructor[MyExpression] = CreateMyExpression;

	(* Load library functions that wrap MyExpression member functions *)
	`LLU`LoadMemberFunction[MyExpression][getText, "GetText", {}, String];
	`LLU`LoadMemberFunction[MyExpression][setText, "SetText", {String}, "Void"];
	`LLU`LoadMathLinkMemberFunction[MyExpression][setTextML, "SetTextML"];
	`LLU`LoadMemberFunction[MyExpression][getCounter, "GetCounter", {}, Integer]; (* this member only works with MLEs that are of type MyChildExpression in C++ *)

	(* Load other library functions *)
	joinText = SafeLibraryFunction["JoinText", {`LLU`Managed[MyExpression], `LLU`Managed[MyExpression]}, String];
	getMyExpressionStoreName = SafeLibraryFunction["GetMyExpressionStoreName", {}, String];
	swapText = SafeMathLinkFunction["SwapText"];

	ManagedMyExprQ = `LLU`ManagedQ[MyExpression];
	ManagedMyExprIDQ = `LLU`ManagedIDQ[MyExpression];

	(* Create new instance of MyExpression *)
	globalExpr = `LLU`NewManagedExpression[MyExpression]["I will live through all tests"];
];

Test[
	LogSymbol
	,
	{ "MyExpression[1] created." }
	,
	SameTest -> LoggerStringTest
	,
	TestID -> "ManagedExpressionsTestSuite-20190718-I7S1K0"
];

TestExecute[
	Clear[LogSymbol];
];

Test[
	ManagedMyExprQ[globalExpr]
	,
	True
	,
	TestID -> "ManagedExpressionsTestSuite-20190916-R3H6L8"
];

Test[
	`LLU`ManagedQ[x][globalExpr]
	,
	False
	,
	TestID -> "ManagedExpressionsTestSuite-20190916-L9C5S2"
];

Test[
	ManagedMyExprIDQ[`LLU`GetManagedID[globalExpr]]
	,
	True
	,
	TestID -> "ManagedExpressionsTestSuite-20190916-A0B2P8"
];

Test[
	ManagedMyExprIDQ[150]
	,
	False
	,
	TestID -> "ManagedExpressionsTestSuite-20190916-X6Z9M9"
];

Test[
	Clear[LogSymbol];
	Block[{e},
		e = `LLU`NewManagedExpression[MyExpression]["I will die when this test ends"];
		e @ getText[]
	]
	,
	"I will die when this test ends"
	,
	TestID -> "ManagedExpressionsTestSuite-20190718-O7N0B8"
];

Test[
	LogSymbol
	,
	{
		"MyExpression[2] created.",
		"MyExpression[2] is dying now."
	}
	,
	SameTest -> LoggerStringTest
	,
	TestID -> "ManagedExpressionsTestSuite-20190718-J5J8Q0"
];

Test[
	MyExpression`getText[globalExpr]
	,
	"I will live through all tests"
	,
	TestID -> "ManagedExpressionsTestSuite-20190718-A4Y1E1"
];

Test[
	MyExpression`setText[globalExpr, "New text for global expr."]
	,
	Null
	,
	TestID -> "ManagedExpressionsTestSuite-20190821-K9R3V7"
];

Test[
	globalExpr @ getText[]
	,
	"New text for global expr."
	,
	TestID -> "ManagedExpressionsTestSuite-20190821-T2J8J8"
];

Test[
	globalExpr @ setText["I will live through all tests"]
	,
	Null
	,
	TestID -> "ManagedExpressionsTestSuite-20190821-B3M7R5"
];

Test[
	Catch @ MyExpression[500] @ getText[]
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
	TestID -> "ManagedExpressionsTestSuite-20190718-B1C1K7"
];

Test[
	Catch @ MyExpression`getText[NotMyExpression[1]]
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
	TestID -> "ManagedExpressionsTestSuite-20190718-E0M9X3"
];

Test[
	NotMyExpression[1] @ getText[]
	,
	NotMyExpression[1] @ getText[]
	,
	TestID -> "ManagedExpressionsTestSuite-20190821-Z2D6S7"
];

Test[
	Clear[LogSymbol];
	joinText[globalExpr, `LLU`NewManagedExpression[MyExpression]["I'm just a temporary"]]
	,
	"I will live through all testsI'm just a temporary"
	,
	TestID -> "ManagedExpressionsTestSuite-20190821-M2R9F3"
];

Test[
	LogSymbol
	,
	{
		"MyExpression[3] created.",
		"MyExpression[3] is dying now."
	}
	,
	SameTest -> LoggerStringTest
	,
	TestID -> "ManagedExpressionsTestSuite-20190821-W5W6N0"
];

Test[
	expr1 = `LLU`NewManagedExpression[MyExpression]["Hello"];
	expr2 = `LLU`NewManagedExpression[MyExpression]["World"];
	swapText[expr1, expr2];
	{expr1 @ getText[], expr2 @ getText[]}
	,
	{"World", "Hello"}
	,
	TestID -> "ManagedExpressionsTestSuite-20190903-A5G8P0"
];

Test[
	expr1 @ setTextML["My new text"];
	expr1 @ getText[]
	,
	"My new text"
	,
	TestID -> "ManagedExpressionsTestSuite-20190903-J6R4H9"
];

Test[
	getMyExpressionStoreName[]
	,
	ToString @ Head @ globalExpr
	,
	TestID -> "ManagedExpressionsTestSuite-20190904-E2I7L4"
];

TestExecute[
	Clear[LogSymbol];
	`LLU`NewManagedExpression[MyExpression]["Hello, I'm a subclass of MyExpression", True];
];

Test[
	LogSymbol
	,
	{
		"MyExpression[6] created.",
		"MyChildExpression[6] created.",
		"MyChildExpression[6] is dying now.",
		"MyExpression[6] is dying now."
	}
	,
	SameTest -> LoggerStringTest
	,
	TestID -> "ManagedExpressionsTestSuite-20190911-U4H9A5"
];

Test[
	subclassExpr = `LLU`NewManagedExpression[MyExpression]["I'm a MyChildExpression", True];
	subclassExpr @ getCounter[];
	subclassExpr @ getCounter[]
	,
	2
	,
	TestID -> "ManagedExpressionsTestSuite-20190911-R0A1T3"
];

Test[
	Block[{e},
		e = `LLU`NewManagedExpression[MyExpression]["I will die when this test ends", True];
		e @ getText[]
	]
	,
	"I'm a subclass! Here is your text: I will die when this test ends"
	,
	TestID -> "ManagedExpressionsTestSuite-20190911-R3Z8U9"
];