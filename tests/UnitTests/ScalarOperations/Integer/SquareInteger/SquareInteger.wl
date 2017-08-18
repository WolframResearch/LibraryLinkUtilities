Needs["CCompilerDriver`"];
lib = CreateLibrary[{"SquareInteger.cpp"} ~Join~ $LLUSources, "SquareInteger", options];
SquareInteger = LibraryFunctionLoad[lib,"SquareInteger",{Integer},Integer];
