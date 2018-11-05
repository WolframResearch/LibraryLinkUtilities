(* Wolfram Language Test file *)
TestRequirement[$VersionNumber>10.3]
TestExecute[
	currentDirectory = DirectoryName[$CurrentFile];
	Get[FileNameJoin[{ParentDirectory[currentDirectory], "TestConfig.wl"}]];
	sourceDirectory = FileNameJoin[{currentDirectory, "TestSources"}];
	Get[FileNameJoin[{sourceDirectory, "NumericArrayOperations.wl"}]];
]

(****************************NumericArray Operations****************************************)
Test[
	ra = NumericArray["UnsignedInteger8", {1, 2, 3, 4}];
	echoNumericArray[ra]
	,
	NumericArray["UnsignedInteger8", {1, 2, 3, 4}]
	,
	TestID->"NumericArrayOperations-20150825-M7G1B2"
]

Test[
	raw = NumericArray["Real64", N @ Range[0, 47]/47];
	res = Developer`NumericArrayQ @ echoNumericArray[raw]
	,
	True
	,
	TestID->"NumericArrayOperations-20150825-P4U4W5"
]

ExactTest[
	getRALength[ra]
	,
	4
	,
	TestID->"NumericArrayOperations-20150827-V6M5C7"
]

ExactTest[
	getRARank[ra]
	,
	1
	,
	TestID->"NumericArrayOperations-20150827-A7Q6J1"
]

Test[
	Developer`NumericArrayQ @ newRA[]
	,
	True
	,
	TestID->"NumericArrayOperations-20150827-B3Y1C2"
]

Test[
	cl = cloneRA[ra];
	SameQ[cl,ra]
	,
	True
	,
	TestID->"NumericArrayOperations-20150827-I0C3X0"
]

Test[(*check NumericArray shared APi's*)
	changeSharedRA[ra];
	sra = getSharedRA[];
	SameQ[sra,ra]
	,
	True
	,
	TestID->"NumericArrayOperations-20150827-O5S8C2"
]

Test[
	accumulateIntegers[NumericArray[Range[10]]]
	,
	Total @ Range[10];
	,
	TestID->"NumericArrayTestSuite-20181030-U5E5U6"
]

Test[
	accumulateIntegers[NumericArray[3.5]]
	,
	LibraryFunctionError["LIBRARY_FUNCTION_ERROR", 6]
	,
	TestID->"NumericArrayTestSuite-20181030-P4G8W4"
]

EndRequirement[]