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
			SetPacletLibrary[libPath];
			SafeLibraryLoad[libPath];
			$GetCErrorCodes = SafeMathLinkFunction["sendRegisteredErrors"];
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

(* Path to the paclet library *)
$PacletLibrary = None;

(* Count how many failures was produced by the paclet during current Kernel session *)
$ErrorCount = 0;

(* Global association for all registered errors *)
$CorePacletFailureLUT = <|
	"LibraryLoadFailure" -> {20, "Failed to load library `LibraryName`."},
	"FunctionLoadFailure" -> {21, "Failed to load the function `FunctionName` from `LibraryName`."},
	"RegisterFailure" -> {22, "Incorrect arguments to RegisterPacletErrors."},
	"UnknownFailure" -> {23, "The error `ErrorName` has not been registered."},
	"ProgressMonInvalidValue" -> {24, "Value for option \"ProgressMonitor\" must be None or an Association."},
	"InvalidProgMonTitle" -> {25, "Value for option \"Title\" must be None, Automatic or a String instead of `Title`."}
|>;


(* ::SubSection:: *)
(* Utility Functions *)
(* ------------------------------------------------------------------------- *)
(* ------------------------------------------------------------------------- *)

ErrorCodeToName[errorCode_Integer]:=
Block[{name = Select[$CorePacletFailureLUT, MatchQ[#, {errorCode, _}] &]},
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
(* ProgressMonitor *)
(* ------------------------------------------------------------------------- *)
(* ------------------------------------------------------------------------- *)

$ProgressMonitorOptions = <|
	"Title" -> None,
	"ShowPercentage" -> True,
	"CaptionFunction" -> None
|>;

Attributes[getPercentage] = { HoldFirst };
getPercentage[x_] := ToString[Round[100 * First[x]]] <> "%";

validateTitle[rawTitle_, fname_?StringQ] :=
Switch[rawTitle,
	None,
	Nothing
	,
	Automatic,
	fname
	,
	_?StringQ,
	rawTitle
	,
	_,
	Throw @ CreatePacletFailure["InvalidProgMonTitle", "MessageParameters" -> <|"Title" -> rawTitle|>]
];

Attributes[preparePanel] = { HoldFirst };
preparePanel[progIndicator_, fname_?StringQ, opts_?AssociationQ] :=
Block[{topRow, title, percentage},
	title = validateTitle[opts["Title"], fname];
	percentage = If[opts["ShowPercentage"] === True,
		Dynamic[getPercentage[progIndicator]]
		,
		SpanFromLeft
	];
	topRow = { title, ProgressIndicator[Dynamic[First @ progIndicator]], percentage };
	If[opts["CaptionFunction"] === None,
		Return[Panel @ Row[topRow, Spacer[10]]];
	];
	Return[Panel @ Grid[{ topRow, { Dynamic[opts["CaptionFunction"][First @ progIndicator]], SpanFromLeft, SpanFromLeft } }]];
]

(* ::SubSection:: *)
(* SafeLibrary* *)
(* ------------------------------------------------------------------------- *)
(* ------------------------------------------------------------------------- *)

SetPacletLibrary[lib_?StringQ] := $PacletLibrary = lib;

SafeLibraryLoad[lib_] :=
	Quiet[
		Check[
			LibraryLoad[lib]
			,
			Throw @ CreatePacletFailure["LibraryLoadFailure", "MessageParameters" -> <|"LibraryName" -> lib|>];
		]
	]

SafeLibraryFunctionLoad[args___] :=
	Quiet[
		Check[
			LibraryFunctionLoad[$PacletLibrary, args]
			,
			Throw @ CreatePacletFailure["FunctionLoadFailure", "MessageParameters" -> <|"FunctionName" -> First[{args}], "LibraryName" -> $PacletLibrary|>];
		]
	]

Options[SafeLibraryFunction] = {
	"ProgressMonitor" -> None,
	"Throws" -> False
};

SafeLibraryFunction[fname_?StringQ, fParams_, retType_, opts : OptionsPattern[SafeLibraryFunction]] :=
Module[{errorHandler, pmOptValue, pmOpts, newParams, f},
    errorHandler = If[TrueQ[OptionValue["Throws"]],
	    CatchAndThrowLibraryFunctionError
	,
	    CatchLibraryFunctionError
    ];
    pmOptValue = Replace[OptionValue["ProgressMonitor"], Automatic -> <||>];
    If[fParams === LinkObject || pmOptValue === None,
	    errorHandler @* SafeLibraryFunctionLoad[fname, fParams, retType]
	    ,
	    If[!AssociationQ[pmOptValue],
		    Throw @ CreatePacletFailure["ProgressMonInvalidValue"];
	    ];
	    pmOpts = Merge[{pmOptValue, $ProgressMonitorOptions}, First];
	    newParams = Append[fParams, {Real, 1, "Shared"}];
	    f = errorHandler @* SafeLibraryFunctionLoad[fname, newParams, retType];
	    Module[{l, pnl, localPMOpts, localParams},
		    l = Developer`ToPackedArray[{0.0}];
		    {localParams, localPMOpts} = parseLibraryFunctionArgs[##];
		    localPMOpts = Merge[{localPMOpts, pmOpts}, First];
		    pnl = preparePanel[Refresh[l, UpdateInterval -> 0.02], fname,  localPMOpts];
		    Monitor[f @@ Append[localParams, l], pnl]
	    ]&
    ]
]

SafeMathLinkFunction[fname_String, opts : OptionsPattern[SafeLibraryFunction]] := 
	SafeLibraryFunction[fname, LinkObject, LinkObject, opts]


Options[parseLibraryFunctionArgs] = {
	"ProgressMonitor" -> <||>
};
parseLibraryFunctionArgs[args___, opts : OptionsPattern[parseLibraryFunctionArgs]] := {{args}, OptionValue["ProgressMonitor"]};

(* ::SubSection:: *)
(* RegisterPacletErrors *)
(* ------------------------------------------------------------------------- *)
(* ------------------------------------------------------------------------- *)

RegisterPacletErrors[libPath_?StringQ, errors_?AssociationQ] :=
Block[{cErrorCodes, max},
	If[!TrueQ[InitLibraryLinkUtils[libPath]],
		Throw @ CreatePacletFailure["LibraryLoadFailure", "MessageParameters" -> <|"LibraryName" -> libPath|>];
	];
	cErrorCodes = $GetCErrorCodes[]; (* <|"TestError1" -> (-1 -> "TestError1 message."), "TestError2" -> (-2 -> "TestError2 message.")|> *)
	If[Length[$CorePacletFailureLUT] > 0,
		max = MaximalBy[$CorePacletFailureLUT, First];
		max = If[Depth[max] > 2 && IntegerQ[max[[1, 1]]] && max[[1, 1]] >= 100,
			max[[1, 1]]
			,
			99
		];
		$CorePacletFailureLUT =
			Association[
				$CorePacletFailureLUT
				,
				MapIndexed[#[[1]] -> {(First[#2] + max), #[[2]]} &, Normal[errors]]
				,
				cErrorCodes
			];
		,
		AssociateTo[$CorePacletFailureLUT,
			Association[
				MapIndexed[#[[1]] -> {(First[#2] + 99), #[[2]]} &, Normal[errors]]
				,
				cErrorCodes
			]	
		];
	];
]

RegisterPacletErrors[___] :=
	Throw @ CreatePacletFailure["RegisterFailure"]


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
	lookup =
		Lookup[
			$CorePacletFailureLUT
			,
			errorType = type
			,
			(	
				AppendTo[msgParam, "ErrorName" -> type];
				$CorePacletFailureLUT[errorType = "UnknownFailure"]
			)
		];
	$ErrorCount++;
	Failure[errorType,
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

Attributes[CatchLibraryFunctionError] = {HoldAll};
Attributes[CatchAndThrowLibraryFunctionError] = {HoldAll};

CatchLibraryFunctionError[f_] :=
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
		CreatePacletFailure[ErrorCodeToName[result[[2]]]]
	, (* else *)
		result
	]
]

CatchAndThrowLibraryFunctionError[f_] :=
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
		Throw@CreatePacletFailure[ErrorCodeToName[result[[2]]]]
	, (* else *)
		result
	]
]
