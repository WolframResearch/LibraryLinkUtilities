Needs["CCompilerDriver`"];
lib = CreateLibrary[{"BooleanAnd.cpp"} ~Join~ $LLUSources, "BooleanAnd", options];
BooleanAnd = LibraryFunctionLoad[lib,"BooleanAnd",{"Boolean", "Boolean"},"Boolean"];
