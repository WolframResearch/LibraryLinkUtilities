(* Wolfram Language Test file *)
TestRequirement[$VersionNumber>=10.4]
(***************************************************************************************************************************************)
				(*
					Set of Testcases to test image apis for library link
				*)
(***************************************************************************************************************************************)
TestExecute[
	testImage = Image[{{0.,.3, .42, .99, .67}, {0., .5, .8, .31,.2}}, "Real32"];
	testImage3D = Image3D[{{{0.1, 0.2}, {0.3, 0.4}, {0.5, 0.6}}, {{0.7, 0.7}, {0.9, 0.}, {0.8, 0.}}}, "Real32"];
	real32 = 1.;
]

Test[
	res = EchoImage1[Image[{{real32}}, "Real32"]];
	And[ImageType[res]==="Real32",ImageData[res,"Real32"]==={{real32}}]
	,
	True
	,
	TestID->"ImageReal32Operations-20150731-H7U4V0"
]

Test[
	res = EchoImage1[testImage3D];
	And[ImageType[res]==="Real32", Length @ Image3DSlices[res] === 2]
	,
	True
	,
	TestID->"ImageReal32Operations-20170804-E8S0A7"
]

Test[
	res = EchoImage2[testImage];
	And[SameQ[ImageType[testImage],ImageType[res]],
		SameQ[ImageData[testImage,"Real32"],ImageData[res,"Real32"]],
		SameQ[ImageChannels[testImage],ImageChannels[res]],
		SameQ[ImageColorSpace[testImage],ImageColorSpace[res]]
	]
	,
	True
	,
	TestID->"ImageReal32Operations-20150731-M0R8N3"
]

Test[
	res = EchoImage2[testImage3D];
	And[SameQ[ImageType[testImage3D],ImageType[res]],
		SameQ[ImageData[testImage3D,"Real32"],ImageData[res,"Real32"]],
		SameQ[ImageChannels[testImage3D],ImageChannels[res]],
		SameQ[ImageColorSpace[testImage3D],ImageColorSpace[res]]
	]
	,
	True
	,
	TestID->"ImageReal32Operations-20170804-Z8J1C6"
]

ExactTest[
	ImageRowCount[testImage]
	,
	2
	,
	TestID->"ImageReal32Operations-20150803-W2O8G4"
]

ExactTest[
	ImageRowCount[testImage3D]
	,
	3
	,
	TestID->"ImageReal32Operations-20170804-A6E4X9"
]

ExactTest[
	ImageColumnCount[testImage]
	,
	5
	,
	TestID->"ImageReal32Operations-20150803-V5C3R0"
]

ExactTest[
	ImageColumnCount[testImage3D]
	,
	2
	,
	TestID->"ImageReal32Operations-20170804-T8M6T5"
]

ExactTest[
	ImageRank[testImage]
	,
	2
	,
	TestID->"ImageReal32Operations-20150805-V5N6L6"
]

ExactTest[
	ImageRank[testImage3D]
	,
	3
	,
	TestID->"ImageReal32Operations-20170804-W2H3Q6"
]
EndRequirement[]
