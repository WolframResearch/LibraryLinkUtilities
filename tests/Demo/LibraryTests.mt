(* Mathematica Test File *)

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
  dll = FileNameJoin[{$DllDemoDir, "LibraryResources", sys, 
     StringJoin[name, DLLExtension[sys]]}];
  If[FileType[dll] === File, dll, $Failed]];


dll = GetDLLFile["demo"];

Test[
	FileType[ dll]
	,
	File
	,
	TestID->"DLLTests-20081130-E1Q8K3"
]


Test[
	fun0 = LibraryFunctionLoad[dll, "demo_I_I", {{Integer, 0}}, {Integer, 0}];
    fun0[20]
	,
	21
	,
	TestID->"DLLTests-20081130-S0V4O2"
]

Test[
	fun0x = LibraryFunctionLoad[dll, "demo_I_I", {{Integer, 0}}, {Integer, 0}];
    fun0x[20]
	,
	21
	,
	TestID->"FindDLLTests-20090217-N1D2U3"
]

Test[
	LibraryFunctionLoad[dll, "demo_I_I", LinkObject, LinkObject]
	,
	$Failed
	,
	{LibraryFunction::funloaded}
	,
	TestID->"FindDLLTests-20090217-O2C1L1"
]


Test[
	fun0a = LibraryFunctionLoad[dll, "demo1_I_I", {Integer}, Integer];
    fun0a[20]
	,
	21
	,
	TestID->"DLLTests-20081130-W1G7F4"
]


Test[
	fun1 = LibraryFunctionLoad[dll, "demo_R_R", {{Real, 0}}, {Real, 0}];
   	fun1[1.5]
	,
	2.25
	,
	TestID->"DLLTests-20081130-B6N3W4"
]


Test[
	fun1a = LibraryFunctionLoad[dll, "demo1_R_R", {Real}, Real];
   	fun1a[1.5]
	,
	2.25
	,
	TestID->"DLLTests-20081130-Z3W8I5"
]



Test[
	fun2 = LibraryFunctionLoad[dll, "demo_IR_R", {{Integer, 0}, {Real, 0}}, {Real, 0}];
   	fun2[3, 1.1]
	,
	3.3
	,
	TestID->"DLLTests-20081130-H9C2I7"
]


Test[
	fun2a = LibraryFunctionLoad[dll, "demo1_IR_R", {Integer, Real}, Real];
   	fun2a[3, 1.1]
	,
	3.3
	,
	TestID->"DLLTests-20081130-R3A6M7"
]



Test[
	fun3 = LibraryFunctionLoad[dll, "demo_TI_R", {{Real, 1}, {Integer, 0}}, {Real, 0}];
   	fun3[Range[1., 50], 6]
	,
	6.0
	,
	TestID->"DLLTests-20081130-O1P1S2"
]



Test[
   	fun3[{1., 2., 3., 4., 5.}, 2]
	,
	2.0
	,
	TestID->"DLLTests-20081204-Q7E4E9"
]

Test[
	fun3a = LibraryFunctionLoad[dll, "demo1_TI_R", {{Real, 1, "Manual"}, {Integer, 0}}, {Real, 0}];
   	fun3a[Range[1., 50], 6]
	,
	6.0
	,
	TestID->"DLLTests-20081204-R1G9V2"
]

Test[
	fun4 = LibraryFunctionLoad[dll, "demo2_TI_R", {{Real, 1, "Shared"}, {Integer, 0}}, {Real, 0}];
   	fun4[Range[1., 50], 6]
	,
	6.0
	,
	TestID->"DLLTests-20081130-L5B1U4"
]




Test[
	fun5 = LibraryFunctionLoad[dll, "demo_I_T", {{Integer, 0}}, {Integer, 1}];
   	fun5[ 6]
	,
	{2, 4, 6, 8, 10, 12}
	,
	TestID->"DLLTests-20081130-I8M1N8"
]



Test[
	fun6 = LibraryFunctionLoad[dll, "demo_TT_T", {{Real, _}, {Integer, _}}, {Real, _}];
   	fun6[Range[1., 50], 8]
	,
	8.
	,
	TestID->"DLLTests-20081203-F7A5R9-bug-136891"
]


