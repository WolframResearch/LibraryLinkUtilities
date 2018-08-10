Needs["CCompilerDriver`"];

lib = CreateLibrary[{"ScalarOperations.cpp"}, "ScalarOperations", options];
getNthRealFromTR1 = LibraryFunctionLoad[lib, "getNthRealFromTR1", {{Real, 1}, Integer}, {Real}];
getNthRealFromTR2 = LibraryFunctionLoad[lib, "getNthRealFromTR2", {{Real, 2}, Integer, Integer}, {Real}];
getNthIntegerFromTR2 = LibraryFunctionLoad[lib, "getNthIntegerFromTR2", {{Integer, 2}, Integer, Integer}, {Integer}];
setNthIntegerT = LibraryFunctionLoad[lib, "setNthIntegerT", {Integer}, {Integer, 1}];
setI0I1T = LibraryFunctionLoad[lib, "setI0I1T", {{Integer, 1}, {Integer, 1}, Integer, Integer}, {Integer, 1}];
getSubpartT = LibraryFunctionLoad[lib, "getSubpartT", {{Integer, 1}, Integer, Integer}, {Integer, 1}];