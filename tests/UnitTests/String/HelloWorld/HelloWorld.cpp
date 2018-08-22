#include <cctype>

#include "LLU/LLU.h"
#include "LLU/LibraryLinkFunctionMacro.h"

EXTERN_C DLLEXPORT int HelloWorld(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	LibraryLinkUtils::MArgumentManager mngr(libData, Argc, Args, Res);

	mngr.setString("Hello World");
	return LIBRARY_NO_ERROR;
}

LIBRARY_LINK_FUNCTION(CapitalizeFirst) {
	LibraryLinkUtils::MArgumentManager mngr(libData, Argc, Args, Res);

	auto* in = mngr.getCString(0);
	in[0] = std::toupper(static_cast<unsigned char>(in[0]));
	mngr.setString(in);
	return LIBRARY_NO_ERROR;
}

LIBRARY_LINK_FUNCTION(CapitalizeAll) {
	LibraryLinkUtils::MArgumentManager mngr(libData, Argc, Args, Res);

	auto in = mngr.getString(0);
	for (auto& c : in) {
		c = std::toupper(static_cast<unsigned char>(c));
	}
	mngr.setString(in);
	return LIBRARY_NO_ERROR;
}

LIBRARY_LINK_FUNCTION(RoundTripCString) {
	LibraryLinkUtils::MArgumentManager mngr(libData, Argc, Args, Res);

	auto* in = mngr.getCString(0);
	mngr.setString(in);
	return LIBRARY_NO_ERROR;
}

LIBRARY_LINK_FUNCTION(RoundTripString) {
	LibraryLinkUtils::MArgumentManager mngr(libData, Argc, Args, Res);

	auto in = mngr.getString(0);
	mngr.setString(in);
	return LIBRARY_NO_ERROR;
}