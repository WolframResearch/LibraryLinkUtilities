Needs["CCompilerDriver`"];
lib = CreateLibrary[{"Greetings.cpp"}, "Greetings", options];
Greetings = LibraryFunctionLoad[lib,"Greetings",{"UTF8String"},"UTF8String"];
