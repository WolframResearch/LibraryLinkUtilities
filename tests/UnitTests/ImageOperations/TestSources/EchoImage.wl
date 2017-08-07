Needs["CCompilerDriver`"];

lib = CreateLibrary[{"EchoImage.cpp"} ~Join~ $LLUSources, "EchoImage", options];
EchoImage1 = LibraryFunctionLoad[lib, "EchoImage1", { LibraryDataType[Image | Image3D] }, LibraryDataType[Image | Image3D] ];
EchoImage2 = LibraryFunctionLoad[lib, "EchoImage2", { LibraryDataType[Image | Image3D] }, LibraryDataType[Image | Image3D] ];