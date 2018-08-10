Needs["CCompilerDriver`"];
lib = CreateLibrary[{"EchoString.cpp"}, "EchoString", options];
EchoString = LibraryFunctionLoad[lib,"EchoString",{ LibraryDataType[String] }, LibraryDataType[String] ];
