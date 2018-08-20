Needs["CCompilerDriver`"];
lib = CreateLibrary[{"IntegerAdd.cpp"}, "IntegerAdd", options];
IntegerAdd = LibraryFunctionLoad[lib,"IntegerAdd",{Integer,Integer},Integer];
