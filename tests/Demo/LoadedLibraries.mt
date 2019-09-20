TestExecute[
	(* WRIunzip or libCurlLink may already be loaded *)
	init = Compile`LoadedLibraries[];
]


Test[
	Complement[Compile`LoadedLibraries[], init]
	,
	{}
	,
	TestID -> "LoadedLibraries-20141110-D9G6V7"
]


Test[
	fun1 = LibraryFunctionLoad["demo", "demo_I_I", {Integer}, Integer];
	Complement[Compile`LoadedLibraries[], init]
	,
	{FindLibrary["demo"]}
	,
	TestID -> "LoadedLibraries-20141110-A4J2X0"
]


Test[
	fun2 = LibraryFunctionLoad["demo_LinkObject", "addtwo", LinkObject, LinkObject];
	Complement[Compile`LoadedLibraries[], Join[init, {FindLibrary["demo"]}]]
	,
	{FindLibrary["demo_LinkObject"]}
	,
	TestID -> "LoadedLibraries-20141110-V1B7G4"
]


Test[
	LibraryUnload["demo"];
	Complement[Compile`LoadedLibraries[], init]
	,
	{FindLibrary["demo_LinkObject"]}
	,
	TestID -> "LoadedLibraries-20141110-R7C8U8"
]


Test[
	LibraryUnload["demo_LinkObject"];
	Compile`LoadedLibraries[]
	,
	init
	,
	TestID -> "LoadedLibraries-20141110-T6Y3N7"
]