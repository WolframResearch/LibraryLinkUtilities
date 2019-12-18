(* Wolfram Language Test file *)
TestRequirement[$VersionNumber >= 12.0];
(***************************************************************************************************************************************)
(*
	Set of test cases to test LLU functionality related to asynchronous tasks and thread pools
*)
(***************************************************************************************************************************************)
TestExecute[
	Needs["CCompilerDriver`"];
	currentDirectory = DirectoryName[$CurrentFile];

	(* Get configuration (path to LLU sources, compilation options, etc.) *)
	Get[FileNameJoin[{ParentDirectory[currentDirectory], "TestConfig.wl"}]];

	(* Thread pool functionality requires C++17 *)
	$CppVersion = "c++17";

	(* Compile the test library *)
	lib = CCompilerDriver`CreateLibrary[
		FileNameJoin[{currentDirectory, "TestSources", #}]& /@ {"PoolTest.cpp", "QueueTest.cpp"},
		"Async",
		options,
		"Defines" -> {"LLU_LOG_DEBUG"}
	];

	Get[FileNameJoin[{$LLUSharedDir, "LibraryLinkUtilities.wl"}]];

	RegisterPacletErrors[lib, <||>];

	`LLU`Logger`FormattedLog := `LLU`Logger`LogToString;
	(* SleepyThreads[n, m, t] spawns n threads and performs m jobs on them, where each job is just sleeping t milliseconds *)
	SleepyThreads = SafeLibraryFunction["SleepyThreads", {Integer, Integer, Integer}, "Void"]
];

TestMatch[
	AbsoluteTiming[SleepyThreads[8, 160, 100]] (* sleep 100ms 160 times which totals to 16s, divided onto 8 threads, so it should take ~2s *)
	,
	{ Null, t_}
	,
	TestID -> "AsyncTestSuite-20190718-I7S1K0"
];
