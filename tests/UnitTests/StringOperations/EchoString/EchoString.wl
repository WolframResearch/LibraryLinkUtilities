Needs["CCompilerDriver`"];
lib = CreateLibrary[{"EchoString.cpp"} ~Join~ $LLUSources, "EchoString", options];
EchoString = LibraryFunctionLoad[lib,"EchoString",{ LibraryDataType[String] }, LibraryDataType[String] ];
