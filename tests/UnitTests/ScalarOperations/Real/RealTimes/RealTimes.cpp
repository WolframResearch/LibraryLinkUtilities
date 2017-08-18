#include "WolframLibrary.h"
#include "MArgumentManager.h"

EXTERN_C DLLEXPORT int RealTimes(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	LibraryLinkUtils::MArgumentManager mngr(libData, Argc, Args, Res);

	auto in1 = mngr.getReal(0);
	auto in2 = mngr.getReal(1);
	auto out = in1 * in2;
	mngr.setReal(out);
	return LIBRARY_NO_ERROR;
}
