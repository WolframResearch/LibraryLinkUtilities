(* Wolfram Language Test file *)
TestRequirement[$VersionNumber > 12.0]
TestExecute[
	currentDirectory = DirectoryName[$CurrentFile];
	Get[FileNameJoin[{ParentDirectory[currentDirectory], "TestConfig.wl"}]];
	sourceDirectory = FileNameJoin[{currentDirectory, "TestSources"}];
	Get[FileNameJoin[{sourceDirectory, "NumericArrayOperations.wl"}]];
]

(****************************NumericArray Operations****************************************)
Test[
	na = NumericArray[{1, 2, 3, 4}];
	echoNumericArray[na]
	,
	NumericArray[{1, 2, 3, 4}, "UnsignedInteger8"]
	,
	TestID->"NumericArrayOperations-20150825-M7G1B2"
]

Test[
	num = NumericArray[N @ Range[0, 47]/47, "Real64"];
	res = NumericArrayQ @ echoNumericArray[num]
	,
	True
	,
	TestID->"NumericArrayOperations-20150825-P4U4W5"
]

ExactTest[
	getNALength[na]
	,
	4
	,
	TestID->"NumericArrayOperations-20150827-V6M5C7"
]

ExactTest[
	getNARank[na]
	,
	1
	,
	TestID->"NumericArrayOperations-20150827-A7Q6J1"
]

Test[
	NumericArrayQ @ newNA[]
	,
	True
	,
	TestID->"NumericArrayOperations-20150827-B3Y1C2"
]

Test[
	cl = cloneNA[na];
	SameQ[cl, na]
	,
	True
	,
	TestID->"NumericArrayOperations-20150827-I0C3X0"
]
	
Test[(*check NumericArray shared APi's*)	
	changeSharedNA[na];
	sna = getSharedNA[];
	SameQ[sna, na]
	,
	True
	,
	TestID->"NumericArrayOperations-20150827-O5S8C2"
]

Test[
	accumulateIntegers[NumericArray[Range[10]]]
	,
	Total @ Range[10]
	,
	TestID->"NumericArrayTestSuite-20181030-U5E5U6"
]

Test[
	accumulateIntegers[NumericArray[{3.5}]]
	,
	Failure["FunctionError", <|"MessageTemplate" -> "An error occurred in the library function.", "MessageParameters" -> <||>, "ErrorCode" -> 6, "Parameters" -> {}|>]
	,
	TestID->"NumericArrayTestSuite-20181030-P4G8W4"
]

Test[
	convertMethodName /@ Range[8]
	,
	{"Check", "ClipCheck", "Coerce", "ClipCoerce", "Round", "ClipRound", "Scale", "ClipScale"}
	,
	TestID->"NumericArrayTestSuite-20190328-S2I8Q2"
]

TestMatch[
	convertMethodName[9]
	,
	_Failure
	,
	TestID->"NumericArrayTestSuite-20190328-R8Y1F1"
]

Test[
	na = NumericArray[{3.5}];
	convert[na, 5 (* Round *), 0]
	,
	NumericArray[NumericArray[{3.5}], "UnsignedInteger16", "Round", Tolerance -> 0]
	,
	TestID->"NumericArrayTestSuite-20181105-I0C6A3"
]

TestMatch[
	convert[NumericArray[{3.5}], 1 (* Check *), 0]
	,
	Failure["NumericArrayConversionError", <|
		"MessageTemplate" -> "Failed to convert NumericArray from different type.", 
		"MessageParameters" -> <||>, 
		"ErrorCode" -> _?CppErrorCodeQ, 
		"Parameters" -> {}|>
	]
	,
	TestID->"NumericArrayTestSuite-20181105-P7M0S7"
]

Test[
	convert[NumericArray[Range[10]], 8 (* ClipAndScale *), 1]
	,
	NumericArray[NumericArray[Range[10]], "UnsignedInteger16", "ClipAndScale", Tolerance -> 1]
	,
	TestID->"NumericArrayTestSuite-20181105-W8Z5G6"
]

Test[
	na = NumericArray[{3.5}];
	convertGeneric[na, 5, 0]
	,
	NumericArray[NumericArray[{3.5}], "UnsignedInteger16", "Round", Tolerance -> 0]
	,
	TestID -> "NumericArrayTestSuite-20190619-X9O3E1"
]

Test[
	convertGeneric[NumericArray[Range[10]], 8, 1]
	,
	NumericArray[NumericArray[Range[10]], "UnsignedInteger16", "ClipAndScale", Tolerance -> 1]
	,
	TestID -> "NumericArrayTestSuite-20190619-O5L5G6"
]

EndRequirement[]