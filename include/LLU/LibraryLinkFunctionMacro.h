/**
 * @file	LibraryLinkFunctionMacro.h
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @date	10/08/2017
 *
 * @brief	Macros designed to workaround a MSVC bug concerning extern "C" and generic lambdas.
 * 			Defined in a separate header file to limit it's scope.
 * 			Use this macro only for interface functions (functions that will be loaded with LibraryFunctionLoad).
 *
 * @see		https://stackoverflow.com/questions/45590594/generic-lambda-in-extern-c-function
 */

#ifndef LLUTILS_LIBRARYLINKFUNCTIONMACRO_H_
#define LLUTILS_LIBRARYLINKFUNCTIONMACRO_H_

#define LIBRARY_LINK_FUNCTION(name) \
		EXTERN_C DLLEXPORT int name(WolframLibraryData, mint, MArgument*, MArgument); \
		int name(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res)

#define LIBRARY_MATHLINK_FUNCTION(name) \
		EXTERN_C DLLEXPORT int name(WolframLibraryData, MLINK); \
		int name(WolframLibraryData libData, MLINK mlp)

#endif /* LLUTILS_LIBRARYLINKFUNCTIONMACRO_H_ */
