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
	ReverseListOfStrings = SafeLibraryFunction["ReverseListOfStrings", {"DataStore"}, "DataStore"];

	(* Test data used across multiple tests *)
	bool = True;
	int = 23456;
	real = -2.7;
	complex = 2.2 - 3.5 I;
	string = "Test string";
	tensor = RandomReal[1, {3, 2}];
	(*numeric = NumericArray[RandomInteger[{0, 255}, {2, 3}], "UnsignedInteger8"];*)
	image = RandomImage[1, {2, 3}, ColorSpace -> "CMYK"];
	(*sparse = SparseArray[{{1, 1} -> 1, {2, 2} -> 2, {3, 3} -> 3, {1, 3} -> 4}];*)
	ds0 = Developer`DataStore[bool, int, real, complex, string, tensor, (*numeric,*) image(*, sparse*)];
	ds1 = Developer`DataStore[bool, int, real, complex, string, tensor, (*numeric,*) image, (*sparse,*) ds0];
	ds2 = Developer`DataStore @@ Thread[Take[Alphabet[], Length[ds1]] -> List @@ ds1];
	ds3 = ArrayReshape[RandomWord[10], {2, 5}] /. List -> Developer`DataStore;
];


(* Compile-time errors *)
(*Test[
	CCompilerDriver`CreateLibrary[{FileNameJoin[{currentDirectory, "MLTestCompilationErrors.cpp"}]}, "MLTestErrors", options]
	,
	$Failed
	,
	{CreateLibrary::cmperr..} * On Linux there should be 6 errors, but MSVC does not like generic lambdas so it spits out more errors *)(*
	,
	TestID->"MathLinkTestSuite-20171129-U5Q3L8"
]*)

(* Basic tests *)

Test[
	EmptyDataStore = SafeLibraryFunction["EmptyDataStore", {}, "DataStore"];
	EmptyDataStore[]
	,
	Developer`DataStore[]
	,
	TestID->"DataListTestSuite-20180903-Y8Z5P1"
];

Test[
	PassDataStore[foo, True]
	,
	LibraryFunction[lib, "PassDataStore", {"DataStore", True|False}, "DataStore"][foo, True]
	,
	{Message[LibraryFunction::cfsa, foo, 1, "DataStore object"]}
	,
	TestID->"DataListTestSuite-20180903-O2B2Y6"
]

Test[
	PassDataStore[{}, True]
	,
	LibraryFunction[lib, "PassDataStore", {"DataStore", True|False}, "DataStore"][{}, True]
	,
	{Message[LibraryFunction::cfsa, {}, 1, "DataStore object"]}
	,
	TestID->"DataListTestSuite-20180903-B6P7O2"
]

Test[
	PassDataStore[Developer`DataStore[{1, "a"}], True]
	,
	LibraryFunction[lib, "PassDataStore", {"DataStore", True|False}, "DataStore"][Developer`DataStore[{1, "a"}], True]
	,
	{Message[LibraryFunction::cfsa, Developer`DataStore[{1, "a"}], 1, "DataStore object"]}
	,
	TestID->"DataListTestSuite-20180903-M6K4Z6"
]

Test[
	PassDataStore[Developer`DataStore[{1, 2, 3}], True]
	,
	Developer`DataStore[{1, 2, 3}]
	,
	TestID->"DataListTestSuite-20180903-Y4V3P7"
]

Test[
	PassDataStore[Developer`DataStore[1, 2, 3], True]
	,
	Developer`DataStore[1, 2, 3]
	,
	TestID->"DataListTestSuite-20180903-S2C1I6"
]

Test[
	PassDataStore[Developer`DataStore[1, 2, "a"], True]
	,
	Developer`DataStore[1, 2, "a"]
	,
	TestID->"DataListTestSuite-20180903-Y5Y7L3"
]

Test[
	PassDataStore[Developer`DataStore[Developer`DataStore[1, 2, 3]],True]
	,
	Developer`DataStore[Developer`DataStore[1, 2, 3]]
	,
	TestID->"DataListTestSuite-20180903-N9I8U3"
]

Test[
	PassDataStore[Developer`DataStore["ąę"->"ąę"],True]
	,
	Developer`DataStore["ąę"->"ąę"]
	,
	TestID->"DataListTestSuite-20180903-E8X1M1"
]

Test[
	PassDataStore[Developer`DataStore["" -> { -3.14 }],True]
	,
	Developer`DataStore[{ -3.14 }]
	,
	TestID->"DataListTestSuite-20180903-T8C7S4"
]

Test[
	PassDataStore[Developer`DataStore[], #]& /@ { True, False }
	,
	{ Developer`DataStore[], Developer`DataStore[] }
	,
	TestID->"DataListTestSuite-20180903-X7K0J0"
]

Test[
	PassDataStore[ds0, #]& /@ { True, False }
	,
	{ ds0, ds0 }
	,
	TestID->"DataListTestSuite-20180903-K0Y6L5"
]

Test[
	PassDataStore[ds1, #]& /@ { True, False }
	,
	{ ds1, ds1 }
	,
	TestID->"DataListTestSuite-20180903-T7W2E4"
]

Test[
	PassDataStore[ds2, #]& /@ { True, False }
	,
	{ ds2, ds2 }
	,
	TestID->"DataListTestSuite-20180903-Y3R9B8"
]

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


Test[
	ReverseListOfStrings[Developer`DataStore["aaaa", "b", 2]]
	,
	Failure["DLInvalidNodeType", <|
		"MessageTemplate" -> "DataStoreNode passed to Node wrapper carries data of invalid type", 
		"MessageParameters" -> <||>, 
		"ErrorCode" -> -48, 
		"Parameters" -> {}
	|>]
	,
	TestID->"DataListTestSuite-20180906-Z7K0I0"
];

(* Timing tests *)
Test[
	los = RandomWord["CommonWords", 100000];
	{timeMathLink, r1} = RepeatedTiming[ReverseListOfStringsMathLink[los]];
	{timeDataStore, r2} = RepeatedTiming[ReverseListOfStrings[Developer`DataStore @@ los]];
	Print["Time when sending list via MathLink: " <> ToString[timeMathLink] <> "s."];
	Print["Time when sending list via DataStore: " <> ToString[timeDataStore] <> "s."];
	r1
	,
	List @@ r2
	,
	TestID->"DataListTestSuite-20180906-W5N4V0"
]



(* Memory leak tests *)

Test[
	MemoryLeakTest[PassDataStore[ds0, #], 5, "ReturnValue" -> Identity] & /@ {False, True}
	,
	{ ds3, ds3 }
	,
	TestID->"DataListTestSuite-20180904-J9J5U5"
]

(*In[48]:= MemoryLeakTest[PassDataStore[ds0, #]] & /@ {False, True}*)

(*Out[48]= {408, 408}*)

(*In[49]:= MemoryLeakTest[PassDataStore[ds1, #]] & /@ {False, True}*)

(*Out[49]= {856, 856}*)

(*In[50]:= MemoryLeakTest[PassDataStore[ds2, #]] & /@ {False, True}*)

(*Out[50]= {936, 936}*)

(*In[52]:= MemoryLeakTest[PassDataStore[ds3, #]] & /@ {False, True}*)

(*Out[52]= {712, 712}*)