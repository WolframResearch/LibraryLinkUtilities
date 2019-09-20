Print["$InstallationDirectory = " <> $InstallationDirectory];
Print["$UserName = " <> $UserName];
Print["$CurrentFile = " <> $CurrentFile];
Print["$SystemID = " <> $SystemID];
Print["\n"];

Get[FileNameJoin[{ FileNameDrop[ $CurrentFile, -1], "DemoTestConfig.wl"}]];

Print[ $Version]

Print[ $CommandLine]

Print[$DllDemoDir]

DLLExtension["MacOSX"] = ".dylib";
DLLExtension["MacOSX-x86"] = ".dylib";
DLLExtension["MacOSX-x86-64"] = ".dylib";
DLLExtension["Linux"] = ".so";
DLLExtension["Linux-x86-64"] = ".so";
DLLExtension["Linux-ARM"] = ".so";
DLLExtension["Windows"] = ".dll";
DLLExtension["Windows-x86-64"] = ".dll";

GetDLLFile[name_String] := Module[{dll, sys = $SystemID},
	dll = FileNameJoin[{$DllDemoDir, "LibraryResources", sys, StringJoin[name, DLLExtension[sys]]}];
	If[FileType[dll] === File, dll, $Failed]
];

dll = GetDLLFile["demo_string"];

Print["dll = " <> dll];

Test[
	FileType[ dll]
	,
	File
	,
	TestID -> "StringDLL-20091213-F9W2A7"
]


Test[
	fun = LibraryFunctionLoad[ dll,
		"countSubstring", {"UTF8String", "UTF8String"}, Integer]
	,
	LibraryFunction[dll, "countSubstring", {"UTF8String", "UTF8String"}, Integer]
	,
	TestID -> "StringDLL-20091213-M1T6Y2"
]

Test[
	fun["abcdabef", "ab"]
	,
	2
	,
	TestID -> "StringDLL-20091213-K8M9M6"
]


Test[
	fun = LibraryFunctionLoad[ dll,
		"encodeString", {"UTF8String", Integer}, "UTF8String"]
	,
	LibraryFunction[dll, "encodeString", {"UTF8String", Integer}, "UTF8String"]
	,
	TestID -> "StringDLL-20091213-T5Z4W3"
]

Test[
	fun["abcdef", 1]
	,
	"bcdefg"
	,
	TestID -> "StringDLL-20091213-L2F4A4"
]

Test[
	fun = LibraryFunctionLoad[ dll,
		"reverseString", {"UTF8String"}, "UTF8String"]
	,
	LibraryFunction[dll, "reverseString", {"UTF8String"}, "UTF8String"]
	,
	TestID -> "StringDLL-20091213-D2W6E7"
]

Test[
	fun["abcdef"]
	,
	"fedcba"
	,
	TestID -> "StringDLL-20091213-H6J8O7"
]




