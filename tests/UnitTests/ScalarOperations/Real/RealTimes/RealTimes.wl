Needs["CCompilerDriver`"];
lib = CreateLibrary[{"RealTimes.cpp"} ~Join~ $LLUSources, "RealTimes", options];
RealTimes = LibraryFunctionLoad[lib,"RealTimes",{_Real, _Real}, _Real];
