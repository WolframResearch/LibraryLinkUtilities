Needs["CCompilerDriver`"];
lib = CreateLibrary[{"MeanValue.cpp"}, "MeanValue", options];
MeanValue = LibraryFunctionLoad[lib,"MeanValue",{{Real,1}},Real];
