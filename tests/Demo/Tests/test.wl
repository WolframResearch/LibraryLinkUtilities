Needs["Demo`"];
Needs["MUnit`"];

$tests = {
	TestCreate[
		Demo`CaesarCipherDecode::usage
		,
		"CaesarCipherDecode[cipherText_String, shift_Integer] restores the original message encoded with Caesar's cipher given the encoded text and the shift."
	],

	TestCreate[
		Demo`CaesarCipherDecode[Demo`CaesarCipherEncode["HelloWorld", 3], 3]
		,
		"HelloWorld"
	],

	TestCreate[
		Demo`CaesarCipherDecode[Demo`CaesarCipherEncode["HelloWorld", 25], 25]
		,
		"HelloWorld"
	],

	VerificationTest[
		f = Catch[Demo`CaesarCipherEncode["HelloWorld", -1], "LLUExceptionTag"];
		FailureQ[f] && First[f] === "NegativeShiftError"
	],

	VerificationTest[
		f = Catch[Demo`CaesarCipherEncode["HelloWorld\[HappySmiley]", 2], "LLUExceptionTag"];
		FailureQ[f] && First[f] === "InvalidCharacterError"
	]
};

report = TestReport[
	$tests,
	MemoryConstraint -> Quantity[100, "Megabytes"],
	TimeConstraint -> Quantity[60, "Seconds"]
];

If[TrueQ @ report["ReportSucceeded"],
	Print["All tests passed!"]
	,
	Exit[1]
]

