(* Wolfram Language Test file *)
TestRequirement[$VersionNumber>=10.4]
(***************************************************************************************************************************************)
				(*
					Set of Testcases to test image apis for library link
				*)
(***************************************************************************************************************************************)
TestExecute[
	testImage = Image[{{0.1, 0.2, 0.3}, {0.4, 0.5, 0.6}, {0.7, 0.7, 0.9}, {0., 0.8, 0.}}, "Bit"];
	testImage3D = Image3D[{{{0.1, 0.2}, {0.3, 0.4}, {0.5, 0.6}}, {{0.7, 0.7}, {0.9, 0.}, {0.8, 0.}}}, "Bit"];
]

Test[
	res = EchoImage1[Image[{{1}}, "Bit"]];
	And[ImageType[res]==="Bit",ImageData[res]==={{1}}]
	,
	True
	,
	TestID->"ImageBitOperations-20150731-H7U4V0"
]

Test[
	res = EchoImage1[testImage3D];
	And[ImageType[res]==="Bit", res == testImage3D]
	,
	True
	,
	TestID->"ImageBitOperations-20170801-F6T0X6"
]

Test[
	res = EchoImage2[testImage];
	And[SameQ[ImageType[testImage],ImageType[res]],
		SameQ[ImageData[testImage],ImageData[res]],
		SameQ[ImageChannels[testImage],ImageChannels[res]],
		SameQ[ImageColorSpace[testImage],ImageColorSpace[res]]
	]
	,
	True
	,
	TestID->"ImageBitOperations-20150731-M0R8N3"
]

Test[
	ConvertImageToByte[testImage]
	,
	Image[testImage, "Byte"]
	,
	TestID->"ImageBitOperations-20170904-J4P2U1"
];


Test[
	ConvertImageToByte[testImage3D]
	,
	Image3D[testImage3D, "Byte"]
	,
	TestID->"ImageBitOperations-20170904-U9Q4T8"
];

Test[
	UnifyImageTypes[ConvertImageToByte[testImage3D], testImage]
	,
	Image[testImage, "Byte"]
	,
	TestID->"ImageBitOperations-20170904-M7Y3X8"
];


Test[
	UnifyImageTypes[ConvertImageToByte[testImage], testImage3D]
	,
	Image3D[testImage3D, "Byte"]
	,
	TestID->"ImageBitOperations-20170904-O5V0V7"
];

Test[
	ImageNegate[Image[{{1}},"Bit"]]//ImageData
	,
	{{0}}
	,
	TestID->"ImageBitOperations-20150803-D9E4U5"
]

Test[
	ImageNegate[testImage3D] === ColorNegate[testImage3D]
	,
	True
	,
	TestID->"ImageBitOperations-20170804-P8M8S1"
]

ExactTest[
	ImageRowCount[testImage]
	,
	4
	,
	TestID->"ImageBitOperations-20150803-W2O8G4"
]

ExactTest[
	ImageRowCount[testImage3D]
	,
	3
	,
	TestID->"ImageBitOperations-20170801-A8P2V0"
]

ExactTest[
	ImageColumnCount[testImage]
	,
	3
	,
	TestID->"ImageBitOperations-20150803-U7I6G8"
]

ExactTest[
	ImageColumnCount[testImage3D]
	,
	2
	,
	TestID->"ImageBitOperations-20170801-X2K8A4"
]

ExactTest[
	ImageRank[testImage]
	,
	2
	,
	TestID->"ImageBitOperations-20150805-D1S6E6"
]

ExactTest[
	ImageRank[testImage3D]
	,
	3
	,
	TestID->"ImageBitOperations-20170801-M1A4H7"
]


EndRequirement[]
