#include "LLU/MArgumentManager.h"

#include <numeric>

using namespace LibraryLinkUtils;

EXTERN_C DLLEXPORT int MeanValue(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	MArgumentManager mngr(libData, Argc, Args, Res);

	auto t = mngr.getTensor<double>(0);

	auto total = std::accumulate(t.begin(), t.end(), 0.0);

	auto result = total / t.size();
	mngr.setReal(result);
	return LIBRARY_NO_ERROR;
}
