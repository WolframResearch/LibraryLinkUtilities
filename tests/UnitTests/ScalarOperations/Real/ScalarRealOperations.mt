(* Wolfram Language Test file *)
(***************************************************************************************************************************************)
				(*
					Set of Testcases to test Real apis for library link
				*)
(***************************************************************************************************************************************)
TestExecute[
	currentDirectory = DirectoryName[$CurrentFile];
]

Test[
	Get[FileNameJoin[{currentDirectory, "RealAdd", "RealAdd.wl"}]];
	RealAdd[1.,2.]
	,
	3.
	,
	TestID->"ScalarRealOperations-20150806-D9W5E1"
]

Test[
	Get[FileNameJoin[{currentDirectory, "RealTimes", "RealTimes.wl"}]];
	RealTimes[4.,5.]
	,
	20.
	,
	TestID->"ScalarRealOperations-20150806-A5D2Q3"
]