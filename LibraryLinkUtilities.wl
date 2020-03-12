Begin["`LLU`"];

(* ::Section:: *)
(* Load Dependencies *)
(* ------------------------------------------------------------------------- *)
(* ------------------------------------------------------------------------- *)

$InitLibraryLinkUtils = False;

InitLibraryLinkUtils[libPath_?StringQ] :=
If[TrueQ[$InitLibraryLinkUtils],
	$InitLibraryLinkUtils
	, (* else *)
	$InitLibraryLinkUtils = (
		SetPacletLibrary[libPath];
		SafeLibraryLoad[libPath];
		$GetCErrorCodes = SafeWSTPFunction["sendRegisteredErrors"];
		$SetLoggerContext = SafeLibraryFunction["setLoggerContext", {"UTF8String"}, "UTF8String", "Optional" -> True];
		$SetExceptionDetailsContext = SafeLibraryFunction["setExceptionDetailsContext", {"UTF8String"}, "UTF8String"];
		SetContexts[Context[$InitLibraryLinkUtils]]; (* Tell C++ part of LLU in which context were top-level symbols loaded. *)
		True
	)
];


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
	"ProgressMonInvalidValue" -> {24, "Expecting None or a Symbol for the option \"ProgressMonitor\"."},
	"InvalidManagedExpressionID" -> {25, "`expr` is not a valid ManagedExpression." },
	"UnexpectedManagedExpression" -> {26, "Expected managed `expected`, got `actual`." }
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
];

AppendBacktickIfNeeded[s_?StringQ] := If[StringEndsQ[s, "`"], s, s <> "`"];

SetLoggerContext[context_?StringQ] :=
	$SetLoggerContext @ AppendBacktickIfNeeded @ context;

SetExceptionDetailsContext[context_?StringQ] :=
	$SetExceptionDetailsContext @ AppendBacktickIfNeeded @ context;

SetContexts[context_?StringQ] := SetContexts[context, context];
SetContexts[loggerContext_?StringQ, exceptionContext_?StringQ] := (
	SetLoggerContext[loggerContext];
	SetExceptionDetailsContext[exceptionContext];
);

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
	];

Options[SafeLibraryFunctionLoad] = {
	"Optional" -> False
};

SafeLibraryFunctionLoad[fname_?StringQ, fParams_, retType_, opts : OptionsPattern[SafeLibraryFunctionLoad]] :=
	Quiet[
		Check[
			(* There are 2 categories of function arguments:
			 * - regular - supported by LibraryLink (String, Integer, NumericArray, etc.)
			 * - special - extensions added by LLU or paclet developers that need extra parsing before they can be passed to LibraryLink,
			 *              for example Managed Expressions
			 *)
			Block[{specialArgs = argumentCategorizer[fParams], specialRetQ = CustomMResultTypeQ[retType], actualRetType, libFunction},
				actualRetType = If[specialRetQ, MResultCustomType[retType], retType];
				libFunction = If[Length @ specialArgs > 0,
					(* If the function that we are registering takes special arguments, we need to compose it with argumentParser function,
					 * which will parse input arguments before every call, so that they are accepted by LibraryLink.*)
					LibraryFunctionLoad[$PacletLibrary, fname, MArgumentCustomType /@ fParams, actualRetType] @* argumentParser[specialArgs]
					,
					LibraryFunctionLoad[$PacletLibrary, fname, fParams, actualRetType]
				];
				If[specialRetQ,
					MResultTransform[retType] @* libFunction
					,
					libFunction
				]
			]
			,
			If[TrueQ @ OptionValue["Optional"],
				Null&
				,
				Throw @ CreatePacletFailure["FunctionLoadFailure", "MessageParameters" -> <|"FunctionName" -> fname, "LibraryName" -> $PacletLibrary|>]
			]
		]
	];

Options[SafeLibraryFunction] = {
	"ProgressMonitor" -> None,
	"Throws" -> False
};

(* Decide whether given input argument to a LibraryFunction needs special treatment.
 * Currently only Managed Expressions do but this may change in the future.
 *)
CustomMArgumentTypeQ[a_Managed] := True;
CustomMArgumentTypeQ[_] := False;

CustomMResultTypeQ[_] := False;

