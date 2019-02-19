Needs["CCompilerDriver`"]
lib = CreateLibrary[{"NumericArrayOperations.cpp"}, "NumericArrayOperations", options];
Print[lib];
echoNumericArray = LibraryFunctionLoad[lib,"echoNumericArray",{"NumericArray"},"NumericArray"];
getNALength = LibraryFunctionLoad[lib, "getNumericArrayLength", {"NumericArray"}, Integer];
getNARank = LibraryFunctionLoad[lib, "getNumericArrayRank", {"NumericArray"}, Integer];
newNA = LibraryFunctionLoad[lib, "newNumericArray", {}, "NumericArray"];
cloneNA = LibraryFunctionLoad[lib, "cloneNumericArray", {"NumericArray"}, "NumericArray"];
changeSharedNA  = LibraryFunctionLoad[lib, "changeSharedNumericArray", {{"NumericArray","Shared"}}, "Void"];
getSharedNA = LibraryFunctionLoad[lib, "getSharedNumericArray", {}, "NumericArray"];
accumulateIntegers = LibraryFunctionLoad[lib, "accumulateIntegers", {{"NumericArray", "Constant"}}, Integer];
convert = LibraryFunctionLoad[lib, "convert", {{"NumericArray", "Constant"}, Integer, Real}, "NumericArray"];