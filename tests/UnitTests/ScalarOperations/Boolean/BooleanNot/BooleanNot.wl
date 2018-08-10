Needs["CCompilerDriver`"];
lib = CreateLibrary[{"BooleanNot.cpp"}, "BooleanNot", options];
BooleanNot = LibraryFunctionLoad[lib,"BooleanNot",{"Boolean"},"Boolean"];
