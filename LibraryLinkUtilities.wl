(* ::Section:: *)
(* Load Dependencies *)
(* ------------------------------------------------------------------------- *)
(* ------------------------------------------------------------------------- *)

$InitLibraryLinkUtils = False;

InitLibraryLinkUtils[libPath_?StringQ] :=
If[TrueQ[$InitLibraryLinkUtils],
	$InitLibraryLinkUtils
	,
	$InitLibraryLinkUtils =
		Catch[
			Quiet@Check[
				LibraryLoad[libPath];
				,
				Throw[$InitLibraryLinkUtils = $Failed];
			];
			$getCErrorCodes =
				Quiet@Check[
					LibraryFunctionLoad[libPath, "sendRegisteredErrors", LinkObject, LinkObject]
					,
					Throw[$InitLibraryLinkUtils = $Failed];
				];
			True
		]
]	


(* ::Section:: *)
(* Internal Utilities *)
(* ------------------------------------------------------------------------- *)
(* ------------------------------------------------------------------------- *)


(* ::SubSection:: *)
(* Globals *)
(* ------------------------------------------------------------------------- *)
(* ------------------------------------------------------------------------- *)

$errID = 0;

$corePacletFailureLUT = <||>;


(* ::SubSection:: *)
(* Utility Functions *)
(* ------------------------------------------------------------------------- *)
(* ------------------------------------------------------------------------- *)

errorCodeToName[errorCode_Integer]:=
Block[{name = Select[$corePacletFailureLUT, MatchQ[#, {errorCode, _}] &]},
	If[Length@name > 0 && Depth[name] > 2,
		First@Keys@name
		,
		""
	]
]


(* ::Section:: *)
(* Developer API *)
(* ------------------------------------------------------------------------- *)
(* ------------------------------------------------------------------------- *)


(* ::SubSection:: *)
(* RegisterPacletErrors *)
(* ------------------------------------------------------------------------- *)
(* ------------------------------------------------------------------------- *)

RegisterPacletErrors[libPath_?StringQ, errors_?AssociationQ] :=
Block[{cErrorCodes, max},
	If[FailureQ[InitLibraryLinkUtils[libPath]],
		Return@Failure["LoadFailure",
			<|
				"MessageTemplate" -> "Error loading library functions.",
				"MessageParameters" -> {},
				"ErrorCode" -> 0,
				"Parameters" -> param
			|>
		];
	];
	cErrorCodes = $getCErrorCodes[]; (* <|"TestError1" -> (-1 -> "TestError1 message."), "TestError2" -> (-2 -> "TestError2 message.")|> *)
	If[Length[$corePacletFailureLUT] > 0,
		max = MaximalBy[$corePacletFailureLUT, First];
		max = If[Depth[max] > 2 && IntegerQ[max[[1, 1]]] && max[[1, 1]] >= 100,
			max[[1, 1]]
			,
			99
		];
		$corePacletFailureLUT =
			Association[
				$corePacletFailureLUT
				,
				MapIndexed[#[[1]] -> {(First[#2] + max), #[[2]]} &, Normal[errors]]
				,
				cErrorCodes
			];
		,
		AssociateTo[$corePacletFailureLUT,
			Association[
				MapIndexed[#[[1]] -> {(First[#2] + 99), #[[2]]} &, Normal[errors]]
				,
				cErrorCodes
			]	
		];
	];
]

RegisterPacletErrors[___] :=
Failure["RegisterFailure",
	<|
		"MessageTemplate" -> "Error loading library functions.",
		"MessageParameters" -> {},
		"ErrorCode" -> 0,
		"Parameters" -> param

	|>
];


(* ::SubSection:: *)
(* CreatePacletFailure *)
(* ------------------------------------------------------------------------- *)
(* ------------------------------------------------------------------------- *)

Options[CreatePacletFailure] = {
	"MessageParameters" -> <||>,
	"Parameters" -> {}
};

CreatePacletFailure[type_?StringQ, opts:OptionsPattern[]] :=
Block[{msgParam, param, lookup},
	msgParam = Replace[OptionValue["MessageParameters"], Except[_?AssociationQ] -> <||>];
	param = Replace[OptionValue["Parameters"], {p_?StringQ :> {p}, Except[{_?StringQ.. }] -> {}}];
	lookup = Lookup[$corePacletFailureLUT, type, 0 -> "Unknown error."];
	Failure[type,
		<|
			"MessageTemplate" -> lookup[[2]],
			"MessageParameters" -> msgParam,
			"ErrorCode" -> lookup[[1]],
			"Parameters" -> param
		|>
	]
]


(* ::SubSection:: *)
(* CatchLibraryLinkError *)
(* ------------------------------------------------------------------------- *)
(* ------------------------------------------------------------------------- *)

CatchLibraryLinkError[expr___] := expr

CatchLibraryLinkError[err_LibraryFunctionError] :=
	CreatePacletFailure[errorCodeToName[err[[2]]]]

