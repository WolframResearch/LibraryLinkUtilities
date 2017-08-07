Needs["CCompilerDriver`"];
lib = CreateLibrary[{"BooleanNot.cpp"} ~Join~ $LLUSources, "BooleanNot", options];
BooleanNot = LibraryFunctionLoad[lib,"BooleanNot",{"Boolean"},"Boolean"];
