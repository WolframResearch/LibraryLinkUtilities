(* Wolfram Language Test file *)
TestRequirement[$VersionNumber>=10.4]
(***************************************************************************************************************************************)
				(*
					Set of Testcases to test image apis for library link
				*)
(***************************************************************************************************************************************)
TestExecute[
	testImage = Image[{{0, 63000, 1270, 1910, 25550}, {0, 5000, 10000, 15700,2000}}, "Bit16"];
	testImage3D = Image3D[{{{65535, 0}, {3456, 5678}, {23345, 1}}, {{23425, 10}, {9, 0}, {55555, 5234}}}, "Bit16"];
]

Test[
	res = EchoImage1[Image[{{5666}}, "Bit16"]];
	And[ImageType[res]==="Bit16",ImageData[res,"Bit16"]==={{5666}}]
	,
	True
	,
	TestID->"ImageBit16Operations-20150731-H7U4V0"
]

Test[
	res = EchoImage1[testImage3D];
	And[ImageType[res] === "Bit16", (Length @ Image3DSlices[res]) === 2]
	,
	True
	,
	TestID->"ImageBit16Operations-20170804-L9E1H5"
]

Test[
	CloneImage[testImage]
	,
	testImage
	,
	TestID -> "ImageBit16Operations-20190731-I9W1X5"
]


Test[
	CloneImage[testImage3D]
	,
	testImage3D
	,
	TestID -> "ImageBit16Operations-20190731-O2B9I8"
]

Test[
	res = EchoImage2[testImage];	
	And[SameQ[ImageType[testImage],ImageType[res]],
		SameQ[ImageData[testImage,"Bit16"],ImageData[res,"Bit16"]],
		SameQ[ImageChannels[testImage],ImageChannels[res]],
		SameQ[ImageColorSpace[testImage],ImageColorSpace[res]]
	]
	,
	True
	,
	TestID->"ImageBit16Operations-20150731-M0R8N3"
]

Test[
	res = EchoImage2[testImage3D];
	And[SameQ[ImageType[testImage3D],ImageType[res]],
		SameQ[ImageData[testImage3D,"Bit16"],ImageData[res,"Bit16"]],
		SameQ[ImageChannels[testImage3D],ImageChannels[res]],
		SameQ[ImageColorSpace[testImage3D],ImageColorSpace[res]]
	]
	,
	True
	,
	TestID->"ImageBit16Operations-20170804-J6D3N1"
]

Test[
	ConvertImageToByte[testImage]
	,
	Image[testImage, "Byte"]
	,
	TestID->"ImageBit16Operations-20170904-J4P2U1"
];


Test[
	ConvertImageToByte[testImage3D]
	,
	Image3D[testImage3D, "Byte"]
	,
	TestID->"ImageBit16Operations-20170904-U9Q4T8"
];

Test[
	UnifyImageTypes[ConvertImageToByte[testImage3D], testImage]
	,
	Image[testImage, "Byte"]
	,
	TestID->"ImageBit16Operations-20170904-M7Y3X8"
];


Test[
	UnifyImageTypes[ConvertImageToByte[testImage], testImage3D]
	,
	Image3D[testImage3D, "Byte"]
	,
	TestID->"ImageBit16Operations-20170904-O5V0V7"
];

Test[
	res = ImageNegate[Image[{{0}},"Byte"]];
	ImageData[res,"Bit16"]
	,
	{{65535}}
	,
	TestID->"ImageBit16Operations-20150803-D9E4U5"
]

Test[
	ImageNegate[testImage3D] === ColorNegate[testImage3D]
	,
	True
	,
	TestID->"ImageBit16Operations-20170804-P8M8S1"
]

ExactTest[
	ImageRowCount[testImage]
	,
	2
	,
	TestID->"ImageBit16Operations-20150803-W2O8G4"
]

ExactTest[
	ImageRowCount[testImage3D]
	,
	3
	,
	TestID->"ImageBit16Operations-20170804-Y8O1D1"
]

ExactTest[
	ImageColumnCount[testImage]
	,
	5
	,
	TestID->"ImageBit16Operations-20150803-V5C3R0"
]

ExactTest[
	ImageColumnCount[testImage3D]
	,
	2
	,
	TestID->"ImageBit16Operations-20170804-Z6Z7O7"
]

ExactTest[(*test rank of 2d image*)
	ImageRank[testImage]
	,
	2
	,
	TestID->"ImageBit16Operations-20150805-R9H3F8"
]

ExactTest[(*test rank of 2d image*)
	ImageRank[testImage3D]
	,
	3
	,
	TestID->"ImageBit16Operations-20170804-T4I3M7"
]

EndRequirement[]
