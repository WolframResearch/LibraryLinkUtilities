Needs["CCompilerDriver`"];
lib = CreateLibrary[{"ComplexAdd.cpp"} ~Join~ $LLUSources, "ComplexAdd", options];
ComplexAdd = LibraryFunctionLoad[lib,"ComplexAdd",{_Complex, _Complex}, _Complex];
