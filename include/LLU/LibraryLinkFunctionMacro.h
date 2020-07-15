/**
 * @file	LibraryLinkFunctionMacro.h
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @date	10/08/2017
 *
 * @brief	Small collection of macros designed to reduce the amount of boilerplate code and to work around certain MSVC bug.
 * 			Defined in a separate header file to limit their scope.
 * 			Use those macros only for interface functions (functions that will be loaded with LibraryFunctionLoad).
 *
 * @see		https://stackoverflow.com/questions/45590594/generic-lambda-in-extern-c-function
 */

#ifndef LLU_LIBRARYLINKFUNCTIONMACRO_H
#define LLU_LIBRARYLINKFUNCTIONMACRO_H

#define LIBRARY_LINK_FUNCTION(name)                                               \
	EXTERN_C DLLEXPORT int name(WolframLibraryData, mint, MArgument*, MArgument); \
	int name([[maybe_unused]] WolframLibraryData libData, mint Argc, MArgument* Args, MArgument Res)

#define LIBRARY_WSTP_FUNCTION(name)                     \
	EXTERN_C DLLEXPORT int name(WolframLibraryData, WSLINK); \
	int name([[maybe_unused]] WolframLibraryData libData, WSLINK wsl)

#define LLU_LIBRARY_FUNCTION(name)                                      \
	void impl_##name(LLU::MArgumentManager&); /* forward declaration */ \
	LIBRARY_LINK_FUNCTION(name) {                                       \
		auto err = LLU::ErrorCode::NoError;                             \
		try {                                                           \
			LLU::MArgumentManager mngr {libData, Argc, Args, Res};      \
			impl_##name(mngr);                                          \
		} catch (const LLU::LibraryLinkError& e) {                      \
			err = e.which();                                            \
		} catch (...) {                                                 \
			err = LLU::ErrorCode::FunctionError;                        \
		}                                                               \
		return err;                                                     \
	}                                                                   \
	void impl_##name(LLU::MArgumentManager& mngr)

#define LLU_WSTP_FUNCTION(name)                                \
	void impl_##name(WSLINK&); /* forward declaration */       \
	LIBRARY_WSTP_FUNCTION(name) {                              \
		auto err = LLU::ErrorCode::NoError;                    \
		try {                                                  \
			impl_##name(wsl);                                  \
		} catch (const LLU::LibraryLinkError& e) {             \
			err = e.which();                                   \
		} catch (...) {                                        \
			err = LLU::ErrorCode::FunctionError;               \
		}                                                      \
		return err;                                            \
	}                                                          \
	void impl_##name(WSLINK& wsl)
	
#endif // LLU_LIBRARYLINKFUNCTIONMACRO_H
