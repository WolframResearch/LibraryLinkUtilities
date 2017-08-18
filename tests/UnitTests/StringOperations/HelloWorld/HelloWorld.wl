Needs["CCompilerDriver`"];
lib = CreateLibrary[{"HelloWorld.cpp"} ~Join~ $LLUSources, "HelloWorld", options];
HelloWorld = LibraryFunctionLoad[lib,"HelloWorld",{},"UTF8String"];
