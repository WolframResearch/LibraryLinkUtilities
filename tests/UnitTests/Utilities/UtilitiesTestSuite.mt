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

	`LLU`RegisterPacletErrors[lib, <||>];

	`LLU`Logger`PrintLogFunctionSelector := Block[{`LLU`Logger`FormattedLog = `LLU`Logger`LogToShortString},
		`LLU`Logger`PrintLogToSymbol[LogSymbol][##]
	]&;


	$OpenRead = `LLU`SafeLibraryFunction["OpenForReading", {String}, Integer];
	$OpenWrite = `LLU`SafeLibraryFunction["OpenForWriting", {String}, Integer];
	$OpenInvalidMode = `LLU`SafeLibraryFunction["OpenInvalidMode", {String}, Integer];

	$ReadStrings = `LLU`SafeLibraryFunction["ReadStrings", {String}, "DataStore"];
	$WriteStrings = `LLU`SafeLibraryFunction["WriteStrings", {String, "DataStore"}, "Void"];

	$OpenManagedFile = `LLU`SafeLibraryFunction["OpenManagedFile", {`LLU`Managed[MyFile], String, Integer}, "Void", "Throws" -> True];
	`LLU`Constructor[MyFile] = $OpenManagedFile;

	$OpenManagedFileStream = `LLU`SafeLibraryFunction["OpenManagedFileStream", {`LLU`Managed[FileStream], String, Integer}, "Void", "Throws" -> True];
	`LLU`Constructor[FileStream] = $OpenManagedFileStream;

	f = FileNameJoin[{$TemporaryDirectory, "some_file_that-hopefully-does_not_exist"}];

	topSecretFile = If[$OperatingSystem === "Windows", "C:\\Windows\\system.ini", "/etc/passwd"];
	
	FailureOnWindowsIntegerOtherwiseQ[expr_] := If[$OperatingSystem == "Windows", FailureQ, IntegerQ][expr];
	
	FailureOnWindowsManagedExprOtherwiseQ[expr_] := If[$OperatingSystem == "Windows", FailureQ, ManagedLibraryExpressionQ][expr];
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
	Block[{myFile},
		myFile = `LLU`NewManagedExpression[MyFile][f, 0 (* read-access*)];
		ManagedLibraryExpressionQ[myFile] && IntegerQ[$OpenRead @ f] && IntegerQ[$OpenWrite @ f]
	]
	,
	TestID -> "UtilitiesTestSuite-20191231-O4Y9S8"
];

VerificationTest[
	Block[{myFile},
		myFile = `LLU`NewManagedExpression[MyFile][f, 1 (* write-access*)];
		ManagedLibraryExpressionQ[myFile] && IntegerQ[$OpenRead @ f] && FailureOnWindowsIntegerOtherwiseQ[$OpenWrite @ f]
	]
	,
	TestID -> "UtilitiesTestSuite-20191231-U6M3T5"
];

VerificationTest[
	Block[{myFile},
		myFile = `LLU`NewManagedExpression[MyFile][f, 2 (* read-write-access*)];
		ManagedLibraryExpressionQ[myFile] && IntegerQ[$OpenRead @ f] && FailureOnWindowsIntegerOtherwiseQ[$OpenWrite @ f]
	]
	,
	TestID -> "UtilitiesTestSuite-20191231-P2D8V0"
];

VerificationTest[
	Block[{fs, fs2, fs3},
		fs = Catch @ `LLU`NewManagedExpression[FileStream][f, 0 (* read-access*)];
		fs2 = Catch @ `LLU`NewManagedExpression[FileStream][f, 1 (* write-access*)];
		fs3 = Catch @ `LLU`NewManagedExpression[FileStream][f, 0 (* read-access*)];
		ManagedLibraryExpressionQ[fs] && FailureOnWindowsManagedExprOtherwiseQ[fs2] && ManagedLibraryExpressionQ[fs3]
	]
	,
	TestID -> "UtilitiesTestSuite-20200102-P6T5D6"
];

VerificationTest[
	Block[{fs, fs2, fs3},
		fs = Catch @ `LLU`NewManagedExpression[FileStream][f, 1 (* write-access*)];
		fs2 = Catch @ `LLU`NewManagedExpression[FileStream][f, 1 (* write-access*)];
		fs3 = Catch @ `LLU`NewManagedExpression[FileStream][f, 0 (* read-access*)];
		ManagedLibraryExpressionQ[fs] && FailureOnWindowsManagedExprOtherwiseQ[fs2] && FailureOnWindowsManagedExprOtherwiseQ[fs3]
	]
	,
	TestID -> "UtilitiesTestSuite-20200102-Z1E0R2"
];