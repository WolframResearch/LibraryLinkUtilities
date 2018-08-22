Needs["CCompilerDriver`"];

lib = CreateLibrary[{"HelloWorld.cpp"} ~ Join ~ $LLUSources, "HelloWorld", options];

HelloWorld = LibraryFunctionLoad[lib, "HelloWorld", {}, "UTF8String"];
CapitalizeFirst = LibraryFunctionLoad[lib, "CapitalizeFirst", {"UTF8String"}, "UTF8String"];
CapitalizeAll = LibraryFunctionLoad[lib, "CapitalizeAll", {"UTF8String"}, "UTF8String"];
RoundTripCString = LibraryFunctionLoad[lib, "RoundTripCString", {"UTF8String"}, "UTF8String"];
RoundTripString = LibraryFunctionLoad[lib, "RoundTripString", {"UTF8String"}, "UTF8String"];