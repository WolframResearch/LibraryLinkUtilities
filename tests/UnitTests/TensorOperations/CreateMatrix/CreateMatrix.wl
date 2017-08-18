Needs["CCompilerDriver`"];
lib = CreateLibrary[{"CreateMatrix.cpp"} ~Join~ $LLUSources, "CreateMatrix", options];
CreateMatrix = LibraryFunctionLoad[lib,"CreateMatrix",{Integer,Integer},{Integer,2}];
