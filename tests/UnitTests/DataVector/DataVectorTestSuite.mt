(* Wolfram Language Test file *)
TestRequirement[$VersionNumber >= 14.2];
(***************************************************************************************************************************************)
(*
	Set of test cases to test LLU functionality related to DataVector
*)
(***************************************************************************************************************************************)
TestExecute[
	Needs["CCompilerDriver`"];
	currentDirectory = DirectoryName[$TestFileName];

	(* Get configuration (path to LLU sources, compilation options, etc.) *)
	Get[FileNameJoin[{ParentDirectory[currentDirectory], "TestConfig.wl"}]];

	(* Compile the test library *)
	lib = CCompilerDriver`CreateLibrary[FileNameJoin[{currentDirectory, "TestSources", #}]& /@ {"DataVectorTest.cpp"}, "DataVectorTest", options];

	Get[FileNameJoin[{$LLUSharedDir, "LibraryLinkUtilities.wl"}]];

	`LLU`InitializePacletLibrary[lib];

	NewNumericTabularColumn = `LLU`PacletFunctionLoad["newNumericTabularColumn", {}, TabularColumn, "Throws" -> False];
	NewStringTabularColumn = `LLU`PacletFunctionLoad["newStringTabularColumn", {}, TabularColumn, "Throws" -> False];
	NewBinaryTabularColumn = `LLU`PacletFunctionLoad["newByteArrayTabularColumn", {}, TabularColumn, "Throws" -> False];
	NewBooleanTabularColumn = `LLU`PacletFunctionLoad["newBooleanTabularColumn", {}, TabularColumn, "Throws" -> False];
	NewFixedWidthBinaryTabularColumn = `LLU`PacletFunctionLoad["newFixedWidthByteArrayTabularColumn", {}, TabularColumn, "Throws" -> False];
	NewDateTabularColumn = `LLU`PacletFunctionLoad["newDateTabularColumn", {}, TabularColumn, "Throws" -> False];
	NewTimeTabularColumn = `LLU`PacletFunctionLoad["newTimeTabularColumn", {}, TabularColumn, "Throws" -> False];

	PassDataVector = `LLU`PacletFunctionLoad["PassDataVector", {TabularColumn, "Boolean"}, TabularColumn];
	NewNumericDV = `LLU`PacletFunctionLoad["NewNumericDV", {NumericArray}, TabularColumn];
	NewStringDV = `LLU`PacletFunctionLoad["NewStringDV", {"DataStore"}, TabularColumn];
	NewBinaryDV = `LLU`PacletFunctionLoad["NewBinaryDV", {NumericArray, {Integer, 1, "Constant"}}, TabularColumn];
	NewBooleanDV = `LLU`PacletFunctionLoad["NewBooleanDV", {}, TabularColumn];
	NewFixedWidthBinaryDV = `LLU`PacletFunctionLoad["NewFixedWidthBinaryDV", {NumericArray}, TabularColumn];
	NewDateDV = `LLU`PacletFunctionLoad["NewDateDV", {NumericArray, Integer, Integer, String}, TabularColumn];
	NewTimeDV = `LLU`PacletFunctionLoad["NewTimeDV", {NumericArray, Integer, Integer}, TabularColumn];
];

(* Numeric *)
VerificationTest[
	numericDV = NewNumericTabularColumn[];
	TabularColumnQ[numericDV]
	,
	TestID -> "DataVectorTestSuite-FFHFMMDKLH"
];

Test[
	Normal[numericDV]
	,
	{0, 1, 2, Missing["NotAvailable"], 4}
	,
	TestID -> "DataVectorTestSuite-EJ1CYNYKDP"
];

VerificationTest[
	MUnit`TestMemoryLeak[NewNumericTabularColumn[]]
	,
	TestID -> "DataVectorTestSuite-HYTQGCX3IC"
];


(* String *)
VerificationTest[
	stringDV = NewStringTabularColumn[];
	TabularColumnQ[stringDV]
	,
	TestID -> "DataVectorTestSuite-FFHFMMDKLH"
];

Test[
	Normal[stringDV]
	,
	{"one", "two", "three", Missing["NotAvailable"], "five"}
	,
	TestID -> "DataVectorTestSuite-EJ1CYNYKDP"
];

