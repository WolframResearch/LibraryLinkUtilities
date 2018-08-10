Needs["CCompilerDriver`"];
lib = CreateLibrary[{"RealAdd.cpp"}, "RealAdd", options];
RealAdd = LibraryFunctionLoad[lib,"RealAdd",{_Real, _Real}, _Real];
