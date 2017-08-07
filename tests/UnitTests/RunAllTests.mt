Needs["MUnit`"]

testFiles = FileNames["*TestSuite.mt", { DirectoryName[$CurrentFile] }, 2];
TestSuite[testFiles];