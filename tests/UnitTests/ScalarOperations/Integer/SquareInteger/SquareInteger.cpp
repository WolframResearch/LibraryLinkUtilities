#include "WolframLibrary.h"
#include "LLU/MArgumentManager.h"

EXTERN_C DLLEXPORT int SquareInteger(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	LibraryLinkUtils::MArgumentManager mngr(libData, Argc, Args, Res);

	auto x = mngr.getInteger<mint>(0);
	auto result = x * x;
	mngr.setInteger(result);
	return LIBRARY_NO_ERROR;
}
