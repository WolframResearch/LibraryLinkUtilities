Needs["CCompilerDriver`"];
lib = CreateLibrary[{"BooleanAnd.cpp"}, "BooleanAnd", options];
BooleanAnd = LibraryFunctionLoad[lib,"BooleanAnd",{"Boolean", "Boolean"},"Boolean"];
