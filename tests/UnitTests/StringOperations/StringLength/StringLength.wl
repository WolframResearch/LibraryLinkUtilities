Needs["CCompilerDriver`"];
lib = CreateLibrary[{"StringLength.cpp"} ~Join~ $LLUSources, "StringLength", options];
StrLength = LibraryFunctionLoad[lib,"StringLength",{ LibraryDataType[String] }, LibraryDataType[Integer] ];