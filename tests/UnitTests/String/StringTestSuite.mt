(* Wolfram Language Test file *)
(***************************************************************************************************************************************)
				(*
					Set of Testcases to test string apis for library link
				*)
(***************************************************************************************************************************************)
TestExecute[
	currentDirectory = DirectoryName[$CurrentFile];
	Get[FileNameJoin[{ParentDirectory[currentDirectory], "TestConfig.wl"}]];
]

ExactTest[
	Get[FileNameJoin[{currentDirectory, "EchoString", "EchoString.wl"}]];
	EchoString["foo bar"]
	,
	"foo bar"	
	,
	TestID->"StringOperations-20150807-D1D4K5"
]

ExactTest[
	Get[FileNameJoin[{currentDirectory, "Greetings", "Greetings.wl"}]];
	Greetings["wolfram"]
	,
	"Greetings wolfram!"
	,
	TestID->"StringOperations-20150807-X0R7U9"
]

ExactTest[
	Get[FileNameJoin[{currentDirectory, "HelloWorld", "HelloWorld.wl"}]];
	HelloWorld[]
	,
	"Hello World"
	,
	TestID->"StringOperations-20150807-K4Y4G8"
]

Test[
	Get[FileNameJoin[{currentDirectory, "StringLength", "StringLength.wl"}]];
	StrLength["this is my null terminated string"]
	,
	33
	,
	TestID->"StringOperations-20150813-B8G3E7"
]
