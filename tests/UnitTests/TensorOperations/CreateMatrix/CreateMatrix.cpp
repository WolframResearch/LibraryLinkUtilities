#include "WolframLibrary.h"
#include "MArgumentManager.h"

using namespace LibraryLinkUtils;

EXTERN_C DLLEXPORT int CreateMatrix(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {

	MArgumentManager mngr(libData, Argc, Args, Res);

	auto rows = mngr.getInteger<mint>(0);
	auto cols = mngr.getInteger<mint>(1);

	Tensor<mint> out(0, { rows, cols });

	mint count = 1;
	for (auto& elem : out) {
		elem = count++;
	}

	mngr.setTensor(out);
	return LIBRARY_NO_ERROR;
}
