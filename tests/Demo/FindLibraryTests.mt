(* Mathematica Test File *)

Get[FileNameJoin[{ FileNameDrop[ $CurrentFile, -1], "DemoTestConfig.wl"}]];

(* Convert drive letter to upper-case; for some reason it is lowercase on test3win64.  This was causing FindDLLTests-20090217-O1H2E1 to fail. *)
$DllDemoDir = StringReplace[$DllDemoDir, {RegularExpression["^([[:alpha:]]):\\\\"] :> ToUpperCase["$1"] <> ":\\"}];


<< ResourceLocator`;

ResourceLocator`ApplicationAdd[$DllDemoDir];

Test[(* 1 *)
	StringQ[FindLibrary[ "demo"]]
	,
	True
	,
	TestID -> "FindDLLTests-20090217-Y5V7A5"
]

Test[(* 2 *)
	FindLibrary[ "notFound"]
	,
	$Failed
	,
	TestID -> "FindDLLTests-20090217-R9H3X3"
]

TestExecute[
	Print["$LibraryPath = " <> $LibraryPath]
]

Test[(* 3 *)
	If[MemberQ[ $LibraryPath, FileNameJoin[{$DllDemoDir , "LibraryResources", $SystemID}]], True, $LibraryPath]
	,
	True
	,
	TestID -> "FindDLLTests-20090217-O1H2E1"
]


Test[(* 4 *)
	LibraryFunctionLoad[ "demo", "demo_I_I", {Integer}, Integer]
	,
	LibraryFunction[ FindLibrary["demo"], "demo_I_I", {Integer}, Integer]
	,
	TestID -> "FindDLLTests-20090217-H0C9H7"
]

Test[(* 5 *)
	FindLibrary["tetgenWolfram"] // Head
	,
	String
	,
	TestID -> "FindDLLTests-20091204-usor1v-bug-137756"
]