VerificationTest[
	MUnit`TestMemoryLeak[NewStringTabularColumn[]]
	,
	TestID -> "DataVectorTestSuite-HYTQGCX3IC"
];


(* Boolean *)
VerificationTest[
	booleanDV = NewBooleanTabularColumn[];
	TabularColumnQ[booleanDV]
	,
	TestID -> "DataVectorTestSuite-FFHFMMDKLH"
];

Test[
	Normal[booleanDV]
	,
	{True, False, False, Missing["NotAvailable"], True}
	,
	TestID -> "DataVectorTestSuite-EJ1CYNYKDP"
];

VerificationTest[
	MUnit`TestMemoryLeak[NewBooleanTabularColumn[]]
	,
	TestID -> "DataVectorTestSuite-HYTQGCX3IC"
];


(* Binary *)
VerificationTest[
	binaryDV = NewBinaryTabularColumn[];
	TabularColumnQ[binaryDV]
	,
	TestID -> "DataVectorTestSuite-FFHFMMDKLH"
];

Test[
	Normal[binaryDV]
	,
	{ByteArray[{0}], ByteArray[{1, 2}], ByteArray[{3, 4, 5}], Missing["NotAvailable"], ByteArray[{6, 7, 8, 9, 10}]}
	,
	TestID -> "DataVectorTestSuite-EJ1CYNYKDP"
];

VerificationTest[
	MUnit`TestMemoryLeak[NewBinaryTabularColumn[]]
	,
	TestID -> "DataVectorTestSuite-HYTQGCX3IC"
];


(* FixedWidthBinary *)
VerificationTest[
	fixedWidthBinaryDV = NewFixedWidthBinaryTabularColumn[];
	TabularColumnQ[fixedWidthBinaryDV]
	,
	TestID -> "DataVectorTestSuite-FFHFMMDKLH"
];

Test[
	Map[Normal, Normal[fixedWidthBinaryDV]]
	,
	{{0, 1, 2, 3}, {4, 5, 6, 7}, {8, 9, 10, 11}, Missing["NotAvailable"], {16, 17, 18, 19}}
	,
	TestID -> "DataVectorTestSuite-EJ1CYNYKDP"
];

VerificationTest[
	MUnit`TestMemoryLeak[NewFixedWidthBinaryTabularColumn[]]
	,
	TestID -> "DataVectorTestSuite-HYTQGCX3IC"
];


(* Date *)
VerificationTest[
	dateDV = NewDateTabularColumn[];
	TabularColumnQ[dateDV]
	,
	TestID -> "DataVectorTestSuite-FFHFMMDKLH"
];

Test[
	Normal[dateDV]
	,
	{
		DateObject[{2024, 1, 18}, "Day"],
		DateObject[{2024, 8, 17}, "Day"],
		DateObject[{2024, 4, 11}, "Day"],
		Missing["NotAvailable"],
		DateObject[{2024, 6, 26}, "Day"]
	}
	,
	TestID -> "DataVectorTestSuite-EJ1CYNYKDP"
];

VerificationTest[
	MUnit`TestMemoryLeak[NewDateTabularColumn[]]
	,
	TestID -> "DataVectorTestSuite-HYTQGCX3IC"
];


(* Time *)
VerificationTest[
	timeDV = NewTimeTabularColumn[];
	TabularColumnQ[timeDV]
	,
	TestID -> "DataVectorTestSuite-FFHFMMDKLH"
];

Test[
	Normal[timeDV]
	,
	{
		TimeObject[{2, 9, 55}, "Instant", None],
		TimeObject[{21, 2, 17}, "Instant", None],
		TimeObject[{2, 37, 7}, "Instant", None],
		Missing["NotAvailable"],
		TimeObject[{6, 22, 41}, "Instant", None]
	}
	,
	TestID -> "DataVectorTestSuite-EJ1CYNYKDP"
];

VerificationTest[
	MUnit`TestMemoryLeak[NewTimeTabularColumn[]]
	,
	TestID -> "DataVectorTestSuite-HYTQGCX3IC"
];

(* Length *)

Test[
	getLength = LibraryFunctionLoad[lib, "getTabularColumnLength", {TabularColumn}, Integer];
	getLength[numericDV]
	,
	5
];

