(* Wolfram Language Test file *)
(***************************************************************************************************************************************)
				(*
					Set of Testcases to test boolean apis for library link
				*)
(***************************************************************************************************************************************)
TestExecute[
	currentDirectory = DirectoryName[$CurrentFile];
]

Test[
	Get[FileNameJoin[{currentDirectory, "BooleanAnd", "BooleanAnd.wl"}]];
	{BooleanAnd[True,False],BooleanAnd[False,True],BooleanAnd[False,False],BooleanAnd[True,True]}
	,
	{False,False,False,True}
	,
	TestID->"ScalarBooleanOperations-20150806-H2A7H8"
]

Test[
	Get[FileNameJoin[{currentDirectory, "BooleanOr", "BooleanOr.wl"}]];
	{BooleanOr[True,False],BooleanOr[False,True],BooleanOr[False,False],BooleanOr[True,True]}
	,
	{True,True,False,True}
	,
	TestID->"ScalarBooleanOperations-20150806-Q1W1V8"
]

Test[
	Get[FileNameJoin[{currentDirectory, "BooleanNot", "BooleanNot.wl"}]];
	{BooleanNot[False],BooleanNot[True]}
	,
	{True,False}
	,
	TestID->"ScalarBooleanOperations-20150806-U1B6I7"
]