Needs["CCompilerDriver`"];
lib = CreateLibrary[{"MeanValue.cpp"} ~Join~ $LLUSources, "MeanValue", options];
MeanValue = LibraryFunctionLoad[lib,"MeanValue",{{Real,1}},Real];
