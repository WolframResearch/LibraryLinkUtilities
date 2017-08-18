Needs["CCompilerDriver`"];
lib = CreateLibrary[{"GetAndSet.cpp"} ~Join~ $LLUSources, "GetAndSet", options];
LLGet = LibraryFunctionLoad[lib,"llGet",{},Integer];
LLSet = LibraryFunctionLoad[lib,"llSet",{Integer},"Void"];
