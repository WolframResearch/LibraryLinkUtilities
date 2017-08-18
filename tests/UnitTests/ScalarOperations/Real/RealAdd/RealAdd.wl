Needs["CCompilerDriver`"];
lib = CreateLibrary[{"RealAdd.cpp"} ~Join~ $LLUSources, "RealAdd", options];
RealAdd = LibraryFunctionLoad[lib,"RealAdd",{_Real, _Real}, _Real];
