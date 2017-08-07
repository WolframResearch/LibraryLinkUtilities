(* Wolfram Language Test file *)
(***************************************************************************************************************************************)
				(*
					Set of Testcases to test complex apis for library link
				*)
(***************************************************************************************************************************************)
TestExecute[
	currentDirectory = DirectoryName[$CurrentFile];
]

Test[
	Get[FileNameJoin[{currentDirectory, "ComplexAdd", "ComplexAdd.wl"}]];
	ComplexAdd[1+2I,3+10I]
	,
	4.+12.I
	,
	TestID->"ScalarComplexOperations-20150806-D9W5E1"
]

Test[
	Get[FileNameJoin[{currentDirectory, "ComplexTimes", "ComplexTimes.wl"}]];
	ComplexTimes[1+2I,3+10I]
	,
	-17.+16.I
	,
	TestID->"ScalarComplexOperations-20150806-A5D2Q3"
]