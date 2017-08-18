Needs["CCompilerDriver`"];
lib = CreateLibrary[{"ImageNegate.cpp"} ~Join~ $LLUSources, "ImageNegate", options ];
ImageNegate = LibraryFunctionLoad[lib,"ImageNegate",{ LibraryDataType[Image | Image3D] }, LibraryDataType[Image | Image3D] ];
