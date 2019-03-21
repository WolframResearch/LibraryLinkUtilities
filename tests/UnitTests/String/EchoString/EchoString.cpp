#include "LLU/MArgumentManager.h"

EXTERN_C DLLEXPORT int EchoString(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	LibraryLinkUtils::MArgumentManager mngr(libData, Argc, Args, Res);

	auto* in1 = mngr.getCString(0);
	mngr.setString(in1);

	return LIBRARY_NO_ERROR;
}
