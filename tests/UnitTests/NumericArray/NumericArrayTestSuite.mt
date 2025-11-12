(* Wolfram Language Test file *)

CatchAll @ TestExecute[
	currentDirectory = DirectoryName[$TestFileName];
	Get[FileNameJoin[{ParentDirectory[currentDirectory], "TestConfig.wl"}]];
	sourceDirectory = FileNameJoin[{currentDirectory, "TestSources"}];
	Get[FileNameJoin[{sourceDirectory, "NumericArrayOperations.wl"}]];
	na = NumericArray[{1, 2, 3, 4}];
	
	Off[General::stop];
];

(****************************NumericArray Operations****************************************)

TestCreate[
	echoNumericArrays[na, na, na]
	,
	Developer`DataStore @@ Table[NumericArray[{1, 2, 3, 4}, "UnsignedInteger8"], 3]
	,
	TestID -> "NumericArrayTestSuite-20190910-D6E1E5"
];

(*Test[
	num = NumericArray[N @ Range[0, 47] / 47, "Real64"];
	echoNumericArrays[{{}, {}}, na, num]
	,
	Developer`DataStore[{{}, {}}, na, num]
	,
	TestID -> "NumericArrayTestSuite-20190910-N9N5N6"
]*)

TestCreate[
	emptyVector[]
	,
	$Failed
	,
	Message[LibraryFunction::nanull, HoldCompleteForm @ NumericArray]
	,
	TestID -> "NumericArrayTestSuite-20190910-L4P0L7"
];

TestCreate[
	Dimensions @ emptyMatrix[]
	,
	{}
	,
	Message[LibraryFunction::nanull, HoldCompleteForm @ NumericArray]
	,
	TestID -> "NumericArrayTestSuite-20190910-C1R3B0"
];

TestMatch[
	testDimensions[{}]
	,
	Failure["NumericArrayNewError", <|
		"MessageTemplate" -> "Failed to create a new NumericArray.",
		"MessageParameters" -> <||>,
		"ErrorCode" -> _?CppErrorCodeQ,
		"Parameters" -> {}|>
	]
	,
	TestID -> "NumericArrayTestSuite-20190910-S4B6X0"
];

TestMatch[
	Normal @* testDimensions /@ {{0}, {3}, {3, 0}, {3, 2}, {3, 2, 0}, {3, 2, 4}}
	,
	{
		$Failed,
		{0., 0., 0.},
		$Failed,
		{{0., 0.}, {0., 0.}, {0., 0.}},
		$Failed,
		{{{0., 0., 0., 0.}, {0., 0., 0., 0.}}, {{0., 0., 0., 0.}, {0., 0., 0., 0.}}, {{0., 0., 0., 0.}, {0., 0., 0., 0.}}}
	}
	,
	{Message[LibraryFunction::nanull, HoldCompleteForm @ NumericArray]..}
	,
	TestID -> "NumericArrayTestSuite-20190910-N6W9L8"
];

TestCreate[
	Normal /@ List @@ testDimensions2[]
	,
	{
		{},
		{0., 0., 0.},
		{{}, {}, {}},
		{{0., 0.}, {0., 0.}, {0., 0.}},
		{{{}, {}}, {{}, {}}, {{}, {}}},
		{{{0., 0., 0., 0.}, {0., 0., 0., 0.}}, {{0., 0., 0., 0.}, {0., 0., 0., 0.}}, {{0., 0., 0., 0.}, {0., 0., 0., 0.}}}
	}
	,
	TestID -> "NumericArrayTestSuite-20190910-E1T0G1"
];

TestCreate[
	getNALength[na]
	,
	4
	,
	TestID -> "NumericArrayTestSuite-20190910-I0L8K4"
];

TestCreate[
	getNARank[na]
	,
	1
	,
	TestID -> "NumericArrayTestSuite-20190910-G8Y0D3"
];

TestCreate[
	NumericArrayQ @ newNA[]
	,
	True
	,
	TestID -> "NumericArrayTestSuite-20190910-E2L8V6"
];

