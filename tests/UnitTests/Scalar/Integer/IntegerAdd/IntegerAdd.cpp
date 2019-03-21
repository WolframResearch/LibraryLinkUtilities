#include "LLU/MArgumentManager.h"
 
EXTERN_C DLLEXPORT int IntegerAdd(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	LibraryLinkUtils::MArgumentManager mngr(libData, Argc, Args, Res);

	auto in1 = mngr.getInteger<mint>(0);
	auto in2 = mngr.getInteger<mint>(1);
	auto out = in1 + in2;
	mngr.setInteger(out);
	return LIBRARY_NO_ERROR;
}
