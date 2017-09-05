Needs["MUnit`"]
Needs["CCompilerDriver`"];

currentDirectory = DirectoryName[$CurrentFile];
Get[FileNameJoin[{ParentDirectory[currentDirectory], "TestConfig.wl"}]];
sourceDirectory = FileNameJoin[{currentDirectory, "TestSources"}];

Get[FileNameJoin[{sourceDirectory, "EchoImage.wl"}]];
Get[FileNameJoin[{sourceDirectory, "ImageNegate.wl"}]];
Get[FileNameJoin[{sourceDirectory, "ImageDimensions.wl"}]];


testFiles = FileNames["Image*Operations.mt", { DirectoryName[$CurrentFile] }];
TestSuite[testFiles];

LibraryUnload[ImgEchoLib];
LibraryUnload[ImgNegLib]; (* <-  Kernel crashes if you try to unload both ImgNegLib and ImgDimLib, not sure why *)
LibraryUnload[ImgDimLib];

(*Delete all created libraries(to avoid CopyFile::filex error seen on windows)*)
DeleteFile[ImgEchoLib];
DeleteFile[ImgNegLib];
DeleteFile[ImgDimLib];
