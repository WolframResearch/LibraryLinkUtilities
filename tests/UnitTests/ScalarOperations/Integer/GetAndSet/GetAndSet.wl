Needs["CCompilerDriver`"];
lib = CreateLibrary[{"GetAndSet.cpp"}, "GetAndSet", options];
LLGet = LibraryFunctionLoad[lib,"llGet",{},Integer];
LLSet = LibraryFunctionLoad[lib,"llSet",{Integer},"Void"];
