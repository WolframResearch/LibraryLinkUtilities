(* Wolfram Language Test file *)
TestRequirement[$VersionNumber >= 12.0]
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
	lib = CCompilerDriver`CreateLibrary[FileNameJoin[{currentDirectory, #}]& /@ {"DataListTest.cpp"}, "DataListTest", options];
	
	Get[FileNameJoin[{$LLUSharedDir, "LibraryLinkUtilities.wl"}]];
	
	RegisterPacletErrors[lib, <||>];

	PassDataStore = SafeLibraryFunction["PassDataStore", {"DataStore", "Boolean"}, "DataStore"];
	JoinDataStores = SafeLibraryFunction["JoinDataStores", {"DataStore", "DataStore", "Boolean"}, "DataStore"];
	TestSelfReferencialDataStore = SafeLibraryFunction["TestSelfReferencialDataStore", {"DataStore"}, "DataStore"];
	ReverseListOfStringsMathLink = SafeMathLinkFunction["ReverseListOfStringsMathLink"];
	ReverseListOfStringsLibraryLink = SafeLibraryFunction["ReverseListOfStringsLibraryLink", {"DataStore"}, "DataStore"];
	ReverseListOfStrings = SafeLibraryFunction["ReverseListOfStrings", {"DataStore"}, "DataStore"];
	SeparateKeysAndValues = SafeLibraryFunction["SeparateKeysAndValues", {"DataStore"}, "DataStore"];
	GetKeys = SafeLibraryFunction["GetKeys", {"DataStore"}, "DataStore"];
	GetValuesReversed = SafeLibraryFunction["GetValuesReversed", {"DataStore"}, "DataStore"];
	FrameDims = SafeLibraryFunction["FrameDims", {"DataStore"}, NumericArray];
	StringsThroughVectorReversed = SafeLibraryFunction["StringsThroughVectorReversed", {"DataStore"}, "DataStore"];
	IntsToNumericArray = SafeLibraryFunction["IntsToNumericArray", {"DataStore"}, NumericArray];

	(* Test data used across multiple tests *)
	bool = True;
	int = 23456;
	real = -2.7;
	complex = 2.2 - 3.5 I;
	string = "Test string";
	tensor = RandomReal[1, {3, 2}];
	numeric = NumericArray[RandomInteger[{0, 255}, {2, 3}], "UnsignedInteger8"];
	image = RandomImage[1, {2, 3}, ColorSpace -> "CMYK"];
	(*sparse = SparseArray[{{1, 1} -> 1, {2, 2} -> 2, {3, 3} -> 3, {1, 3} -> 4}];*)
	ds0 = Developer`DataStore[bool, int, real, complex, string, tensor, (*numeric,*) image(*, sparse*)];
	ds1 = Developer`DataStore[bool, int, real, complex, string, tensor, (*numeric,*) image, (*sparse,*) ds0];
	ds2 = Developer`DataStore @@ Thread[Take[Alphabet[], Length[ds1]] -> List @@ ds1];
	ds3 = ArrayReshape[RandomWord[10], {2, 5}] /. List -> Developer`DataStore;
];


(* Compile-time errors *)
Test[
	CCompilerDriver`CreateLibrary[{FileNameJoin[{currentDirectory, "DataListCompilationErrors.cpp"}]}, "DataListErrors", options]
	,
	$Failed
	,
	{CreateLibrary::cmperr..} (* On Linux there should be 6 errors, but MSVC does not like generic lambdas so it spits out more errors *)
	,
	TestID->"DataListTestSuite-20180903-Y8Z5P1"
];

(* Basic tests *)

