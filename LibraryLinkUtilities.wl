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
	"UnknownFailure" -> {23, "The error `ErrorName` has not been registered."}
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

Options[SafeLibraryFunction] = { "Throws" -> False };

SafeLibraryFunction[args___, opts : OptionsPattern[SafeLibraryFunction]] := 
	If[TrueQ[OptionValue["Throws"]], CatchAndThrowLibraryFunctionError, CatchLibraryFunctionError] @* SafeLibraryFunctionLoad[args]
	

SafeMathLinkFunction[fname_String, opts : OptionsPattern[SafeLibraryFunction]] := 
	SafeLibraryFunction[fname, LinkObject, LinkObject, opts]

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
