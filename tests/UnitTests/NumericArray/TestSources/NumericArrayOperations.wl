Needs["CCompilerDriver`"];
Needs["LibraryLink`"]
lib = CreateLibrary[{"RawArrayOperations.cpp"}, "RawArrayOperations", options];
echoRawArray = LibraryFunctionLoad[lib,"echoRawArray",{"NumericArray"},"NumericArray"];
getRALength = LibraryFunctionLoad[lib, "getRawArrayLength", {"NumericArray"}, Integer];
getRARank = LibraryFunctionLoad[lib, "getRawArrayRank", {"NumericArray"}, Integer];
newRA = LibraryFunctionLoad[lib, "newRawArray", {}, "NumericArray"];
cloneRA = LibraryFunctionLoad[lib, "cloneRawArray", {"NumericArray"}, "NumericArray"];
changeSharedRA  = LibraryFunctionLoad[lib, "changeSharedRawArray", {{"NumericArray","Shared"}}, "Void"];
getSharedRA = LibraryFunctionLoad[lib, "getSharedRawArray", {}, "NumericArray"];