VerificationTest[
	MUnit`TestMemoryLeak[getLength[numericDV]]
	,
	TestID -> "DataVectorTestSuite-HYTQGCX3IC"
];

(* Missing count *)
Test[
	getMissingCount = LibraryFunctionLoad[lib, "getTabularColumnMissingCount", {TabularColumn}, Integer];
	getMissingCount[numericDV]
	,
	1
];

VerificationTest[
	MUnit`TestMemoryLeak[getMissingCount[numericDV]]
	,
	TestID -> "DataVectorTestSuite-HYTQGCX3IC"
];

(* Validity *)
VerificationTest[
	getValidity = LibraryFunctionLoad[lib, "getTabularColumnValidity", {TabularColumn}, TabularColumn];
	validity = getValidity[numericDV];
	TabularColumnQ[validity]
];

Test[
	Normal[validity]
	,
	{True, True, True, False, True}
];

VerificationTest[
	MUnit`TestMemoryLeak[getValidity[numericDV]]
	,
	TestID -> "DataVectorTestSuite-HYTQGCX3IC"
];


(* Numeric data *)
Test[
	getDataNumeric = LibraryFunctionLoad[lib, "getDataNumeric", {TabularColumn}, NumericArray];
	getDataNumeric[numericDV]
	,
	NumericArray[{0, 1, 2, 3, 4}, "Integer8"]
];

VerificationTest[
	MUnit`TestMemoryLeak[getDataNumeric[numericDV]]
	,
	TestID -> "DataVectorTestSuite-HYTQGCX3IC"
];

(* String data *)
Test[
	getDataString = LibraryFunctionLoad[lib, "getDataString", {TabularColumn}, "DataStore"];
	getDataString[stringDV]
	,
	Developer`DataStore["one", "two", "three", "", "five"]
];

VerificationTest[
	MUnit`TestMemoryLeak[getDataString[stringDV]]
	,
	TestID -> "DataVectorTestSuite-HYTQGCX3IC"
];

(* Boolean data *)
VerificationTest[
	getDataBoolean = LibraryFunctionLoad[lib, "getDataBoolean", {TabularColumn}, TabularColumn];
	boolData = getDataBoolean[booleanDV];
	TabularColumnQ[boolData]
];

Test[
	Normal[boolData]
	,
	{True, False, False, Missing["NotAvailable"], True}
];

VerificationTest[
	MUnit`TestMemoryLeak[getDataBoolean[booleanDV]]
	,
	TestID -> "DataVectorTestSuite-HYTQGCX3IC"
];

(* Binary data *)
VerificationTest[
	getDataBinary = LibraryFunctionLoad[lib, "getDataBinary", {TabularColumn}, TabularColumn];
	binaryData = getDataBinary[binaryDV];
	TabularColumnQ[binaryData]
];

Test[
	Map[Normal, Normal[binaryData]]
	,
	{{0}, {1, 2}, {3, 4, 5}, Missing["NotAvailable"], {6, 7, 8, 9, 10}}
];

VerificationTest[
	MUnit`TestMemoryLeak[getDataBinary[binaryDV]]
	,
	TestID -> "DataVectorTestSuite-HYTQGCX3IC"
];


(* Fixed width binary data *)
Test[
	getDataFixedWidthBinary = LibraryFunctionLoad[lib, "getDataFixedWidthBinary", {TabularColumn}, NumericArray];
	getDataFixedWidthBinary[fixedWidthBinaryDV]
	,
	NumericArray[{{0, 1, 2, 3}, {4, 5, 6, 7}, {8, 9, 10, 11}, {0, 0, 0, 0}, {16, 17, 18, 19}}, "UnsignedInteger8"]
];

VerificationTest[
	MUnit`TestMemoryLeak[getDataFixedWidthBinary[fixedWidthBinaryDV]]
	,
	TestID -> "DataVectorTestSuite-HYTQGCX3IC"
];

(* Date data *)
VerificationTest[
	getDataDate = LibraryFunctionLoad[lib, "getDataDate", {TabularColumn}, TabularColumn];
	dateData = getDataDate[dateDV];
	TabularColumnQ[dateData]
];

Test[
	Normal[dateData]
	,
	{
		DateObject[{2024, 1, 18}, "Day"],
		DateObject[{2024, 8, 17}, "Day"],
		DateObject[{2024, 4, 11}, "Day"],
		Missing["NotAvailable"],
		DateObject[{2024, 6, 26}, "Day"]
	}
];

VerificationTest[
	MUnit`TestMemoryLeak[getDataDate[dateDV]]
	,
	TestID -> "DataVectorTestSuite-HYTQGCX3IC"
];