Test[
	fun7 = LibraryFunctionLoad[dll, "demo_TTT_T", {{Real, _}, {Integer, _}, {Real, _}}, {Integer, _}];
   	fun7[Range[1., 50], 8, 3.]
	,
	11
	,
	TestID->"DLLTests-20081203-R1M2O0"
]



Test[
	fun8 = LibraryFunctionLoad[dll, "demo_T_T", {{Real, _}}, {Real, _}];
   	fun8[Developer`ToPackedArray[ {{1., 2., 3.}, {8., 4., 5.}}]]
	,
	{1., 2., 3., 8., 4., 5., 2., 3., 6., 2., 3.}
	,
	TestID->"DLLTests-20081204-B6Q8S9"
]


Test[
	fun9 = LibraryFunctionLoad[dll, "demo1_T_T", {{Integer, _}}, {Integer, _}];
   	fun9[Developer`ToPackedArray[ {{1, 2, 3}, {8, 4, 5}}]]
	,
	{1, 2, 3, 8, 4, 5, 2, 3, 6, 2, 2}
	,
	TestID->"DLLTests-20081204-H8G7F7"
]




Test[
	fun10 = LibraryFunctionLoad[dll, "demo1_I_T", {Integer}, {Integer, _}];
   	fun10[10]
	,
	{2, 4, 6, 8, 10, 12, 14, 16, 18, 20}
	,
	TestID->"DLLTests-20081204-M2E7S3"
]



Test[
	fun15a = LibraryFunctionLoad[dll, 
  			 "demo_TII_I", {{Integer, _}, Integer, Integer}, Integer];
   	fun15a[{{1, 2, 3, 4}, {-1, -2, -3, -4}}, 2, 3]
	,
	-3
	,
	TestID->"DLLTests-20090121-S5D4E3"
]


Test[
	fun15b = LibraryFunctionLoad[dll, 
   "demo_TIII_I", {{Integer, _}, Integer, Integer, Integer}, Integer];
   	fun15b[{{1, 2, 3, 4}, {-1, -2, -3, -4}}, 2, 3, 10]
	,
	10
	,
	TestID->"DLLTests-20090121-M2R5Z2"
]


Test[
	fun16a = LibraryFunctionLoad[dll, 
  			 "demo_TII_R", {{Real, _}, Integer, Integer}, Real];
   	fun16a[{{1., 2., 3., 4.}, {-1., -2., -3.1, -4.}}, 2, 3]
	,
	-3.1
	,
	TestID->"DLLTests-20090121-R8M9C7"
]

Test[
	fun16b = LibraryFunctionLoad[dll, 
   			"demo_TIIR_R", {{Real, _}, Integer, Integer, Real}, Real];
   	fun16b[{{1., 2., 3., 4.}, {-1., -2., -3., -4.}}, 2, 3, 10.2]
	,
	10.2
	,
	TestID->"DLLTests-20090121-Y2K9A9"
]

Test[
	fun18b = LibraryFunctionLoad[dll, 
  		 "demo_TTII_T", {{Integer, _}, {Integer, _}, Integer, 
   				 Integer}, {Integer, _}];
   	arr = Table[i + j + k, {i, 3}, {j, 2}, {k, 4}];
	new = {-10, -11, -12, -13};
	fun18b[arr, new, 2, 2]
	,
	{{{3, 4, 5, 6}, {4, 5, 6, 7}}, {{4, 5, 6, 7}, 
		{-10, -11, -12, -13}}, {{5, 6, 7, 8}, {6, 7, 8, 9}}}
	,
	TestID->"DLLTests-20090121-R8D4N3"
]


(*
 Boolean input and output
*)

Test[
	funBoolean1 = LibraryFunctionLoad[dll, 
  		 "demoBoolean1", {True|False}, True|False];
 	funBoolean1[ True]
	,
	False
	,
	TestID->"DLLTests-20090318-B2L6Q5"
]


(*
 Complex input and output
*)

Test[
	funComplex1 = LibraryFunctionLoad[dll, 
  		 "demoComplex1", {Complex}, Complex];
 	funComplex1[ 1. + 1. I]
	,
	1. - 1. I
	,
	TestID->"DLLTests-20090318-W8X0U0"
]