(*Test[
	cloneNA[NumericArray[{}, "UnsignedInteger8"], {{}, {}}, na]
	,
	Developer`DataStore[{}, {{}, {}}, na]
	,
	TestID -> "NumericArrayTestSuite-20190910-Z1A9U0"
]*)

TestCreate[(*check NumericArray shared APi's*)
	changeSharedNA[na];
	sna = getSharedNA[];
	SameQ[sna, na]
	,
	True
	,
	TestID -> "NumericArrayTestSuite-20190910-R4C9R1"
];

TestCreate[
	accumulateIntegers[NumericArray[Range[10]]]
	,
	Total @ Range[10]
	,
	TestID -> "NumericArrayTestSuite-20190910-P0Q4I7"
];

TestCreate[
	accumulateIntegers[NumericArray[{3.5}]]
	,
	Failure["FunctionError", <|"MessageTemplate" -> "An error occurred in the library function.", "MessageParameters" -> <||>, "ErrorCode" -> 6, "Parameters" -> {}|>]
	,
	TestID -> "NumericArrayTestSuite-20190910-L8X7U3"
];

TestCreate[
	convertMethodName /@ Range[8]
	,
	{"Check", "ClipCheck", "Coerce", "ClipCoerce", "Round", "ClipRound", "Scale", "ClipScale"}
	,
	TestID -> "NumericArrayTestSuite-20190910-Y7M9C9"
];

TestMatch[
	convertMethodName[9]
	,
	_Failure
	,
	TestID -> "NumericArrayTestSuite-20190910-S9G5N5"
]

TestCreate[
	na = NumericArray[{3.5}];
	convert[na, 5 (* Round *), 0]
	,
	NumericArray[NumericArray[{3.5}], "UnsignedInteger16", "Round", Tolerance -> 0]
	,
	TestID -> "NumericArrayTestSuite-20190910-J2Y2X5"
];

TestMatch[
	convert[NumericArray[{3.5}], 1 (* Check *), 0]
	,
	Failure["NumericArrayConversionError", <|
		"MessageTemplate" -> "Failed to convert NumericArray from different type.",
		"MessageParameters" -> <||>,
		"ErrorCode" -> _?CppErrorCodeQ,
		"Parameters" -> _?ListQ|>
	]
	,
	TestID -> "NumericArrayTestSuite-20190910-D8W3R2"
];

TestCreate[
	convert[NumericArray[Range[10]], 8 (* ClipAndScale *), 1]
	,
	NumericArray[NumericArray[Range[10]], "UnsignedInteger16", "ClipAndScale", Tolerance -> 1]
	,
	TestID -> "NumericArrayTestSuite-20190910-P4Z3Z8"
];

TestCreate[
	na = NumericArray[{3.5}];
	convertGeneric[na, 5, 0]
	,
	NumericArray[NumericArray[{3.5}], "UnsignedInteger16", "Round", Tolerance -> 0]
	,
	TestID -> "NumericArrayTestSuite-20190910-Q6K1B9"
];

TestCreate[
	convertGeneric[NumericArray[Range[10]], 8, 1]
	,
	NumericArray[NumericArray[Range[10]], "UnsignedInteger16", "ClipAndScale", Tolerance -> 1]
	,
	TestID -> "NumericArrayTestSuite-20190910-D3E3K8"
];

(*Test[
	FlattenThroughList[NumericArray[{{}, {}}, "Integer32"]]
	,
	{}
	,

	TestID -> "NumericArrayTestSuite-20190910-P4K7R7"
];*)

TestCreate[
	FlattenThroughList[NumericArray[{{1, 2}, {3, 4}}, "Integer32"]]
	,
	NumericArray[{1, 2, 3, 4}, "Integer32"]
	,
	TestID -> "NumericArrayTestSuite-20190910-W1P5E0"
];

(*Test[
	CopyThroughTensor[NumericArray[{{}, {}}, "Real64"]]
	,
	{{}, {}}
	,
	TestID -> "NumericArrayTestSuite-20190910-G1X3J3"
];*)

TestCreate[
	CopyThroughTensor[NumericArray[{{1.9, 2.8}, {3.7, 4.6}}, "Real64"]]
	,
	NumericArray[{{1.9, 2.8}, {3.7, 4.6}}, "Real64"]
	,
	TestID -> "NumericArrayTestSuite-20190910-I3D6Q7"
];

TestCreate[
	GetLargest[
		NumericArray[RandomInteger[1000, {100, 100}], "UnsignedInteger16"],
		NumericArray[RandomInteger[1000, {200, 100}], "Integer32"],
		NumericArray[RandomReal[1., {200, 99}], "Real64"]
	]
	,
	1
	,
	TestID -> "NumericArrayTestSuite-20191127-E8Q1B9"
];

TestCreate[
	EmptyView[]
	,
	{-1, -1, 0, 0}
	,
	TestID -> "NumericArrayTestSuite-20191127-H2I2Z7"
];

TestCreate[
	SumLargestDimensions[
		NumericArray[RandomInteger[1000, {100, 50}], "UnsignedInteger16"],
		NumericArray[RandomInteger[1000, {200, 100}], "Integer32"]
	]
	,
	300
	,
	TestID -> "NumericArrayTestSuite-20191127-V8L4I2"
];


TestCreate[
	ReverseNA[NumericArray[Range[100], "UnsignedInteger16"]]
	,
	NumericArray[Reverse @ Range[100], "UnsignedInteger16"]
	,
	TestID -> "NumericArrayTestSuite-20191129-A6J2D9"
];

TestCreate[
	ReverseNA[NumericArray[{{1.9, 2.8}, {3.7, 4.6}}, "Real64"]]
	,
	NumericArray[{{4.6, 3.7}, {2.8, 1.9}}, "Real64"]
	,
	TestID -> "NumericArrayTestSuite-20191129-Y2C7M0"
];

(**************************** ByteArray tests ****************************************)

TestCreate[
	ReverseNA[ByteArray @ Range[100]] (* ByteArray can be passed where NumericArray is expected *)
	,
	NumericArray[Reverse @ Range[100], "UnsignedInteger8"]
	,
	TestID -> "NumericArrayTestSuite-20220928-A6JF9V"
];

TestCreate[
	ReverseBA[ByteArray @ Range[100]]
	,
	ByteArray[Reverse @ Range[100]]
	,
	TestID -> "NumericArrayTestSuite-20220928-G63DKU"
];

TestCreate[
	ReverseBA[NumericArray[{{2,3},{4,5}}, "UnsignedInteger8"]]
	,
	$Failed
	,
	Message[LibraryFunction::nanull, HoldCompleteForm @ ByteArray]
	,
	TestID -> "NumericArrayTestSuite-20220928-BP4D5F"
];