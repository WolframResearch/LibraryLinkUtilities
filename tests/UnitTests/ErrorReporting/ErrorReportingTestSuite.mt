(* Wolfram Language Test file *)
TestRequirement[$VersionNumber > 10.3];
(***************************************************************************************************************************************)
				(*
					Set of test cases to test LLU functionality related to error reporting
				*)
(***************************************************************************************************************************************)
TestExecute[
	Needs["CCompilerDriver`"];
	currentDirectory = DirectoryName[$CurrentFile];

	(* Get configuration (path to LLU sources, compilation options, etc.) *)
	Get[FileNameJoin[{ParentDirectory[currentDirectory], "TestConfig.wl"}]];

	(* Compile the test library *)
	lib = CCompilerDriver`CreateLibrary[FileNameJoin[{currentDirectory, "TestSources", #}]& /@ {"ErrorReportingTest.cpp"},
		"ErrorReporting", options];

	Get[FileNameJoin[{$LLUSharedDir, "LibraryLinkUtilities.wl"}]];

	RegisterPacletErrors[lib, <|
		"StaticTopLevelError" -> "This top-level error has a static error message.",
		"TopLevelNamedSlotsError" -> "Hi `name`! Error occurred `when`.",
		"TopLevelNumberedSlotsError" -> "Slot number one: `1`, number two: `2`."
	|>];

	(* Make sure the log file used in "ReadDataWithLoggingError" does not exist *)
	Quiet @ DeleteFile["LLUErrorLog.txt"];
];

(*********************************************************** Top-level failures **************************************************************)
Test[
	CreatePacletFailure["NoSuchError", "MessageParameters" -> <|"X" -> 1|>]
	,
	Failure["UnknownFailure", <|
		"MessageTemplate" -> "The error `ErrorName` has not been registered.",
		"MessageParameters" -> <|"X" -> 1, "ErrorName" -> "NoSuchError"|>,
		"ErrorCode" -> 23,
		"Parameters" -> {}
	|>]
	,
	TestID->"ErrorReportingTestSuite-20171201-L1W7O4"
];

TestMatch[
	CreatePacletFailure["StaticTopLevelError"]
	,
	Failure["StaticTopLevelError", <|
		"MessageTemplate" -> "This top-level error has a static error message.",
		"MessageParameters" -> <||>,
		"ErrorCode" -> n_,
		"Parameters" -> {}
	|>]/; n > 7
	,
	TestID->"ErrorReportingTestSuite-20190320-V9F7V7"
];

TestMatch[
	CreatePacletFailure["StaticTopLevelError", "MessageParameters" -> <|"X" -> 3|>, "Parameters" -> {"p1", "p2"}]
	,
	Failure["StaticTopLevelError", <|
		"MessageTemplate" -> "This top-level error has a static error message.",
		"MessageParameters" -> <|"X" -> 3|>,
		"ErrorCode" -> n_?TopLevelErrorCodeQ,
		"Parameters" -> {"p1", "p2"}
	|>]
	,
	TestID->"ErrorReportingTestSuite-20190320-C8I1M4"
];

TestMatch[
	CreatePacletFailure["StaticTopLevelError", "MessageParameters" -> "Must be Association or List", "Parameters" -> {1, 2}]
	,
	Failure["StaticTopLevelError", <|
		"MessageTemplate" -> "This top-level error has a static error message.",
		"MessageParameters" -> <||>,
		"ErrorCode" -> n_?TopLevelErrorCodeQ,
		"Parameters" -> {}
	|>]
	,
	TestID->"ErrorReportingTestSuite-20190320-N4O5P9"
];

TestMatch[
	CreatePacletFailure["TopLevelNamedSlotsError", "MessageParameters" -> <|"name" -> "John", "when" -> ToString[Now], "unused" -> "param"|>]
	,
	Failure["TopLevelNamedSlotsError", <|
		"MessageTemplate" -> "Hi `name`! Error occurred `when`.",
		"MessageParameters" ->  <|"name" -> "John", "when" -> _String, "unused" -> "param"|>,
		"ErrorCode" -> n_?TopLevelErrorCodeQ,
		"Parameters" -> {}
	|>]
	,
	TestID->"ErrorReportingTestSuite-20190320-D1Q8T7"
];

TestMatch[
	CreatePacletFailure["TopLevelNumberedSlotsError", "MessageParameters" -> {"x", "y", "z"}]
	,
	Failure["TopLevelNumberedSlotsError", <|
		"MessageTemplate" -> "Slot number one: `1`, number two: `2`.",
		"MessageParameters" ->  {"x", "y", "z"},
		"ErrorCode" -> n_?TopLevelErrorCodeQ,
		"Parameters" -> {}
	|>]
	,
	TestID->"ErrorReportingTestSuite-20190320-Z5Q7P0"
];

(*********************************************************** C++ code failures **************************************************************)

TestMatch[
	ReadData = SafeLibraryFunction["ReadData", {String}, "Void"];
	ReadData["test.txt"]
	,
	Failure["DataFileError", <|
		"MessageTemplate" -> "Data in file `fname` in line `lineNumber` is invalid because `reason`.",
		"MessageParameters" ->  <|"fname" -> "test.txt", "lineNumber" -> 8, "reason" -> "data type is not supported"|>,
		"ErrorCode" -> n_?CppErrorCodeQ,
		"Parameters" -> {}
	|>]
	,
	TestID->"ErrorReportingTestSuite-20190320-Z5Q2A7"
];

TestMatch[
	ReadData["somefile.txt"]
	,
	Failure["DataFileError", <|
		"MessageTemplate" -> "Data in file `fname` in line `lineNumber` is invalid because `reason`.",
		"MessageParameters" ->  <|"fname" -> "somefile.txt", "lineNumber" -> 12, "reason" -> "data type is not supported"|>,
		"ErrorCode" -> n_?CppErrorCodeQ,
		"Parameters" -> {}
	|>]
	,
	TestID->"ErrorReportingTestSuite-20190320-W3B2B3"
];

TestMatch[
	ReadData2 = SafeLibraryFunction["ReadDataLocalWLD", {String}, "Void"];
	ReadData2["test.txt"]
	,
	Failure["DataFileError", <|
		"MessageTemplate" -> "Data in file `fname` in line `lineNumber` is invalid because `reason`.",
		"MessageParameters" ->  <|"fname" -> "test.txt", "lineNumber" -> 8, "reason" -> "data type is not supported"|>,
		"ErrorCode" -> n_?CppErrorCodeQ,
		"Parameters" -> {}
	|>]
	,
	TestID->"ErrorReportingTestSuite-20190320-V5I1S9"
];

TestMatch[
	ReadData2["somefile.txt"]
	,
	Failure["DataFileError", <|
		"MessageTemplate" -> "Data in file `fname` in line `lineNumber` is invalid because `reason`.",
		"MessageParameters" ->  <|"fname" -> "somefile.txt", "lineNumber" -> 12, "reason" -> "data type is not supported"|>,
		"ErrorCode" -> n_?CppErrorCodeQ,
		"Parameters" -> {}
	|>]
	,
	TestID->"ErrorReportingTestSuite-20190320-A4B7N1"
];

TestMatch[
	RepeatedTemplate = SafeLibraryFunction["RepeatedTemplate", {}, "Void"];
	RepeatedTemplate[]
	,
	Failure["RepeatedTemplateError", <|
		"MessageTemplate" -> "Cannot accept `x` nor `y` because `x` is unacceptable. So are `y` and `z`.",
		"MessageParameters" ->  <|"x" -> "x", "y" -> "y", "z" -> "z"|>,
		"ErrorCode" -> n_?CppErrorCodeQ,
		"Parameters" -> {}
	|>]
	,
	TestID->"ErrorReportingTestSuite-20190320-G2N3F5"
];

TestMatch[
	NumberedSlots = SafeLibraryFunction["NumberedSlots", {}, "Void"];
	NumberedSlots[]
	,
	Failure["NumberedSlotsError", <|
		"MessageTemplate" -> "First slot is `1` and second is `2`.",
		"MessageParameters" ->  {1, {"2", "3", "4"}},
		"ErrorCode" -> n_?CppErrorCodeQ,
		"Parameters" -> {}
	|>]
	,
	TestID->"ErrorReportingTestSuite-20190320-N1J5Q8"
];

TestMatch[
	RepeatedNumberTemplate = SafeLibraryFunction["RepeatedNumberTemplate", {}, "Void"];
	RepeatedNumberTemplate[]
	,
	Failure["RepeatedNumberTemplateError", <|
		"MessageTemplate" -> "Cannot accept `` nor `` because `1` is unacceptable. So are `2` and ``.",
		"MessageParameters" ->  {"x", "y", "z"},
		"ErrorCode" -> n_?CppErrorCodeQ,
		"Parameters" -> {}
	|>]
	,
	TestID->"ErrorReportingTestSuite-20190320-R9L9R5"
];

TestMatch[
	TooManyValues = SafeLibraryFunction["TooManyValues", {}, "Void"];
	TooManyValues[]
	,
	Failure["NumberedSlotsError", <|
		"MessageTemplate" -> "First slot is `1` and second is `2`.",
		"MessageParameters" ->  {1, 2, 3, 4, 5},
		"ErrorCode" -> n_?CppErrorCodeQ,
		"Parameters" -> {}
	|>]
	,
	TestID->"ErrorReportingTestSuite-20190320-A9U4T2"
];

TestMatch[
	TooFewValues = SafeLibraryFunction["TooFewValues", {}, "Void"];
	TooFewValues[]
	,
	Failure["NumberedSlotsError", <|
		"MessageTemplate" -> "First slot is `1` and second is `2`.",
		"MessageParameters" ->  {},
		"ErrorCode" -> n_?CppErrorCodeQ,
		"Parameters" -> {}
	|>]
	,
	TestID->"ErrorReportingTestSuite-20190320-R0E3G0"
];

TestMatch[
	MixedSlots = SafeLibraryFunction["MixedSlots", {}, "Void"];
	MixedSlots[]
	,
	Failure["MixedSlotsError", <|
		"MessageTemplate" -> "This message `` mixes `2` different `kinds` of `` slots.",
		"MessageParameters" ->  {1, 2, <|"kinds" -> 3|>, 4},
		"ErrorCode" -> n_?CppErrorCodeQ,
		"Parameters" -> {}
	|>]
	,
	TestID->"ErrorReportingTestSuite-20190320-C0V5L0"
];


(* Unit tests of ErrorManager::throwCustomException *)

TestMatch[
	ReadDataWithLoggingError = SafeLibraryFunction["ReadDataWithLoggingError", {String}, "Void"];
	ReadDataWithLoggingError["test.txt"]
	,
	Failure["DataFileError", <|
		"MessageTemplate" -> "Data in file `fname` in line `lineNumber` is invalid because `reason`.",
		"MessageParameters" ->  <|"fname" -> "test.txt", "lineNumber" -> 8, "reason" -> "data type is not supported"|>,
		"ErrorCode" -> n_?CppErrorCodeQ,
		"Parameters" -> {}
	|>]
	,
	TestID->"ErrorReportingTestSuite-20190404-F2M3A2"
];

TestMatch[
	ReadDataWithLoggingError["ThisFileHasExtremelyLongName.txt"]
	,
	Failure["DataFileError", <|
		"MessageTemplate" -> "Data in file `fname` in line `lineNumber` is invalid because `reason`.",
		"MessageParameters" ->  <|"fname" -> "ThisFileHasExtremelyLongName.txt", "lineNumber" -> 32, "reason" -> "file name is too long"|>,
		"ErrorCode" -> n_?CppErrorCodeQ,
		"Parameters" -> {}
	|>]
	,
	TestID->"ErrorReportingTestSuite-20190404-B7J4Y9"
];

TestMatch[
	ReadDataWithLoggingError["Secret:Data"]
	,
	Failure["DataFileError", <|
		"MessageTemplate" -> "Data in file `fname` in line `lineNumber` is invalid because `reason`.",
		"MessageParameters" ->  <|"fname" -> "Secret:Data", "lineNumber" -> 0, "reason" -> "file name contains a possibly problematic character \":\""|>,
		"ErrorCode" -> n_?CppErrorCodeQ,
		"Parameters" -> {}
	|>]
	,
	TestID->"ErrorReportingTestSuite-20190404-K3J3E1"
];

Test[
	exCount = StringCount[Import["LLUErrorLog.txt"], "Exception"];
	Quiet @ DeleteFile["LLUErrorLog.txt"];
	exCount
	,
	3
	,
	TestID->"ErrorReportingTestSuite-20190404-U4H9N8"
];


(* Unit tests of ErrorManager::sendParamatersImmediately *)

Test[
  GetSPI = SafeLibraryFunction["GetSendParametersImmediately", {}, "Boolean"];
  GetSPI[]
  ,
  True
  ,
  TestID->"ErrorReportingTestSuite-20190404-F9O0O1"
];

Test[
  SetSPI = SafeLibraryFunction["SetSendParametersImmediately", {"Boolean"}, "Void"];
  SetSPI[False];

  `LLU`$LastFailureParameters = {"This", "will", "be", "overwritten"};
  ReadData["somefile.txt"];
  `LLU`$LastFailureParameters
  ,
  {}
  ,
  TestID->"ErrorReportingTestSuite-20190404-O3A4K4"
];

TestMatch[
  ReadDataDelayedParametersTransfer = SafeLibraryFunction["ReadDataDelayedParametersTransfer", {String}, "Void"];
  ReadDataDelayedParametersTransfer["somefile.txt"]
  ,
  Failure["DataFileError", <|
    "MessageTemplate" -> "Data in file `fname` in line `lineNumber` is invalid because `reason`.",
    "MessageParameters" ->  <|"fname" -> "somefile.txt", "lineNumber" -> 12, "reason" -> "data type is not supported"|>,
    "ErrorCode" -> n_?CppErrorCodeQ,
    "Parameters" -> {}
  |>]
  ,
  TestID->"ErrorReportingTestSuite-20190404-N7X5J6"
];

(*********************************************************** Logging tests **************************************************************)
TestExecute[
	loggerTestPath = FileNameJoin[{currentDirectory, "TestSources", "LoggerTest.cpp"}];
	libLogDebug = CCompilerDriver`CreateLibrary[{loggerTestPath}, "LogDebug", options, "Defines" -> {"LLU_LOG_DEBUG"}];

	$InitLibraryLinkUtils = False;
	RegisterPacletErrors[libLogDebug, <||>];

	`LLU`Logger`PrintLogFunctionSelector := Block[{`LLU`Logger`FormattedLog = `LLU`Logger`LogToAssociation},
		`LLU`Logger`PrintLogToSymbol[TestLogSymbol][##]
	]&;
];

