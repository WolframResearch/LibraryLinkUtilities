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
		FileNameJoin[{currentDirectory, "TestSources", #}]& /@ {"UtilitiesTest.cpp"},
		"Utilities",
		options
	];

	Get[FileNameJoin[{$LLUSharedDir, "LibraryLinkUtilities.wl"}]];

	RegisterPacletErrors[lib, <||>];

	`LLU`Logger`PrintLogFunctionSelector := Block[{`LLU`Logger`FormattedLog = `LLU`Logger`LogToShortString},
		`LLU`Logger`PrintLogToSymbol[LogSymbol][##]
	]&;


	$OpenRead = SafeLibraryFunction["OpenForReading", {String}, Integer];
	$OpenWrite = SafeLibraryFunction["OpenForWriting", {String}, Integer];
	$OpenInvalidMode = SafeLibraryFunction["OpenInvalidMode", {String}, Integer];

	$ReadStrings = SafeLibraryFunction["ReadStrings", {String}, "DataStore"];
	$WriteStrings = SafeLibraryFunction["WriteStrings", {String, "DataStore"}, "Void"];

	$OpenManagedFile = SafeLibraryFunction["OpenManagedFile", {`LLU`Managed[MyFile], String, Integer}, "Void"];
	`LLU`Constructor[MyFile] = $OpenManagedFile;

	f = FileNameJoin[{$TemporaryDirectory, "some_file_that-hopefully-does_not_exist"}];

	topSecretFile = If[$OperatingSystem === "Windows", "C:\\Windows\\system.ini", "/etc/passwd"]
];

TestExecute[
	DeleteFile[f];
];

TestMatch[
	$OpenRead @ f
	,
	Failure["OpenFileFailed", <|
		"MessageTemplate" -> "Could not open file `f`.",
		"MessageParameters" -><|"f" -> f|>,
		"ErrorCode" -> _?CppErrorCodeQ,
		"Parameters" -> {}
	|>]
	,
	TestID -> "UtilitiesTestSuite-20190718-I7S1K0"
];

Test[
	$OpenWrite @ f
	,
	0
	,
	TestID -> "UtilitiesTestSuite-20191221-T0X0K0"
];

Test[
	Developer`EvaluateProtected @ $OpenRead @ f
	,
	0
	,
	TestID -> "UtilitiesTestSuite-20191221-P3Q0Q8"
];

TestMatch[
	$OpenInvalidMode @ f
	,
	Failure["InvalidOpenMode", <|
		"MessageTemplate" -> "Specified open mode is invalid.",
		"MessageParameters" -> <||>,
		"ErrorCode" -> _?CppErrorCodeQ,
		"Parameters" -> {}
	|>]
	,
	TestID -> "UtilitiesTestSuite-20191221-M2O8P0"
];

TestMatch[
	Developer`EvaluateProtected @ $OpenWrite[topSecretFile]
	,
	Failure["PathNotValidated", <|
		"MessageTemplate" -> "File path `path` could not be validated under desired open mode.",
		"MessageParameters" -><|"path" -> topSecretFile|>,
		"ErrorCode" -> _?CppErrorCodeQ,
		"Parameters" -> {}
	|>]
	,
	TestID -> "UtilitiesTestSuite-20191221-T5I6L4"
];

TestExecute[
	words = {"upraise", "saturnalia", "phonology", "salutatory", "molester", "evolution", "hoist", "humbled", "rusty", "fluctuate"};
];

Test[
	$WriteStrings[f, Developer`DataStore @@ words]
	,
	Null
	,
	TestID -> "UtilitiesTestSuite-20191221-I2C5M4"
];

Test[
	$ReadStrings[f]
	,
	Developer`DataStore @@ words
	,
	TestID -> "UtilitiesTestSuite-20191221-D3K3Z9"
];

VerificationTest[
	myFile = `LLU`NewManagedExpression[MyFile][f, 0 (* read-access*)];
	ManagedLibraryExpressionQ[myFile] && IntegerQ[$OpenRead @ f] && IntegerQ[$OpenWrite @ f]
	,
	TestID -> "UtilitiesTestSuite-20191231-O4Y9S8"
];

VerificationTest[
	myFile2 = `LLU`NewManagedExpression[MyFile][f, 1 (* write-access*)];
	ManagedLibraryExpressionQ[myFile2] && IntegerQ[$OpenRead @ f] && FailureQ[$OpenWrite @ f]
	,
	TestID -> "UtilitiesTestSuite-20191231-U6M3T5"
];

VerificationTest[
	myFile3 = `LLU`NewManagedExpression[MyFile][f, 2 (* read-write-access*)];
	ManagedLibraryExpressionQ[myFile3] && IntegerQ[$OpenRead @ f] && FailureQ[$OpenWrite @ f]
	,
	TestID -> "UtilitiesTestSuite-20191231-P2D8V0"
];