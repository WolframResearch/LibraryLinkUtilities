AntLog[$Version];
(* full messages *)
$MessagePrePrint = .;

extraPath = AntProperty[ "ExtraPath"];
If[StringQ[extraPath],
	extraPathDirs = StringSplit[extraPath, ","];
	Print["Extra path dirs: ", InputForm[extraPathDirs]];
	$Path = Join[extraPathDirs, $Path];
	Scan[PacletDirectoryAdd, extraPathDirs];
	,
	Print["Error: extraPath not defined: ", InputForm@extraPath]
];
Print["CCompilerDriver location: ", InputForm[FindFile[ "CCompilerDriver`"]]]
Print["AntCCompilerBuildHelper location: ", InputForm[FindFile[ "AntCCompilerBuildHelper`"]]];

Needs["CCompilerDriver`"];
Needs["AntCCompilerBuildHelper`"];

targetID = AntProperty["targetID"];
If[ !StringQ[targetID] || targetID === "Automatic",
	targetID = $SystemID
];
Print[targetID];
Switch[targetID,
	"MacOSX-x86-64",
	compoptions = "-std=c++14 -fvisibility=hidden"  (*-install_name " <> outputName <> ".dylib"*),

	"Linux-x86-64" | "Linux",
	compoptions = "-O2 -fPIC -std=c++14 -Wall -Wextra -Wpedantic -Wno-unused-parameter -fvisibility=hidden",

	"Windows-x86-64" | "Windows",
	compoptions = "/O2 /EHsc",
	_,
	AntFail["Unrecognized target $SystemID " <> ToString[targetID]]
];

SetCCompilerOptionsFromAnt[
	"CleanIntermediate" -> True,
	"IncludeDirectories" -> AntProperty["LLUIncludeFiles"],
	"CompileOptions" -> compoptions,
	"LinkerOptions" -> If[targetID === "Windows-x86-64", {"/MACHINE:x64"}, {}],
	"ShellOutputFunction" -> AntLog,
	"ShellCommandFunction" -> AntLog,
	"Language" -> "C++"
];

LLUsrc = FileNames["*.c*", AntProperty["LLUSourceFiles"], 2];

(* Build the demo libraries used in LibraryLink examples *)
demoFiles = FileNames["*.cpp", AntProperty["sourceDir"]];
demoFileNamePairs = Map[{#, FileBaseName[#]}&, demoFiles];

result = Apply[ 
	Function[{srcFile, outputName},
    	{resFiles, mkResourceCmds} = CreateResourceFileCommands[outputName];

    	outputName -> CreateLibrary[{srcFile} ~Join~ LLUsrc, outputName,
    		"ExtraObjectFiles" -> resFiles, 
			"PreCompileCommands" -> mkResourceCmds
		]
	], demoFileNamePairs, {1}
];

If[ Quiet[Union[Map[FileType[#[[2]]]&, result]]] =!= {File}, 
	AntFail["Libraries not all produced " <> ToString[Column[result]]]
]
