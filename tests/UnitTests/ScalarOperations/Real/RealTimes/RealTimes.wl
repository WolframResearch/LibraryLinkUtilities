Needs["CCompilerDriver`"];
lib = CreateLibrary[{"RealTimes.cpp"}, "RealTimes", options];
RealTimes = LibraryFunctionLoad[lib,"RealTimes",{_Real, _Real}, _Real];
