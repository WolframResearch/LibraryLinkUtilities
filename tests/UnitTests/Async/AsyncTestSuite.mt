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
	SleepyThreads = SafeLibraryFunction["SleepyThreads", {Integer, Integer, Integer}, "Void"];

	(* ParallelAccumulate[NA, n, bs] separates a NumericArray NA into blocks of bs elements and sums them in parallel on n threads.
	 * Returns a one-element NumericArray with the sum of all elements of NA *)
	ParallelAccumulate = SafeLibraryFunction["Accumulate", {{NumericArray, "Constant"}, Integer, Integer}, NumericArray]
];

TestMatch[
	AbsoluteTiming[SleepyThreads[8, 160, 100]] (* sleep 100ms 160 times which totals to 16s, divided onto 8 threads, so it should take slightly more than 2s *)
	,
	{ t_, Null } /; (t >= 2 && t < 2.5)
	,
	TestID -> "AsyncTestSuite-20190718-I7S1K0"
];

TestMatch[
	data = NumericArray[RandomInteger[{-100, 100}, 10000000], "Integer64"];
	{systemTime, sum} = AbsoluteTiming @ Total[data, Infinity];
	Print["Total[] time = ", systemTime];
	{parallelTime, parallelSum} = AbsoluteTiming @ ParallelAccumulate[data, 16, 10000];
	Print["ParallelAccumulate[] time = ", parallelTime];
	First @ Normal @ parallelSum
	,
	sum
	,
	TestID -> "AsyncTestSuite-20191219-Y8B1L5"
];

VerificationTest[
	data = NumericArray[RandomComplex[{-100 - 100I, 100 + 100I}, 10000000], "ComplexReal64"];
	{systemTime, sum} = AbsoluteTiming @ Total[data, Infinity];
	Print["Total[] time = ", systemTime];
	{parallelTime, parallelSum} = AbsoluteTiming @ ParallelAccumulate[data, 16, 10000];
	Print["ParallelAccumulate[] time = ", parallelTime];
	Abs[First @ Normal @ parallelSum - sum] < 0.0001
	,
	TestID -> "AsyncTestSuite-20191219-O4K0H1"
];