Needs["CCompilerDriver`"];
lib = CreateLibrary[{"EchoTensor.cpp"}, "EchoTensor", options];
EchoTensor = LibraryFunctionLoad[lib, "EchoTensor", {{Integer, _}}, {Integer, _}];
EchoFirst = LibraryFunctionLoad[lib, "EchoFirst", {{Integer, 1}}, Integer];
EchoLast = LibraryFunctionLoad[lib, "EchoLast", {{Integer, 1} }, Integer];
EchoElement = LibraryFunctionLoad[lib, "EchoElement", {{"NumericArray", "Constant"}, {Integer, 1}}, Integer];