ImgNegLib = CreateLibrary[{"ImageNegate.cpp"}, "ImageNegate", options ];
ImageNegate = LibraryFunctionLoad[ImgNegLib,"ImageNegate",{ LibraryDataType[Image | Image3D] }, LibraryDataType[Image | Image3D] ];
