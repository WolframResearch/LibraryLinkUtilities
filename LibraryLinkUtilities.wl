(* ::Section:: *)
(* Load Dependencies *)
(* ------------------------------------------------------------------------- *)
(* ------------------------------------------------------------------------- *)

$InitLibraryLinkUtils = False;

InitLibraryLinkUtils[libPath_?StringQ] :=
If[TrueQ[$InitLibraryLinkUtils],
	$InitLibraryLinkUtils
	, (* else *)
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
	"ProgressMonInvalidValue" -> {24, "Expecting None or a Symbol for the option \"ProgressMonitor\"."}
|>;


(* ::SubSection:: *)
(* Utility Functions *)
(* ------------------------------------------------------------------------- *)
(* ------------------------------------------------------------------------- *)

ErrorCodeToName[errorCode_Integer]:=
Block[{name = Select[$CorePacletFailureLUT, MatchQ[#, {errorCode, _}] &]},
	If[Length[name] > 0 && Depth[name] > 2,
		First @ Keys @ name
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

Options[SafeLibraryFunction] = {
	"ProgressMonitor" -> None,
	"Throws" -> False
};

holdSet[Hold[sym_], rhs_] := sym = rhs;

SafeLibraryFunction[fname_?StringQ, fParams_, retType_, opts : OptionsPattern[SafeLibraryFunction]] :=
Module[{errorHandler, pmSymbol, newParams, f},
    errorHandler = If[TrueQ[OptionValue["Throws"]],
	    CatchAndThrowLibraryFunctionError
		,
	    CatchLibraryFunctionError
    ];
    pmSymbol = OptionValue[Automatic, Automatic, "ProgressMonitor", Hold];
    If[fParams === LinkObject || pmSymbol === Hold[None],
	    errorHandler @* SafeLibraryFunctionLoad[fname, fParams, retType]
	    , (* else *)
	    If[Not @ Developer`SymbolQ @ ReleaseHold @ pmSymbol,
		    Throw @ CreatePacletFailure["ProgressMonInvalidValue"];
	    ];
	    newParams = Append[fParams, {Real, 1, "Shared"}];
	    f = errorHandler @* SafeLibraryFunctionLoad[fname, newParams, retType];
	    (
		    holdSet[pmSymbol, Developer`ToPackedArray[{0.0}]];
		    f[##, ReleaseHold[pmSymbol]]
	    )&
    ]
]

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
Block[{msgParam, param, errorCode, msgTemplate, errorType},
	msgParam = Replace[OptionValue["MessageParameters"], Except[_?AssociationQ | _List] -> <||>];
	param = Replace[OptionValue["Parameters"], {p_?StringQ :> {p}, Except[{_?StringQ.. }] -> {}}];
	{errorCode, msgTemplate} =
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
	If[errorCode < 0, (* if failure comes from the C++ code, extract message template parameters *)
		msgParam = GetCCodeFailureParams[msgTemplate];
	];
	Failure[errorType,
		<|
			"MessageTemplate" -> msgTemplate,
			"MessageParameters" -> msgParam,
			"ErrorCode" -> errorCode,
			"Parameters" -> param
		|>
	]
]

GetCCodeFailureParams[msgTemplate_String?StringQ] :=
Block[{slotNames, slotValues, data},
	slotNames = Cases[First @ StringTemplate[msgTemplate], TemplateSlot[s_] -> s];
	slotNames = DeleteDuplicates[slotNames];
	slotValues = If[ListQ[LLU`$LastFailureParameters], LLU`$LastFailureParameters, {}];
	If[MatchQ[slotNames, {_Integer..}],
		(* for numbered slots return just a list of slot template values *)
		slotValues
		, (* otherwise, return an Association with slot names as keys *)
		(* If too many slot values came from C++ code - drop some, otherwise - pad with empty strings *)
		slotValues = PadRight[slotValues, Length[slotNames], ""];
		If[VectorQ[slotNames, StringQ],
			AssociationThread[slotNames, slotValues]
			, (* mixed slots are not officially supported but let's do the best we can *)
			MapThread[If[StringQ[#1], <|#1 -> #2|>, #2]&, {slotNames, slotValues}]
		]
	]
];

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
		Throw @ CreatePacletFailure[ErrorCodeToName[result[[2]]]]
		, (* else *)
		result
	]
]


(* ::SubSection:: *)
(* Logging *)
(* ------------------------------------------------------------------------- *)
(* ------------------------------------------------------------------------- *)

(************** Functions defining how to style different parts of a log message *************)

(* Colors associated with different log severities *)
LLU`Logger`LevelColor = <|"Error" -> Red;, "Warning" -> Orange, "Debug" -> Darker[Green]|>;

(* Styled part of a message containing log level description *)
LLU`Logger`StyledLevel[logLevel_] :=
		Style["[" <> ToString @ logLevel <> "]", LLU`Logger`LevelColor[logLevel]];

(* Styled part of a message containing info on where the log was issued *)
LLU`Logger`StyledMessageLocation[file_, line_, fn_] :=
		Tooltip[Style["Line " <> ToString[line] <> " in " <> FileNameTake[file] <> ", function " <> fn, Darker[Gray]], file];

(* Styled part of a message containing the actual log text *)
LLU`Logger`StyledMessageText[args_List, size_:Automatic] :=
		Style[StringJoin @@ ToString /@ args, size];

(************* Functions defining how to format a log message *************)

(* Put all message parts in a list unstyled *)
LLU`Logger`LogToList[args___] := {args};

(* Put all message parts in Association *)
LLU`Logger`LogToAssociation[logLevel_, line_, file_, fn_, args___] :=
		Association["Level" -> logLevel, "Line" -> line, "File" -> file, "Function" -> fn, "Message" -> LLU`Logger`StyledMessageText[{args}]];

(* Combine all log parts to a String. No styling, contains a newline character. *)
LLU`Logger`LogToString[logLevel_, line_, file_, fn_, args___] :=
	"[" <> ToString @ logLevel <> "] In file " <> file <> ", line " <> ToString[line] <> ", function " <> fn <> ":\n" <> (StringJoin @@ ToString /@ {args});

(* Combine all log parts to a condensed String. No styling, single line (unless message text contains newlines). *)
LLU`Logger`LogToShortString[logLevel_, line_, file_, fn_, args___] :=
	"[" <> ToString @ logLevel <> "] " <> FileNameTake[file] <> ":" <> ToString[line] <> " (" <> fn <> "): " <> (StringJoin @@ ToString /@ {args});

(* Place fully styled log message in a TextGrid. Looks nice, good default choice for printing to the notebook. *)
LLU`Logger`LogToGrid[logLevel_, line_, file_, fn_, args___] :=
		TextGrid[{
			{LLU`Logger`StyledLevel[logLevel], LLU`Logger`StyledMessageLocation[file, line, fn]},
			{SpanFromAbove, LLU`Logger`StyledMessageText[{args}, 14]}
		}];

(* Fully styled, condensed log message in a Row. Good choice if you expect many log messages and want to see them all in the notebook. *)
LLU`Logger`LogToRow[logLevel_, line_, file_, fn_, args___] :=
    Row[{Style["(" <> FileNameTake[file] <> ":" <> ToString[line] <> ")", LLU`Logger`LevelColor[logLevel]], LLU`Logger`StyledMessageText[{args}]}];

(* This is a selector called by other functions below. Feel free to modify/Block this symbol, see examples. *)
LLU`Logger`FormattedLog = LLU`Logger`LogToGrid;

(************* Functions defining where to place a log message *************)

(* Print to current notebook *)
LLU`Logger`PrintToNotebook[args___] :=
		Print @ LLU`Logger`FormattedLog[args];

(* Print to Messages window. Remember that this window may be hidden by default. *)
LLU`Logger`PrintToMessagesWindow[args___] :=
    NotebookWrite[MessagesNotebook[], Cell[RawBoxes @ ToBoxes[LLU`Logger`FormattedLog[args]], "Output"]];

(* Append to a list and assign to given symbol. Good choice if you don't want to see logs immediately, but want to store them for later analysis. *)
Attributes[LLU`Logger`PrintToSymbol] = {HoldFirst};
LLU`Logger`PrintToSymbol[x_] := (
	If[Not @ ListQ @ x,
		x = {}
	];
	AppendTo[x, LLU`Logger`FormattedLog[##]];
)&;

(* This is a function MathLink will call from the C++ code. It all starts here. Feel free to modify/Block this symbol, see examples. *)
LLU`Logger`Print = LLU`Logger`PrintToNotebook;

(************* Examples of overriding default logger behavior *************)

(*** Make logger format logs as Association and append to a list under a symbol TestLogSymbol:

LLU`Logger`Print =
	Block[{LLU`Logger`FormattedLog = LLU`Logger`LogToAssociation},
		LLU`Logger`PrintToSymbol[TestLogSymbol][##]
	]&

after you evaluate some library function the TestLogSymbol may be a list similar this:

{
	<|
		"Level" -> "Debug",
		"Line" -> 17,
		"File" -> "main.cpp",
		"Function" -> "ReadData",
		"Message" -> Style["Library function entered with 4 arguments.", Automatic]
	|>,
	<|
		"Level" -> "Warning",
		"Line" -> 20,
		"File" -> "Utilities.cpp",
		"Function" -> "validateDimensions",
		"Message" -> Style["Dimensions are too large.", Automatic]
	|>,
	...
}
*)
(*** Log styled condensed logs to Messages window:

LLU`Logger`Print = Block[{LLU`Logger`FormattedLog = LLU`Logger`LogToRow},
	LLU`Logger`PrintToNotebook[##]
]&
*)
(*** Sow logs formatted as short Strings instead of printing:

LLU`Logger`Print = Sow @* LLU`Logger`LogToShortString;

Remember to call library functions inside Reap!
*)