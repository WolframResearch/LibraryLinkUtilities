(* Project base directory *)
$baseDir = FileNameDrop[$CurrentFile, -3]; 

(* Path to directory containing LibraryLinkUtilities *)
$LLUPath = FileNameJoin[{$baseDir, "src"}];

(* LLU source files *)
$LLUSources = FileNames["*.cpp", {$LLUPath}];

(* Compilations options for all tests *)
options = {
	"CleanIntermediate" -> True,
	"IncludeDirectories" -> {$LLUPath},
	"CompileOptions" -> If[MatchQ[$SystemID, "Windows-x86-64" | "Windows"], "/EHsc" , "-O2 -std=c++14 -Wall"],
	"ShellOutputFunction" -> Print,
	"ShellCommandFunction" -> Print,
	"Language" -> "C++"
};