(* Wolfram Language Test file *)
TestRequirement[$VersionNumber > 10.3]
(***************************************************************************************************************************************)
				(*
					Set of test cases to test LLU functionality related to MathLink
				*)
(***************************************************************************************************************************************)
TestExecute[
	Needs["CCompilerDriver`"];
	currentDirectory = DirectoryName[$CurrentFile];
	
	(* Get configuration (path to LLU sources, compilation options, etc.) *)
	Get[FileNameJoin[{ParentDirectory[currentDirectory], "TestConfig.wl"}]];

	(* Compile the test library *)
	lib = CCompilerDriver`CreateLibrary[{FileNameJoin[{currentDirectory, "MLTest.cpp"}]} ~Join~ $LLUSources, "MLTest", options];
	
	Get[FileNameJoin[{ParentDirectory[currentDirectory, 3], "LibraryLinkUtilities.wl"}]];
	
	InitLibraryLinkUtils[lib];
	
	RegisterPacletErrors[lib, <||>];
	
	i8Range = {0, 255};
	i16Range = {-2^15, 2^15-1};
	i32Range = {-2^31, 2^31-1};
	i64Range = {-2^63, 2^63-1};
]


(* Scalars *)
Test[
	SameInts = SafeMathLinkFunction["SameInts"];
	SameInts[0, -1, -1, -1] (* Integer8 is actually UnsignedInteger8 in MathLink, so send 0 insted of -1. Other integer types are signed *)
	,
	{0, -1, -1, -1}
	,
	TestID->"MathLinkTestSuite-20171129-U4Q3L8"
]

Test[
	SameInts[2^7-1, 2^15-1, 2^31-1, 2^63-1]
	,
	{2^7-1, 2^15-1, 2^31-1, 2^63-1}
	,
	TestID->"MathLinkTestSuite-20171201-Z2N4U1"
]

Test[
	MaxInts = SafeMathLinkFunction["MaxInts"];
	MaxInts[RandomInteger[i8Range], RandomInteger[i16Range], RandomInteger[i32Range], RandomInteger[i64Range]]
	,
	{2^8-1, 2^15-1, 2^31-1, 2^63-1}
	,
	TestID->"MathLinkTestSuite-20171201-O7D3B0"
]

Test[
	SameFloats = SafeMathLinkFunction["SameFloats"];
	{pi, e} = SameFloats[N[Pi], N[E]];
	(Abs[Pi-pi] < 10^-4) && (E == e)
	,
	True
	,
	TestID->"MathLinkTestSuite-20171201-X6F6R7"
]

Test[
	BoolAnd = SafeMathLinkFunction["BoolAnd"];
	BoolAnd[True, True, True, False]
	,
	False
	,
	TestID->"MathLinkTestSuite-20171201-F3S0Q7"
]

Test[
	BoolAnd[True, True, True, True, True, True]
	,
	True
	,
	TestID->"MathLinkTestSuite-20171201-P0O0A8"
]

Test[
	BoolAnd[True, True, True, True, Pi]
	,
	Failure["MLWrongSymbolForBool", <|
		"MessageTemplate" -> "Tried to read something else than \"True\" or \"False\" as boolean.", 
		"MessageParameters" -> <||>, 
		"ErrorCode" -> -505, 
		"Parameters" -> {}
	|>]
	,
	TestID->"MathLinkTestSuite-20171201-L1W7O4"
]

(* Lists *)
Test[
	GetReversed = SafeMathLinkFunction["GetReversed8"];
	GetReversed[m = RandomInteger[i8Range, 100000]]
	,
	Reverse[m]
	,
	TestID->"MathLinkTestSuite-20171201-T0E6W4"
]

Test[
	GetReversed = SafeMathLinkFunction["GetReversed16"];
	GetReversed[m = RandomInteger[i16Range, 10000]]
	,
	Reverse[m]
	,
	TestID->"MathLinkTestSuite-20171205-H1L8Z6"
]

Test[
	GetReversed = SafeMathLinkFunction["GetReversed32"];
	GetReversed[m = RandomInteger[i32Range, 1000]]
	,
	Reverse[m]
	,
	TestID->"MathLinkTestSuite-20171205-V3M5Z6"
]

Test[
	GetReversed = SafeMathLinkFunction["GetReversed64"];
	GetReversed[m = RandomInteger[i64Range, 100]]
	,
	Reverse[m]
	,
	TestID->"MathLinkTestSuite-20171205-L4B1P5"
]

Test[
	GetReversed = SafeMathLinkFunction["GetReversedDouble"];
	GetReversed[m = RandomReal[1., 100]]
	,
	Reverse[m]
	,
	TestID->"MathLinkTestSuite-20171205-I4G4U8"
]

Test[
	GetFloatList = SafeMathLinkFunction["GetFloatList"];
	f = GetFloatList[r = RandomReal[1., 100]];
	Max[Abs[f - r]] < 10^-4
	,
	True
	,
	TestID->"MathLinkTestSuite-20171205-D5X3H2"
]

(* Arrays *)

Test[
	GetSame = SafeMathLinkFunction["GetSame8"];
	Reshape = SafeMathLinkFunction["Reshape8"];
	s = GetSame[m = RandomInteger[i8Range, {10, 10, 10, 20, 5}]];
	r = Reshape[m];
	ArrayReshape[s, {10, 10, 10, 5, 20}]
	,
	r
	,
	TestID->"MathLinkTestSuite-20171205-K0X1L2"
]

Test[
	GetSame = SafeMathLinkFunction["GetSame16"];
	Reshape = SafeMathLinkFunction["Reshape16"];
	s = GetSame[m = RandomInteger[i16Range, {10, 10, 20, 5}]];
	r = Reshape[m];
	ArrayReshape[s, {10, 10, 5, 20}]
	,
	r
	,
	TestID->"MathLinkTestSuite-20171205-V8W4L1"
]

Test[
	GetSame = SafeMathLinkFunction["GetSame32"];
	Reshape = SafeMathLinkFunction["Reshape32"];
	s = GetSame[m = RandomInteger[i32Range, {10, 20, 5}]];
	r = Reshape[m];
	ArrayReshape[s, {10, 5, 20}]
	,
	r
	,
	TestID->"MathLinkTestSuite-20171205-X6T1G7"
]

Test[
	GetSame = SafeMathLinkFunction["GetSame64"];
	Reshape = SafeMathLinkFunction["Reshape64"];
	s = GetSame[m = RandomInteger[i64Range, {20, 5}]];
	r = Reshape[m];
	ArrayReshape[s, {5, 20}]
	,
	r
	,
	TestID->"MathLinkTestSuite-20171205-K5D5R0"
]

Test[
	GetSame = SafeMathLinkFunction["GetSameDouble"];
	Reshape = SafeMathLinkFunction["ReshapeDouble"];
	s = GetSame[m = RandomReal[1., {20, 5}]];
	r = Reshape[m];
	ArrayReshape[s, {5, 20}]
	,
	r
	,
	TestID->"MathLinkTestSuite-20171205-Z8T6P5"
]

Test[
	ToList = SafeMathLinkFunction["ComplexToList"];
	c = RandomComplex[1 + I, {7, 8, 9}];
	ToList[c]
	,
	c /. Complex[x_, y_] -> {x, y}
	,
	TestID->"MathLinkTestSuite-20171205-W6B3U7"
]

Test[ (* Test if releasing memory works, if not the memory usage should drastically increase after this test *)
	ReceiveAndFreeArray = SafeMathLinkFunction["ReceiveAndFreeArray"];
	r = RandomReal[1., {1000, 1000, 100}];
	Do[ReceiveAndFreeArray[r], 500];
	Clear[r];
	,
	Null
	,
	TestID->"MathLinkTestSuite-20171205-D4D6S4"
]

(* Strings *)
Test[
	testString = FromCharacterCode[{97, 322, 945, 63488, 63264}];
	RepeatString = SafeMathLinkFunction["RepeatString"];
	RepeatString[testString]
	,
	StringRepeat[testString, 2]
	,
	TestID->"MathLinkTestSuite-20171205-C3X0I2"
]

Test[
	RepeatUTF8 = SafeMathLinkFunction["RepeatUTF8"];
	RepeatUTF8[testString]
	,
	StringRepeat[testString, 2]
	,
	TestID->"MathLinkTestSuite-20171205-F0A7B0"
]

Test[
	RepeatUTF16 = SafeMathLinkFunction["RepeatUTF16"];
	RepeatUTF16[testString]
	,
	StringRepeat[testString, 2]
	,
	TestID->"MathLinkTestSuite-20171205-M2B7E4"
]

Test[
	RepeatUTF32 = SafeMathLinkFunction["RepeatUTF32"];
	RepeatUTF32[testString]
	,
	StringRepeat[testString, 2]
	,
	TestID->"MathLinkTestSuite-20171205-S9R5Q1"
]

Test[ (* Test if releasing strings works, if not the memory usage should drastically increase after this test *)
	ReceiveAndFreeString = SafeMathLinkFunction["ReceiveAndFreeString"];
	s = StringJoin[RandomChoice[CharacterRange["A", "z"], 10000]];
	Do[ReceiveAndFreeString[s], 500]
	,
	Null
	,
	TestID->"MathLinkTestSuite-20171205-T6V1J3"
]

(* Symbols and Arbitrary Functions *)
Test[
	GetList = SafeMathLinkFunction["GetList"];
	GetList[]
	,
	{{1, 2, 3}, {1.5, 2.5, 3.5}, "Hello!"}
	,
	TestID->"MathLinkTestSuite-20171205-A4D8U2"
]

Test[
	ReverseSymbolsOrder = LibraryFunctionLoad[lib, "ReverseSymbolsOrder", LinkObject, LinkObject];
	ReverseSymbolsOrder[Pi, E, GoldenRatio, x] (* Things like I or Infinity are not symbols *)
	, 
	{x, GoldenRatio, E, Pi}
	,
	TestID->"MathLinkTestSuite-20171214-Q8O4B9"
]

Test[
	TakeLibraryFunction = LibraryFunctionLoad[lib, "TakeLibraryFunction", LinkObject, LinkObject];
	TakeLibraryFunction @ TakeLibraryFunction
	,
	ReverseSymbolsOrder
	,
	TestID->"MathLinkTestSuite-20171214-N1Z1H6"
]

Test[
	TakeLibraryFunction = LibraryFunctionLoad[lib, "TakeLibraryFunction", LinkObject, LinkObject];
	(TakeLibraryFunction @ TakeLibraryFunction)[Pi, E, GoldenRatio, x]
	,
	{x, GoldenRatio, E, Pi}
	,
	TestID->"MathLinkTestSuite-20171214-K6Z5T3"
]

(* Associations/Maps *)
Test[
	ReadNestedMap = SafeMathLinkFunction["ReadNestedMap"];
	r = RandomReal[{-Pi, Pi}, 10];
	Sort @ ReadNestedMap[<|
		"Multiply" -> <|3 -> r, 0 -> r, -3 -> r|>,
		"DoNothing" -> <|3 -> r, 0 -> r|>,
		"Negate" -> <|3 -> r, 0 -> r|>,
		"Add" -> <|-5 -> r|>
	|>]
	,
	Sort @ <|
		"Multiply" -> <|-3 -> -3r, 0 -> 0r, 3 -> 3r|>,
		"DoNothing" -> <|0 -> r, 3 -> r|>,
		"Negate" -> <|0 -> -r, 3 -> -r|>,
		"Add" -> <|-5 -> r - 5|>
	|>
	,
	TestID->"MathLinkTestSuite-20171214-F6N1C7"
]

(* Local Loopback Link *)
(* not implemented yet *)