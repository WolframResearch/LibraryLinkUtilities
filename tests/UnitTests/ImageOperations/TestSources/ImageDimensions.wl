Needs["CCompilerDriver`"];
lib = CreateLibrary[{"ImageDimensions.cpp"} ~Join~ $LLUSources, "ImageDimensions", options];
ImageColumnCount = LibraryFunctionLoad[lib, "ImageColumnCount", { LibraryDataType[Image | Image3D] }, LibraryDataType[Integer] ];
ImageRowCount = LibraryFunctionLoad[lib, "ImageRowCount", { LibraryDataType[Image | Image3D] }, LibraryDataType[Integer] ];
ImageRank = LibraryFunctionLoad[lib, "ImageRank", {LibraryDataType[Image | Image3D] }, LibraryDataType[Integer] ];