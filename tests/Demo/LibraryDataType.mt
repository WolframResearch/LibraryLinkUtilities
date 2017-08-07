Test[
	Attributes[LibraryDataType]
	,
	{Protected}
	,
	TestID->"LibraryDataType-20140528-P6H7Q9"
]

Test[
	Options[LibraryDataType]
	,
	{}
	,
	TestID->"LibraryDataType-20140528-U2C1V6"
]

TestExecute[

Get[FileNameJoin[{ FileNameDrop[ $CurrentFile, -1], "DemoTestConfig.wl"}]];

Print[ $Version];

Print[ $CommandLine];

Print[$DllDemoDir];

Off[LibraryFunction::notensor];

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

]

Test[
	FileType[ dll]
	,
	File
	,
	TestID->"LibraryDataType-20140528-E1Q8K3"
]

Test[
	fun0 = LibraryFunctionLoad["demo", "demo_I_I", {LibraryDataType[Integer]}, LibraryDataType[Integer]];
    fun0[20]
	,
	21
	,
	TestID->"LibraryDataType-20140528-R7W4D3"
]

Test[
	fun1 = LibraryFunctionLoad[dll, "demo1_R_R", {LibraryDataType[Real]}, LibraryDataType[Real]];
   	fun1[1.5]
	,
	2.25
	,
	TestID->"LibraryDataType-20140528-T8S7F9"
]

Test[
	fun2 = LibraryFunctionLoad[dll, "demo_TI_R", {LibraryDataType[List], {Integer, 0}}, {Real, 0}];
   	fun2[Range[1., 50], 6]
	,
	6.0
	,
	TestID->"LibraryDataType-20140528-D9A6R6"
]

Test[
	fun3 = LibraryFunctionLoad[dll, "demo_I_T", {{Integer, 0}}, LibraryDataType[List]];
   	fun3[ 6]
   	,
	{2, 4, 6, 8, 10, 12}
	,
	TestID->"LibraryDataType-20140528-V2B3Y8"
]

Test[
	fun4 = LibraryFunctionLoad[dll, "demo_TT_T", {LibraryDataType[List], LibraryDataType[List]}, LibraryDataType[List]];
   	fun4[Range[1., 50], 8]
	,
	8.
	,
	TestID->"LibraryDataType-20140528-D5U3O8"
]

Test[
	fun5 = LibraryFunctionLoad[dll, 
  		 "demoBoolean1", {LibraryDataType[True|False]}, LibraryDataType[True|False]];
 	fun5[ True]
	,
	False
	,
	TestID->"LibraryDataType-20140528-K6K1P3"
]

Test[
	fun5[ False]
	,
	True
	,
	TestID->"LibraryDataType-20140528-G1G7Y3"
]

Test[
	fun6 = LibraryFunctionLoad[dll, 
  		 "demoComplex1", {LibraryDataType[Complex]}, LibraryDataType[Complex]];
 	fun6[ 1. + 1. I]
	,
	1. - 1. I
	,
	TestID->"LibraryDataType-20140528-E6C9J3"
]

Test[
	fun7 = LibraryFunctionLoad[dll, "demoVoid", {LibraryDataType[Integer]}, LibraryDataType[Null]];
 	fun7[  1]
	,
	Null
	,
	TestID->"LibraryDataType-20140528-U0F2P8"
]

Test[
	Head[fun8 = LibraryFunctionLoad["demo_string", "reverseString", {LibraryDataType[String]}, LibraryDataType[String]]]
	,
	LibraryFunction
	,
	TestID->"LibraryDataType-20140528-N4I5P7"
]

Test[
	fun8["abcde"]
	,
	"edcba"
	,
	TestID->"LibraryDataType-20140528-T2C8F0"
]

Test[
	props = LibraryFunctionLoad["demo_sparse", 
  "sparse_properties", {"UTF8String", {LibraryDataType[
     SparseArray]}}, {_, _}];
     s = SparseArray[{{1, 0, 0}, {2 , 0, 3}}];
     props["ExplicitPositions", s]
	,
	{{1, 1}, {2, 1}, {2, 3}}
	,
	TestID->"LibraryDataType-20140528-L8R1W6"
]

NTest[
	fun9 = LibraryFunctionLoad["demo_sparse", 
   "sparse_modify_values", {LibraryDataType[SparseArray], 
    LibraryDataType[List]}, LibraryDataType[SparseArray]];
	Chop[fun9[SparseArray[{{1.1, 0}, {0, 0}}], {1.0}]]
	,
	SparseArray[Automatic, {2, 2}, 0, {1, {{0, 1, 1}, {{1}}}, {1.}}]
	,
	TestID->"LibraryDataType-20140528-Z9Q1H8"
]

Test[
	fun10 = LibraryFunctionLoad["demo_image", 
   "color_negate", {LibraryDataType[Image|Image3D]}, LibraryDataType[Image|Image3D]];
   Head[fun10]
	,
	LibraryFunction
	,
	TestID->"LibraryDataType-20140528-G7I9S4"
]

NTest[
	fun10[Image[List[List[0.1,0.2,0.3],List[0.4,0.5,0.6],List[0.7,0.7,0.9]],"Real",Rule[ColorSpace,Automatic],Rule[Interleaving,None]]]
	,
	Image[List[List[0.9,0.8,0.7],List[0.6,0.5,0.4],List[0.30000000000000004,0.30000000000000004,0.09999999999999998]],"Real",Rule[ColorSpace,Automatic],Rule[Interleaving,None]]
	,
	TestID->"LibraryDataType-20140528-Y8H9D1"
]

Test[
	Head[fun10[Image[RandomReal[1, {10, 10}]]]]
	,
	Image
	,
	TestID->"LibraryDataType-20140528-Y1W6J0"
]
