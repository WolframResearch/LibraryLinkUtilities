(* Wolfram Language Test file *)
TestRequirement[$VersionNumber>=10.4]
(***************************************************************************************************************************************)
				(*
					Set of Testcases to test image apis for library link
				*)
(***************************************************************************************************************************************)
TestExecute[
	testImage = Image[{{.6, .5, .3}, {.8, .1, .4}}, "Real"];
	real32Num = 1.;
]

Test[
	res = EchoImage1[Image[{{real32Num}}, "Real"]];
	And[ImageType[res]==="Real64",ImageData[res,"Real"]==={{real32Num}}]
	,
	True
	,
	TestID->"ImageRealOperations-20150731-H7U4V0"
]

Test[
	res = EchoImage2[testImage];
	And[SameQ[ImageType[testImage],ImageType[res]],
		SameQ[ImageData[testImage,"Real"],ImageData[res,"Real"]],
		SameQ[ImageChannels[testImage],ImageChannels[res]],
		SameQ[ImageColorSpace[testImage],ImageColorSpace[res]]
	]
	,
	True
	,
	TestID->"ImageRealOperations-20150731-M0R8N3"
]

Test[
	ConvertImageToByte[testImage]
	,
	Image[testImage, "Byte"]
	,
	TestID->"ImageRealOperations-20170904-J4P2U1"
];


Test[
	ConvertImageToByte[testImage3D]
	,
	Image3D[testImage3D, "Byte"]
	,
	TestID->"ImageRealOperations-20170904-U9Q4T8"
];

Test[
	UnifyImageTypes[ConvertImageToByte[testImage3D], testImage]
	,
	Image[testImage, "Byte"]
	,
	TestID->"ImageRealOperations-20170904-M7Y3X8"
];


Test[
	UnifyImageTypes[ConvertImageToByte[testImage], testImage3D]
	,
	Image3D[testImage3D, "Byte"]
	,
	TestID->"ImageRealOperations-20170904-O5V0V7"
];

ExactTest[
	ImageRowCount[testImage]
	,
	2
	,
	TestID->"ImageRealOperations-20150803-W2O8G4"
]

ExactTest[
	ImageColumnCount[testImage]
	,
	3
	,
	TestID->"ImageRealOperations-20150803-V5C3R0"
]

ExactTest[
	ImageRank[testImage]
	,
	2
	,
	TestID->"ImageRealOperations-20150805-Z0C1B7"
]

EndRequirement[]
