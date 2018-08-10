Needs["CCompilerDriver`"];
lib = CreateLibrary[{"CreateMatrix.cpp"}, "CreateMatrix", options];
CreateMatrix = LibraryFunctionLoad[lib,"CreateMatrix",{Integer,Integer},{Integer,2}];