Test[
	GreaterAt = SafeLibraryFunction["GreaterAt", {String, {_, 1}, Integer, Integer}, "Boolean"];
	GreaterAt["file.txt", {5, 6, 7, 8, 9}, 1, 3];
	TestLogSymbol
	,
	{
		<|
			"Level" -> "Debug", 
			"Line" -> 19,
			"File" -> loggerTestPath, 
			"Function" -> "GreaterAt", 
			"Message" -> Style["Library function entered with 4 arguments.", FontSize -> Inherited]
		|>,
		<|
			"Level" -> "Debug", 
			"Line" -> 22,
			"File" -> loggerTestPath, 
			"Function" -> "GreaterAt", 
			"Message" -> Style["Starting try-block, current error code: 0", FontSize -> Inherited]
		|>, 
		<|
			"Level" -> "Debug", 
			"Line" -> 28,
			"File" -> loggerTestPath, 
			"Function" -> "GreaterAt", 
			"Message" -> Style["Input tensor is of type: 2", FontSize -> Inherited]
		|>, 
		<|
			"Level" -> "Debug", 
			"Line" -> 41,
			"File" -> loggerTestPath, 
			"Function" -> "GreaterAt", 
			"Message" -> Style["Comparing 5 with 7", FontSize -> Inherited]
		|>
	}
	,
	TestID->"ErrorReportingTestSuite-20190409-U4I2Y8"
];

