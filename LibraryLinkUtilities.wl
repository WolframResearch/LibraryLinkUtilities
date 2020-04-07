(* $Context holds the private paclet context that this file was loaded in. *)
System`Private`NewContextPath[{"System`", $Context <> "LLU`"}];
Begin["`LLU`"];

InitializePacletLibrary::usage = "InitializePacletLibrary[libPath_]
Initialization of LLU that involves loading the main paclet library. Must be called by every paclet that uses LLU and will be evaluated only once
unless it failed. Failures are indicated by Throwing.
Loading is done lazily, when the first library function is loaded.
libPath - path to the main paclet library (the one that LLU was linked into)";

RegisterPacletErrors::usage = "RegisterPacletErrors[errors_?AssociationQ]
Adds custom top-level errors.";
CreatePacletFailure::usage = "CreatePacletFailure[type_?StringQ, opts:OptionsPattern[]]
Emits a Failure object for the custom error named by type.";

LoadLibraryFunction::usage = "LoadLibraryFunction[resultSymbol_, lib_, f_, fParams_, fResultType_, opts___]
Attempts to load an exported function f from a dynamic library lib and assign the result to resultSymbol.
Loading is lazy, which means that it will actually happen on the first evaluation of the resultSymbol.
Arguments:
- resultSymbol_ - a WL symbol to represent the loaded function
- lib_String - name of the dynamic library
- f_String - name of the function to load from the dynamic library
- fParams_ - parameter types of the library function to be loaded
- fResultType_ - result type
Options:
All options for SafeLibraryFunction and SafeLibraryFunctionLoad are accepted.";
LoadWSTPFunction::usage = "LoadWSTPFunction[resultSymbol_, lib_, f_, opts___]
A convenient wrapper around LoadLibraryFunction for easier loading of WSTP functions.";

Constructor::usage = "`LLU`Constructor[exprHead_] = Function[...]
Loads a constructor wrapper for a managed expression. The instanceID is passed to the wrapper as its first argument.";
NewManagedExpression::usage = "NewManagedExpression[exprHead_][args___]
Creates a MLE instance";
ManagedQ::usage = "ManagedQ[exprHead_][expr]
Checks whether expr is a valid MLE instance";
ManagedIDQ::usage = "ManagedIDQ[exprHead_][expr]
Checks whether expr is a valid id of a MLE instance";
LoadMemberFunction::usage = "LoadMemberFunction[exprHead_][memberSymbol_?Developer`SymbolQ, libraryName_, fname_, fParams_, retType_, opts : OptionsPattern[]]
Loads a library function that can be invoked on instances of exprHead like so: instance @ memberSymbol[...]";
LoadWSTPMemberFunction::usage = "LoadWSTPMemberFunction[exprHead_][memberSymbol_, libraryName_, fname_?StringQ, opts : OptionsPattern[]]
A convenient wrapper around LoadMemberFunction for easier loading of WSTP functions.";

LoadFilesInContext::usage = "LoadFilesInContext[files: {__?StringQ} | _?StringQ, exportedContext_?StringQ, loadingContext_?StringQ, opts: OptionsPattern[]]
Finds all symbols exported in exportedContext and Gets the Wolfram Language code files in loadingContext.";

Memoize::usage = "Memoize[expr_]
Evaluates expr only once, when f is first used. It should be used like so: f := Memoize @ expr;";
LazyLoad::usage = "LazyLoad[f_?Developer`SymbolQ, expr_]
Evaluates expr only once, when f is first used.";

