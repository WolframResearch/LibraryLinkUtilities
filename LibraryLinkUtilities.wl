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

$ErrorCount = 0;

$corePacletFailureLUT = <|
	"LoadFailure" -> {20, "LibraryLinkUtilities initialization failed."},
	"RegisterFailure" -> {21, "Incorrect arguments to RegisterPacletErrors"},
	"UnknownFailure" -> {22, "Trying to throw error that was not registered."}
|>;


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
		Return@CreatePacletFailure["LoadFailure"];
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
	CreatePacletFailure["RegisterFailure"]


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
	lookup = Lookup[$corePacletFailureLUT, type, $corePacletFailureLUT["UnknownFailure"]];
	$ErrorCount++;
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

Attributes[CatchLibraryLinkError] = {HoldAll};

CatchLibraryLinkError[f_] := 	
With[{result = Quiet[f, {
		LibraryFunction::typerr, 
		LibraryFunction::rnkerr, 
		LibraryFunction::dimerr, 
		LibraryFunction::numerr, 
		LibraryFunction::memerr, 
		LibraryFunction::verserr, 
		LibraryFunction::rterr
	}]}, 
	
	If[Head[result] === LibraryFunctionError,
		CreatePacletFailure[errorCodeToName[result[[2]]]]
	, (* else *)
		result
	]
];

