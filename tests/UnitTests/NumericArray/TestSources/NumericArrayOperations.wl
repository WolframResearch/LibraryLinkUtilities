Needs["CCompilerDriver`"]
lib = CreateLibrary[{"NumericArrayOperations.cpp"}, "NumericArrayOperations", options, "Defines" -> {"LLU_LOG_DEBUG"}];
Get[FileNameJoin[{$LLUSharedDir, "LibraryLinkUtilities.wl"}]];
`LLU`InitializePacletLibrary[lib];

emptyVector = `LLU`SafeLibraryFunction["CreateEmptyVector", {}, NumericArray];
emptyMatrix = `LLU`SafeLibraryFunction["CreateEmptyMatrix", {}, NumericArray];
echoNumericArrays = `LLU`SafeLibraryFunction["echoNumericArrays", {NumericArray, {NumericArray, "Manual"}, {NumericArray, "Shared"}}, "DataStore"];
getNALength = `LLU`SafeLibraryFunction["getNumericArrayLength", {NumericArray}, Integer];
getNARank = `LLU`SafeLibraryFunction["getNumericArrayRank", {NumericArray}, Integer];
newNA = `LLU`SafeLibraryFunction["newNumericArray", {}, NumericArray];
cloneNA = `LLU`SafeLibraryFunction["cloneNumericArrays", {{NumericArray, "Constant"}, {NumericArray, "Manual"}, {NumericArray, "Shared"}}, "DataStore"];
changeSharedNA = `LLU`SafeLibraryFunction["changeSharedNumericArray", {{NumericArray, "Shared"}}, Integer];
getSharedNA = `LLU`SafeLibraryFunction["getSharedNumericArray", {}, NumericArray];
accumulateIntegers = `LLU`SafeLibraryFunction["accumulateIntegers", {{NumericArray, "Constant"}}, Integer];
convertMethodName = `LLU`SafeLibraryFunction["convertMethodName", {Integer}, String];
convert = `LLU`SafeLibraryFunction["convert", {{NumericArray, "Constant"}, Integer, Real}, NumericArray];
convertGeneric = `LLU`SafeLibraryFunction["convertGeneric", {{NumericArray, "Constant"}, Integer, Real}, NumericArray];
testDimensions = `LLU`SafeLibraryFunction["TestDimensions", {{Integer, 1, "Constant"}}, NumericArray];
testDimensions2 = `LLU`SafeLibraryFunction["TestDimensions2", {}, "DataStore"];
FlattenThroughList = `LLU`SafeLibraryFunction["FlattenThroughList", {NumericArray}, NumericArray];
CopyThroughTensor = `LLU`SafeLibraryFunction["CopyThroughTensor", {NumericArray}, NumericArray];
GetLargest = `LLU`SafeLibraryFunction["GetLargest", {NumericArray, {NumericArray, "Constant"}, {NumericArray, "Manual"}}, Integer];
EmptyView = `LLU`SafeLibraryFunction["EmptyView", {}, {Integer, 1}];
SumLargestDimensions = `LLU`SafeLibraryFunction["SumLargestDimensions", {NumericArray, {NumericArray, "Constant"}}, Integer];
ReverseNA = `LLU`SafeLibraryFunction["Reverse", {{NumericArray, "Constant"}}, NumericArray];