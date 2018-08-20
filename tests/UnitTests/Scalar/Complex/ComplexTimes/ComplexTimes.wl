Needs["CCompilerDriver`"];
lib = CreateLibrary[{"ComplexTimes.cpp"}, "ComplexTimes", options];
ComplexTimes = LibraryFunctionLoad[lib,"ComplexTimes",{_Complex, _Complex}, _Complex];
