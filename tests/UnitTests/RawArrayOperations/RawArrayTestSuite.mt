(* Wolfram Language Test file *)
TestRequirement[$VersionNumber>10.3]
TestExecute[
	currentDirectory = DirectoryName[$CurrentFile];
	Get[FileNameJoin[{ParentDirectory[currentDirectory], "TestConfig.wl"}]];
	sourceDirectory = FileNameJoin[{currentDirectory, "TestSources"}];
	Get[FileNameJoin[{sourceDirectory, "RawArrayOperations.wl"}]];
]

(****************************RawArray Operations****************************************)
Test[
	ra = RawArray["UnsignedInteger8", {1, 2, 3, 4}];
	echoRawArray[ra]
	,
	RawArray["UnsignedInteger8", {1, 2, 3, 4}]
	,
	TestID->"RawArrayOperations-20150825-M7G1B2"
]

Test[
	raw = RawArray["Real64", N@Range[0, 47]/47];
	res = echoRawArray[raw]//Head
	,
	RawArray
	,
	TestID->"RawArrayOperations-20150825-P4U4W5"
]

ExactTest[
	getRALength[ra]
	,
	4
	,
	TestID->"RawArrayOperations-20150827-V6M5C7"
]

ExactTest[
	getRARank[ra]
	,
	1
	,
	TestID->"RawArrayOperations-20150827-A7Q6J1"
]

Test[
	newRA[]//Head
	,
	RawArray
	,
	TestID->"RawArrayOperations-20150827-B3Y1C2"
]

Test[
	cl = cloneRA[ra];
	SameQ[cl,ra]
	,
	True
	,
	TestID->"RawArrayOperations-20150827-I0C3X0"
]

Test[(*check RawArray shared APi's*)
	changeSharedRA[ra];
	sra = getSharedRA[];
	SameQ[sra,ra]
	,
	True
	,
	TestID->"RawArrayOperations-20150827-O5S8C2"
]

EndRequirement[]