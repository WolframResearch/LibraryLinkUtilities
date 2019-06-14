/**
 * @file	Basic.cpp
 * @brief	
 */

#include <numeric>

#include "LLU/LLU.h"
#include "LLU/LibraryLinkFunctionMacro.h"

using namespace LLU;

EXTERN_C DLLEXPORT int CreateMatrix(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	MArgumentManager mngr(libData, Argc, Args, Res);

	auto rows = mngr.getInteger<mint>(0);
	auto cols = mngr.getInteger<mint>(1);

	Tensor<mint> out(0, {rows, cols});

	mint count = 1;
	for (auto &elem : out) {
		elem = count++;
	}

	mngr.setTensor(out);
	return LIBRARY_NO_ERROR;
}

LIBRARY_LINK_FUNCTION(EchoTensor) {
	MArgumentManager mngr(libData, Argc, Args, Res);

	mngr.operateOnTensor(0, [&mngr](auto &&t) {
		mngr.setTensor(t);
	});
	return LIBRARY_NO_ERROR;
}

LIBRARY_LINK_FUNCTION(EchoFirst) {
	MArgumentManager mngr(libData, Argc, Args, Res);

	auto t = mngr.getTensor<mint>(0);
	mngr.setInteger(t.front());

	return ErrorCode::NoError;
}

LIBRARY_LINK_FUNCTION(EchoLast) {
	MArgumentManager mngr(libData, Argc, Args, Res);

	auto t = mngr.getTensor<mint>(0);
	mngr.setInteger(t.back());

	return ErrorCode::NoError;
}

LIBRARY_LINK_FUNCTION(EchoElement) {
	auto err = ErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, Res);
		auto na{mngr.getNumericArray<std::int64_t>(0)};
		auto coords = mngr.getTensor<mint>(1);
		std::vector<mint> coordsVec(coords.begin(), coords.end());
		mngr.setInteger(na.at(coordsVec));
	}
	catch (const LibraryLinkError &e) {
		err = e.which();
		std::cout << e.what() << std::endl;
	}
	catch (...) {
		err = ErrorCode::FunctionError;
	}
	return err;
}

EXTERN_C DLLEXPORT int IntegerMatrixTranspose(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	auto err = ErrorCode::NoError;
	try {
		MArgumentManager mngr(libData, Argc, Args, Res);

		auto t = mngr.getTensor<mint>(0);
		Tensor<mint> out(0, {t.dimension(1), t.dimension(0)});

		/* Set the elements of the output matrix */
		for (mint row = 0; row < out.dimension(0); row++) {
			for (mint col = 0; col < out.dimension(1); col++) {
				out[{row, col}] = t[{col, row}];
			}
		}
		mngr.setTensor(out);
	}
	catch (LibraryLinkError &e) {
		err = e.which();
	}
	catch (std::exception &e) {
		err = ErrorCode::FunctionError;
	}
	return err;
}

EXTERN_C DLLEXPORT int MeanValue(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	MArgumentManager mngr(libData, Argc, Args, Res);

	auto t = mngr.getTensor<double>(0);

	auto total = std::accumulate(t.begin(), t.end(), 0.0);

	auto result = total / t.size();
	mngr.setReal(result);
	return LIBRARY_NO_ERROR;
}
