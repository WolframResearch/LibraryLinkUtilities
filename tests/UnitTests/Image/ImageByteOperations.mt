(* Wolfram Language Test file *)
TestRequirement[$VersionNumber>=10.4]
(***************************************************************************************************************************************)
				(*
					Set of Testcases to test image apis for library link
				*)
(***************************************************************************************************************************************)
TestExecute[
	testImage = Image[ {{0, 63, 127, 191, 255}, {0, 50, 100, 150, 200}}, "Byte"];
	testImage3D = Image3D[{{{65, 0}, {34, 56}, {233, 1}}, {{255, 10}, {9, 0}, {55, 52}}}, "Byte"];
]

Test[
	res = EchoImage1[testImage3D];
	And[ImageType[res]==="Byte", Length @ Image3DSlices[res] === 2]
	,
	True
	,
	TestID->"ImageByteOperations-20150731-H7U4V0"
]

Test[
	res = EchoImage1[Image[{{60}}, "Byte"]];
	And[ImageType[res]==="Byte",ImageData[res,"Byte"]==={{60}}]
	,
	True
	,
	TestID->"ImageByteOperations-20170804-Y7S1M7"
]

Test[
	res = EchoImage2[testImage];
	And[SameQ[ImageType[testImage],ImageType[res]],
		SameQ[ImageData[testImage,"Byte"],ImageData[res,"Byte"]],
		SameQ[ImageChannels[testImage],ImageChannels[res]],
		SameQ[ImageColorSpace[testImage],ImageColorSpace[res]]
	]
	,
	True
	,
	TestID->"ImageByteOperations-20150731-M0R8N3"
]

Test[
	res = EchoImage2[testImage3D];
	And[SameQ[ImageType[testImage3D],ImageType[res]],
		SameQ[ImageData[testImage3D,"Byte"],ImageData[res,"Byte"]],
		SameQ[ImageChannels[testImage3D],ImageChannels[res]],
		SameQ[ImageColorSpace[testImage3D],ImageColorSpace[res]]
	]
	,
	True
	,
	TestID->"ImageByteOperations-20170804-S2G4R2"
]

Test[
	res = ImageNegate[Image[{{70}},"Byte"]];
	ImageData[res,"Byte"]
	,
	{{185}}
	,
	TestID->"ImageByteOperations-20150803-D9E4U5"
]

Test[
	ImageNegate[testImage3D] === ColorNegate[testImage3D]
	,
	True
	,
	TestID->"ImageByteOperations-20170804-P8M8S1"
]

ExactTest[
	ImageRowCount[testImage]
	,
	2
	,
	TestID->"ImageByteOperations-20150803-W2O8G4"
]

ExactTest[
	ImageRowCount[testImage3D]
	,
	3
	,
	TestID->"ImageByteOperations-20170804-Z9A3L4"
]

ExactTest[
	ImageColumnCount[testImage]
	,
	5
	,
	TestID->"ImageByteOperations-20170804-X1E1E1"
]

ExactTest[
	ImageColumnCount[testImage3D]
	,
	2
	,
	TestID->"ImageByteOperations-20170804-H9L6J9"
]


ExactTest[
	ImageRank[testImage]
	,
	2
	,
	TestID->"ImageByteOperations-20150805-E4V0X3"
]

ExactTest[
	ImageRank[testImage3D]
	,
	3
	,
	TestID->"ImageByteOperations-20170804-T5K0Q6"
]

EndRequirement[]
