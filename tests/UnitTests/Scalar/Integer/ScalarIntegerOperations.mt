(* Wolfram Language Test file *)
(***************************************************************************************************************************************)
				(*
					Set of Testcases to test Integer apis for library link
				*)
(***************************************************************************************************************************************)
TestExecute[
	currentDirectory = DirectoryName[$CurrentFile];
]

ExactTest[
	Get[FileNameJoin[{currentDirectory, "GetAndSet", "GetAndSet.wl"}]];
	LLSet[5];
	LLGet[]
	,
	5
	,
	TestID->"ScalarIntegerOperations-20150806-M0L0V7"
]

ExactTest[
	Get[FileNameJoin[{currentDirectory, "SquareInteger", "SquareInteger.wl"}]];
	SquareInteger[2]
	,
	4
	,
	TestID->"ScalarIntegerOperations-20150806-F8V5U1"
]

ExactTest[
	Get[FileNameJoin[{currentDirectory, "IntegerAdd", "IntegerAdd.wl"}]];
	IntegerAdd[2,3]
	,
	5
	,
	TestID->"ScalarIntegerOperations-20150806-D9W5E1"
]

ExactTest[
	Get[FileNameJoin[{currentDirectory, "IntegerTimes", "IntegerTimes.wl"}]];
	IntegerTimes[4,6]
	,
	24
	,
	TestID->"ScalarIntegerOperations-20150806-A5D2Q3"
]