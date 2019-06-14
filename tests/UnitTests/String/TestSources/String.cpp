/**
 * @file	String.cpp
 * @brief	
 */

#include <cctype>
#include <string>

#include "LLU/LLU.h"
#include "LLU/LibraryLinkFunctionMacro.h"

EXTERN_C DLLEXPORT int Greetings(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	LLU::MArgumentManager mngr(libData, Argc, Args, Res);

	auto name = mngr.getString(0);
	mngr.setString(std::string("Greetings ") + name + "!");
	return LIBRARY_NO_ERROR;
}

EXTERN_C DLLEXPORT int EchoString(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	LLU::MArgumentManager mngr(libData, Argc, Args, Res);

	auto *in1 = mngr.getCString(0);
	mngr.setString(in1);

	return LIBRARY_NO_ERROR;
}

EXTERN_C DLLEXPORT int HelloWorld(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	LLU::MArgumentManager mngr(libData, Argc, Args, Res);

	mngr.setString("Hello World");
	return LIBRARY_NO_ERROR;
}

LIBRARY_LINK_FUNCTION(CapitalizeFirst) {
	LLU::MArgumentManager mngr(libData, Argc, Args, Res);

	auto *in = mngr.getCString(0);
	in[0] = std::toupper(static_cast<unsigned char>(in[0]));
	mngr.setString(in);
	return LIBRARY_NO_ERROR;
}

LIBRARY_LINK_FUNCTION(CapitalizeAll) {
	LLU::MArgumentManager mngr(libData, Argc, Args, Res);

	auto in = mngr.getString(0);
	for (auto &c : in) {
		c = std::toupper(static_cast<unsigned char>(c));
	}
	mngr.setString(in);
	return LIBRARY_NO_ERROR;
}

LIBRARY_LINK_FUNCTION(RoundTripCString) {
	LLU::MArgumentManager mngr(libData, Argc, Args, Res);

	auto *in = mngr.getCString(0);
	mngr.setString(in);
	return LIBRARY_NO_ERROR;
}

LIBRARY_LINK_FUNCTION(RoundTripString) {
	LLU::MArgumentManager mngr(libData, Argc, Args, Res);

	auto in = mngr.getString(0);
	mngr.setString(in);
	return LIBRARY_NO_ERROR;
}

EXTERN_C DLLEXPORT int StringLength(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	LLU::MArgumentManager mngr(libData, Argc, Args, Res);

	auto name = mngr.getString(0);
	mngr.setInteger(name.length());
	return LIBRARY_NO_ERROR;
}