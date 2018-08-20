Needs["MUnit`"]

currentDirectory = DirectoryName[$CurrentFile];
Get[FileNameJoin[{ParentDirectory[currentDirectory], "TestConfig.wl"}]];

testFiles = FileNames["Scalar*Operations.mt", { DirectoryName[$CurrentFile] }, 2];
TestSuite[testFiles];


