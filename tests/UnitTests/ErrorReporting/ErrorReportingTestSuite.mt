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
	lib = CCompilerDriver`CreateLibrary[{FileNameJoin[{currentDirectory, "ErrorReportingTest.cpp"}]}, "ErrorReporting", options];


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

Tes[
  SetSPI = SafeLibraryFunction["SetSendParametersImmediately", {"Boolean"}, "Void"];
  SetSPI[False];

  LLU`$LastFailureParameters = {"This", "will", "not", "be", "overwritten"};
  ReadData["somefile.txt"];
  LLU`$LastFailureParameters
  ,
  {"This", "will", "not", "be", "overwritten"}
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