TestExecute[
	TestLogSymbol = 5; (* assign a number to TestLogSymbol to see if LLU`Logger`PrintToSymbol can handle it *)
	`LLU`Logger`PrintLogFunctionSelector := Block[{`LLU`Logger`FormattedLog = `LLU`Logger`LogToList},
		`LLU`Logger`PrintLogToSymbol[TestLogSymbol][##]
	]&
];

Test[
	GreaterAt["my:file.txt", {5, 6, 7, 8, 9}, 1, 3];
	TestLogSymbol
	,
	{
		{"Debug", 19, loggerTestPath, "GreaterAt", "Library function entered with ", 4, " arguments."},
		{"Debug", 22, loggerTestPath, "GreaterAt", "Starting try-block, current error code: ", 0},
		{"Warning", 26, loggerTestPath, "GreaterAt", "File name ", "my:file.txt", " contains a possibly problematic character \":\"."}, 
		{"Debug", 28, loggerTestPath, "GreaterAt", "Input tensor is of type: ", 2}, 
		{"Debug", 41, loggerTestPath, "GreaterAt", "Comparing ", 5, " with ", 7}
	}
	,
	TestID->"ErrorReportingTestSuite-20190409-L8V2U9"
];

Test[
	MultiThreadedLog = SafeLibraryFunction["LogsFromThreads", {Integer}, "Void"];
	Clear[TestLogSymbol];
	MultiThreadedLog[3];
	And @@ (MatchQ[
		{"Debug", 90, loggerTestPath, "LogsFromThreads", "Starting ", 3, " threads."} |
		{"Debug", 93, loggerTestPath, "operator()" | "operator ()", "Thread ", _, " going to sleep."} |
		{"Debug", 95, loggerTestPath, "operator()" | "operator ()", "Thread ", _, " slept for ", _, "ms."} |
		{"Debug", 103, loggerTestPath, "LogsFromThreads", "All threads joined."}
	] /@ TestLogSymbol)
	&&
	First[TestLogSymbol] === {"Debug", 90, loggerTestPath, "LogsFromThreads", "Starting ", 3, " threads."}
	&&
	Last[TestLogSymbol] === {"Debug", 103, loggerTestPath, "LogsFromThreads", "All threads joined."}
	,
	True
	,
	TestID->"ErrorReportingTestSuite-20190415-Y8F3L2"
];

