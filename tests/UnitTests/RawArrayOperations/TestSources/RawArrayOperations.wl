Needs["CCompilerDriver`"];
Needs["LibraryLink`"]
lib = CreateLibrary[{"RawArrayOperations.cpp"} ~Join~ $LLUSources, "RawArrayOperations", options,"Debug"->True];
echoRawArray = LibraryFunctionLoad[lib,"echoRawArray",{"RawArray"},"RawArray"];
getRALength = LibraryFunctionLoad[lib, "getRawArrayLength", {"RawArray"}, Integer];
getRARank = LibraryFunctionLoad[lib, "getRawArrayRank", {"RawArray"}, Integer];
newRA = LibraryFunctionLoad[lib, "newRawArray", {}, "RawArray"];
cloneRA = LibraryFunctionLoad[lib, "cloneRawArray", {"RawArray"}, "RawArray"];
changeSharedRA  = LibraryFunctionLoad[lib, "changeSharedRawArray", {{"RawArray","Shared"}}, "Void"];
getSharedRA = LibraryFunctionLoad[lib, "getSharedRawArray", {}, "RawArray"];