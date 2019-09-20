Print["$InstallationDirectory = " <> $InstallationDirectory];
Print["$UserName = " <> $UserName];
Print["$CurrentFile = " <> $CurrentFile];
Print["$SystemID = " <> $SystemID];
Print["\n"];

Get[FileNameJoin[{ FileNameDrop[ $CurrentFile, -1], "DemoTestConfig.wl"}]];

Print[ $Version]

Print[ $CommandLine]

DLLExtension["MacOSX"] = ".dylib";
DLLExtension["MacOSX-x86"] = ".dylib";
DLLExtension["MacOSX-x86-64"] = ".dylib";
DLLExtension["Linux"] = ".so";
DLLExtension["Linux-ARM"] = ".so";
DLLExtension["Linux-x86-64"] = ".so";
DLLExtension["Windows"] = ".dll";
DLLExtension["Windows-x86-64"] = ".dll";

GetDLLFile[name_String] := Module[{dll, sys = $SystemID},
	dll = FileNameJoin[{$DllDemoDir, "LibraryResources", sys,
		StringJoin[name, DLLExtension[sys]]}];
	If[FileType[dll] === File, dll, $Failed]];

dll = GetDLLFile["demo"];

Print["dll = " <> dll];

Test[
	FileType[ dll]
	,
	File
	,
	TestID -> "MultiLoadDLLTests-20081130-E1Q8K3"
]

Test[
	fun = LibraryFunctionLoad[dll, "demo_I_I", {Integer}, {Integer, 0}]
	,
	LibraryFunction[dll, "demo_I_I", {Integer}, Integer]
	,
	TestID -> "MultiLoadDLLTests-20081130-S0V4O2"
]


Test[
	fun = LibraryFunctionLoad[dll, "demo_I_I", {Integer}, {Integer, 0}]
	,
	LibraryFunction[dll, "demo_I_I", {Integer}, Integer]
	,
	TestID -> "MultiLoadDLLTests-20090217-C0E1Z4"
]


Test[
	fun = LibraryFunctionLoad[dll, "demo_I_I", {Real}, {Integer, 0}]
	,
	LibraryFunction[dll, "demo_I_I", {Real}, Integer]
	,
	{LibraryFunction::overload}
	,
	TestID -> "MultiLoadDLLTests-20090217-P7O0W9"
]



Test[
	fun1 = LibraryFunctionLoad[dll, "demo_T_T", {{Real, 1}}, {Real, 1}]
	,
	LibraryFunction[dll, "demo_T_T", {{Real, 1}}, {Real, 1}]
	,
	TestID -> "MultiLoadDLLTests-20091213-M0N3H3"
]




Test[
	fun2 = LibraryFunctionLoad[dll, "demo_T_T", {{Real, 2}}, {Real, 1}]
	,
	LibraryFunction[dll, "demo_T_T", {{Real, 2}}, {Real, 1}]
	,
	{LibraryFunction::overload}
	,
	TestID -> "MultiLoadDLLTests-20091213-X4Y5P6"
]



Test[
	fun1[{1., 2., 3., 4.}]
	,
	{1., 2., 3., 4., 4., 4., 1., 3.}
	,
	TestID -> "MultiLoadDLLTests-20091213-F1K2C6"
]



Test[
	fun2[{{1., 2.}, {3., 4.}}]
	,
	{1., 2., 3., 4., 2., 2., 4., 2., 3.}
	,
	TestID -> "MultiLoadDLLTests-20091213-B5F0C0-bug-181705"
]


Test[
	LibraryFunctionUnload[ fun2]
	,
	Null
	,
	TestID -> "MultiLoadDLLTests-20091213-I3C2H1"
]

Test[
	LibraryFunctionUnload[ fun2]
	,
	$Failed
	,
	{LibraryFunction::nofun}
	,
	TestID -> "MultiLoadDLLTests-20091213-H3N2F2"
]

Test[
	Part[fun2[{{1., 2.}, {3., 4.}}], 0, 0]
	,
	LibraryFunction
	,
	{LibraryFunction::nofun}
	,
	TestID -> "MultiLoadDLLTests-20091213-H2H9N2"
]

Test[
	fun2 = LibraryFunctionLoad[dll, "demo_T_T", {{Real, 2}}, {Real, 1}]
	,
	LibraryFunction[dll, "demo_T_T", {{Real, 2}}, {Real, 1}]
	,
	{LibraryFunction::overload}
	,
	TestID -> "MultiLoadDLLTests-20091213-E0Q3I4"
]

Test[
	LibraryUnload[ dll]
	,
	Null
	,
	TestID -> "MultiLoadDLLTests-20091213-G3W6J4"
]

Test[
	Part[fun1[{1., 2., 3., 4.}], 0, 0]
	,
	LibraryFunction
	,
	{LibraryFunction::nofun}
	,
	TestID -> "MultiLoadDLLTests-20091213-F0B9E0"
]


Test[
	Part[fun2[{{1., 2.}, {3., 4.}}], 0, 0]
	,
	LibraryFunction
	,
	{LibraryFunction::nofun}
	,
	TestID -> "MultiLoadDLLTests-20091213-B1Z3E2"
]

Test[
	LibraryUnload[ dll]
	,
	$Failed
	,
	{LibraryFunction::unloadlib}
	,
	TestID -> "MultiLoadDLLTests-20091213-C6A0E4"
]

