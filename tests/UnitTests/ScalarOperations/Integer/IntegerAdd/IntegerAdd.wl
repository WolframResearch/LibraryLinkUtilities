Needs["CCompilerDriver`"];
lib = CreateLibrary[{"IntegerAdd.cpp"} ~Join~ $LLUSources, "IntegerAdd", options];
IntegerAdd = LibraryFunctionLoad[lib,"IntegerAdd",{Integer,Integer},Integer];
