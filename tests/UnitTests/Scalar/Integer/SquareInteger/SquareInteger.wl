Needs["CCompilerDriver`"];
lib = CreateLibrary[{"SquareInteger.cpp"}, "SquareInteger", options];
SquareInteger = LibraryFunctionLoad[lib,"SquareInteger",{Integer},Integer];