Test[
	EmptyDataStore = SafeLibraryFunction["EmptyDataStore", {}, "DataStore"];
	EmptyDataStore[]
	,
	Developer`DataStore[]
	,
	TestID->"DataListTestSuite-20180907-E5N8N9"
];

Test[
	PassDataStore[foo, True]
	,
	LibraryFunction[lib, "PassDataStore", {"DataStore", True|False}, "DataStore"][foo, True]
	,
	{Message[LibraryFunction::cfsa, foo, 1, "DataStore object"]}
	,
	TestID->"DataListTestSuite-20180903-O2B2Y6"
];

Test[
	PassDataStore[{}, True]
	,
	LibraryFunction[lib, "PassDataStore", {"DataStore", True|False}, "DataStore"][{}, True]
	,
	{Message[LibraryFunction::cfsa, {}, 1, "DataStore object"]}
	,
	TestID->"DataListTestSuite-20180903-B6P7O2"
];

Test[
	PassDataStore[Developer`DataStore[{1, "a"}], True]
	,
	LibraryFunction[lib, "PassDataStore", {"DataStore", True|False}, "DataStore"][Developer`DataStore[{1, "a"}], True]
	,
	{Message[LibraryFunction::cfsa, Developer`DataStore[{1, "a"}], 1, "DataStore object"]}
	,
	TestID->"DataListTestSuite-20180903-M6K4Z6"
];

