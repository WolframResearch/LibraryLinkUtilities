(* Wolfram Language Test file *)
TestRequirement[$VersionNumber > 10.3]
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
]

Test[
	CreatePacletFailure["StaticTopLevelError"]
	,
	Failure["StaticTopLevelError", <|
		"MessageTemplate" -> "This top-level error has a static error message.",
		"MessageParameters" -> <||>, 
		"ErrorCode" -> n_, 
		"Parameters" -> {}
	|>]/; n > 7
	,
	SameTest -> MatchQ
	,
	TestID->"ErrorReportingTestSuite-20190320-V9F7V7"
]

Test[
	CreatePacletFailure["StaticTopLevelError", "MessageParameters" -> <|"X" -> 3|>, "Parameters" -> {"p1", "p2"}]
	,
	Failure["StaticTopLevelError", <|
		"MessageTemplate" -> "This top-level error has a static error message.",
		"MessageParameters" -> <|"X" -> 3|>,
		"ErrorCode" -> n_,
		"Parameters" -> {"p1", "p2"}
	|>]/; n > 7
	,
	SameTest -> MatchQ
	,
	TestID->"ErrorReportingTestSuite-20190320-C8I1M4"
]

Test[
	CreatePacletFailure["StaticTopLevelError", "MessageParameters" -> {"Must", "be", "association"}, "Parameters" -> {1, 2}]
	,
	Failure["StaticTopLevelError", <|
		"MessageTemplate" -> "This top-level error has a static error message.",
		"MessageParameters" -> <||>,
		"ErrorCode" -> n_,
		"Parameters" -> {}
	|>]/; n > 7
	,
	SameTest -> MatchQ
	,
	TestID->"ErrorReportingTestSuite-20190320-N4O5P9"
]

Test[
	CreatePacletFailure["TopLevelNamedSlotsError", "MessageParameters" -> <|"name" -> "John", "when" -> ToString[Now], "unused" -> "param"|>]
	,
	Failure["TopLevelNamedSlotsError", <|
		"MessageTemplate" -> "Hi `name`! Error occurred `when`.",
		"MessageParameters" ->  <|"name" -> "John", "when" -> _String, "unused" -> "param"|>,
		"ErrorCode" -> n_,
		"Parameters" -> {}
	|>]/; n > 7
	,
	SameTest -> MatchQ
	,
	TestID->"ErrorReportingTestSuite-20190320-D1Q8T7"
]

Test[
	CreatePacletFailure["TopLevelNumberedSlotsError", "MessageParameters" -> {"x", "y", "z"}]
	,
	Failure["TopLevelNumberedSlotsError", <|
		"MessageTemplate" -> "Slot number one: `1`, number two: `2`.",
		"MessageParameters" ->  {"x", "y", "z"},
		"ErrorCode" -> n_,
		"Parameters" -> {}
	|>]/; n > 7
	,
	SameTest -> MatchQ
	,
	TestID->"ErrorReportingTestSuite-20190320-Z5Q7P0"
]

(*********************************************************** C++ code failures **************************************************************)

Test[
	ReadData = SafeLibraryFunction["ReadData", {String}, "Void"];
	ReadData["test.txt"]
	,
	Failure["DataFileError", <|
		"MessageTemplate" -> "Data in file `fname` in line `lineNumber` is invalid because `reason`.",
		"MessageParameters" ->  <|"fname" -> "test.txt", "line" -> 8, "reason" -> "data type is not supported"|>,
		"ErrorCode" -> n_,
		"Parameters" -> {}
	|>]/; n < 0
	,
	SameTest -> MatchQ
	,
	TestID->"ErrorReportingTestSuite-20190320-Z5Q2A7"
]

Test[
	ReadData["somefile.txt"]
	,
	Failure["DataFileError", <|
		"MessageTemplate" -> "Data in file `fname` in line `lineNumber` is invalid because `reason`.",
		"MessageParameters" ->  <|"fname" -> "somefile.txt", "line" -> 12, "reason" -> "data type is not supported"|>,
		"ErrorCode" -> n_,
		"Parameters" -> {}
	|>]/; n < 0
	,
	SameTest -> MatchQ
	,
	TestID->"ErrorReportingTestSuite-20190320-W3B2B3"
]

