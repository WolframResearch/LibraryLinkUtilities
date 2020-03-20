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
		FileNameJoin[{currentDirectory, "TestSources", #}]& /@ {"PoolTest.cpp"},
		"Async",
		options,
		"Defines" -> {"LLU_LOG_DEBUG"}
	];

	Get[FileNameJoin[{$LLUSharedDir, "LibraryLinkUtilities.wl"}]];

	`LLU`InitializeLLU[lib];

	`LLU`Logger`FormattedLog := `LLU`Logger`LogToShortString;
	(* SleepyThreads[n, m, t] spawns n threads and performs m jobs on them, where each job is just sleeping t milliseconds *)
	SleepyThreads = `LLU`SafeLibraryFunction["SleepyThreads", {Integer, Integer, Integer}, "Void"];
	(* SleepyThreadsWithPause[n, m, t] works same as SleepyThreads but pauses the pool for 1 second, submits all tasks and then resumes. *)
	SleepyThreadsWithPause = `LLU`SafeLibraryFunction["SleepyThreadsWithPause", {Integer, Integer, Integer}, "Void"];
	(* Same as SleepyThreads only using Basic thread pool. *)
	SleepyThreadsBasic = `LLU`SafeLibraryFunction["SleepyThreadsBasic", {Integer, Integer, Integer}, "Void"];

	(* ParallelAccumulate[NA, n, bs] separates a NumericArray NA into blocks of bs elements and sums them in parallel on n threads.
	 * Returns a one-element NumericArray with the sum of all elements of NA *)
	ParallelAccumulate = `LLU`SafeLibraryFunction["Accumulate", {{NumericArray, "Constant"}, Integer, Integer}, NumericArray];
	SequentialAccumulate = `LLU`SafeLibraryFunction["AccumulateSequential", {{NumericArray, "Constant"}}, NumericArray];
	ParallelAccumulateBasic = `LLU`SafeLibraryFunction["AccumulateBasic", {{NumericArray, "Constant"}, Integer, Integer}, NumericArray];

	(* ParallelLcm[NA, n, bs] calculates LCM of all "UnsignedIntegers64" in NA recursively, running in parallel on n threads.
     * This function tests running async jobs on a thread pool that can themselves submit new jobs to the pool. *)
	ParallelLcm = `LLU`SafeLibraryFunction["LcmParallel", {{NumericArray, "Constant"}, Integer, Integer}, NumericArray];
	SequentialLcm = `LLU`SafeLibraryFunction["LcmSequential", {{NumericArray, "Constant"}}, NumericArray];
];

TestMatch[
	AbsoluteTiming[SleepyThreads[8, 80, 100]] (* sleep 100ms 80 times which totals to 8s, divided onto 8 threads, so it should take slightly more than 1s *)
	,
	{ t_, Null } /; (t >= 1 && t < 1.5)
	,
	TestID -> "AsyncTestSuite-20190718-I7S1K0"
];

TestMatch[
	AbsoluteTiming[SleepyThreadsWithPause[8, 80, 100]] (* it should take slightly more than 1s + 1s paused, so >2s *)
	,
	{ t_, Null } /; (t >= 2 && t < 2.5)
	,
	TestID -> "AsyncTestSuite-20200113-X6B2Q8"
];

TestMatch[
	AbsoluteTiming[SleepyThreadsBasic[8, 80, 100]]
	,
	{ t_, Null } /; (t >= 1 && t < 1.5)
	,
	TestID -> "AsyncTestSuite-20200115-S8F3X4"
];

VerificationTest[
	data = NumericArray[RandomInteger[{-100, 100}, 50000000], "Integer16"];
	{systemTime, sum} = RepeatedTiming @ SequentialAccumulate[data];
	Print["SequentialAccumulate[] time for Integer16 = ", systemTime];
	{parallelTime, parallelSum} = RepeatedTiming @ ParallelAccumulate[data, 8, 50000];
	Print["ParallelAccumulate[] time for Integer16 = ", parallelTime];
	parallelSum == sum
	,
	TestID -> "AsyncTestSuite-20191219-Y8B1L5"
];

VerificationTest[
	data = NumericArray[RandomComplex[{-100 - 100I, 100 + 100I}, 50000000], "ComplexReal64"];
	{systemTime, sum} = RepeatedTiming @ SequentialAccumulate[data];
	Print["SequentialAccumulate[] time for ComplexReal64 = ", systemTime];
	{parallelTime, parallelSum} = RepeatedTiming @ ParallelAccumulate[data, 8, 50000];
	Print["ParallelAccumulate[] time for ComplexReal64 = ", parallelTime];
	Abs[First @ Normal[parallelSum - sum]] < 0.00001
	,
	TestID -> "AsyncTestSuite-20191219-O4K0H1"
];

VerificationTest[
	data = NumericArray[RandomInteger[{-100, 100}, 50000000], "Integer32"];
	{systemTime, sum} = RepeatedTiming @ SequentialAccumulate[data];
	Print["SequentialAccumulate[] time for Integer32 = ", systemTime];
	parallelTime = First @ RepeatedTiming @ ParallelAccumulate[data, 8, 50000];
	Print["ParallelAccumulate[] time for Integer32 = ", parallelTime];
	{parallelTime, parallelSum} = RepeatedTiming @ ParallelAccumulateBasic[data, 8, 50000];
	Print["ParallelAccumulate[] with basic pool time for Integer32 = ", parallelTime];
	parallelSum == sum
	,
	TestID -> "AsyncTestSuite-20200115-P8I3W8"
];

(* Uncomment to see how parallel accumulate compares to Total. *)
(*
VerificationTest[
	data = NumericArray[RandomInteger[{-100, 100}, 50000000], "Integer64"];
	{systemTime, sum} = RepeatedTiming @ Total[data, Infinity];
	Print["Total[] time = ", systemTime];
	{parallelTime, parallelSum} = RepeatedTiming @ ParallelAccumulate[data, 8, 50000];
	Print["ParallelAccumulate[] time = ", parallelTime];
	First @ Normal @ parallelSum == sum
	,
	TestID -> "AsyncTestSuite-20191223-T8T9H1"
];

VerificationTest[
	data = NumericArray[RandomComplex[{-100 - 100I, 100 + 100I}, 50000000], "ComplexReal64"];
	{systemTime, sum} = RepeatedTiming @ Total[data, Infinity];
	Print["Total[] time = ", systemTime];
	{parallelTime, parallelSum} = RepeatedTiming @ ParallelAccumulate[data, 8, 50000];
	Print["ParallelAccumulate[] time = ", parallelTime];
	Abs[First @ Normal @ parallelSum - sum] < 0.00001
	,
	TestID -> "AsyncTestSuite-20191223-J9V5Q1"
];
*)

VerificationTest[
	data = NumericArray[RandomInteger[{0, 40}, 50000000], "UnsignedInteger64"];
	{systemTime, lcmSeq} = RepeatedTiming @ SequentialLcm[data];
	Print["SequentialLcm[] time = ", systemTime];
	{parallelTime, parallelLcm} = RepeatedTiming @ ParallelLcm[data, 12, 50000];
	Print["ParallelLcm[] time = ", parallelTime];
	(parallelLcm == lcmSeq) && (First @ Normal[parallelLcm] == LCM @@ Normal[data])
	,
	TestID -> "AsyncTestSuite-20191227-Y7R7Q4"
];
