(* Project base directory *)
$baseDir = FileNameDrop[$CurrentFile, -3]; 

(* Path to directory containing LibraryLinkUtilities *)
$LLUPath = FileNameJoin[{$baseDir, "src"}];

(* LLU source files *)
$LLUSources = FileNames["*.cpp", {$LLUPath},2];

(* Compilations options for all tests *)
options = {
	"CleanIntermediate" -> True,
	"IncludeDirectories" -> {$LLUPath, FileNameJoin[{$LLUPath, "ML"}]},
	"CompileOptions" -> If[MatchQ[$SystemID, "Windows-x86-64" | "Windows"], "/O2 /EHsc /W4" , "-O2 -std=c++14 -Wall --pedantic -fvisibility=hidden"],
	"ShellOutputFunction" -> Print,
	"ShellCommandFunction" -> Print,
	"Language" -> "C++"
};