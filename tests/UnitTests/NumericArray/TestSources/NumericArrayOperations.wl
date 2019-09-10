Needs["CCompilerDriver`"]
lib = CreateLibrary[{"NumericArrayOperations.cpp"}, "NumericArrayOperations", options, "Defines" -> {"LLU_LOG_DEBUG"}];
Get[FileNameJoin[{$LLUSharedDir, "LibraryLinkUtilities.wl"}]];
RegisterPacletErrors[lib, <||>];

emptyVector = SafeLibraryFunction["CreateEmptyVector", {}, NumericArray];
emptyMatrix = SafeLibraryFunction["CreateEmptyMatrix", {}, NumericArray];
echoNumericArrays = SafeLibraryFunction["echoNumericArrays",{NumericArray, {NumericArray, "Manual"}, {NumericArray, "Shared"}}, "DataStore"];
getNALength = SafeLibraryFunction["getNumericArrayLength", {NumericArray}, Integer];
getNARank = SafeLibraryFunction["getNumericArrayRank", {NumericArray}, Integer];
newNA = SafeLibraryFunction["newNumericArray", {}, NumericArray];
cloneNA = SafeLibraryFunction["cloneNumericArrays", {{NumericArray, "Constant"}, {NumericArray, "Manual"}, {NumericArray, "Shared"}}, "DataStore"];
changeSharedNA  = SafeLibraryFunction["changeSharedNumericArray", {{NumericArray,"Shared"}}, Integer];
getSharedNA = SafeLibraryFunction["getSharedNumericArray", {}, NumericArray];
accumulateIntegers = SafeLibraryFunction["accumulateIntegers", {{NumericArray, "Constant"}}, Integer];
convertMethodName = SafeLibraryFunction["convertMethodName", {Integer}, String];
convert = SafeLibraryFunction["convert", {{NumericArray, "Constant"}, Integer, Real}, NumericArray];
convertGeneric = SafeLibraryFunction["convertGeneric", {{NumericArray, "Constant"}, Integer, Real}, NumericArray];
testDimensions = SafeLibraryFunction["TestDimensions", {{Integer, 1, "Constant"}}, NumericArray];
testDimensions2 = SafeLibraryFunction["TestDimensions2", {}, "DataStore"];
FlattenThroughList = SafeLibraryFunction["FlattenThroughList", {NumericArray}, NumericArray];
CopyThroughTensor = SafeLibraryFunction["CopyThroughTensor", {NumericArray}, NumericArray];