Needs["CCompilerDriver`"];
lib = CreateLibrary[{"IntegerTimes.cpp"}, "IntegerTimes", options];
IntegerTimes = LibraryFunctionLoad[lib,"IntegerTimes",{Integer,Integer},Integer];
