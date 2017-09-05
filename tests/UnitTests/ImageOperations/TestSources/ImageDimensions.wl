Needs["CCompilerDriver`"];
ImgDimLib = CreateLibrary[{"ImageDimensions.cpp"} ~Join~ $LLUSources, "ImageDimensions", options];
ImageColumnCount = LibraryFunctionLoad[ImgDimLib, "ImageColumnCount", { LibraryDataType[Image | Image3D] }, LibraryDataType[Integer] ];
ImageRowCount = LibraryFunctionLoad[ImgDimLib, "ImageRowCount", { LibraryDataType[Image | Image3D] }, LibraryDataType[Integer] ];
ImageRank = LibraryFunctionLoad[ImgDimLib, "ImageRank", {LibraryDataType[Image | Image3D] }, LibraryDataType[Integer] ];