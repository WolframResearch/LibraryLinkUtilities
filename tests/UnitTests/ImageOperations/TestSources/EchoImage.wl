ImgEchoLib = CreateLibrary[{"EchoImage.cpp"} ~Join~ $LLUSources, "EchoImage", options];
EchoImage1 = LibraryFunctionLoad[ImgEchoLib, "EchoImage1", { LibraryDataType[Image | Image3D] }, LibraryDataType[Image | Image3D] ];
EchoImage2 = LibraryFunctionLoad[ImgEchoLib, "EchoImage2", { LibraryDataType[Image | Image3D] }, LibraryDataType[Image | Image3D] ];
ConvertImageToByte = LibraryFunctionLoad[ImgEchoLib, "ConvertImageToByte", { LibraryDataType[Image | Image3D] }, LibraryDataType[Image | Image3D] ];
UnifyImageTypes = LibraryFunctionLoad[ImgEchoLib, "UnifyImageTypes", { LibraryDataType[Image | Image3D], LibraryDataType[Image | Image3D] }, LibraryDataType[Image | Image3D] ];