(* Lower-level functions that shouldn't be needed in many cases *)
SafeLibraryLoad::usage = "SafeLibraryLoad[lib_]";
SafeLibraryFunction::usage = "SafeLibraryFunction[libName_, fname_?StringQ, fParams_, retType_, opts : OptionsPattern[]]";
SafeWSTPFunction::usage = "SafeWSTPFunction[fname_String, opts : OptionsPattern[SafeLibraryFunction]]";
LibraryMemberFunction::usage = "LibraryMemberFunction[exprHead_][libName_, fname_String, fParams_, retType_, opts : OptionsPattern[SafeLibraryFunction]]";
SafeLibraryFunctionLoad::usage = "SafeLibraryFunctionLoad[libName_, fname_?StringQ, fParams_, retType_, opts : OptionsPattern[SafeLibraryFunctionLoad]]";

(* ::Section:: *)
(* Load Dependencies *)
(* ------------------------------------------------------------------------- *)
(* ------------------------------------------------------------------------- *)

Begin["`Private`"];

(* LLU depends on WSTP, so WSTP must be loaded before the paclet library.
 * Paclets are not required to carry their own copy of WSTP shared library,
 * instead every paclet attempts to load the WSTP located within the current installation of Mathematica.
 * This may cause problems if paclet was built with different WSTP interface version but this should be extremely rare.
 * On MacOS WSTP is a framework and it is on the Kernel's rpath so the loading should happen automatically.
 *)
LoadWSTPLibrary[] :=
	Block[{wstpName, wstpPath},
		wstpName = "WSTP" <> ToString[$SystemWordLength] <> "i" <> ToString[MathLink`Information`$InterfaceNumber];
		wstpPath = System`Private`LocateDynamicLibrary[wstpName];
		SafeLibraryLoad @ wstpPath
	] /; ($OperatingSystem =!= "MacOSX");

(* Initialization of LLU that involves loading the main paclet library. Must be called by every paclet that uses LLU and will be evaluated only once
 * unless it failed. Failures are indicated by Throwing.
 * Loading is done lazily, when the first library function is loaded.
 * libPath - path to the main paclet library (the one that LLU was linked into)
 *)
SetAttributes[InitializePacletLibrary, HoldFirst];
InitializePacletLibrary[libPath_] :=
	$PacletLibrary := SetPacletLibrary[$InitializePacletLibrary[libPath]];

$InitializePacletLibrary[libPath_?StringQ] := (
	(* Load WSTP *)
	LoadWSTPLibrary[];

	(* Load paclet library. This has to be done, because LLU needs its own init functions in the C++ code, that are part of paclet library. *)
	SetPacletLibrary[SafeLibraryLoad[libPath]];

	(* Initialize error handling part of LLU by loading errors from the C++ code *)
	LoadWSTPFunction[$GetCErrorCodes, "sendRegisteredErrors", "Throws" -> True, "Lazy" -> False];
	RegisterCppErrors[];

	(* Load library functions for initializing different parts of LLU. *)
	LoadLibraryFunction[$SetLoggerContext, "setLoggerContext", {String}, String, "Optional" -> True, "Throws" -> True, "Lazy" -> False];
	LoadLibraryFunction[$SetExceptionDetailsContext, "setExceptionDetailsContext", {String}, String, "Throws" -> True, "Lazy" -> False];
	SetContexts[Context[$PacletLibrary]]; (* Tell C++ part of LLU in which context were top-level symbols loaded. *)
	$PacletLibrary
);

(* ::Section:: *)
(* Internal Utilities *)
(* ------------------------------------------------------------------------- *)
(* ------------------------------------------------------------------------- *)

(* ::SubSection:: *)
(* Globals *)
(* ------------------------------------------------------------------------- *)
(* ------------------------------------------------------------------------- *)

(* Path to the paclet library *)
$PacletLibrary;

SetPacletLibrary[lib_?StringQ] := $PacletLibrary = lib;

(* Count how many failures was produced by the paclet during current Kernel session *)
$ErrorCount = 0;

(* Global association for all registered errors *)
$CorePacletFailureLUT = <|
	"LibraryLoadFailure" -> {20, "Failed to load library `LibraryName`. Details: `Details`."},
	"FunctionLoadFailure" -> {21, "Failed to load the function `FunctionName` from `LibraryName`. Details: `Details`."},
	"RegisterFailure" -> {22, "Incorrect arguments to RegisterPacletErrors."},
	"UnknownFailure" -> {23, "The error `ErrorName` has not been registered."},
	"ProgressMonInvalidValue" -> {24, "Expecting None or a Symbol for the option \"ProgressMonitor\"."},
	"InvalidManagedExpressionID" -> {25, "`Expr` is not a valid ManagedExpression." },
	"UnexpectedManagedExpression" -> {26, "Expected managed `Expected`, got `Actual`." }
|>;

(* Every error used in the paclet must have unique ID. We distinguish 4 ranges of IDs:
 * (-Infinity, 0) - for errors returned from the C++ code,
 * [0, 20) - reserved for LibraryLink
 * [20, $MaxLLUBuiltinErrorID] - reserved for built-in LLU top-level errors
 * [$MaxLLUBuiltinErrorID + 1, Infinity) - for user-defined top-level errors
 *
 * IMPORTANT!
 * Developers never explicitly specify IDs for their errors. This is done internally by LLU.
 *)
$MaxLLUBuiltinErrorID = 99;

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
(* User-defined library function argument types *)
(* ------------------------------------------------------------------------- *)

(* Function to extend the set of basic LibraryLink argument types. Developers can register their custom types. Three parameters must be provided:
 * - customType - new argument type, can be a Symbol, String, or some other expression
 * - actualType - a type or a list of basic LibraryLink argument types that correspond to a single argument of customType
 * - transformation - a function that takes a single expression of customType and returns a sequence of corresponding values of actualType
 *)
MArgumentType[customType_, actualType_, transformation_] := (
	CustomMArgumentTypeQ[customType] := True;
	MArgumentCustomType[customType] := Sequence @@ actualType;
	MArgumentTransform[customType] := transformation;
);

(* Function to extend the set of basic LibraryLink return types. Developers can register their custom types. Three parameters must be provided:
 * - customType - new return type, can be a Symbol, String, or some other expression
 * - actualType - a single basic LibraryLink type that corresponds to an expression of customType
 * - transformation - a function that takes an expression of actualType and returns a value of customType
 *)
MResultType[customType_, actualType_, transformation_] := (
	CustomMResultTypeQ[customType] := True;
	MResultCustomType[customType] := actualType;
	MResultTransform[customType] := transformation;
);

(* Decide whether given LibraryFunction argument/return type is a user-defined type and needs special treatment.
 * Downvalues for these symbols can be provided by developers either directly or by calling MArgumentType/MResultType
 *)
CustomMArgumentTypeQ[_] := False;
CustomMResultTypeQ[_] := False;

(* Get basic LibraryLink type(s) corresponding to a given user-defined type.
 * MArgumentCustomType[t] must evaluate to a sequence of basic LibraryLink types that corresponds to a single argument of "type" t.
 * MResultCustomType[t] must evaluate to a single basic LibraryLink type that corresponds to a library function return value of "type" t.
 * Downvalues for these symbols can be provided by developers either directly or by calling MArgumentType/MResultType
 *)
MArgumentCustomType[t_] := t;
MResultCustomType[t_] := t;

(* Get transformation function for user-defined argument/return type.
 * MArgumentTransform[t] must be a one-argument function that takes an expression of "type" t and returns
 * a sequence of LibraryLink-compatible expressions (String, Integer, NumericArray, etc.).
 * MResultTransform[t] shall be a one-argument function that takes a LibraryLink-compatible expression and returns an expression of "type" t.
 * Downvalues for these symbols can be provided by developers either directly or by calling MArgumentType/MResultType
 *)
MArgumentTransform[_] := Identity;
MResultTransform[_] := Identity;

(* Simple function expected to select all user-defined arguments from a list of argument types for a LibraryFunction. *)
SelectSpecialArgs[argTypes_List] := Select[AssociationThread[Range[Length[#]], #]& @ argTypes, CustomMArgumentTypeQ];
SelectSpecialArgs[LinkObject] = {};

(* Parse user-defined arguments before they are passed to a LibraryFunction. Parsing is based on MArgumentTransform function.
 * NOTE: This function is invoked before every call to a LibraryFunction so efficiency is top concern here!
 *)
ArgumentParser[specialArgs_?AssociationQ] := Sequence @@ MapIndexed[MArgumentTransform[specialArgs[First @ #2]][#1] &, {##}]&;

(* ::SubSection:: *)
(* Loading dynamic libraries and library functions *)
(* ------------------------------------------------------------------------- *)

SafeLibraryLoad[lib_] :=
	Quiet @ Check[
		LibraryLoad[lib]
		,
		Throw @ CreatePacletFailure[
			"LibraryLoadFailure",
			"MessageParameters" -> <|"LibraryName" -> lib, "Details" -> ToString @ LibraryLink`$LibraryError|>
		];
	];

Options[SafeLibraryFunctionLoad] = {
	"Optional" -> False
};

SafeLibraryFunctionLoad[fname_?StringQ, fParams_, retType_, opts : OptionsPattern[SafeLibraryFunctionLoad]] :=
	SafeLibraryFunctionLoad[$PacletLibrary, fname, fParams, retType, opts];

SafeLibraryFunctionLoad[libName_?StringQ, fname_?StringQ, fParams_, retType_, opts : OptionsPattern[SafeLibraryFunctionLoad]] :=
	Quiet @ Check[
		(* There are 2 categories of function arguments:
		 * - regular - supported by LibraryLink (String, Integer, NumericArray, etc.)
		 * - special - extensions added by LLU or paclet developers that need extra parsing before they can be passed to LibraryLink,
		 *             for example Managed Expressions
		 *)
		Block[{specialArgs = SelectSpecialArgs[fParams], specialRetQ = CustomMResultTypeQ[retType], actualRetType, libFunction},
			actualRetType = If[specialRetQ, MResultCustomType[retType], retType];
			libFunction = If[Length @ specialArgs > 0,
				(* If the function that we are registering takes special arguments, we need to compose it with argumentParser function,
				 * which will parse input arguments before every call, so that they are accepted by LibraryLink.*)
				LibraryFunctionLoad[libName, fname, MArgumentCustomType /@ fParams, actualRetType] @* ArgumentParser[specialArgs]
				,
				LibraryFunctionLoad[libName, fname, fParams, actualRetType]
			];
			If[specialRetQ,
				MResultTransform[retType] @* libFunction
				,
				libFunction
			]
		]
		,
		If[TrueQ @ OptionValue["Optional"],
			Missing["NotInLibrary"]
			,
			Throw @ CreatePacletFailure[
				"FunctionLoadFailure",
				"MessageParameters" -> <|"FunctionName" -> fname, "LibraryName" -> libName, "Details" -> ToString @ LibraryLink`$LibraryError|>]
		]
	];

Options[SafeLibraryFunction] = {
	"ProgressMonitor" -> None,
	"Throws" -> False
};

holdSet[Hold[sym_], rhs_] := sym = rhs;

SafeLibraryFunction[fname_?StringQ, fParams_, retType_, opts : OptionsPattern[]] :=
    SafeLibraryFunction[$PacletLibrary, fname, fParams, retType, opts];

SafeLibraryFunction[libName_?StringQ, fname_?StringQ, fParams_, retType_, opts : OptionsPattern[]] :=
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
	    errorHandler @* SafeLibraryFunctionLoad[libName, fname, fParams, retType, loadOptions]
	    , (* else *)
	    If[Not @ Developer`SymbolQ @ ReleaseHold @ pmSymbol,
		    Throw @ CreatePacletFailure["ProgressMonInvalidValue"];
	    ];
	    newParams = Append[fParams, {Real, 1, "Shared"}];
	    f = errorHandler @* SafeLibraryFunctionLoad[libName, fname, newParams, retType, loadOptions];
	    (
		    holdSet[pmSymbol, Developer`ToPackedArray[{0.0}]];
		    f[##, ReleaseHold[pmSymbol]]
	    )&
    ]
];

SafeWSTPFunction[libName_String, fname_String, opts : OptionsPattern[SafeLibraryFunction]] :=
	SafeLibraryFunction[libName, fname, LinkObject, LinkObject, opts];

SafeWSTPFunction[fname_String, opts : OptionsPattern[SafeLibraryFunction]] :=
	SafeLibraryFunction[fname, LinkObject, LinkObject, opts];

LibraryMemberFunction[exprHead_][libName_, fname_String, fParams_, retType_, opts : OptionsPattern[SafeLibraryFunction]] :=
    If[fParams === LinkObject && retType === LinkObject,
	    SafeWSTPFunction[libName, fname, opts]
	    ,
		SafeLibraryFunction[libName, fname, Prepend[fParams, Managed[exprHead]], retType, opts]
    ];

Attributes[iLoadLibraryFunction] = {HoldFirst};
iLoadLibraryFunction[symbol_, loader_, libraryName_, args___, opts : OptionsPattern[]] :=
	Module[{loadingOpts, functionOpts},
		loadingOpts = FilterRules[{opts}, Options[LoadLibraryFunction]];
		functionOpts = Complement[{opts}, loadingOpts];
		Clear[symbol];
		LazyLoad[
			symbol,
			(
				(* Library name is evaluated at the point of calling LoadLibraryFunction and it may be None if the library has not been loaded yet.
				 * In this case, we do not pass it to a loading function, but instead we let the loading function use $PacletLibrary, which must be
				 * initialized by the time the function is actually being loaded. *)
				If[libraryName === None,
					loader[args, functionOpts]
					,
					loader[libraryName, args, functionOpts]
				]
			)
		]
	];

Attributes[guessFunctionNameFromSymbol] = {HoldFirst};
guessFunctionNameFromSymbol[symbol_] := StringReplace["$" ~~ s_ :> s] @ SymbolName[Unevaluated[symbol]];


(* LoadLibraryFunction[resultSymbol_, lib_, f_, fParams_, fResultType_, opts___] attempts to load an exported function f from a dynamic library lib and assign
 * the result to resultSymbol. Loading is lazy, which means that it will actually happen on the first evaluation of the resultSymbol.
 * Arguments:
 * - resultSymbol_ - a WL symbol to represent the loaded function
 * - lib_String - name of the dynamic library
 * - f_String - name of the function to load from the dynamic library
 * - fParams_ - parameter types of the library function to be loaded
 * - fResultType_ - result type
 * Options:
 * All options for SafeLibraryFunction and SafeLibraryFunctionLoad are accepted.
 *)
Options[LoadLibraryFunction] = Union[Options[SafeLibraryFunction], Options[SafeLibraryFunctionLoad]];

Attributes[LoadLibraryFunction] = {HoldFirst};
LoadLibraryFunction[symbol_, libraryName_, funcNameInLib_?StringQ, paramTypes_, retType_, opts : OptionsPattern[]] :=
	iLoadLibraryFunction[symbol, SafeLibraryFunction, libraryName, funcNameInLib, paramTypes, retType, opts];

LoadLibraryFunction[symbol_, funcNameInLib_?StringQ, paramTypes_, retType_, opts : OptionsPattern[]] :=
	LoadLibraryFunction[symbol, $PacletLibrary, funcNameInLib, paramTypes, retType, opts];

LoadLibraryFunction[symbol_, paramTypes_, retType_, opts : OptionsPattern[]] :=
	LoadLibraryFunction[symbol, $PacletLibrary, guessFunctionNameFromSymbol[symbol], paramTypes, retType, opts];

(* LoadWSTPFunction[resultSymbol_, lib_, f_, opts___] - convenient wrapper around LoadLibraryFunction for easier loading of WSTP functions. *)
Attributes[LoadWSTPFunction] = {HoldFirst};
LoadWSTPFunction[symbol_, libraryName_, funcNameInLib_?StringQ, opts : OptionsPattern[]] :=
	iLoadLibraryFunction[symbol, SafeLibraryFunction, libraryName, funcNameInLib, LinkObject, LinkObject, opts];
LoadWSTPFunction[symbol_, funcNameInLib_?StringQ, opts : OptionsPattern[]] :=
	LoadWSTPFunction[symbol, $PacletLibrary, funcNameInLib, opts];
LoadWSTPFunction[symbol_, opts : OptionsPattern[]] :=
	LoadWSTPFunction[symbol, $PacletLibrary, guessFunctionNameFromSymbol[symbol], opts];


(* ::SubSection:: *)
(* Lazy loading library functions *)
(* ------------------------------------------------------------------------- *)

(* Here is a typical way to load a library function and assign it to a symbol:
 *     f = SafeLibraryFunction["functionName", {Integer}, String];
 * This code will immediately evaluate SafeLibraryFunction call and assign the result to f.
 * If your paclet loads 50 functions during initialization, it may be frustrating for the users to wait that long. Even worse,
 * if you initialized LLU with a relative path to the dynamic library, you will end up calling FindLibrary 50 times, which takes considerable amount of time.
 *
 * One possible solution is to replace "f = " with "f := " ta avoid immediate evaluation of the RHS, but this would result in evaluating RHS every time
 * f is used. To address this, LLU defines a helper function Memoize:
 *)

Attributes[Memoize] = {HoldAll};
Memoize[expr_] := expr = expr;

(* You can now lazy load your functions like this:
 *     f := Memoize @ SafeLibraryFunction["functionName", {Integer}, String];
 *
 * In case you want to be even more explicit about what your code does, LLU also provides a function LazyLoad, to be used in the following way:
 *     LazyLoad[f, SafeLibraryFunction["functionName", {Integer}, String]];
 * This will evaluate the second argument only once, when f is first used.
 *
 * Both LazyLoad and Memoize can be used not only with SafeLibraryFunction but also with related functions like SafeWSTPFunction.
 *)

Attributes[LazyLoad] = {HoldAll};
LazyLoad[f_?Developer`SymbolQ, expr_] := (f := f = expr);


(* ::SubSection:: *)
(* RegisterPacletErrors *)
(* ------------------------------------------------------------------------- *)
(* ------------------------------------------------------------------------- *)

RegisterCppErrors[] := AssociateTo[$CorePacletFailureLUT, $GetCErrorCodes[]];

RegisterPacletErrors[errors_?AssociationQ] :=
	Block[{maxIDError, maxID},
		maxIDError = First[MaximalBy[$CorePacletFailureLUT, First], {}];
		maxID = Max[First[maxIDError, 0], $MaxLLUBuiltinErrorID];
		AssociateTo[$CorePacletFailureLUT, MapIndexed[#[[1]] -> {(First[#2] + maxID), #[[2]]} &, Normal[errors]]]
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
Block[{slotNames, slotValues},
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

End[]; (* `Private` *)

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

Begin["`Private`"];

(* ::SubSection:: *)
(* Managed Expressions *)
(* ------------------------------------------------------------------------- *)
(* ------------------------------------------------------------------------- *)

(* Register Managed Expressions as custom argument type *)
CustomMArgumentTypeQ[a_Managed] := True;

(* When ManagedExpression is passed to a library function it will be translated to an Integer *)
MArgumentCustomType[_Managed] := Integer;

(* Parse ManagedExpression before it is passed to a LibraryFunction. First argument is expected ManagedExpression type and the second is actual instance
 * of a MangedExpression. If the instance type does not match the expected type a paclet Failure will be thrown.
 *)
MArgumentTransform[Managed[expectedHead_]] := Replace[{
	expectedHead[id_Integer] :> If[ManagedLibraryExpressionQ[expectedHead[id]],
		id
		,
		Throw @ CreatePacletFailure["InvalidManagedExpressionID", "MessageParameters" -> <|"Expr" -> expectedHead[id]|>]
	]
	,
	id_Integer :> id (* Passing bare IDs of Managed Expressions is supported but may be more error prone than passing proper MLEs *)
	,
	e_ :> Throw @ CreatePacletFailure["UnexpectedManagedExpression", "MessageParameters" -> <|"Expected" -> expectedHead, "Actual" -> e|>]
}];

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

LoadMemberFunction[exprHead_][memberSymbol_?Developer`SymbolQ, libraryName_, fname_, fParams_, retType_, opts : OptionsPattern[]] := (
	If[Not @ Developer`SymbolQ @ ClassMember[exprHead, memberSymbol],
		Clear @ Evaluate @ ClassMemberName[exprHead, memberSymbol];
	];
	exprHead /: exprHead[id_][memberSymbol[args___]] := ClassMember[exprHead, memberSymbol][exprHead[id], args];
	iLoadLibraryFunction[Evaluate @ ClassMember[exprHead, memberSymbol], LibraryMemberFunction[exprHead], libraryName, fname, fParams, retType, opts];
);
LoadMemberFunction[exprHead_][memberSymbol_, fname_, fParams_, retType_, opts : OptionsPattern[]] :=
	LoadMemberFunction[exprHead][memberSymbol, $PacletLibrary, fname, fParams, retType, opts];
LoadMemberFunction[exprHead_][memberSymbol_, fParams_, retType_, opts : OptionsPattern[]] :=
	LoadMemberFunction[exprHead][memberSymbol, $PacletLibrary, guessFunctionNameFromSymbol[memberSymbol], fParams, retType, opts];

LoadWSTPMemberFunction[exprHead_][memberSymbol_, libraryName_, fname_?StringQ, opts : OptionsPattern[]] :=
	LoadMemberFunction[exprHead][memberSymbol, libraryName, fname, LinkObject, LinkObject, opts];
LoadWSTPMemberFunction[exprHead_][memberSymbol_, fname_?StringQ, opts : OptionsPattern[]] :=
	LoadMemberFunction[exprHead][memberSymbol, $PacletLibrary, fname, LinkObject, LinkObject, opts];
LoadWSTPMemberFunction[exprHead_][memberSymbol_, opts : OptionsPattern[]] :=
	LoadMemberFunction[exprHead][memberSymbol, $PacletLibrary, guessFunctionNameFromSymbol[memberSymbol], LinkObject, LinkObject, opts];

(* ::SubSection:: *)
(* Specific paclet utilities *)
(* ------------------------------------------------------------------------- *)

(*
 * This section contains utility functions to be used in complex paclets that load multiple WL source files, define subcontexts, etc.
 * They are independent of any other part of LLU and they are not directly related to LibraryLink, so it is likely that they will be moved
 * to a different location in the future.
 *)

(*
 * Find all symbols exported from a paclet under the given context
 *)
FindPacletSymbols[context_?StringQ] :=
	Module[{paclet, extensions},
		paclet = First[PacletFind[First @ StringSplit[context, "`"]], {}];
		If[!PacletObjectQ[paclet],
			Return[{}]
		];
		extensions = Select[
			paclet["Extensions"],
			MatchQ[{"Kernel", ___, "Context" -> {context} | context, ___}]
		];
		Join @@ Cases[extensions, ("Symbols" -> x_) :> x, Infinity]
	];
FindPacletSymbols[___] := {};

(*
 * Load the specified files, protecting and readprotecting all symbols in `exportedContext`.
 * During the loading of the files, $Context will be set to `loadingContext` and the $ContextPath will be
 * `{exportedContext, "System`"}` unless specified otherwise with the "ContextPath" option.
 * File names given are interpreted relatively to `DirectoryName[$InputFileName]` unless overriden with the "FilePath" option.
 *)
Options[LoadFilesInContext] = {
	(* If "FilePath" is Automatic, it's assumed that the location of the files is `DirectoryName[$InputFileName]`.
	 * Otherwise, specify the directory containing the files as the value of the "FilePath" option. If not a valid directory,
	 * the option will be ignored
	 *)
	"FilePath" -> Automatic,

	(* If "ContextPath" is Automatic, $ContextPath will be set to `{exportedContext, "System`"}`.
	 * Alternatively, one can specify an arbitrary valid list of contexts. If the value of the option is not
	 * a list of strings ending in `, the option will be ignored
	 *)
	"ContextPath" -> Automatic
};
LoadFilesInContext[files: {__?StringQ} | _?StringQ, exportedContext_?StringQ, loadingContext_?StringQ, opts: OptionsPattern[]] :=
	Module[{symbols, path, contextPath},
		symbols = FindPacletSymbols[exportedContext];
		path = With[
			{folder = OptionValue["FilePath"]},
			If[DirectoryQ[folder],
				folder
				,
				DirectoryName[$InputFileName]
			]
		];
		contextPath = OptionValue["ContextPath"];
		If[Quiet @ !VectorQ[contextPath, StringEndsQ["`"]],
			contextPath = {exportedContext, "System`"}
		];
		Unprotect @@ symbols;
		Clear @@ symbols;
		Block[
			{$Context = loadingContext, $ContextPath = contextPath},
			Map[
				Get[FileNameJoin[{path, #}]] &,
				Developer`ToList @ files
			];
		];
		SetAttributes[#, {Protected, ReadProtected}] & /@ symbols;
	];
LoadFilesInContext[___] := $Failed;

End[]; (* `Private` *)
End[]; (* `LLU` *)

System`Private`RestoreContextPath[];