(* Time data *)
VerificationTest[
	getDataTime = LibraryFunctionLoad[lib, "getDataTime", {TabularColumn}, TabularColumn];
	timeData = getDataTime[timeDV];
	TabularColumnQ[timeData]
];

Test[
	Normal[timeData]
	,
	{
		TimeObject[{2, 9, 55}, "Instant", None],
		TimeObject[{21, 2, 17}, "Instant", None],
		TimeObject[{2, 37, 7}, "Instant", None],
		Missing["NotAvailable"],
		TimeObject[{6, 22, 41}, "Instant", None]
	}
];

VerificationTest[
	MUnit`TestMemoryLeak[getDataTime[timeDV]]
	,
	TestID -> "DataVectorTestSuite-HYTQGCX3IC"
];


(*********************************************************************************************************************)
(*********************************************************************************************************************)
(*                                                   LLU wrappers                                                    *)
(*********************************************************************************************************************)
(*********************************************************************************************************************)


(* Numeric *)
VerificationTest[
	numericDV = NewNumericDV[NumericArray[{1, 2, 3, 4, 5}, "Integer32"]];
	TabularColumnQ[numericDV]
	,
	TestID -> "DataVectorTestSuite-FFHFMMDKLH"
];

Test[
	Normal[numericDV]
	,
	{1, 2, 3, 4, 5}
	,
	TestID -> "DataVectorTestSuite-EJ1CYNYKDP"
];

(* String *)
VerificationTest[
	stringDV = NewStringDV[Developer`DataStore["ala", "ma", "kota"]];
	TabularColumnQ[stringDV]
	,
	TestID -> "DataVectorTestSuite-FFHFMMDKLH"
];

Test[
	Normal[stringDV]
	,
	{"ala", "ma", "kota"}
	,
	TestID -> "DataVectorTestSuite-EJ1CYNYKDP"
];

(* Boolean *)
VerificationTest[
	booleanDV = NewBooleanDV[];
	TabularColumnQ[booleanDV]
	,
	TestID -> "DataVectorTestSuite-FFHFMMDKLH"
];

Test[
	Normal[booleanDV]
	,
	{True, False, False, False}
	,
	TestID -> "DataVectorTestSuite-EJ1CYNYKDP"
];

(* Binary *)
VerificationTest[
	binaryDV = NewBinaryDV[ByteArray @ Range[6], {1, 2, 3}];
	TabularColumnQ[binaryDV]
	,
	TestID -> "DataVectorTestSuite-FFHFMMDKLH"
];

Test[
	Normal[binaryDV]
	,
	{ByteArray[{1}], ByteArray[{2, 3}], ByteArray[{4, 5, 6}]}
	,
	TestID -> "DataVectorTestSuite-EJ1CYNYKDP"
];

(* FixedWidthBinary *)
VerificationTest[
	fixedWidthBinaryDV = NewFixedWidthBinaryDV[NumericArray[{{1, 2}, {3, 4}, {5, 6}}, "UnsignedInteger8"]];
	TabularColumnQ[fixedWidthBinaryDV]
	,
	TestID -> "DataVectorTestSuite-FFHFMMDKLH"
];

Test[
	Normal[fixedWidthBinaryDV]
	,
	{ByteArray[{1, 2}], ByteArray[{3, 4}], ByteArray[{5, 6}]}
	,
	TestID -> "DataVectorTestSuite-EJ1CYNYKDP"
];

(* Date *)
VerificationTest[
	dateDV = NewDateDV[NumericArray[{1, 2, 3, 4, 5}, "Integer32"], -40, -1, ""];
	TabularColumnQ[dateDV]
	,
	TestID -> "DataVectorTestSuite-FFHFMMDKLH"
];

Test[
	dateDV["RawData"] // Normal
	,
	{1, 2, 3, 4, 5}
	,
	TestID -> "DataVectorTestSuite-EJ1CYNYKDP"
];

(* Time *)
VerificationTest[
	timeDV = NewTimeDV[NumericArray[{1, 2, 3, 4, 5}, "Integer32"],-40, -1];
	TabularColumnQ[timeDV]
	,
	TestID -> "DataVectorTestSuite-FFHFMMDKLH"
];

Test[
	timeDV["RawData"] // Normal
	,
	{1, 2, 3, 4, 5}
	,
	TestID -> "DataVectorTestSuite-EJ1CYNYKDP"
];