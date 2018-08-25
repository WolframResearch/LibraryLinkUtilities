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
	"CompileOptions" -> If[MatchQ[$SystemID, "Windows-x86-64" | "Windows"], "/O2 /EHsc /W3" , "-O2 -std=c++14 -Wall --pedantic -fvisibility=hidden"],
	"ShellOutputFunction" -> Print,
	"ShellCommandFunction" -> Print,
	"Language" -> "C++"
};

(* If dynamic version of LLU was built, we want to load it to Mathematica before test libs are loaded *)
LibraryLoad /@ FileNames[{"*.so", "*.dll", "*.dylib"}, $LLULibDir];