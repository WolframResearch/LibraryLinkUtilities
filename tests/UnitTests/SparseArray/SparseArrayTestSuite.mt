(* Wolfram Language Test file *)
TestRequirement[$VersionNumber >= 12];
(***************************************************************************************************************************************)
(*
	Set of test cases for LLU functionality related to handling and exchanging sparse arrays
*)
(***************************************************************************************************************************************)
TestExecute[
	Needs["CCompilerDriver`"];
	currentDirectory = DirectoryName[$TestFileName];

	(* Get configuration (path to LLU sources, compilation options, etc.) *)
	Get[FileNameJoin[{ParentDirectory[currentDirectory], "TestConfig.wl"}]];

	(* Compile the test library *)
	lib = CCompilerDriver`CreateLibrary[
		FileNameJoin[{currentDirectory, "TestSources", #}]& /@ {"SparseArrayOperations.cpp"},
		"SparseArrayTest",
		options (* defined in TestConfig.wl *)
	];

	Get[FileNameJoin[{$LLUSharedDir, "LibraryLinkUtilities.wl"}]];
	`LLU`InitializePacletLibrary[lib];

	`LLU`PacletFunctionSet[$GetImplicitValue, {{LibraryDataType[SparseArray, _, _], "Constant"}}, {_, _}];
	`LLU`PacletFunctionSet[$GetExplicitValues, {{LibraryDataType[SparseArray, _, _], "Constant"}}, {_, _}];
	`LLU`PacletFunctionSet[$GetRowPointers, {{LibraryDataType[SparseArray, _, _], "Constant"}}, {_, _}];
	`LLU`PacletFunctionSet[$GetColumnIndices, {{LibraryDataType[SparseArray, _, _], "Constant"}}, {_, _}];
	`LLU`PacletFunctionSet[$GetExplicitPositions, {{LibraryDataType[SparseArray, _, _], "Constant"}}, {_, _}];
	`LLU`PacletFunctionSet[$ToTensor, {{LibraryDataType[SparseArray, _, _], "Constant"}}, {_, _}];
	`LLU`PacletFunctionSet[$SetImplicitValue, {LibraryDataType[SparseArray, _, _], {_, _, "Constant"}}, LibraryDataType[SparseArray]];
	`LLU`PacletFunctionSet[$ModifyValues, {{LibraryDataType[SparseArray, _, _], "Shared"}, {_, _, "Constant"}}, "Void"];
	`LLU`PacletFunctionSet[$ModifyValues2, {{LibraryDataType[SparseArray, _, _], "Shared"}, {_, _, "Constant"}}, "Void"];
	`LLU`PacletFunctionSet[$GetImplicitValueTyped, {{LibraryDataType[SparseArray, Real], "Constant"}}, Real];
	`LLU`PacletFunctionSet[$GetExplicitValuesTyped, {{LibraryDataType[SparseArray, Real], "Constant"}}, {Real, _}];
	`LLU`PacletFunctionSet[$GetRowPointersTyped, {{LibraryDataType[SparseArray, Real], "Constant"}}, {Integer, _}];
	`LLU`PacletFunctionSet[$GetColumnIndicesTyped, {{LibraryDataType[SparseArray, Real], "Constant"}}, {Integer, _}];
	`LLU`PacletFunctionSet[$GetExplicitPositionsTyped, {{LibraryDataType[SparseArray, Real], "Constant"}}, {Integer, 2}];
	`LLU`PacletFunctionSet[$ToTensorTyped, {{LibraryDataType[SparseArray, Real], "Constant"}}, {Real, _}];
	`LLU`PacletFunctionSet[$SetImplicitValueTyped, {LibraryDataType[SparseArray, Real], Real}, LibraryDataType[SparseArray]];

	sparse = SparseArray[{{1., 0., 0., 0.}, {2., 1., 0., 0.}, {4., 0., 3., 0.}, {0., 0., 0., 1.}}];
	zero = SparseArray[{0}];

];

Test[
	$GetImplicitValue[sparse]
	,
	0.
	,
	TestID->"SparseArrayTestSuite-20201130-Q0V0F5"
];

Test[
	$GetImplicitValue[zero]
	,
	0
	,
	TestID->"SparseArrayTestSuite-20201130-Y0L3W6"
];

Test[
	$GetExplicitValues[sparse]
	,
	{1., 2., 1., 4., 3., 1.}
	,
	TestID->"SparseArrayTestSuite-20201130-Q8Q5J9"
];

Test[
	$GetExplicitValues[zero]
	,
	{}
	,
	TestID->"SparseArrayTestSuite-20201130-C8B7X3"
];

Test[
	$GetRowPointers[sparse]
	,
	{0, 1, 3, 5, 6}
	,
	TestID->"SparseArrayTestSuite-20201130-R1M2S0"
];

Test[
	$GetRowPointers[zero]
	,
	{0, 0}
	,
	TestID->"SparseArrayTestSuite-20201130-Z1Z3Q9"
];

Test[
	$GetColumnIndices[sparse]
	,
	{{1}, {1}, {2}, {1}, {3}, {4}}
	,
	TestID->"SparseArrayTestSuite-20201130-O1T1O2"
];

Test[
	$GetColumnIndices[zero]
	,
	{}
	,
	TestID->"SparseArrayTestSuite-20201130-K0M1W1"
];

Test[
	$GetExplicitPositions[sparse]
	,
	{{1, 1}, {2, 1}, {2, 2}, {3, 1}, {3, 3}, {4, 4}}
	,
	TestID->"SparseArrayTestSuite-20201130-F9L3K7"
];

Test[
	$GetExplicitPositions[zero]
	,
	{}
	,
	TestID->"SparseArrayTestSuite-20201130-E5E9L4"
];

Test[
	$ToTensor[sparse]
	,
	{{1., 0., 0., 0.}, {2., 1., 0., 0.}, {4., 0., 3., 0.}, {0., 0., 0., 1.}}
	,
	TestID->"SparseArrayTestSuite-20201130-X2E9G1"
];

Test[
	$ToTensor[zero]
	,
	{0}
	,
	TestID->"SparseArrayTestSuite-20201130-U0R1P8"
];

Test[
	$SetImplicitValue[sparse, 1.]
	,
	SparseArray[
		Automatic, 
		{4, 4}, 
		1., 
		{
			1, 
			{{0, 3, 6, 10, 13}, {{2}, {3}, {4}, {1}, {3}, {4}, {1}, {2}, {3}, {4}, {1}, {2}, {3}}}, 
			{0., 0., 0., 2., 0., 0., 4., 0., 3., 0., 0., 0., 0.}
		}
	]
	,
	TestID->"SparseArrayTestSuite-20201130-Y4S0B5"
];

Test[
	$SetImplicitValue[zero, 1]
	,
	SparseArray[Automatic, {1}, 1, {1, {{0, 1}, {{1}}}, {0}}]
	,
	TestID->"SparseArrayTestSuite-20201130-A3S2K4"
];

Test[
	$ModifyValues[sparse, {3.5, .5, -7.}];
	$GetExplicitValues[sparse]
	,
	{3.5, .5, -7., 4., 3., 1.}
	,
	TestID->"SparseArrayTestSuite-20210115-J6F3M0"
];

Test[
	$ModifyValues2[sparse, {-2.3, .5, -7.1}];
	$GetExplicitValues[sparse]
	,
	{-2.3, .5, -7.1, 4., 3., 1.}
	,
	TestID->"SparseArrayTestSuite-20210115-N8G4F8"
];

TestMatch[
	Catch[$ModifyValues2[sparse, {-5.4 + 3.5 * I}], _]
	,
	Failure["TensorTypeError", <|
		"MessageTemplate" -> "An error was caused by an MTensor type mismatch.",
		"MessageParameters" -> <||>,
		"ErrorCode" -> _?CppErrorCodeQ,
		"Parameters" -> {}|>
	]
	,
	TestID->"SparseArrayTestSuite-20210115-B7V1P0"
];

TestExecute[
	sparse = SparseArray[{{1., 0., 0., 0.}, {2., 1., 0., 0.}, {4., 0., 3., 0.}, {0., 0., 0., 1.}}];
];

Test[
	$GetImplicitValueTyped[sparse]
	,
	0.
	,
	TestID->"SparseArrayTestSuite-20210202-Z6Z6J6"
];

Test[
	$GetExplicitValuesTyped[sparse]
	,
	{1., 2., 1., 4., 3., 1.}
	,
	TestID->"SparseArrayTestSuite-20210202-M2I0M4"
];

Test[
	$GetRowPointersTyped[sparse]
	,
	{0, 1, 3, 5, 6}
	,
	TestID->"SparseArrayTestSuite-20210202-P4J9J2"
];

Test[
	$GetColumnIndicesTyped[sparse]
	,
	{{1}, {1}, {2}, {1}, {3}, {4}}
	,
	TestID->"SparseArrayTestSuite-20210202-T3R7B9"
];

Test[
	$GetExplicitPositionsTyped[sparse]
	,
	{{1, 1}, {2, 1}, {2, 2}, {3, 1}, {3, 3}, {4, 4}}
	,
	TestID->"SparseArrayTestSuite-20210202-O6A5L7"
];

Test[
	$ToTensorTyped[sparse]
	,
	{{1., 0., 0., 0.}, {2., 1., 0., 0.}, {4., 0., 3., 0.}, {0., 0., 0., 1.}}
	,
	TestID->"SparseArrayTestSuite-20210202-Z9L6I6"
];

Test[
	$SetImplicitValueTyped[sparse, 1.]
	,
	SparseArray[
		Automatic,
		{4, 4},
		1.,
		{
			1,
			{{0, 3, 6, 10, 13}, {{2}, {3}, {4}, {1}, {3}, {4}, {1}, {2}, {3}, {4}, {1}, {2}, {3}}},
			{0., 0., 0., 2., 0., 0., 4., 0., 3., 0., 0., 0., 0.}
		}
	]
	,
	TestID->"SparseArrayTestSuite-20210202-M7T4Z9"
];