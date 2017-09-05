#include "WolframLibrary.h"
#include "MArgumentManager.h"

using namespace LibraryLinkUtils;

EXTERN_C DLLEXPORT int IntegerMatrixTranspose(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr(libData, Argc, Args, Res);

		auto t = mngr.getTensor<mint>(0);
		Tensor<mint> out(0, { t.dimension(1), t.dimension(0) });

		/* Set the elements of the output matrix */
		for (mint row = 0; row < out.dimension(0); row++) {
			for (mint col = 0; col < out.dimension(1); col++) {
				out[{ row, col }] = t[{ col, row }];
			}
		}
		mngr.setTensor(out);
	}
	catch (LibraryLinkError& e) {
		err = e.which();
	}
	catch (std::exception& e) {
		err = LLErrorCode::FunctionError;
	}
	return static_cast<int>(err);
}
