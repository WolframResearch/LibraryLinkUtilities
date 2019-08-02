Needs["CCompilerDriver`"]
lib = CreateLibrary[{"NumericArrayOperations.cpp"}, "NumericArrayOperations", options];
Get[FileNameJoin[{$LLUSharedDir, "LibraryLinkUtilities.wl"}]];
RegisterPacletErrors[lib, <||>];

emptyVector = SafeLibraryFunction["CreateEmptyVector", {}, NumericArray];
emptyMatrix = SafeLibraryFunction["CreateEmptyMatrix", {}, NumericArray];
echoNumericArray = SafeLibraryFunction["echoNumericArray",{"NumericArray"},"NumericArray"];
getNALength = SafeLibraryFunction["getNumericArrayLength", {"NumericArray"}, Integer];
getNARank = SafeLibraryFunction["getNumericArrayRank", {"NumericArray"}, Integer];
newNA = SafeLibraryFunction["newNumericArray", {}, "NumericArray"];
cloneNA = SafeLibraryFunction["cloneNumericArray", {{"NumericArray", "Constant"}}, "NumericArray"];
changeSharedNA  = SafeLibraryFunction["changeSharedNumericArray", {{"NumericArray","Shared"}}, Integer];
getSharedNA = SafeLibraryFunction["getSharedNumericArray", {}, "NumericArray"];
accumulateIntegers = SafeLibraryFunction["accumulateIntegers", {{"NumericArray", "Constant"}}, Integer];
convertMethodName = SafeLibraryFunction["convertMethodName", {Integer}, String];
convert = SafeLibraryFunction["convert", {{"NumericArray", "Constant"}, Integer, Real}, "NumericArray"];
convertGeneric = SafeLibraryFunction["convertGeneric", {{"NumericArray", "Constant"}, Integer, Real}, "NumericArray"];
testDimensions = SafeLibraryFunction["TestDimensions", {{Integer, 1, "Constant"}}, "NumericArray"];
testDimensions2 = SafeLibraryFunction["TestDimensions2", {}, "DataStore"];