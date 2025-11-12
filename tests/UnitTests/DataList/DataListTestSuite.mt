(***************************************************************************************************************************************)
(*
	Set of test cases to test LLU functionality related to WSTP
*)
(***************************************************************************************************************************************)
TestExecute[
	Needs["CCompilerDriver`"];
	currentDirectory = DirectoryName[$TestFileName];

	(* Get configuration (path to LLU sources, compilation options, etc.) *)
	Get[FileNameJoin[{ParentDirectory[currentDirectory], "TestConfig.wl"}]];

	(* Compile the test library *)
	lib = CCompilerDriver`CreateLibrary[FileNameJoin[{currentDirectory, "TestSources", #}]& /@ {"DataListTest.cpp"}, "DataListTest", options];

	Get[FileNameJoin[{$LLUSharedDir, "LibraryLinkUtilities.wl"}]];

	`LLU`InitializePacletLibrary[lib];

	PassDataStore = `LLU`PacletFunctionLoad["PassDataStore", {"DataStore", "Boolean"}, "DataStore", "Throws" -> False];
	JoinDataStores = `LLU`PacletFunctionLoad["JoinDataStores", {"DataStore", "DataStore", "Boolean"}, "DataStore"];
	TestSelfReferencialDataStore = `LLU`PacletFunctionLoad["TestSelfReferencialDataStore", {"DataStore"}, "DataStore"];
	ReverseListOfStringsWSTP = `LLU`PacletFunctionLoad["ReverseListOfStringsWSTP", LinkObject, LinkObject];
	ReverseListOfStringsLibraryLink = `LLU`PacletFunctionLoad["ReverseListOfStringsLibraryLink", {"DataStore"}, "DataStore"];
	ReverseListOfStringsGeneric = `LLU`PacletFunctionLoad["ReverseListOfStringsGeneric", {"DataStore"}, "DataStore"];
	ReverseListOfStringsGenericIn = `LLU`PacletFunctionLoad["ReverseListOfStringsGenericIn", {"DataStore"}, "DataStore"];
	ReverseListOfStringsGenericOut = `LLU`PacletFunctionLoad["ReverseListOfStringsGenericOut", {"DataStore"}, "DataStore"];
	ReverseListOfStrings = `LLU`PacletFunctionLoad["ReverseListOfStrings", {"DataStore"}, "DataStore"];
	SeparateKeysAndValues = `LLU`PacletFunctionLoad["SeparateKeysAndValues", {"DataStore"}, "DataStore"];
	GetKeys = `LLU`PacletFunctionLoad["GetKeys", {"DataStore"}, "DataStore"];
	GetValuesReversed = `LLU`PacletFunctionLoad["GetValuesReversed", {"DataStore"}, "DataStore"];
	FrameDims = `LLU`PacletFunctionLoad["FrameDims", {"DataStore"}, NumericArray];
	StringsThroughVectorReversed = `LLU`PacletFunctionLoad["StringsThroughVectorReversed", {"DataStore"}, "DataStore"];
	IntsToNumericArray = `LLU`PacletFunctionLoad["IntsToNumericArray", {"DataStore"}, NumericArray];
	GetLength = `LLU`PacletFunctionLoad["GetLength", {"DataStore"}, Integer];
	CheckSizeChange = `LLU`PacletFunctionLoad["CheckSizeChange", {Integer}, {Integer, 1}];

	(* Test data used across multiple tests *)
	bool = True;
	int = 23456;
	real = -2.7;
	complex = 2.2 - 3.5 I;
	string = "Test string";
	tensor = RandomReal[1, {3, 2}];
	numeric = NumericArray[RandomInteger[{0, 255}, {2, 3}], "UnsignedInteger8"];
	image = RandomImage[1, {2, 3}, ColorSpace -> "CMYK"];
	sparse = SparseArray[{{1, 1} -> 1, {2, 2} -> 2, {3, 3} -> 3, {1, 3} -> 4}];
	ds0 = Developer`DataStore[bool, int, real, complex, tensor, sparse, numeric, image, string];
	ds1 = Developer`DataStore[bool, int, real, complex, tensor, sparse, numeric, image, string, ds0];
	ds2 = Developer`DataStore @@ Thread[Take[Alphabet[], Length[ds1]] -> List @@ ds1];
	ds3 = ArrayReshape[RandomWord[10], {2, 5}] /. List -> Developer`DataStore;
	
	Off[General::stop]
];


(* Compile-time errors *)
TestCreate[
	CCompilerDriver`CreateLibrary[{FileNameJoin[{currentDirectory, "TestSources", "DataListCompilationErrors.cpp"}]}, "DataListErrors", options]
	,
	$Failed
	,
	{Repeated[CreateLibrary::cmperr, {1,4}]} (* There should be between 1 and 4 compilation errors *)
	,
	TestID -> "DataListTestSuite-20180903-Y8Z5P1"
];

