(* Wolfram Language Test file *)
TestRequirement[$VersionNumber >= 14.1];
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

	NewNumericDataVector = `LLU`PacletFunctionLoad["newNumericDataVector", {}, DataVector, "Throws" -> False];
	NewStringDataVector = `LLU`PacletFunctionLoad["newStringDataVector", {}, LibraryDataType[DataVector], "Throws" -> False];
	NewBinaryDataVector = `LLU`PacletFunctionLoad["newBinaryDataVector", {}, LibraryDataType[DataVector], "Throws" -> False];
	NewBooleanDataVector = `LLU`PacletFunctionLoad["newBooleanDataVector", {}, LibraryDataType[DataVector], "Throws" -> False];
	NewFixedWidthBinaryDataVector = `LLU`PacletFunctionLoad["newFixedWidthBinaryDataVector", {}, LibraryDataType[DataVector], "Throws" -> False];
	NewDateDataVector = `LLU`PacletFunctionLoad["newDateDataVector", {}, LibraryDataType[DataVector], "Throws" -> False];
	NewTimeDataVector = `LLU`PacletFunctionLoad["newTimeDataVector", {}, LibraryDataType[DataVector], "Throws" -> False];

	PassDataVector = `LLU`PacletFunctionLoad["PassDataVector", {DataVector, "Boolean"}, DataVector];
	NewNumericDV = `LLU`PacletFunctionLoad["NewNumericDV", {NumericArray}, DataVector];
	NewStringDV = `LLU`PacletFunctionLoad["NewStringDV", {"DataStore"}, DataVector];
	NewBinaryDV = `LLU`PacletFunctionLoad["NewBinaryDV", {NumericArray, NumericArray}, DataVector];
	NewBooleanDV = `LLU`PacletFunctionLoad["NewBooleanDV", {}, DataVector];
	NewFixedWidthBinaryDV = `LLU`PacletFunctionLoad["NewFixedWidthBinaryDV", {NumericArray}, DataVector];
	NewDateDV = `LLU`PacletFunctionLoad["NewDateDV", {NumericArray, Integer, Integer, String}, DataVector];
	NewTimeDV = `LLU`PacletFunctionLoad["NewTimeDV", {NumericArray, Integer, Integer}, DataVector];
];



(* Numeric *)
VerificationTest[
	numericDV = NewNumericDataVector[];
	DataVectorQ[numericDV]
	,
	TestID -> "DataVectorTestSuite-FFHFMMDKLH"
];

Test[
	Normal[numericDV]
	,
	{0, 1, 2, Missing[], 4}
	,
	TestID -> "DataVectorTestSuite-EJ1CYNYKDP"
];

VerificationTest[
	MUnit`TestMemoryLeak[NewNumericDataVector[]]
	,
	TestID -> "DataVectorTestSuite-HYTQGCX3IC"
];


(* String *)
VerificationTest[
	stringDV = NewStringDataVector[];
	DataVectorQ[stringDV]
	,
	TestID -> "DataVectorTestSuite-FFHFMMDKLH"
];

Test[
	Normal[stringDV]
	,
	{"one", "two", "three", Missing[], "five"}
	,
	TestID -> "DataVectorTestSuite-EJ1CYNYKDP"
];

VerificationTest[
	MUnit`TestMemoryLeak[NewStringDataVector[]]
	,
	TestID -> "DataVectorTestSuite-HYTQGCX3IC"
];


(* Boolean *)
VerificationTest[
	booleanDV = NewBooleanDataVector[];
	DataVectorQ[booleanDV]
	,
	TestID -> "DataVectorTestSuite-FFHFMMDKLH"
];

Test[
	Normal[booleanDV]
	,
	{True, False, False, Missing[], True}
	,
	TestID -> "DataVectorTestSuite-EJ1CYNYKDP"
];

VerificationTest[
	MUnit`TestMemoryLeak[NewBooleanDataVector[]]
	,
	TestID -> "DataVectorTestSuite-HYTQGCX3IC"
];


(* Binary *)
VerificationTest[
	binaryDV = NewBinaryDataVector[];
	DataVectorQ[binaryDV]
	,
	TestID -> "DataVectorTestSuite-FFHFMMDKLH"
];

Test[
	Normal[binaryDV]
	,
	{ByteArray[{0}], ByteArray[{1, 2}], ByteArray[{3, 4, 5}], Missing[], ByteArray[{6, 7, 8, 9, 10}]}
	,
	TestID -> "DataVectorTestSuite-EJ1CYNYKDP"
];

VerificationTest[
	MUnit`TestMemoryLeak[NewBinaryDataVector[]]
	,
	TestID -> "DataVectorTestSuite-HYTQGCX3IC"
];


(* FixedWidthBinary *)
VerificationTest[
	fixedWidthBinaryDV = NewFixedWidthBinaryDataVector[];
	DataVectorQ[fixedWidthBinaryDV]
	,
	TestID -> "DataVectorTestSuite-FFHFMMDKLH"
];

Test[
	Map[Normal, Normal[fixedWidthBinaryDV]]
	,
	{{0, 1, 2, 3}, {4, 5, 6, 7}, {8, 9, 10, 11}, Missing[], {16, 17, 18, 19}}
	,
	TestID -> "DataVectorTestSuite-EJ1CYNYKDP"
];

