#include "WolframLibrary.h"
#include "LLU/MArgumentManager.h"

EXTERN_C DLLEXPORT int BooleanNot(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	LibraryLinkUtils::MArgumentManager mngr(libData, Argc, Args, Res);
	auto in1 = mngr.getBoolean(0);
	mngr.setBoolean(!in1);
	return LIBRARY_NO_ERROR;
}