(* Basic tests *)

TestCreate[
	EmptyDataStore = `LLU`PacletFunctionLoad["EmptyDataStore", {}, "DataStore"];
	EmptyDataStore[]
	,
	Developer`DataStore[]
	,
	TestID -> "DataListTestSuite-20180907-E5N8N9"
];

TestMatch[
	Catch[PassDataStore[foo, True], "LLUExceptionTag"]
	,
	_?FailureQ(*unevaluated LibraryFunction*)
	,
	{Message[LibraryFunction::cfsa, HoldCompleteForm @ foo, HoldCompleteForm @ 1, HoldCompleteForm @ "DataStore object"]}
	,
	TestID -> "DataListTestSuite-20180903-O2B2Y6"
];

TestMatch[
	Catch[PassDataStore[{}, True], "LLUExceptionTag"]
	,
	_?FailureQ(*unevaluated LibraryFunction*)
	,
	{Message[LibraryFunction::cfsa, HoldCompleteForm @ {}, HoldCompleteForm @ 1, HoldCompleteForm @ "DataStore object"]}
	,
	TestID -> "DataListTestSuite-20180903-B6P7O2"
];

TestMatch[
	Catch[PassDataStore[Developer`DataStore[{1, "a"}], True], "LLUExceptionTag"]
	,
	_?FailureQ(*unevaluated LibraryFunction*)
	,
	{Message[LibraryFunction::cfsa, HoldCompleteForm @ Developer`DataStore[{1, "a"}], HoldCompleteForm @ 1, HoldCompleteForm @ "DataStore object"]}
	,
	TestID -> "DataListTestSuite-20180903-M6K4Z6"
];

TestCreate[
	PassDataStore[Developer`DataStore[{1, 2, 3}], True]
	,
	Developer`DataStore[{1, 2, 3}]
	,
	TestID -> "DataListTestSuite-20180903-Y4V3P7"
];

TestCreate[
	PassDataStore[Developer`DataStore[1, 2, 3], True]
	,
	Developer`DataStore[1, 2, 3]
	,
	TestID -> "DataListTestSuite-20180903-S2C1I6"
];

TestCreate[
	PassDataStore[Developer`DataStore[1, 2, "a"], True]
	,
	Developer`DataStore[1, 2, "a"]
	,
	TestID -> "DataListTestSuite-20180903-Y5Y7L3"
];

TestCreate[
	PassDataStore[Developer`DataStore[Developer`DataStore[1, 2, 3]], True]
	,
	Developer`DataStore[Developer`DataStore[1, 2, 3]]
	,
	TestID -> "DataListTestSuite-20180903-N9I8U3"
];

TestCreate[
	PassDataStore[Developer`DataStore["ąę" -> "ąę"], True]
	,
	Developer`DataStore["ąę" -> "ąę"]
	,
	TestID -> "DataListTestSuite-20180903-E8X1M1"
];

TestCreate[
	PassDataStore[Developer`DataStore["" -> { -3.14 }], True]
	,
	Developer`DataStore[{ -3.14 }]
	,
	TestID -> "DataListTestSuite-20180903-T8C7S4"
];

TestCreate[
	PassDataStore[Developer`DataStore[], #]& /@ { True, False }
	,
	{ Developer`DataStore[], Developer`DataStore[] }
	,
	TestID -> "DataListTestSuite-20180903-X7K0J0"
];

