Needs["CCompilerDriver`"];
lib = CreateLibrary[{"BooleanOr.cpp"}, "BooleanOr", options];
BooleanOr = LibraryFunctionLoad[lib,"BooleanOr",{"Boolean", "Boolean"},"Boolean"];
