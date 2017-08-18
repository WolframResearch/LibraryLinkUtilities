Needs["CCompilerDriver`"];
lib = CreateLibrary[{"ComplexTimes.cpp"} ~Join~ $LLUSources, "ComplexTimes", options];
ComplexTimes = LibraryFunctionLoad[lib,"ComplexTimes",{_Complex, _Complex}, _Complex];