TestCreate[
	PassDataStore[ds0, #]& /@ { True, False }
	,
	{ ds0, ds0 }
	,
	TestID -> "DataListTestSuite-20180903-K0Y6L5"
];

TestCreate[
	PassDataStore[ds1, #]& /@ { True, False }
	,
	{ ds1, ds1 }
	,
	TestID -> "DataListTestSuite-20180903-T7W2E4"
];

TestCreate[
	PassDataStore[ds2, #]& /@ { True, False }
	,
	{ ds2, ds2 }
	,
	TestID -> "DataListTestSuite-20180903-Y3R9B8"
];

TestCreate[
	PassDataStore[ds3, #]& /@ { True, False }
	,
	{ ds3, ds3 }
	,
	TestID -> "DataListTestSuite-20180903-N7Z2V6"
];

TestCreate[
	JoinDataStores[Developer`DataStore[], Developer`DataStore[], #] & /@ {True, False}
	,
	{Developer`DataStore[Developer`DataStore[], Developer`DataStore[]], Developer`DataStore[Developer`DataStore[], Developer`DataStore[]]}
	,
	TestID -> "DataListTestSuite-20180904-Z4Q5M1"
];

TestCreate[
	JoinDataStores[ds0, Reverse[ds0], #]& /@ {True, False}
	,
	{Developer`DataStore[ds0, Reverse[ds0]], Developer`DataStore[ds0, Reverse[ds0]]}
	,
	TestID -> "DataListTestSuite-20180904-N5U8F0"
];

TestCreate[
	JoinDataStores[ds1, Reverse[ds1], #]& /@ {True, False}
	,
	{Developer`DataStore[ds1, Reverse[ds1]], Developer`DataStore[ds1, Reverse[ds1]]}
	,
	TestID -> "DataListTestSuite-20180904-I2U6H7"
];

TestCreate[
	JoinDataStores[ds2, Reverse[ds2], #]& /@ {True, False}
	,
	{Developer`DataStore[ds2, Reverse[ds2]], Developer`DataStore[ds2, Reverse[ds2]]}
	,
	TestID -> "DataListTestSuite-20180904-J7W2N2"
];

TestCreate[
	JoinDataStores[ds3, Reverse[ds3], #]& /@ {True, False}
	,
	{Developer`DataStore[ds3, Reverse[ds3]], Developer`DataStore[ds3, Reverse[ds3]]}
	,
	TestID -> "DataListTestSuite-20180904-U9J2V6"
];

(* Self referential DataStore *)
TestCreate[
	TestSelfReferencialDataStore[ds0]
	,
	Developer`DataStore[Sequence @@ ds0, Null]
	,
	{Message[LibraryFunction::dsself]}
	,
	TestID -> "DataListTestSuite-20180906-R1F1D5"
];

TestCreate[
	TestSelfReferencialDataStore[ds1]
	,
	Developer`DataStore[Sequence @@ ds1, Null]
	,
	{Message[LibraryFunction::dsself]}
	,
	TestID -> "DataListTestSuite-20180906-E3A3S7"
];

TestCreate[
	TestSelfReferencialDataStore[ds2]
	,
	Developer`DataStore[Sequence @@ ds2, Null]
	,
	{Message[LibraryFunction::dsself]}
	,
	TestID -> "DataListTestSuite-20180906-X6P9H5"
];

TestCreate[
	TestSelfReferencialDataStore[ds3]
	,
	Developer`DataStore[Sequence @@ ds3, Null]
	,
	{Message[LibraryFunction::dsself]}
	,
	TestID -> "DataListTestSuite-20180906-A6C8C9"
];


TestMatch[
	Catch[
		ReverseListOfStrings[Developer`DataStore["aaaa", "b", 2]],
		_String
	]
	,
	Failure["DLInvalidNodeType", <|
		"MessageTemplate" -> "DataStoreNode passed to Node wrapper carries data of invalid type",
		"MessageParameters" -> <||>,
		"ErrorCode" -> n_?IntegerQ,
		"Parameters" -> {}
	|>]
	,
	TestID -> "DataListTestSuite-20180906-Z7K0I0"
];

TestCreate[
	ReverseListOfStrings[Developer`DataStore[]]
	,
	Developer`DataStore[]
	,
	TestID -> "DataListTestSuite-20180907-X0G8X3"
];

TestCreate[
	ReverseListOfStrings[Developer`DataStore["alpha", "beta", "gamma"]]
	,
	Developer`DataStore["ahpla", "ateb", "ammag"]
	,
	TestID -> "DataListTestSuite-20200429-S3Y8K2"
];

TestCreate[
	SeparateKeysAndValues[Developer`DataStore["a" -> 1 + 2.5 * I, "b" -> -3. - 6.I, 2I]]
	,
	Developer`DataStore["Keys" -> Developer`DataStore["a", "b", ""], "Values" -> Developer`DataStore[1. + 2.5 * I, -3. - 6.I, 2.I]]
	,
	TestID -> "DataListTestSuite-20180907-U7I7O8"
];

TestCreate[
	`LLU`PacletFunctionSet[SeparateKeysAndValuesViaAdaptors, {"DataStore"}, "DataStore"];
	SeparateKeysAndValuesViaAdaptors[Developer`DataStore["a" -> 1 + 2.5 * I, "b" -> -3. - 6.I, 2I]]
	,
	Developer`DataStore["Keys" -> Developer`DataStore["a", "b", ""], "Values" -> Developer`DataStore[1. + 2.5 * I, -3. - 6.I, 2.I]]
	,
	TestID -> "DataListTestSuite-20200513-G3Y0I0"
];

TestCreate[
	GetKeys[Developer`DataStore[]]
	,
	Developer`DataStore[]
	,
	TestID -> "DataListTestSuite-20180908-S5C4D2"
];

TestCreate[
	GetKeys[Developer`DataStore[1]]
	,
	Developer`DataStore[""]
	,
	TestID -> "DataListTestSuite-20180908-J2I0I5"
];

TestCreate[
	GetKeys[Developer`DataStore["x" -> 1, "x" -> 1, "y" -> RandomImage[]]]
	,
	Developer`DataStore["x", "x", "y"]
	,
	TestID -> "DataListTestSuite-20180908-S5H9Z0"
];


TestCreate[
	GetValuesReversed[Developer`DataStore[]]
	,
	Developer`DataStore[]
	,
	TestID -> "DataListTestSuite-20180908-T1U8K8"
];

TestCreate[
	GetValuesReversed[Developer`DataStore["a" -> 1 + 2.5 * I, "b" -> -3. - 6.I, 2I]]
	,
	Developer`DataStore[2.I, -3. - 6.I, 1. + 2.5I]
	,
	TestID -> "DataListTestSuite-20180908-S4T6Z9"
];

TestCreate[
	GetValuesReversed[Developer`DataStore["a" -> 1, NumericArray[{3, 5, 7}]]]
	,
	Developer`DataStore[NumericArray[{3, 5, 7}, "UnsignedInteger8"], 1]
	,
	TestID -> "DataListTestSuite-20180908-C7I2J6"
];

TestCreate[
	GetValuesReversed[Developer`DataStore[{3}, {4}, {5}]]
	,
	Developer`DataStore[{5}, {4}, {3}]
	,
	TestID->"DataListTestSuite-20180908-C7DJ8D"
];

TestCreate[
	FrameDims[Developer`DataStore[RandomImage[1., 100], RandomImage[1., 300], RandomImage[1., 200]]]
	,
	NumericArray[{{100, 100}, {300, 300}, {200, 200}}, "UnsignedInteger64"]
	,
	TestID -> "DataListTestSuite-20180907-J9A6U6"
];

