(* Wolfram Language Test file *)
(***************************************************************************************************************************************)
				(*
					Set of Testcases to test string apis for library link
				*)
(***************************************************************************************************************************************)
TestExecute[
	currentDirectory = DirectoryName[$CurrentFile];
	Get[FileNameJoin[{ParentDirectory[currentDirectory], "TestConfig.wl"}]];
]

ExactTest[
	Get[FileNameJoin[{currentDirectory, "EchoString", "EchoString.wl"}]];
	EchoString["foo bar"]
	,
	"foo bar"	
	,
	TestID->"StringOperations-20150807-D1D4K5"
]

ExactTest[
	Get[FileNameJoin[{currentDirectory, "Greetings", "Greetings.wl"}]];
	Greetings["wolfram"]
	,
	"Greetings wolfram!"
	,
	TestID->"StringOperations-20150807-X0R7U9"
]

ExactTest[
	Get[FileNameJoin[{currentDirectory, "HelloWorld", "HelloWorld.wl"}]];
	HelloWorld[]
	,
	"Hello World"
	,
	TestID->"StringOperations-20150807-K4Y4G8"
]

ExactTest[
	CapitalizeFirst["hello World"]
	,
	"Hello World"
	,
	TestID->"StringTestSuite-20180821-Y4A0E2"
]

Test[
	CapitalizeAll["Hello World"]
	,
	ToUpperCase["Hello World"]
	,
	TestID->"StringTestSuite-20180821-X0K5Z1"
]

Test[
	largeString = StringJoin @ RandomChoice[Alphabet[], 100000000];

	timeCString = First @ RepeatedTiming[Do[RoundTripCString[largeString], 5];];
	Print["C-string time: " <> ToString[timeCString]];

	timeString = First @ RepeatedTiming[Do[RoundTripString[largeString], 5];];
	Print["std::string time: " <> ToString[timeString]];
	
	Clear[largeString];
	
	timeString >= timeCString (* Not a very reliable unit test, don't worry too much if it fails *)
	,
	True
	,
	TestID->"StringTestSuite-20180821-Y0C3Q1"
]

Test[
	Get[FileNameJoin[{currentDirectory, "StringLength", "StringLength.wl"}]];
	StrLength["this is my null terminated string"]
	,
	33
	,
	TestID->"StringOperations-20150813-B8G3E7"
]
