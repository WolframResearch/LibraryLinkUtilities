#include "WolframLibrary.h"
#include "LLU/MArgumentManager.h"

EXTERN_C DLLEXPORT int BooleanAnd(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	LibraryLinkUtils::MArgumentManager mngr(libData, Argc, Args, Res);
	auto in1 = mngr.getBoolean(0);
	auto in2 = mngr.getBoolean(1);
	mngr.setBoolean(in1 && in2);
	return LIBRARY_NO_ERROR;
}