TestCreate[
	StringsThroughVectorReversed[Developer`DataStore[]]
	,
	Developer`DataStore[]
	,
	TestID -> "DataListTestSuite-20180910-I1U7A9"
];

TestCreate[
	los = Developer`DataStore @@ RandomWord[30];
	StringsThroughVectorReversed[los]
	,
	Reverse[los]
	,
	TestID -> "DataListTestSuite-20180910-Q5U3A6"
];

TestCreate[
	IntsToNumericArray[Developer`DataStore[]]
	,
	$Failed
	,
	{Message[LibraryFunction::nanull, HoldCompleteForm @ NumericArray]}
	,
	TestID -> "DataListTestSuite-20180910-J1W7Z6"
];

TestCreate[
	ints = RandomInteger[2^22, 100];
	IntsToNumericArray[Developer`DataStore @@ ints]
	,
	NumericArray[ints, If[Developer`$MaxMachineInteger > 2^32, "Integer64", "Integer32"]]
	,
	TestID -> "DataListTestSuite-20180910-P0I5K7"
];

TestCreate[
	GetLength[Developer`DataStore[]]
	,
	0
	,
	TestID -> "DataListTestSuite-20190823-P9W5H6"
];

TestCreate[
	GetLength[Developer`DataStore["x" -> 2, 3, 4, "y" -> Developer`DataStore[]]]
	,
	4
	,
	TestID -> "DataListTestSuite-20190823-L2D8R7"
];

TestCreate[
	CheckSizeChange[5]
	,
	{5, 5, 5}
	,
	TestID -> "DataListTestSuite-20200401-L5V4B8"
];

TestExecute[
	oldDS1 = ds1;
];

TestCreate[
	`LLU`PacletFunctionSet[PullAndPush, {"DataStore"}, "DataStore"];
	PullAndPush[ds1]
	,
	Developer`DataStore[
		bool, bool, "bool" -> bool, "mbool" -> bool, 
		int, "mint" -> int, 
		real, "mreal" -> real, 
		complex, complex, "complex" -> complex, "mcomplex" -> complex,
		tensor, "Tensor" -> tensor,
		sparse, "SparseArray" -> sparse,
		numeric, "NumericArray" -> numeric,
		image, "Image" -> image,
		string, string, "String" -> string, "RawString" -> string,
		ds0, "DataList" -> ds0
	]
	,
	TestID -> "DataListTestSuite-20200401-L5KJGB"
];

VerificationTest[
	oldDS1 == ds1
	,
	TestID->"DataListTestSuite-20200505-N8W6E1"
];

TestCreate[
	MemoryLeakTest[PullAndPush[ds1]]
	,
	0
	,
	TestID -> "DataListTestSuite-20180904-J9J5U5"
];

TestCreate[
	`LLU`PacletFunctionSet[PullAndPush2, {"DataStore"}, "DataStore"];
	PullAndPush2[ds1]
	,
	Developer`DataStore[
		bool, int, real, complex,
		tensor,
		sparse,
		numeric,
		image,
		string,
		ds0
	]
	,
	TestID -> "DataListTestSuite-20200505-L9U9K6"
];

VerificationTest[
	oldDS1 == ds1
	,
	TestID->"DataListTestSuite-20200505-O3V6J8"
];

TestCreate[
	MemoryLeakTest[PullAndPush2[ds1]]
	,
	0
	,
	TestID -> "DataListTestSuite-20200505-Z7Z9O7"
];

TestCreate[
	`LLU`PacletFunctionSet[FromInitList, {}, "DataStore"];
	FromInitList[]
	,
	Developer`DataStore[
		Developer`DataStore["a" -> True, "b" -> False], 
		Developer`DataStore[2, 3, 5, 7, 11], 
		Developer`DataStore["a" -> 2.34, "b" -> 3.14], 
		Developer`DataStore[2. + 3.*I, 3. + 0.*I, 5.1 - 1.23*I, 7. + 0.*I, 0. + 11.*I], 
		Developer`DataStore["a" -> "x", "b" -> "y"]
	]
	,
	TestID->"DataListTestSuite-20200508-D7S0D5"
];

(* Timing tests *)
VerificationTest[
	getSlowdown[x_] := ToString[N[(x/timeDataStore - 1) * 100]] <> "% slower than DataStore.";
	SeedRandom[0];
	los = RandomWord["CommonWords", 300000];
	ds = Developer`DataStore @@ los;

	{timeDataStore, r2} = RepeatedTiming[ReverseListOfStringsLibraryLink[ds]];
	Print["Reverse strings - DataStore: " <> ToString[timeDataStore] <> "s."];

	{timeGeneric, r3} = RepeatedTiming[ReverseListOfStringsGeneric[ds]];
	Print["Reverse strings - GenericDataList: " <> ToString[timeGeneric] <> "s. " <> getSlowdown[timeGeneric]];

	{timeGenericOut, r5} = RepeatedTiming[ReverseListOfStringsGenericOut[ds]];
	Print["Reverse strings - [In]DataList/[Out]GenericDataList: " <> ToString[timeGenericOut] <> "s. " <> getSlowdown[timeGenericOut]];

	{timeGenericIn, r4} = RepeatedTiming[ReverseListOfStringsGenericIn[ds]];
	Print["Reverse strings - [In]GenericDataList/[Out]DataList: " <> ToString[timeGenericIn] <> "s. " <> getSlowdown[timeGenericIn]];

	{timeDataList, r6} = RepeatedTiming[ReverseListOfStrings[ds]];
	Print["Reverse strings - DataList: " <> ToString[timeDataList] <> "s. " <> getSlowdown[timeDataList]];

	{timeWSTP, r1} = RepeatedTiming[ReverseListOfStringsWSTP[los]];
	Print["Reverse strings - WSTP: " <> ToString[timeWSTP] <> "s. " <> getSlowdown[timeWSTP]];

	r1 === List @@ r2 === List @@ r3 === List @@ r4 === List @@ r5 === List @@ r6
	,
	TestID -> "DataListTestSuite-20180906-W5N4V0"
];



(* Memory leak tests *)
TestCreate[
	MemoryLeakTest[PassDataStore[ds0, #]] & /@ {False, True}
	,
	{ 0, 0 }
	,
	TestID -> "DataListTestSuite-20200505-E5A4L8"
];

TestCreate[
	MemoryLeakTest[PassDataStore[ds1, #]] & /@ {False, True}
	,
	{ 0, 0 }
	,
	TestID -> "DataListTestSuite-20180908-B6X4I7"
];

TestCreate[
	MemoryLeakTest[PassDataStore[ds2, #]] & /@ {False, True}
	,
	{ 0, 0 }
	,
	TestID -> "DataListTestSuite-20180908-S3Q1N7"
];

TestCreate[
	MemoryLeakTest[PassDataStore[ds3, #]] & /@ {False, True}
	,
	{ 0, 0 }
	,
	TestID -> "DataListTestSuite-20180908-S4C7Q2"
];

TestCreate[
	LibraryFunctionUnload[Last @ PassDataStore];
	PassDataStore = `LLU`PacletFunctionLoad["PassDataStore", {{"DataStore", "Manual"}, "Boolean"}, "DataStore"];
	MemoryLeakTest[PassDataStore[ds0, #]] & /@ {False, True}
	,
	{ 0, n_ } /; n > 0   (* when copy is made we expect a leak, because the C++ code is not aware of the "Manual" passing of the input DataStore *)
	,
	TestID -> "DataListTestSuite-20180908-C4W5X3"
	,
	SameTest -> MatchQ
];

TestCreate[
	MemoryLeakTest[PassDataStore[ds1, #]] & /@ {False, True}
	,
	{ 0, n_ } /; n > 0
	,
	TestID -> "DataListTestSuite-20180908-R9U2G0"
	,
	SameTest -> MatchQ
];

TestCreate[
	MemoryLeakTest[PassDataStore[ds2, #]] & /@ {False, True}
	,
	{ 0, n_ } /; n > 0
	,
	TestID -> "DataListTestSuite-20180908-K7W7L1"
	,
	SameTest -> MatchQ
];

TestCreate[
	MemoryLeakTest[PassDataStore[ds3, #]] & /@ {False, True}
	,
	{ 0, n_ } /; n > 0
	,
	TestID -> "DataListTestSuite-20180908-Z1Y1Q5"
	,
	SameTest -> MatchQ
];
