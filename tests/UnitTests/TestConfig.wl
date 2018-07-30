(* Project base directory *)
$baseDir = FileNameDrop[$CurrentFile, -3]; 

(* Path to directory containing LibraryLinkUtilities *)
$LLUIncDir = FileNameJoin[{$baseDir, "include"}];

(* Path to directory containing LibraryLinkUtilities *)
$LLUSrcDir = FileNameJoin[{$baseDir, "src"}];

(* LLU source files *)
$LLUSources = FileNames["*.cpp", {$LLUSrcDir}, Infinity];

(* Compilations options for all tests *)
options = {
	"CleanIntermediate" -> True,
	"IncludeDirectories" -> { $LLUIncDir },
	"CompileOptions" -> If[MatchQ[$SystemID, "Windows-x86-64" | "Windows"], "/O2 /EHsc /W3" , "-O2 -std=c++14 -Wall --pedantic -fvisibility=hidden"],
	"ShellOutputFunction" -> Print,
	"ShellCommandFunction" -> Print,
	"Language" -> "C++"
};