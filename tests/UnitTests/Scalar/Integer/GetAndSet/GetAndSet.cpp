#include "WolframLibrary.h"
#include "LLU/MArgumentManager.h"

mint value = 0;

EXTERN_C DLLEXPORT int llGet(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	LibraryLinkUtils::MArgumentManager mngr(libData, Argc, Args, Res);
	mngr.setInteger(value);
	return LIBRARY_NO_ERROR;
}

EXTERN_C DLLEXPORT int llSet(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	LibraryLinkUtils::MArgumentManager mngr(libData, Argc, Args, Res);
	value = mngr.getInteger<mint>(0);
	return LIBRARY_NO_ERROR;
}
