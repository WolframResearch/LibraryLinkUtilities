Needs["CCompilerDriver`"];
lib = CreateLibrary[{"HelloWorld.cpp"}, "HelloWorld", options];
HelloWorld = LibraryFunctionLoad[lib,"HelloWorld",{},"UTF8String"];
