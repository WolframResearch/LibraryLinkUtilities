#include "WolframLibrary.h"
#include "LLU/MArgumentManager.h"

EXTERN_C DLLEXPORT int ComplexTimes(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	LibraryLinkUtils::MArgumentManager mngr(libData, Argc, Args, Res);

	auto in1 = mngr.getComplex(0);
	auto in2 = mngr.getComplex(1);
	mngr.setComplex(in1 * in2);
	return LIBRARY_NO_ERROR;
}
