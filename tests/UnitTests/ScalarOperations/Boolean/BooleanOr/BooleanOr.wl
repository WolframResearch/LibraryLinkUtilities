Needs["CCompilerDriver`"];
lib = CreateLibrary[{"BooleanOr.cpp"} ~Join~ $LLUSources, "BooleanOr", options];
BooleanOr = LibraryFunctionLoad[lib,"BooleanOr",{"Boolean", "Boolean"},"Boolean"];