(* Simple function expected to select all "special" arguments from a list of argument types for a LibraryFunction. *)
argumentCategorizer[argTypes_List] := Select[AssociationThread[Range[Length[#]], #]& @ argTypes, CustomMArgumentTypeQ];
argumentCategorizer[LinkObject] = {};


(* Parse ManagedExpression before it is passed to a LibraryFunction. First argument is expected ManagedExpression type and the second is actual instance
 * of a MangedExpression. If the instance type does not match the expected type a paclet Failure will be thrown.
 *)
MArgumentTransform[Managed[expectedHead_]] := Replace[{
	expectedHead[id_Integer] :> If[ManagedLibraryExpressionQ[expectedHead[id]],
		id
		,

		Throw @ CreatePacletFailure["InvalidManagedExpressionID", "MessageParameters" -> <|"expr" -> expectedHead[id]|>]
	]
	,
	id_Integer :> id (* Passing bare IDs of Managed Expressions is supported but may be more error prone than passing proper MLEs *)
	,
	e_ :> Throw @ CreatePacletFailure["UnexpectedManagedExpression", "MessageParameters" -> <|"expected" -> expectedHead, "actual" -> e|>]
}];
MArgumentTransform[_] := Identity;

(* Parse special arguments before they are passed to a LibraryFunction. Currently the only implemented type of special arguments are ManagedExpressions, so
 * just call parseManaged directly but in the future this implementation might need to be generalised.
 *
 * NOTE: This function is invoked before every call to a LibraryFunction so efficiency is top concern here!
 *)
argumentParser[specialArgs_?AssociationQ] := Sequence @@ MapIndexed[MArgumentTransform[specialArgs[First @ #2]][#1] &, {##}]&;

MArgumentCustomType[_Managed] := Integer;
MArgumentCustomType[t_] := t;

MResultCustomType[t_] := t;
MResultTransform[_] := Identity;

MArgumentType[customType_, actualType_, transformation_] := (
	CustomMArgumentTypeQ[customType] := True;
	MArgumentCustomType[customType] := Sequence @@ actualType;
	MArgumentTransform[customType] := transformation;
);

MResultType[customType_, actualType_, transformation_] := (
	CustomMResultTypeQ[customType] := True;
	MResultCustomType[customType] := actualType;
	MResultTransform[customType] := transformation;
);

holdSet[Hold[sym_], rhs_] := sym = rhs;

SafeLibraryFunction[fname_?StringQ, fParams_, retType_, opts : OptionsPattern[]] :=
Module[{errorHandler, pmSymbol, newParams, f, functionOptions, loadOptions},
	functionOptions = FilterRules[{opts}, Options[SafeLibraryFunction]];
    errorHandler = If[TrueQ[OptionValue[Automatic, functionOptions, "Throws"]],
	    CatchAndThrowLibraryFunctionError
		,
	    CatchLibraryFunctionError
    ];
    pmSymbol = OptionValue[Automatic, functionOptions, "ProgressMonitor", Hold];
	loadOptions = FilterRules[{opts}, Options[SafeLibraryFunctionLoad]];
    If[fParams === LinkObject || pmSymbol === Hold[None],
	    errorHandler @* SafeLibraryFunctionLoad[fname, fParams, retType, loadOptions]
	    , (* else *)
	    If[Not @ Developer`SymbolQ @ ReleaseHold @ pmSymbol,
		    Throw @ CreatePacletFailure["ProgressMonInvalidValue"];
	    ];
	    newParams = Append[fParams, {Real, 1, "Shared"}];
	    f = errorHandler @* SafeLibraryFunctionLoad[fname, newParams, retType, loadOptions];
	    (
		    holdSet[pmSymbol, Developer`ToPackedArray[{0.0}]];
		    f[##, ReleaseHold[pmSymbol]]
	    )&
    ]
];

SafeWSTPFunction[fname_String, opts : OptionsPattern[SafeLibraryFunction]] :=
	SafeLibraryFunction[fname, LinkObject, LinkObject, opts];

LibraryMemberFunction[exprHead_][fname_String, fParams_, retType_, opts : OptionsPattern[SafeLibraryFunction]] :=
    If[fParams === LinkObject && retType === LinkObject,
	    SafeWSTPFunction[fname, opts]
	    ,
		SafeLibraryFunction[fname, Prepend[fParams, Managed[exprHead]], retType, opts]
    ];

(* ::SubSection:: *)
(* RegisterPacletErrors *)
(* ------------------------------------------------------------------------- *)
(* ------------------------------------------------------------------------- *)

RegisterPacletErrors[libPath_?StringQ, errors_?AssociationQ] :=
Block[{cErrorCodes, max},
	InitLibraryLinkUtils[libPath];
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
];

RegisterPacletErrors[___] :=
	Throw @ CreatePacletFailure["RegisterFailure"];


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
];

(* We need a symbol that will store values for TemplateSlots in the most recently thrown exception. Exceptions are thrown in C++ and slots values provided
 * in ErrorManager::throwException are transferred in a List via WSTP and assigned to this symbol.
 * Later, the error handling routine in WL, CatchLibraryFunctionError, checks this symbol and creates Failure object.
 *)
$LastFailureParameters;

GetCCodeFailureParams[msgTemplate_String?StringQ] :=
Block[{slotNames, slotValues, data},
	slotNames = Cases[First @ StringTemplate[msgTemplate], TemplateSlot[s_] -> s];
	slotNames = DeleteDuplicates[slotNames];
	slotValues = If[ListQ[$LastFailureParameters], $LastFailureParameters, {}];
	$LastFailureParameters = {};
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
];

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
];


(* ::SubSection:: *)
(* Logging *)
(* ------------------------------------------------------------------------- *)
(* ------------------------------------------------------------------------- *)

Begin["`Logger`"];

(************** Functions defining how to style different parts of a log message *************)

(* Colors associated with different log severities *)
LevelColorMapForLLULogs = <|"Error" -> Red, "Warning" -> Orange, "Debug" -> Darker[Green]|>;

(* Styled part of a message containing log level description *)
StyledLLULogLevel[logLevel_] :=
		Style["[" <> ToString @ logLevel <> "]", LevelColorMapForLLULogs[logLevel]];

(* Styled part of a message containing info on where the log was issued *)
StyledLLULogMessageLocation[file_, line_, fn_] :=
		Tooltip[Style["Line " <> ToString[line] <> " in " <> FileNameTake[file] <> ", function " <> fn, Darker[Gray]], file];

(* Styled part of a message containing the actual log text *)
StyledLLULogMessageText[args_List, size_:Inherited] :=
		Style[StringJoin @@ ToString /@ args, FontSize -> size];

(************* Functions defining how to format a log message *************)

(* Put all message parts in a list unstyled *)
LogToList[args___] := {args};

(* Put all message parts in Association *)
LogToAssociation[logLevel_, line_, file_, fn_, args___] :=
		Association["Level" -> logLevel, "Line" -> line, "File" -> file, "Function" -> fn, "Message" -> StyledLLULogMessageText[{args}]];

(* Combine all log parts to a String. No styling, contains a newline character. *)
LogToString[logLevel_, line_, file_, fn_, args___] :=
	"[" <> ToString @ logLevel <> "] In file " <> file <> ", line " <> ToString[line] <> ", function " <> fn <> ":\n" <> (StringJoin @@ ToString /@ {args});

(* Combine all log parts to a condensed String. No styling, single line (unless message text contains newlines). *)
LogToShortString[logLevel_, line_, file_, fn_, args___] :=
	"[" <> ToString @ logLevel <> "] " <> FileNameTake[file] <> ":" <> ToString[line] <> " (" <> fn <> "): " <> (StringJoin @@ ToString /@ {args});

(* Place fully styled log message in a TextGrid. Looks nice, good default choice for printing to the notebook. *)
LogToGrid[logLevel_, line_, file_, fn_, args___] :=
		TextGrid[{
			{StyledLLULogLevel[logLevel], StyledLLULogMessageLocation[file, line, fn]},
			{SpanFromAbove, StyledLLULogMessageText[{args}, 14]}
		}];

(* Fully styled, condensed log message in a Row. Good choice if you expect many log messages and want to see them all in the notebook. *)
LogToRow[logLevel_, line_, file_, fn_, args___] :=
    Row[{Style["(" <> FileNameTake[file] <> ":" <> ToString[line] <> ")", LevelColorMapForLLULogs[logLevel]], StyledLLULogMessageText[{args}]}];

(* This is a "selector" called by other functions below. Feel free to modify/Block this symbol, see examples. *)
FormattedLog := LogToGrid;


(************* Functions filtering log messages *************)

(* Define a symbol for filtered-out messages *)
LogFiltered = Missing["FilteredOut"];

(* Simple filter that does no filtering *)
FilterAcceptAll[args___] := args;

(* Filter that rejects everything *)
FilterRejectAll[___] := LogFiltered;

(* Meta function for defining filters that filter by a single element of a log: level, line, file name or function name *)
FilterBySingleFeature[featureIndex_][test_] := Sequence @@ If[TrueQ @ test[Slot[featureIndex]], {##}, {LogFiltered}]&;

(* Define single element filters *)
{FilterByLevel, FilterByLine, FilterByFile, FilterByFunction} = (FilterBySingleFeature /@ Range[4]);

(* Define custom filter - test function have access to all elements of the log *)
FilterCustom[test_] := Sequence @@ If[TrueQ @ test[##], {##}, {LogFiltered}]&;

(* This is a "selector" called by other functions below. Feel free to modify/Block this symbol, see examples. *)
LogFilterSelector := FilterAcceptAll;

(************* Functions defining where to place a log message *************)

(* Discard the log *)
DiscardLog[___] := Null;

(* Print to current notebook *)
PrintLogToNotebook[args___] :=
	Print @ FormattedLog[args];
PrintLogToNotebook[LogFiltered] := DiscardLog[];

(* Print to Messages window. Remember that this window may be hidden by default. *)
PrintLogToMessagesWindow[args___] :=
    NotebookWrite[MessagesNotebook[], Cell[RawBoxes @ ToBoxes[FormattedLog[args]], "Output"]];
PrintLogToMessagesWindow[LogFiltered] := DiscardLog[];

(* Append to a list and assign to given symbol. Good choice if you don't want to see the logs immediately, but want to store them for later analysis. *)
Attributes[PrintLogToSymbol] = {HoldFirst};
PrintLogToSymbol[x_] := (
	If[Not @ ListQ @ x,
		x = {}
	];
	AppendTo[x, FormattedLog[##]];
)&;
PrintLogToSymbol[LogFiltered] := DiscardLog[];

(* This is a "selector" called by other functions below. Feel free to modify/Block this symbol, see examples. *)
PrintLogFunctionSelector := PrintLogToNotebook;


(* This is a function WSTP will call from the C++ code. It all starts here. Feel free to modify/Block this symbol, see examples. *)
LogHandler := PrintLogFunctionSelector @* LogFilterSelector;

End[]; (* `Logger` *)


(************* Examples of overriding default logger behavior *************)

(***
	Make logger format logs as Association and append to a list under a symbol TestLogSymbol:

		Logger`PrintLogFunctionSelector := Block[{Logger`FormattedLog = Logger`LogToAssociation},
			Logger`PrintLogToSymbol[TestLogSymbol][##]
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
***)

(***
	Log styled condensed logs to Messages window:

		Logger`PrintLogFunctionSelector := Block[{Logger`FormattedLog = Logger`LogToRow},
			Logger`PrintLogToMessagesWindow[##]
		]&
***)

(***
	Sow logs formatted as short Strings instead of printing:

		Logger`PrintLogFunctionSelector :=
			If[## =!= Logger`LogFiltered,
				Sow @ Logger`LogToShortString[##]
			]&;

	Remember that in this case library functions must be wrapped with Reap.

	You could theoretically write simply

		LLU`Logger`PrintLogFunctionSelector := Sow @* LLU`Logger`LogToShortString;

	But in this case, you are loosing the correct handling of filtered-out messages so it's only fine with the default "accept-all" filter.
***)


(* ::SubSection:: *)
(* Managed Expressions *)
(* ------------------------------------------------------------------------- *)
(* ------------------------------------------------------------------------- *)
Constructor;

NewManagedExpression[exprHead_][args___] :=
	Block[{res},
		res = CreateManagedLibraryExpression[SymbolName[exprHead], exprHead];
		Constructor[exprHead][ManagedLibraryExpressionID[res], args];
		res
	];

ManagedQ[exprHead_] := ManagedLibraryExpressionQ[#, SymbolName[exprHead]]&;
ManagedIDQ[exprHead_] := ManagedLibraryExpressionQ[exprHead[#], SymbolName[exprHead]]&;

GetManagedID[instance_] := ManagedLibraryExpressionID[instance];

SetAttributes[ClassMemberName, HoldAll];
ClassMemberName[className_, f_] := SymbolName[className] <> "`" <> SymbolName[Unevaluated[f]];

SetAttributes[ClassMember, HoldAll];
ClassMember[className_, f_] := Symbol @ ClassMemberName[className, f];

LoadMemberFunction[exprHead_][memberSymbol_?Developer`SymbolQ, fname_String, fParams_, retType_, opts : OptionsPattern[SafeLibraryFunction]] :=
	(
		If[Not @ Developer`SymbolQ @ ClassMember[exprHead, memberSymbol],
			Clear @ Evaluate @ ClassMemberName[exprHead, memberSymbol];
		];
		exprHead /: exprHead[id_][memberSymbol[args___]] := ClassMember[exprHead, memberSymbol][exprHead[id], args];
		Evaluate[ClassMember[exprHead, memberSymbol]] = LibraryMemberFunction[exprHead][fname, fParams, retType, opts];
	);

LoadWSTPMemberFunction[exprHead_][memberSymbol_?Developer`SymbolQ, fname_String, opts : OptionsPattern[SafeLibraryFunction]] :=
	LoadMemberFunction[exprHead][memberSymbol, fname, LinkObject, LinkObject, opts];

End[]; (* `LLU` *)
