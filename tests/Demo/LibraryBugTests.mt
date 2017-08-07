(* Mathematica Test File *)

Needs["CCompilerDriver`"]

TestExecute[
	$UseDLLToolsFromLayout = False;
	]

TestExecute[

Get[FileNameJoin[{ FileNameDrop[ $CurrentFile, -1], "DemoTestConfig.wl"}]];
Print[ $Version];
Print[ $CommandLine];
Print[$DllDemoDir];

DLLExtension["MacOSX"] = ".dylib";
DLLExtension["MacOSX-x86"] = ".dylib";
DLLExtension["MacOSX-x86-64"] = ".dylib";
DLLExtension["Linux"] = ".so";
DLLExtension["Linux-x86-64"] = ".so";
DLLExtension["Windows"] = ".dll";
DLLExtension["Windows-x86-64"] = ".dll";

GetDLLFile[name_String] := Module[{dll, sys = $SystemID},
		dll = FileNameJoin[{$DllDemoDir, "LibraryResources", sys, 
	  		      StringJoin[name, DLLExtension[sys]]}];
	        If[FileType[dll] === File, dll, $Failed]];

dll = GetDLLFile["demo"];
]

(* Note that most of the time, a facility called ResourceLocator` is 
   available on the Mathematica build. This facility helps one to automatically
   locate packages or files that lie in one of the directories listed in 
   $Path. In that case, it is not necessary to use 
   	LibraryFunctionLoad[GetDLLFile["demo"], ...]. 
   A direct call to LibraryFunctionLoad["demo", ...] can be made. 
   This is usually the case with the daily builds available at 
   /home/Mathematica/M-LINUX-L/8.0.0/current/Executables/math 
   However, the 'mathkernel' builds don't have this facility as of now.
   01/18/10 *)


TestExecute[
	fun4 = LibraryFunctionLoad[dll, 
	   "demo_TI_R", {{Real, _}, {Integer, 0}}, {Real, 0}];
	   ]

Test[(* 1 *)
	   fun4[{1., 2., 3., 4.}, 2]
	   ,
	   2.
	   ,
	TestID->"LibraryBugTests-20100118-zxu211-bug-138895"
	]

Test[ (* 2 *)
	DateString[{"2010-03-18-16-48-23",Riffle[{"Year","Month","Day","Hour","Minute","Second"},"-"]}]
	,
	"Thu 18 Mar 2010 16:48:23"
	,
	{}
	,
	TestID->"LibraryBugTests-20100326-Q2D2E0-bug-146690-146601"
]

Test[ (* 3 *)  (* test loading of libcal more directly (this is essentailly the same as DLLBugTests-2010326-Q2D2E0-bug-146690,
                  but I'm keeping both of them in case one fails and not the other. *)
 	cf = LibraryFunctionLoad["libcal", "parseDateString",	{"UTF8String", "UTF8String", Real}, {Integer, 1}];
 	Drop[Apply[List,cf],1]
	,
	{"parseDateString", {"UTF8String", "UTF8String", Real}, {Integer, 1}}
	,
	{}
	,
	TestID->"LibraryBugTests-20100422-X0P5L4-bug-146690-146601"
]

NTest[(* 4 *) 
	FinancialDerivative[{"European", "Call"}, {"StrikePrice" -> 50.00, 
  	"Expiration" -> 1},  {"InterestRate" -> 0.1, "Volatility" -> 0.5, 
  	"CurrentPrice" -> 50, "Dividend" -> 0.05}]
  	,
  	10.3649`5
  	,
  	{}
	,
	TestID->"LibraryBugTests-20100427-E0P5G4-bug-150808-205919"
]

Test[  
  addonelib = CreateLibrary["#include \"WolframLibrary.h\"\n\n
  DLLEXPORT int WolframLibrary_initialize( WolframLibraryData \
libData) {
  return LIBRARY_NO_ERROR;
  }
  
  DLLEXPORT void WolframLibrary_uninitialize( WolframLibraryData \
libData) {
  return;
  }
  
  DLLEXPORT int addone( WolframLibraryData libData, mint Argc, \
MArgument
  *Args, MArgument Res) {
  mint I0;
  mint I1;
  I0 = MArgument_getInteger(Args[0]);
  I1 = I0 + 1;
  MArgument_setInteger(Res, I1);
  return LIBRARY_NO_ERROR;
  }
  ", "NameWithNonLatinCharacters\[Theta]"];
  FileExistsQ[addonelib]
  ,
  True
  ,
  TestID->"LibraryBugTests-20110127-C2Z1B3-bug-192869"
]

Test[
	addonefunction = LibraryFunctionLoad[addonelib,"addone", {{Integer, 0}}, {Integer, 0}];
  	addonefunction[53]
 	,
 	54
	,
	TestID->"LibraryBugTests-20110127-G8Q6M3-bug-177127-192869"	
]