Needs["CCompilerDriver`"];
lib = CreateLibrary[{"Greetings.cpp"} ~Join~ $LLUSources, "Greetings", options];
Greetings = LibraryFunctionLoad[lib,"Greetings",{"UTF8String"},"UTF8String"];
