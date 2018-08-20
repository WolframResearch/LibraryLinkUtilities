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