TestExecute[
	`LLU`Logger`PrintLogFunctionSelector :=
		If[## =!= `LLU`Logger`LogFiltered,
			Sow @ `LLU`Logger`LogToShortString[##]
		]&;
];

TestMatch[
	Reap @ GreaterAt["file.txt", {5, 6, 7, 8, 9}, -1, 3]
	,
	{
		Failure["TensorIndexError", <|
			"MessageTemplate" -> "An error was caused by attempting to access a nonexistent Tensor element.",
			"MessageParameters" -> <||>,
			"ErrorCode" ->  n_?CppErrorCodeQ, 
			"Parameters" -> {}
		|>], {
			{
				"[Debug] LoggerTest.cpp:19 (GreaterAt): Library function entered with 4 arguments.", 
				"[Debug] LoggerTest.cpp:22 (GreaterAt): Starting try-block, current error code: 0", 
				"[Debug] LoggerTest.cpp:28 (GreaterAt): Input tensor is of type: 2", 
				"[Error] LoggerTest.cpp:45 (GreaterAt): Caught LLU exception TensorIndexError: Indices (-1, 3) must be positive."
			}
		}
	}
	,
	TestID->"ErrorReportingTestSuite-20190409-P1S6Y9"
];

TestExecute[
	(* Disable logging in top-level, messages are still transferred from the library *)
	`LLU`Logger`LogFilterSelector = `LLU`Logger`FilterRejectAll;
];

Test[
	Reap @ GreaterAt["my:file.txt", {5, 6, 7, 8, 9}, 1, 3]
	,
	{False, {}}
	,
	TestID->"ErrorReportingTestSuite-20190410-R2D4P1"
];

TestExecute[
	(* Log only warnings *)
	`LLU`Logger`LogFilterSelector = `LLU`Logger`FilterByLevel[StringMatchQ["warning", IgnoreCase -> True]];
];

Test[
	Reap @ GreaterAt["my:file.txt", {5, 6, 7, 8, 9}, 1, 3]
	,
	{False, {{"[Warning] LoggerTest.cpp:26 (GreaterAt): File name my:file.txt contains a possibly problematic character \":\"."}}}
	,
	TestID->"ErrorReportingTestSuite-20190410-H8S6D5"
];

TestExecute[
	(* Log only messages issued from even line numbers *)
	`LLU`Logger`LogFilterSelector = `LLU`Logger`FilterByLine[EvenQ];
];

Test[
	Reap @ GreaterAt["my:file.txt", {5, 6, 7, 8, 9}, 1, 3]
	,
	{
		False, {
			{
				"[Debug] LoggerTest.cpp:22 (GreaterAt): Starting try-block, current error code: 0",
				"[Warning] LoggerTest.cpp:26 (GreaterAt): File name my:file.txt contains a possibly problematic character \":\".",
				"[Debug] LoggerTest.cpp:28 (GreaterAt): Input tensor is of type: 2"
			}
		}
	}
	,
	TestID->"ErrorReportingTestSuite-20190410-G6A5W4"
];

TestExecute[
	libLogWarning = CCompilerDriver`CreateLibrary[FileNameJoin[{currentDirectory, "TestSources", #}]& /@ {"LoggerTest.cpp"},
		"LogWarning", options, "Defines" -> {"LLU_LOG_WARNING"}];
		
	$InitLibraryLinkUtils = False;
	RegisterPacletErrors[libLogWarning, <||>];
	GreaterAtW = SafeLibraryFunction["GreaterAt", {String, {_, 1}, Integer, Integer}, "Boolean"];
];

Test[
	Reap @ GreaterAtW["my:file.txt", {5, 6, 7, 8, 9}, 1, 3]
	,
	{
		False, {{"[Warning] LoggerTest.cpp:26 (GreaterAt): File name my:file.txt contains a possibly problematic character \":\"."}}
	}
	,
	TestID->"ErrorReportingTestSuite-20190415-F5I9D0"
];

TestExecute[
	Get[FileNameJoin[{$LLUSharedDir, "LibraryLinkUtilities.wl"}]];
	RegisterPacletErrors[libLogWarning, <||>];
];

Test[
	Reap @ GreaterAtW["my:file.txt", {5, 6, 7, 8, 9}, 1, 3]
	,
	{
		False, {}
	}
	,
	TestID->"ErrorReportingTestSuite-20190415-A6S8Y7"
];

TestExecute[
	`LLU`Logger`PrintLogFunctionSelector := Sow @ `LLU`Logger`LogToShortString[##]&;
];

TestMatch[
	Reap @ GreaterAtW["file.txt", {5, 6, 7, 8, 9}, -1, 3]
	,
	{
		Failure["TensorIndexError", <|
			"MessageTemplate" -> "An error was caused by attempting to access a nonexistent Tensor element.",
			"MessageParameters" -> <||>,
			"ErrorCode" ->  n_?CppErrorCodeQ, 
			"Parameters" -> {}
		|>], {
			{
				"[Error] LoggerTest.cpp:45 (GreaterAt): Caught LLU exception TensorIndexError: Indices (-1, 3) must be positive."
			}
		}
	}
	,
	TestID->"ErrorReportingTestSuite-20190415-P3C4F8"
];

TestExecute[
	TestLogSymbol = {};
	`LLU`Logger`PrintLogFunctionSelector := Block[{`LLU`Logger`FormattedLog = `LLU`Logger`LogToList},
		`LLU`Logger`PrintLogToSymbol[TestLogSymbol][##]
	]&;
	LogDemo = SafeLibraryFunction["LogDemo", {Integer, Integer, Integer, Integer, Integer}, Integer];
];

Test[
	{LogDemo[1, 6, 7, 8, 9], TestLogSymbol}
	,
	{6,{}}
	,
	TestID->"ErrorReportingTestSuite-20190415-C0D7H6"
];

Test[
	{LogDemo[5, 6, 7, 8, 9], TestLogSymbol}
	,
	{
		9,
		{
			{"Warning", 66, loggerTestPath, "LogDemo", "Index ", 5, " is too big for the number of arguments: ", 5, ". Changing to ", 4}
		}
	}
	,
	TestID->"ErrorReportingTestSuite-20190415-J1G2K9"
];

Test[
	TestLogSymbol = {};
	{LogDemo[-1, 6, 7, 8, 9], TestLogSymbol}
	,
	{
		Failure["MArgumentIndexError", 
			<|
				"MessageTemplate" -> "An error was caused by an incorrect argument index.", 
				"MessageParameters" -> <||>, 
				"ErrorCode" -> -2, 
				"Parameters" -> {}
			|>
		],
		{
			{"Error", 73, loggerTestPath, "LogDemo", "Caught LLU exception ", "MArgumentIndexError", ": ", "Index 4294967295 out-of-bound when accessing LibraryLink argument"}
		}
	}
	,
	TestID->"ErrorReportingTestSuite-20190415-U9M7O6"
];
