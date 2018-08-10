Needs["CCompilerDriver`"];
lib = CreateLibrary[{"StringLength.cpp"}, "StringLength", options];
StrLength = LibraryFunctionLoad[lib,"StringLength",{ LibraryDataType[String] }, LibraryDataType[Integer] ];