(*
 No input and output
*)

Test[
	funVoid = LibraryFunctionLoad[dll, "demoNoInput", {}, Integer];
 	funVoid[  ]
	,
	0
	,
	TestID->"DLLTests-20090318-S9S7M4"
]

Test[
	funVoid = LibraryFunctionLoad[dll, "demoVoid", {Integer}, "Void"];
 	funVoid[  1]
	,
	Null
	,
	TestID->"DLLTests-20090318-H3O2H6"
]

Test[
	funVoid = LibraryFunctionLoad[dll, "demoNoInputVoid", {}, "Void"];
 	funVoid[  ]
	,
	Null
	,
	TestID->"DLLTests-20090318-V1G3T5"
]


dll1 = GetDLLFile["demo_shared"];

Test[
	FileType[ dll1]
	,
	File
	,
	TestID->"DLLTests-20081204-O2E6Z3"
]

Test[
	loadFun = LibraryFunctionLoad[dll1, "loadArray", {{Real, _, "Shared"}}, Integer];
	unloadFun = LibraryFunctionLoad[dll1, "unloadArray", {}, Integer];
	setFunVector = LibraryFunctionLoad[dll1, "setElementVector", {Integer, Real}, Integer];
	getFunVector = LibraryFunctionLoad[dll1, "getElementVector", {Integer}, Real];
	setFun = LibraryFunctionLoad[dll1, "setElement", {Integer, Integer, Real}, Integer];
	getFun = LibraryFunctionLoad[dll1, "getElement", {Integer, Integer}, Real];
	,
	Null
	,
	TestID->"DLLTests-20081204-Z6I7Q3"
]

Test[
	array = Range[1., 20]
	,
	{1., 2., 3., 4., 5., 6., 7., 8., 9., 10., 11., 12., 13., 14., 15.,
				16., 17., 18., 19., 20.}
	,
	TestID->"DLLTests-20081204-F0O2L1"
]




Test[
	loadFun[ array]
	,
	0
	,
	TestID->"DLLTests-20081204-D4P8V4"
]


Test[
	setFunVector[5, -5.]
	,
	0
	,
	TestID->"DLLTests-20081204-C3A5F4"
]


Test[
	Part[ array, 5]
	,
	-5.
	,
	TestID->"DLLTests-20081204-J1L1S9"
]


Test[
	getFunVector[5]
	,
	-5.
	,
	TestID->"DLLTests-20081204-O8P1M5"
]



Test[
	unloadFun[]
	,
	0
	,
	TestID->"DLLTests-20081204-Q1E7Z9"
]


Test[
	loadFun[ array]
	,
	0
	,
	TestID->"DLLTests-20081204-Q5Q8F7"
]


Test[
	loadFun[ array]
	,
	0
	,
	TestID->"DLLTests-20081204-C4I8B0"
]


Test[
	getFunVector[5]
	,
	-5.
	,
	TestID->"DLLTests-20081204-X7R2J3"
]



(*
Exclude the following tests since LLU is not fully compatible with LibraryLink in terms of shared data structures.
See https://bugs.wolfram.com/show?number=337331

Test[
	unloadFun[]
	,
	0
	,
	TestID->"DLLTests-20081204-J3A2N3"
]


Test[
	unloadFun[]
	,
	0
	,
	TestID->"DLLTests-20081204-E2F0V8"
]
*)


Test[
	unloadFun[]
	,
	0
	,
	{LibraryFunction::notensor}
	,
	TestID->"DLLTests-20090122-Y4M8V7"
]





Test[
	array = Table[ N[i + j/9.], {i, 4}, {j, 3}];
	loadFun[ array]
	,
	0
	,
	{LibraryFunction::shrnopack}
	,
	TestID->"DLLTests-20090122-B1P0N1"
]

Test[
	unloadFun[]
	,
	0
	,
	TestID->"DLLTests-20090122-I6G8F3"
]


