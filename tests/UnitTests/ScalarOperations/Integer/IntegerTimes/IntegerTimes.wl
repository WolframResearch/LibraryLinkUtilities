Needs["CCompilerDriver`"];
lib = CreateLibrary[{"IntegerTimes.cpp"} ~Join~ $LLUSources, "IntegerTimes", options];
IntegerTimes = LibraryFunctionLoad[lib,"IntegerTimes",{Integer,Integer},Integer];
