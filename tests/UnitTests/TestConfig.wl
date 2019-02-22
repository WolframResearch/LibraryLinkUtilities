(* Project base directory *)
$baseDir = FileNameDrop[$CurrentFile, -3];

(* Path to directory containing include folder from LibraryLinkUtilities installation *)
$LLUIncDir = FileNameJoin[{$baseDir, "install", "include"}];

(* Path to LibraryLinkUtilities static lib *)
$LLULibDir = FileNameJoin[{$baseDir, "install", "lib"}];

(* LibraryLinkUtilities library name *)
$LLULib = "LLU";

(* Path to LibraryLinkUtilities shared resources *)
$LLUSharedDir = FileNameJoin[{$baseDir, "install", "share"}];

(* Compilations options for all tests *)
options = {
	"CleanIntermediate" -> True,
	"IncludeDirectories" -> { $LLUIncDir },
	"Libraries" -> { $LLULib },
	"LibraryDirectories" -> { $LLULibDir },
	"CompileOptions" -> If[MatchQ[$SystemID, "Windows-x86-64" | "Windows"],
		"/O2 /EHsc /W4 /std:c++17"
		,
		"-O2 -std=c++17 -Wall -Wextra --pedantic -fvisibility=hidden"
	],
	"ShellOutputFunction" -> Print,
	"ShellCommandFunction" -> Print,
	"Language" -> "C++"
};

(* If dynamic version of LLU was built, we want to load it to Mathematica before test libs are loaded *)
LibraryLoad /@ FileNames[{"*.so", "*.dll", "*.dylib"}, $LLULibDir];


(* Memory leak test *)
ClearAll[MemoryLeakTest];
SetAttributes[MemoryLeakTest, HoldAll];
Options[MemoryLeakTest] = {"ReturnValue" -> Last};

MemoryLeakTest[expression_, opts : OptionsPattern[]] :=
    MemoryLeakTest[expression, {i, 10}, opts];

MemoryLeakTest[expression_, repetitions_Integer?Positive, opts : OptionsPattern[]] :=
	MemoryLeakTest[expression, {i, repetitions}, opts];

MemoryLeakTest[expression_, {s_Symbol, repetitions__}, opts : OptionsPattern[]] :=
	Block[{$MessageList},
		Module[{res, memory},
			$MessageList = {};
			ClearSystemCache[];
			res = Table[
				memory = MemoryInUse[];
				expression;
				$MessageList = {};
				ClearSystemCache[];
				MemoryInUse[] - memory
				,
				{s, repetitions}
			];
			OptionValue["ReturnValue"] @ res
		]
	];

MemoryLeakTestWithMessages[expression_] :=
	MemoryLeakTestWithMessages[expression, 10];

MemoryLeakTestWithMessages[expression_, repetitions_Integer?Positive] :=
    Block[{mem},
	    Do[
		    mem = MemoryInUse[];
		    expression;
		    Print[MemoryInUse[] - mem]
		    ,
		    {repetitions}
	    ]
	];