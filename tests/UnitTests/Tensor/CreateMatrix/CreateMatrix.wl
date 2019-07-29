Needs["CCompilerDriver`"];
lib = CreateLibrary[{"CreateMatrix.cpp"}, "CreateMatrix", options];
CreateMatrix = LibraryFunctionLoad[lib,"CreateMatrix",{Integer,Integer},{Integer,2}];
EmptyVector = LibraryFunctionLoad[lib, "CreateEmptyVector", {}, {Integer, 1}];
EmptyMatrix = LibraryFunctionLoad[lib, "CreateEmptyMatrix", {}, {Integer, _}];