NTest[
	array = Developer`ToPackedArray[ array]
	,
	{{1.1111111111111112, 1.2222222222222223, 1.3333333333333333}, 
		{2.111111111111111, 2.2222222222222223, 2.3333333333333335}, 
			{3.111111111111111, 3.2222222222222223, 3.3333333333333335}, 
				{4.111111111111111, 4.222222222222222, 4.333333333333333}}	
	,
	TestID->"DLLTests-20090122-V0Y3F1"
]



Test[
	loadFun[ array]
	,
	0
	,
	TestID->"DLLTests-20090122-M7C9H0"
]



Test[
	setFun[3, 2, -5.]
	,
	0
	,
	TestID->"DLLTests-20090122-T7M6G8"
]


NTest[
	array
	,
	{{1.1111111111111112, 1.2222222222222223, 1.3333333333333333}, 
		{2.111111111111111, 2.2222222222222223, 2.3333333333333335}, 
			{3.111111111111111, -5., 3.3333333333333335}, 
				{4.111111111111111, 4.222222222222222, 4.333333333333333}}	
	,
	TestID->"DLLTests-20090122-T3F0W9"
]


Test[
	getFun[3, 2]
	,
	-5.
	,
	TestID->"DLLTests-20090122-A3L1H2"
]



Test[
	unloadFun[]
	,
	0
	,
	TestID->"DLLTests-20090122-N1V1R8"
]


edll = GetDLLFile["demo_error"];

Test[
	FileType[edll]
	,
	File
	,
	TestID->"DLLTests-20090211-A5E1F4"
]

Test[
	Head[ef1 = LibraryFunctionLoad[edll, "errordemo1", {{Integer, _}, Integer}, Real]]
	,
	LibraryFunction
	,
	TestID->"DLLTests-20090211-Z4J7A6"
]

Test[
	ef1[{2,1}, 1]
	,
	LibraryFunctionError["LIBRARY_USER_ERROR", -304]
	,
	{LibraryFunction::rterr}
	,
	TestID->"DLLTests-20090211-T9W0H6"
]

Test[
	Head[ef2 = LibraryFunctionLoad[edll, "errordemo2", {{Integer, _}, Integer}, Real]]
	,
	LibraryFunction
	,
	TestID->"DLLTests-20090211-Z0O3C9"
]

(* An error is made (wrong tensor type), but it is
   handled by the program by returning 0. *)
Test[
	ef2[{2,1}, 1]
	,
	0.
	,
	TestID->"DLLTests-20090211-B4X3I1"
]

Test[
	Head[ef3 = LibraryFunctionLoad[edll, "errordemo3", {{Integer, _}, Integer}, Real]]
	,
	LibraryFunction
	,
	TestID->"DLLTests-20090211-Z0A4S9"
]

(* An error is made (wrong tensor type), it is caught
   and then it is generated again to give the message. *)
Test[
	ef3[{2,1}, 1]
	,
	LibraryFunctionError["LIBRARY_USER_ERROR", -304]
	,
	{LibraryFunction::rterr}
	,
	TestID->"DLLTests-20090211-X0W7I6"
]

Test[
	Head[ef4 = LibraryFunctionLoad[edll, "errordemo4", {{Real, _},Integer}, Real]]
	,
	LibraryFunction
	,
	TestID->"DLLTests-20090211-Y1M3T2"
]

(* This tests the DLL meassaing capability *)
Test[
	ef4[{3., 2., 1.}, 4]
	,
	LibraryFunctionError["LIBRARY_DIMENSION_ERROR", 3]
	,
	{LibraryFunction::outofrange, LibraryFunction::dimerr}
	,
	TestID->"DLLTests-20090211-K1K7Q5"
]

Test[
	Head[ef5 = LibraryFunctionLoad[edll, "errordemo5", {Integer, Integer, {Integer, _}}, {_, _}]]
	,
	LibraryFunction
	,
	TestID->"DLLTests-20090211-H4Q3R8"
]

Test[
	ef5[2, 1, {5}]
	,
	ConstantArray[0,5]
	,
	TestID->"DLLTests-20090211-K7N9I6"
]

Test[
	Head[customError = LibraryFunctionLoad[edll, "customError", {Integer}, Real]]
	,
	LibraryFunction
	,
	TestID->"LibraryTests-20170818-B3D3Q1"
]

Test[
	customError[42]
	,
	LibraryFunctionError["LIBRARY_USER_ERROR", -1]
	,
	{LibraryFunction::rterr}
	,
	TestID->"LibraryTests-20170818-O3S6S4"
]

Test[
	customError[-42]
	,
	LibraryFunctionError["LIBRARY_USER_ERROR", -152] (* ErrorManagerThrowNameError *)
	,
	{LibraryFunction::rterr}
	,
	TestID->"LibraryTests-20170818-D4D3N0"
]

Test[
	Head[getCErrorCodes = LibraryFunctionLoad[edll, "sendRegisteredErrors", LinkObject, LinkObject]]
	,
	LibraryFunction
	,
	TestID->"LibraryTests-20170818-R2D0U9"
]

Test[
	Head @ getCErrorCodes[]
	,
	Association
	,
	TestID->"LibraryTests-20170818-V9E2F2"
]

Test[
	Length @ getCErrorCodes[]
	,
	53
	,
	TestID->"LibraryTests-20170818-R7K6X2"
]

Test[
	getCErrorCodes[]["TensorTypeError"]
	,
	{ -304, "An error was caused by an MTensor type mismatch." }
	,
	TestID->"LibraryTests-20170818-H4B5A2"
]

$CharacterEncoding = "UTF8";
Test[
	getCErrorCodes[][FromCharacterCode[{261, 281, 69, 114, 114, 111, 114}, "UTF8"]] (* The key is "ąęError" *)
	,
	{ -2, "Let me try non-ASCII: łódź" }
	,
	TestID->"LibraryTests-20170818-G4W2V6"
]

Test[
	Get[FileNameJoin[{FileNameDrop[$CurrentFile, -3], "LibraryLinkUtilities.wl"}]];
	RegisterPacletErrors[edll,
		<|
			"TestError1" -> "Test message 1."
		|>
	];
	Head@CreatePacletFailure["TestError1"]
	,
	Failure
	,
	TestID->"LibraryTests-20170829-f48611"
]

Test[
	CatchLibraryFunctionError[StringQ@"Clearly not an error"]
	,
	True
	,
	TestID->"LibraryTests-20170829-4996a6"
]

Test[
	Head@CatchLibraryFunctionError[LibraryFunctionError["LIBRARY_USER_ERROR", -1]]
	,
	Failure
	,
	TestID->"LibraryTests-20170829-81b559"
]


(* Last two examples in this section will make more sense as soon as we start extracting error descriptions from C++ code *)
Test[
	Head[nonASCIIError = LibraryFunctionLoad[edll, "nonASCIIError", {Integer}, Real]]
	,
	LibraryFunction
	,
	TestID->"LibraryTests-20170818-K0N6G3"
]

Test[
	nonASCIIError[42]
	,
	LibraryFunctionError["LIBRARY_USER_ERROR", -2]
	,
	{LibraryFunction::rterr}
	,
	TestID->"LibraryTests-20170818-E6A4U8"
]

(*$SaveMemoryLimit = $TestMemoryLimit;
$TestMemoryLimit = None; (* The following tests tend to fail if there is any memory constraint. *)

(* This tests the ability to continue when 
   the requested memory cannot be allocated *)
Test[
	Catch[ef5[2, 10, Table[10, {10}]], _SystemException, #2[[1]] &]
	,
	"MemoryAllocationFailure"
	,
	{General::nomem}
	,
	TestID->"DLLTests-20090211-T0T8P7-bug-192992-273269"
]*)

Test[
	Head[ef6 = LibraryFunctionLoad[edll, "errordemo6", {Integer, {Integer, 1, "Shared"}}, "Void"]]
	,
	LibraryFunction
	,
	TestID->"DLLTests-20090211-Q4R5Z2-bug-273269"
]

itc = Developer`ToPackedArray[{0}];

(* Checks that the correct values is put in the reference to itc *)
Test[
	ef6[2^30, itc];
	itc
	,
	{2^30}
	,
	TestID->"DLLTests-20090211-W9L9G4-bug-273269"
]

(* Checks that there was an interrupt *)
Test[
	TimeConstrained[ef6[2^30, itc], 1.]
	,
	$Aborted
	,
	TestID->"DLLTests-20090211-Q8P2G4-bug-273269"
]

(* Checks that the interrupt was caaught in the DLL function *)
Test[
	If[itc[[1]] < 2^30,True,itc[[1]]]
	,
	True
	,
	TestID->"DLLTests-20090211-V1W4E7-bug-273269"
]

$TestMemoryLimit = $SaveMemoryLimit;

Test[
	Head[ef7 = LibraryFunctionLoad[edll, "errorTest_Return", {Integer}, "Void"]]
	,
	LibraryFunction
	,
	TestID->"DLLTests-20100121-V3P0A7"
]

Test[
	ef7[0]
	,
	Null
	,
	TestID->"DLLTests-20100121-D9S9X5"
]

Test[
	ef7[1]
	,
	LibraryFunctionError[ "LIBRARY_TYPE_ERROR", 1]
	,
	{LibraryFunction::typerr}
	,
	TestID->"DLLTests-20100121-Z6D4J3"
]


Test[
	ef7[2]
	,
	LibraryFunctionError[ "LIBRARY_RANK_ERROR", 2]
	,
	{LibraryFunction::rnkerr}
	,
	TestID->"DLLTests-20100121-M1T7T6"
]


Test[
	ef7[3]
	,
	LibraryFunctionError[ "LIBRARY_DIMENSION_ERROR", 3]
	,
	{LibraryFunction::dimerr}
	,
	TestID->"DLLTests-20100121-P1C1M9"
]

Test[
	ef7[4]
	,
	LibraryFunctionError[ "LIBRARY_NUMERICAL_ERROR", 4]
	,
	{LibraryFunction::numerr}
	,
	TestID->"DLLTests-20100121-S3J3E7"
]

Test[
	ef7[5]
	,
	LibraryFunctionError[ "LIBRARY_MEMORY_ERROR", 5]
	,
	{LibraryFunction::memerr}
	,
	TestID->"DLLTests-20100121-E2P6B6"
]

Test[
	ef7[6]
	,
	LibraryFunctionError[ "LIBRARY_FUNCTION_ERROR", 6]
	,
	TestID->"DLLTests-20100121-R2Y6I7"
]

Test[
	ef7[7]
	,
	LibraryFunctionError[ "LIBRARY_VERSION_ERROR", 7]
	,
	{LibraryFunction::verserr}
	,
	TestID->"DLLTests-20100121-G4Q6Y1"
]


(* The next two return the pointer (cast to int)
   of the Args and Res argument respecitvely --
   should return 0 with no args and result respecitvely *)
Test[
	LibraryFunctionLoad[dll, "demoNoArguments", {}, Integer][]
	,
	0
	,
	TestID->"DLLTests-20090211-E9N9E9"
]

Test[
	res = Developer`ToPackedArray[{-99}]; 
	LibraryFunctionLoad[dll, "demoNoResult", {{Integer, _, "Shared"}}, "Void"][res];
	res
	,
	{0}
	,
	TestID->"DLLTests-20090211-Z2W4A3"
]


dllML = GetDLLFile["demo_LinkObject"];

If[dllML == $Failed, dllML == GetDLLFile["demo_WolframLink"]];

Test[
	FileType[ dllML]
	,
	File
	,
	TestID->"DLLTests-20090302-N9D7U8"
]

Test[
	Head[mlFunAdd = LibraryFunctionLoad[dllML, "addtwo", LinkObject, LinkObject]]
	,
	LibraryFunction
	,
	TestID->"DLLTests-20090302-W9G4H0"
]

Test[
	mlFunAdd[4,5]
	,
	9
	,
	TestID->"DLLTests-20090302-Q9S4O0-bug-248088"
]

Test[
	mlFunAdd[4,5,p]
	,
	LibraryFunctionError["LIBRARY_FUNCTION_ERROR", 6]
	,
	TestID->"DLLTests-20090302-T9P8V2"
]

Test[
	Head[mlFunReverse = LibraryFunctionLoad[dllML, "reverseString", LinkObject, LinkObject]]
	,
	LibraryFunction
	,
	TestID->"DLLTests-20090302-E7X2S4"
]

Test[
	mlFunReverse["Foo1"]
	,
	"1ooF"
	,
	TestID->"DLLTests-20090302-W9X6J9"
]



tensorArbitraryFile = GetDLLFile["arbitraryTensor"];

Test[
	Head[funLoad = 
 		LibraryFunctionLoad[ tensorArbitraryFile, 
  			"loadArray", {{_, _, "Shared"}}, "Void"]]
	,
	LibraryFunction
	,
	TestID->"DLLTests-20090713-E3T7G2"
]


Test[
	Head[funUnLoad = 
 		LibraryFunctionLoad[ tensorArbitraryFile, "unloadArray", {}, "Void"]]
	,
	LibraryFunction
	,
	TestID->"DLLTests-20090713-A0Q2K3"
]

Test[
	Head[funGetShared = 
 		LibraryFunctionLoad[ tensorArbitraryFile, 
  			"getElementShared", {{Integer}}, Real]]
	,
	LibraryFunction
	,
	TestID->"DLLTests-20090713-E9F9E0"
]

Test[
	Head[funGetNoShared = 
 		LibraryFunctionLoad[ tensorArbitraryFile, 
  			"getElementNonShared", {{Integer}, {_, _}}, Real]]
	,
	LibraryFunction
	,
	TestID->"DLLTests-20090713-D8V8H1"
]


packDouble = Developer`ToPackedArray[{{1., 2., 3.}}];

Test[
	funLoad[packDouble]
	,
	Null
	,
	TestID->"DLLTests-20090713-R8S2C3"
]


Test[
	funGetShared[0]
	,
	1.
	,
	TestID->"DLLTests-20090713-A2K9Y7"
]

Test[
	funGetShared[1]
	,
	2.
	,
	TestID->"DLLTests-20090713-P6H5N2"
]

Test[
	funUnLoad[]
	,
	Null
	,
	TestID->"DLLTests-20090713-Y3N7H7"
]


Test[
	funLoad[1.5]
	,
	Null
	,
	TestID->"DLLTests-20090713-Z2A9O8"
]


Test[
	funGetShared[0]
	,
	1.5
	,
	TestID->"DLLTests-20090713-H0W5M7"
]


Test[
	funUnLoad[]
	,
	Null
	,
	TestID->"DLLTests-20090713-A4H5L5"
]


Test[
	funGetNoShared[0, packDouble]
	,
	1.
	,
	TestID->"DLLTests-20090713-K9P8Y0"
]


Test[
	funGetNoShared[1, packDouble]
	,
	2.
	,
	TestID->"DLLTests-20090713-B5J9T3"
]

Test[
	funGetNoShared[0, 1.5]
	,
	1.5
	,
	TestID->"DLLTests-20090713-O1Z7H0"
]

Test[
	fun17b = LibraryFunctionLoad[dll, 
   		"demo_TTI_T", {{Integer, _}, {Integer, _}, Integer}, {Integer, _}];
   	arr = {{1, 2, 3, 4}, {-1, -2, -3, -4}};
	new = {10, 11, 12, 13};
	fun17b[arr, new, 2]
	,
	{{1, 2, 3, 4}, {10, 11, 12, 13}}
	,
	TestID->"DLLTests-20090121-O8C2F7"
]

(* Moving these 3 tests to the end since they has been crashing for the past 2 months and causing the rest of the tests afterwards to be skipped. *)

Test[
	fun17a = LibraryFunctionLoad[dll, 
  					 "demo_TI_T", {{Integer, _}, Integer}, {Integer, _}];
   	arr = {{1, 2, 3, 4}, {-1, -2, -3, -4}};
	fun17a[arr, 1]
	,
	{1, 2, 3, 4}
	,
	TestID->"DLLTests-20090121-H5B4R2"
]

Test[
	fun18a = LibraryFunctionLoad[dll, 
  		 "demo_TII_T", {{Integer, _}, Integer, Integer}, {Integer, _}];
  	arr = Table[i + j + k, {i, 3}, {j, 2}, {k, 4}];
	fun18a[arr, 1, 1]
	,
	{3, 4, 5, 6}
	,
	TestID->"DLLTests-20090121-Q2A6R4"
]