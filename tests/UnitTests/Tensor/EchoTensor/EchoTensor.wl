Needs["CCompilerDriver`"];
lib = CreateLibrary[{"EchoTensor.cpp"}, "EchoTensor", options];
EchoTensor = LibraryFunctionLoad[lib, "EchoTensor", {{Integer, 1}}, {Integer, 1}];
EchoFirst = LibraryFunctionLoad[lib, "EchoFirst", {{Integer, 1}}, Integer];
EchoLast = LibraryFunctionLoad[lib, "EchoLast", {{Integer, 1} }, Integer];
EchoElement = LibraryFunctionLoad[lib, "EchoElement", {{"NumericArray", "Constant"}, {Integer, 1}}, Integer];