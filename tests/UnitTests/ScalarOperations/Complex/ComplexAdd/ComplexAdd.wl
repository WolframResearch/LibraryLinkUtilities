Needs["CCompilerDriver`"];
lib = CreateLibrary[{"ComplexAdd.cpp"}, "ComplexAdd", options];
ComplexAdd = LibraryFunctionLoad[lib,"ComplexAdd",{_Complex, _Complex}, _Complex];
