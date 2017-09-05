ImgNegLib = CreateLibrary[{"ImageNegate.cpp"} ~Join~ $LLUSources, "ImageNegate", options ];
ImageNegate = LibraryFunctionLoad[ImgNegLib,"ImageNegate",{ LibraryDataType[Image | Image3D] }, LibraryDataType[Image | Image3D] ];
