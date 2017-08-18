Needs["CCompilerDriver`"];
lib = CreateLibrary[{"EchoTensor.cpp"} ~Join~ $LLUSources, "EchoTensor", options];
EchoTensor = LibraryFunctionLoad[lib,"EchoTensor",{ {Integer,1} }, {Integer,1} ];
