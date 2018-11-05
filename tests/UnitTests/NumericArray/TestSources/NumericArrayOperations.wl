Needs["CCompilerDriver`"]
lib = CreateLibrary[{"NumericArrayOperations.cpp"}, "NumericArrayOperations", options];
Print[lib];
echoNumericArray = LibraryFunctionLoad[lib,"echoNumericArray",{"NumericArray"},"NumericArray"];
getRALength = LibraryFunctionLoad[lib, "getNumericArrayLength", {"NumericArray"}, Integer];
getRARank = LibraryFunctionLoad[lib, "getNumericArrayRank", {"NumericArray"}, Integer];
newRA = LibraryFunctionLoad[lib, "newNumericArray", {}, "NumericArray"];
cloneRA = LibraryFunctionLoad[lib, "cloneNumericArray", {"NumericArray"}, "NumericArray"];
changeSharedRA  = LibraryFunctionLoad[lib, "changeSharedNumericArray", {{"NumericArray","Shared"}}, "Void"];
getSharedRA = LibraryFunctionLoad[lib, "getSharedNumericArray", {}, "NumericArray"];
accumulateIntegers = LibraryFunctionLoad[lib, "accumulateIntegers", {{"NumericArray", "Constant"}}, Integer];