VerificationTest[
	MUnit`TestMemoryLeak[NewFixedWidthBinaryDataVector[]]
	,
	TestID -> "DataVectorTestSuite-HYTQGCX3IC"
];


(* Date *)
VerificationTest[
	dateDV = NewDateDataVector[];
	DataVectorQ[dateDV]
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
		Missing[],
		DateObject[{2024, 6, 26}, "Day"]
	}
	,
	TestID -> "DataVectorTestSuite-EJ1CYNYKDP"
];

VerificationTest[
	MUnit`TestMemoryLeak[NewDateDataVector[]]
	,
	TestID -> "DataVectorTestSuite-HYTQGCX3IC"
];


(* Time *)
VerificationTest[
	timeDV = NewTimeDataVector[];
	DataVectorQ[timeDV]
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
		Missing[],
		TimeObject[{6, 22, 41}, "Instant", None]
	}
	,
	TestID -> "DataVectorTestSuite-EJ1CYNYKDP"
];

VerificationTest[
	MUnit`TestMemoryLeak[NewTimeDataVector[]]
	,
	TestID -> "DataVectorTestSuite-HYTQGCX3IC"
];

(* Length *)

Test[
	getLength = LibraryFunctionLoad[lib, "getDataVectorLength", {LibraryDataType[DataVector]}, Integer];
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
	getMissingCount = LibraryFunctionLoad[lib, "getDataVectorMissingCount", {LibraryDataType[DataVector]}, Integer];
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
	getValidity = LibraryFunctionLoad[lib, "getDataVectorValidity", {LibraryDataType[DataVector]}, LibraryDataType[DataVector]];
	validity = getValidity[numericDV];
	DataVectorQ[validity]
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
	getDataNumeric = LibraryFunctionLoad[lib, "getDataNumeric", {LibraryDataType[DataVector]}, LibraryDataType[NumericArray]];
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
	getDataString = LibraryFunctionLoad[lib, "getDataString", {LibraryDataType[DataVector]}, "DataStore"];
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
	getDataBoolean = LibraryFunctionLoad[lib, "getDataBoolean", {LibraryDataType[DataVector]}, LibraryDataType[DataVector]];
	boolData = getDataBoolean[booleanDV];
	DataVectorQ[boolData]
];

Test[
	Normal[boolData]
	,
	{True, False, False, Missing[], True}
];

VerificationTest[
	MUnit`TestMemoryLeak[getDataBoolean[booleanDV]]
	,
	TestID -> "DataVectorTestSuite-HYTQGCX3IC"
];

(* Binary data *)
VerificationTest[
	getDataBinary = LibraryFunctionLoad[lib, "getDataBinary", {LibraryDataType[DataVector]}, LibraryDataType[DataVector]];
	binaryData = getDataBinary[binaryDV];
	DataVectorQ[binaryData]
];

Test[
	Map[Normal, Normal[binaryData]]
	,
	{{0}, {1, 2}, {3, 4, 5}, Missing[], {6, 7, 8, 9, 10}}
];

VerificationTest[
	MUnit`TestMemoryLeak[getDataBinary[binaryDV]]
	,
	TestID -> "DataVectorTestSuite-HYTQGCX3IC"
];


(* Fixed width binary data *)
Test[
	getDataFixedWidthBinary = LibraryFunctionLoad[lib, "getDataFixedWidthBinary", {LibraryDataType[DataVector]}, LibraryDataType[NumericArray]];
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
	getDataDate = LibraryFunctionLoad[lib, "getDataDate", {LibraryDataType[DataVector]}, LibraryDataType[DataVector]];
	dateData = getDataDate[dateDV];
	DataVectorQ[dateData]
];

Test[
	Normal[dateData]
	,
	{
		DateObject[{2024, 1, 18}, "Day"],
		DateObject[{2024, 8, 17}, "Day"],
		DateObject[{2024, 4, 11}, "Day"],
		Missing[],
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
	getDataTime = LibraryFunctionLoad[lib, "getDataTime", {LibraryDataType[DataVector]}, LibraryDataType[DataVector]];
	timeData = getDataTime[timeDV];
	DataVectorQ[timeData]
];

Test[
	Normal[timeData]
	,
	{
		TimeObject[{2, 9, 55}, "Instant", None],
		TimeObject[{21, 2, 17}, "Instant", None],
		TimeObject[{2, 37, 7}, "Instant", None],
		Missing[],
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
	DataVectorQ[numericDV]
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
	DataVectorQ[stringDV]
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
	DataVectorQ[booleanDV]
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
	DataVectorQ[binaryDV]
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
	DataVectorQ[fixedWidthBinaryDV]
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
	dateDV = NewDateDV[NumericArray[{1, 2, 3, 4, 5}, "Integer32"], 2, 3, ""];
	DataVectorQ[dateDV]
	,
	TestID -> "DataVectorTestSuite-FFHFMMDKLH"
];

Test[
	Normal[dateDV]
	,
	{1, 2, 3, 4, 5}
	,
	TestID -> "DataVectorTestSuite-EJ1CYNYKDP"
];

(* Time *)
VerificationTest[
	timeDV = NewTimeDV[NumericArray[{1, 2, 3, 4, 5}, "Integer32"], 2, 3];
	DataVectorQ[timeDV]
	,
	TestID -> "DataVectorTestSuite-FFHFMMDKLH"
];

Test[
	Normal[timeDV]
	,
	{1, 2, 3, 4, 5}
	,
	TestID -> "DataVectorTestSuite-EJ1CYNYKDP"
];