Test[
	ReadData2 = SafeLibraryFunction["ReadDataLocalWLD", {String}, "Void"];
	ReadData2["test.txt"]
	,
	Failure["DataFileError", <|
		"MessageTemplate" -> "Data in file `fname` in line `lineNumber` is invalid because `reason`.",
		"MessageParameters" ->  <|"fname" -> "test.txt", "line" -> 8, "reason" -> "data type is not supported"|>,
		"ErrorCode" -> n_,
		"Parameters" -> {}
	|>]/; n < 0
	,
	SameTest -> MatchQ
	,
	TestID->"ErrorReportingTestSuite-20190320-V5I1S9"
]

Test[
	ReadData2["somefile.txt"]
	,
	Failure["DataFileError", <|
		"MessageTemplate" -> "Data in file `fname` in line `lineNumber` is invalid because `reason`.",
		"MessageParameters" ->  <|"fname" -> "somefile.txt", "line" -> 12, "reason" -> "data type is not supported"|>,
		"ErrorCode" -> n_,
		"Parameters" -> {}
	|>]/; n < 0
	,
	SameTest -> MatchQ
	,
	TestID->"ErrorReportingTestSuite-20190320-A4B7N1"
]

Test[
	RepeatedTemplate = SafeLibraryFunction["RepeatedTemplate", {}, "Void"];
	RepeatedTemplate[]
	,
	Failure["RepeatedTemplateError", <|
		"MessageTemplate" -> "Cannot accept `x` nor `y` because `x` is unacceptable. So are `y` and `z`.",
		"MessageParameters" ->  <|"x" -> "x", "y" -> "y", "z" -> "z"|>,
		"ErrorCode" -> n_,
		"Parameters" -> {}
	|>]/; n < 0
	,
	SameTest -> MatchQ
	,
	TestID->"ErrorReportingTestSuite-20190320-G2N3F5"
]

Test[
	NumberedSlots = SafeLibraryFunction["NumberedSlots", {}, "Void"];
	NumberedSlots[]
	,
	Failure["NumberedSlotsError", <|
		"MessageTemplate" -> "First slot is `1` and second is `2`.",
		"MessageParameters" ->  {1, {"2", "3", "4"}},
		"ErrorCode" -> n_,
		"Parameters" -> {}
	|>]/; n < 0
	,
	SameTest -> MatchQ
	,
	TestID->"ErrorReportingTestSuite-20190320-N1J5Q8"
]

Test[
	RepeatedNumberTemplate = SafeLibraryFunction["RepeatedNumberTemplate", {}, "Void"];
	RepeatedNumberTemplate[]
	,
	Failure["RepeatedNumberTemplateError", <|
		"MessageTemplate" -> "Cannot accept `` nor `` because `1` is unacceptable. So are `2` and ``.",
		"MessageParameters" ->  {"x", "y", "z"},
		"ErrorCode" -> n_,
		"Parameters" -> {}
	|>]/; n < 0
	,
	SameTest -> MatchQ
	,
	TestID->"ErrorReportingTestSuite-20190320-R9L9R5"
]

Test[
	TooManyValues = SafeLibraryFunction["TooManyValues", {}, "Void"];
	TooManyValues[]
	,
	Failure["NumberedSlotsError", <|
		"MessageTemplate" -> "First slot is `1` and second is `2`.",
		"MessageParameters" ->  {1, 2, 3, 4, 5},
		"ErrorCode" -> n_,
		"Parameters" -> {}
	|>]/; n < 0
	,
	SameTest -> MatchQ
	,
	TestID->"ErrorReportingTestSuite-20190320-A9U4T2"
]

Test[
	TooFewValues = SafeLibraryFunction["TooFewValues", {}, "Void"];
	TooFewValues[]
	,
	Failure["NumberedSlotsError", <|
		"MessageTemplate" -> "First slot is `1` and second is `2`.",
		"MessageParameters" ->  {},
		"ErrorCode" -> n_,
		"Parameters" -> {}
	|>]/; n < 0
	,
	SameTest -> MatchQ
	,
	TestID->"ErrorReportingTestSuite-20190320-R0E3G0"
]

Test[
	MixedSlots = SafeLibraryFunction["MixedSlots", {}, "Void"];
	MixedSlots[]
	,
	Failure["MixedSlotsError", <|
		"MessageTemplate" -> "This message `` mixes `2` different `kinds` of `` slots.",
		"MessageParameters" ->  {1, 2, 3, 4},
		"ErrorCode" -> n_,
		"Parameters" -> {}
	|>]/; n < 0
	,
	SameTest -> MatchQ
	,
	TestID->"ErrorReportingTestSuite-20190320-C0V5L0"
]