Needs["CCompilerDriver`"];
lib = CreateLibrary[{"CreateMatrix.cpp"}, "CreateMatrix", options];
CreateMatrix = LibraryFunctionLoad[lib,"CreateMatrix",{Integer,Integer},{Integer,2}];
EmptyVector = LibraryFunctionLoad[lib, "CreateEmptyVector", {}, {Integer, 1}];
EmptyMatrix = LibraryFunctionLoad[lib, "CreateEmptyMatrix", {}, {Integer, _}];
CloneTensor = LibraryFunctionLoad[lib, "CloneTensor", {{_, _, "Constant"}}, {_, _}];
TestDimensions = LibraryFunctionLoad[lib, "TestDimensions", {{Integer, 1, "Constant"}}, {Real, _}];
TestDimensions2 = LibraryFunctionLoad[lib, "TestDimensions2", {}, "DataStore"];