Test[
	PassDataStore[Developer`DataStore[{1, 2, 3}], True]
	,
	Developer`DataStore[{1, 2, 3}]
	,
	TestID->"DataListTestSuite-20180903-Y4V3P7"
];

Test[
	PassDataStore[Developer`DataStore[1, 2, 3], True]
	,
	Developer`DataStore[1, 2, 3]
	,
	TestID->"DataListTestSuite-20180903-S2C1I6"
];

Test[
	PassDataStore[Developer`DataStore[1, 2, "a"], True]
	,
	Developer`DataStore[1, 2, "a"]
	,
	TestID->"DataListTestSuite-20180903-Y5Y7L3"
];

Test[
	PassDataStore[Developer`DataStore[Developer`DataStore[1, 2, 3]],True]
	,
	Developer`DataStore[Developer`DataStore[1, 2, 3]]
	,
	TestID->"DataListTestSuite-20180903-N9I8U3"
];

Test[
	PassDataStore[Developer`DataStore["ąę"->"ąę"],True]
	,
	Developer`DataStore["ąę"->"ąę"]
	,
	TestID->"DataListTestSuite-20180903-E8X1M1"
];

Test[
	PassDataStore[Developer`DataStore["" -> { -3.14 }],True]
	,
	Developer`DataStore[{ -3.14 }]
	,
	TestID->"DataListTestSuite-20180903-T8C7S4"
];

Test[
	PassDataStore[Developer`DataStore[], #]& /@ { True, False }
	,
	{ Developer`DataStore[], Developer`DataStore[] }
	,
	TestID->"DataListTestSuite-20180903-X7K0J0"
];

Test[
	PassDataStore[ds0, #]& /@ { True, False }
	,
	{ ds0, ds0 }
	,
	TestID->"DataListTestSuite-20180903-K0Y6L5"
];

Test[
	PassDataStore[ds1, #]& /@ { True, False }
	,
	{ ds1, ds1 }
	,
	TestID->"DataListTestSuite-20180903-T7W2E4"
];

Test[
	PassDataStore[ds2, #]& /@ { True, False }
	,
	{ ds2, ds2 }
	,
	TestID->"DataListTestSuite-20180903-Y3R9B8"
];

Test[
	PassDataStore[ds3, #]& /@ { True, False }
	,
	{ ds3, ds3 }
	,
	TestID->"DataListTestSuite-20180903-N7Z2V6"
];

Test[
	JoinDataStores[Developer`DataStore[], Developer`DataStore[], #] & /@ {True, False}
	,
	{Developer`DataStore[Developer`DataStore[], Developer`DataStore[]], Developer`DataStore[Developer`DataStore[], Developer`DataStore[]]}
	,
	TestID->"DataListTestSuite-20180904-Z4Q5M1"
];

Test[
	JoinDataStores[ds0, Reverse[ds0], #]& /@ {True, False}
	,
	{Developer`DataStore[ds0, Reverse[ds0]], Developer`DataStore[ds0, Reverse[ds0]]}
	,
	TestID->"DataListTestSuite-20180904-N5U8F0"
];

Test[
	JoinDataStores[ds1, Reverse[ds1], #]& /@ {True, False}
	,
	{Developer`DataStore[ds1, Reverse[ds1]], Developer`DataStore[ds1, Reverse[ds1]]}
	,
	TestID->"DataListTestSuite-20180904-I2U6H7"
];

Test[
	JoinDataStores[ds2, Reverse[ds2], #]& /@ {True, False}
	,
	{Developer`DataStore[ds2, Reverse[ds2]], Developer`DataStore[ds2, Reverse[ds2]]}
	,
	TestID->"DataListTestSuite-20180904-J7W2N2"
];

Test[
	JoinDataStores[ds3, Reverse[ds3], #]& /@ {True, False}
	,
	{Developer`DataStore[ds3, Reverse[ds3]], Developer`DataStore[ds3, Reverse[ds3]]}
	,
	TestID->"DataListTestSuite-20180904-U9J2V6"
];

(* Self referential DataStore *)
Test[
	TestSelfReferencialDataStore[ds0]
	,
	Developer`DataStore[Sequence @@ ds0, Null]
	,
	{Message[LibraryFunction::dsself]}
	,
	TestID->"DataListTestSuite-20180906-R1F1D5"
];

Test[
	TestSelfReferencialDataStore[ds1]
	,
	Developer`DataStore[Sequence @@ ds1, Null]
	,
	{Message[LibraryFunction::dsself]}
	,
	TestID->"DataListTestSuite-20180906-E3A3S7"
];

Test[
	TestSelfReferencialDataStore[ds2]
	,
	Developer`DataStore[Sequence @@ ds2, Null]
	,
	{Message[LibraryFunction::dsself]}
	,
	TestID->"DataListTestSuite-20180906-X6P9H5"
];

Test[
	TestSelfReferencialDataStore[ds3]
	,
	Developer`DataStore[Sequence @@ ds3, Null]
	,
	{Message[LibraryFunction::dsself]}
	,
	TestID->"DataListTestSuite-20180906-A6C8C9"
];


TestMatch[
	ReverseListOfStrings[Developer`DataStore["aaaa", "b", 2]]
	,
	Failure["DLInvalidNodeType", <|
		"MessageTemplate" -> "DataStoreNode passed to Node wrapper carries data of invalid type", 
		"MessageParameters" -> <||>, 
		"ErrorCode" -> n_?IntegerQ, 
		"Parameters" -> {}
	|>]
	,
	TestID->"DataListTestSuite-20180906-Z7K0I0"
];

Test[
	ReverseListOfStrings[Developer`DataStore[]]
	,
	Developer`DataStore[]
	,
	TestID->"DataListTestSuite-20180907-X0G8X3"
];

Test[
	SeparateKeysAndValues[Developer`DataStore["a" -> 1 + 2.5*I, "b" -> -3. - 6.I, 2I]]
	,
	Developer`DataStore["Keys" -> Developer`DataStore["a", "b", ""], "Values" -> Developer`DataStore[1. + 2.5*I, -3. - 6.I, 2.I]]
	,
	TestID->"DataListTestSuite-20180907-U7I7O8"
];

Test[
	GetKeys[Developer`DataStore[]]
	,
	Developer`DataStore[]
	,
	TestID->"DataListTestSuite-20180908-S5C4D2"
];

Test[
	GetKeys[Developer`DataStore[1]]
	,
	Developer`DataStore[""]
	,
	TestID->"DataListTestSuite-20180908-J2I0I5"
];

Test[
	GetKeys[Developer`DataStore["x" -> 1, "x" -> 1, "y" -> RandomImage[]]]
	,
	Developer`DataStore["x", "x", "y"]
	,
	TestID->"DataListTestSuite-20180908-S5H9Z0"
];


Test[
	GetValuesReversed[Developer`DataStore[]]
	,
	Developer`DataStore[]
	,
	TestID->"DataListTestSuite-20180908-T1U8K8"
];

Test[
	GetValuesReversed[Developer`DataStore["a" -> 1 + 2.5*I, "b" -> -3. - 6.I, 2I]]
	,
	Developer`DataStore[2.I, -3. - 6.I, 1. + 2.5I]
	,
	TestID->"DataListTestSuite-20180908-S4T6Z9"
];

Test[
	GetValuesReversed[Developer`DataStore["a" -> 1, NumericArray[{3,5,7}]]]
	,
	Developer`DataStore[NumericArray[{3,5,7}, "UnsignedInteger8"], 1]
	,
	TestID->"DataListTestSuite-20180908-C7I2J6"
];

Test[
	FrameDims[Developer`DataStore[RandomImage[1., 100], RandomImage[1., 300], RandomImage[1., 200]]]
	,
	NumericArray[{{100, 100}, {300, 300}, {200, 200}}, "UnsignedInteger64"]
	,
	TestID->"DataListTestSuite-20180907-J9A6U6"
];

Test[
	StringsThroughVectorReversed[Developer`DataStore[]]
	,
	Developer`DataStore[]
	,
	TestID->"DataListTestSuite-20180910-I1U7A9"
];

Test[
	los = Developer`DataStore @@ RandomWord[30];
	StringsThroughVectorReversed[los]
	,
	Reverse[los]
	,
	TestID->"DataListTestSuite-20180910-Q5U3A6"
];

Test[
	IntsToNumericArray[Developer`DataStore[]]
	,
	{}
	,
	TestID->"DataListTestSuite-20180910-J1W7Z6"
];

Test[
	ints = RandomInteger[2^22, 100];
	IntsToNumericArray[Developer`DataStore @@ ints]
	,
	NumericArray[ints, If[Developer`$MaxMachineInteger > 2^32, "Integer64", "Integer32"]]
	,
	TestID->"DataListTestSuite-20180910-P0I5K7"
];

(* Timing tests *)
Test[
	los = RandomWord["CommonWords", 300000];
	ds = Developer`DataStore @@ los;
	{timeMathLink, r1} = RepeatedTiming[ReverseListOfStringsMathLink[los]];
	{timeDataStore, r2} = RepeatedTiming[ReverseListOfStringsLibraryLink[ds]];
	{timeDataList, r3} = RepeatedTiming[ReverseListOfStrings[ds]];
	Print["Time when sending list via MathLink: " <> ToString[timeMathLink] <> "s."];
	Print["Time when sending list via DataStore: " <> ToString[timeDataStore] <> "s."];
	Print["Time when sending list via DataList: " <> ToString[timeDataList] <> "s."];
	r1
	,
	List @@ r3
	,
	TestID->"DataListTestSuite-20180906-W5N4V0"
];



(* Memory leak tests *)

Test[
	MemoryLeakTest[PassDataStore[ds0, #]] & /@ {False, True}
	,
	{ 0, 0 }
	,
	TestID->"DataListTestSuite-20180904-J9J5U5"
];

Test[
	MemoryLeakTest[PassDataStore[ds1, #]] & /@ {False, True}
	,
	{ 0, 0 }
	,
	TestID->"DataListTestSuite-20180908-B6X4I7"
];

Test[
	MemoryLeakTest[PassDataStore[ds2, #]] & /@ {False, True}
	,
	{ 0, 0 }
	,
	TestID->"DataListTestSuite-20180908-S3Q1N7"
];

Test[
	MemoryLeakTest[PassDataStore[ds3, #]] & /@ {False, True}
	,
	{ 0, 0 }
	,
	TestID->"DataListTestSuite-20180908-S4C7Q2"
];

Test[
	LibraryFunctionUnload[Last @ PassDataStore];
	PassDataStore = SafeLibraryFunction["PassDataStore", {{"DataStore", "Manual"}, "Boolean"}, "DataStore"];
	MemoryLeakTest[PassDataStore[ds0, #]] & /@ {False, True}
	,
	{ 0, n_ }/; n > 0  (* when copy is made we expect a leak, because the C++ code is not aware of the "Manual" passing of the input DataStore *)
	,
	TestID->"DataListTestSuite-20180908-C4W5X3"
	,
	SameTest -> MatchQ
];

Test[
	MemoryLeakTest[PassDataStore[ds1, #]] & /@ {False, True}
	,
	{ 0, n_ }/; n > 0
	,
	TestID->"DataListTestSuite-20180908-R9U2G0"
	,
	SameTest -> MatchQ
];

Test[
	MemoryLeakTest[PassDataStore[ds2, #]] & /@ {False, True}
	,
	{ 0, n_ }/; n > 0
	,
	TestID->"DataListTestSuite-20180908-K7W7L1"
	,
	SameTest -> MatchQ
];

Test[
	MemoryLeakTest[PassDataStore[ds3, #]] & /@ {False, True}
	,
	{ 0, n_ }/; n > 0
	,
	TestID->"DataListTestSuite-20180908-Z1Y1Q5"
	,
	SameTest -> MatchQ
];