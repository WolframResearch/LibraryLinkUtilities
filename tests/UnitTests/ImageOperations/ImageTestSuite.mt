Needs["MUnit`"]

currentDirectory = DirectoryName[$CurrentFile];
Get[FileNameJoin[{ParentDirectory[currentDirectory], "TestConfig.wl"}]];
sourceDirectory = FileNameJoin[{currentDirectory, "TestSources"}];

Get[FileNameJoin[{sourceDirectory, "EchoImage.wl"}]];
Get[FileNameJoin[{sourceDirectory, "ImageNegate.wl"}]];
Get[FileNameJoin[{sourceDirectory, "ImageDimensions.wl"}]];


testFiles = FileNames["Image*Operations.mt", { DirectoryName[$CurrentFile] }];
TestSuite[testFiles];


LibraryFunctionUnload[EchoImage1];
LibraryFunctionUnload[EchoImage2];
LibraryFunctionUnload[ImageNegate];
LibraryFunctionUnload[ImageRowCount];
LibraryFunctionUnload[ImageColumnCount];
LibraryFunctionUnload[ImageRank];
(*Delete all created libraries(to avoid CopyFile::filex error seen on windows)*)
DeleteFile[FindLibrary["EchoImage"]];
DeleteFile[FindLibrary["ImageNegate"]];
DeleteFile[FindLibrary["ImageDimensions"]];

