Needs["CCompilerDriver`"]
lib = CreateLibrary[{"NumericArrayOperations.cpp"}, "NumericArrayOperations", options];
Get[FileNameJoin[{$LLUSharedDir, "LibraryLinkUtilities.wl"}]];
RegisterPacletErrors[lib, <||>];

echoNumericArray = SafeLibraryFunction["echoNumericArray",{"NumericArray"},"NumericArray"];
getNALength = SafeLibraryFunction["getNumericArrayLength", {"NumericArray"}, Integer];
getNARank = SafeLibraryFunction["getNumericArrayRank", {"NumericArray"}, Integer];
newNA = SafeLibraryFunction["newNumericArray", {}, "NumericArray"];
cloneNA = SafeLibraryFunction["cloneNumericArray", {"NumericArray"}, "NumericArray"];
changeSharedNA  = SafeLibraryFunction["changeSharedNumericArray", {{"NumericArray","Shared"}}, "Void"];
getSharedNA = SafeLibraryFunction["getSharedNumericArray", {}, "NumericArray"];
accumulateIntegers = SafeLibraryFunction["accumulateIntegers", {{"NumericArray", "Constant"}}, Integer];
convertMethodName = SafeLibraryFunction["convertMethodName", {Integer}, String];
convert = SafeLibraryFunction["convert", {{"NumericArray", "Constant"}, Integer, Real}, "NumericArray"];