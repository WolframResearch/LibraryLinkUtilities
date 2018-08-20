Needs["CCompilerDriver`"];
lib = CreateLibrary[{"IntegerMatrixTranspose.cpp"}, "IntegerMatrixTranspose", options];
IntegerMatrixTranspose = LibraryFunctionLoad[lib, "IntegerMatrixTranspose", {{Integer, 2}}, {Integer, 2}];
