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
	return ErrorCode::NoError;
}

EXTERN_C DLLEXPORT int CreateEmptyVector(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	MArgumentManager mngr(libData, Argc, Args, Res);

	Tensor<mint> out(0, {0});

	mngr.setTensor(out);
	return ErrorCode::NoError;
}

EXTERN_C DLLEXPORT int CreateEmptyMatrix(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	MArgumentManager mngr(libData, Argc, Args, Res);

	Tensor<mint> out(0, {3, 5, 0});

	mngr.setTensor(out);
	return ErrorCode::NoError;
}

//clone Tensor
LIBRARY_LINK_FUNCTION(CloneTensor) {
	auto err = ErrorCode::NoError;
	try {
		MArgumentManager mngr(libData, Argc, Args, Res);
		mngr.operateOnTensor(0, [&mngr](auto&& t1) {
			using T = typename std::decay_t<decltype(t1)>::value_type;
			Tensor<T, LLU::Passing::Manual> t2{t1};  // test copy constructor
			Tensor<T> t3;
			t3 = t2;  // test copy assignment
			mngr.setTensor(t3);
		});
	}
	catch (const LibraryLinkError &e) {
		err = e.which();
	}
	catch (...) {
		err = ErrorCode::FunctionError;
	}
	return err;
}

LIBRARY_LINK_FUNCTION(TestDimensions) {
	auto err = ErrorCode::NoError;
	try {
		MArgumentManager mngr(libData, Argc, Args, Res);
		auto dims = mngr.getTensor<mint, Passing::Manual>(0);
		Tensor<double> na(0.0, MArrayDimensions { dims.begin(), dims.end() });
		mngr.setTensor(na);
	} catch (const LibraryLinkError &e) {
		err = e.which();
	}
	return err;
}


LIBRARY_LINK_FUNCTION(TestDimensions2) {
	auto err = ErrorCode::NoError;
	try {
		MArgumentManager mngr(libData, Argc, Args, Res);
		DataList<MArgumentType::Tensor> naList;

		std::vector<std::vector<mint>> dimsList{
				{0},
				{3},
				{3, 0},
				{3, 2},
				{3, 2, 0},
				{3, 2, 4}
		};

		for (auto &dims : dimsList) {
			Tensor<double> na(0.0, dims);
			naList.push_back(na);
		}

		mngr.setDataList(naList);
	} catch (const LibraryLinkError &e) {
		err = ErrorCode::FunctionError;
	}
	return err;
}

LIBRARY_LINK_FUNCTION(EchoTensor) {
	auto err = ErrorCode::NoError;
	try {
		MArgumentManager mngr(libData, Argc, Args, Res);
		mngr.operateOnTensor(0, [&mngr](auto t1) {
			using T = typename std::decay_t<decltype(t1)>::value_type;
			auto t2 {std::move(t1)};  // test move constructor
			Tensor<T> t3;
			t3 = std::move(t2); // test move assignment
			mngr.setTensor(t3);
		});
	} catch (const LibraryLinkError &e) {
		err = e.which();
	}
	return err;
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
		auto na = mngr.getNumericArray<std::int64_t>(0);
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
	return ErrorCode::NoError;
}

LIBRARY_LINK_FUNCTION(FromVector) {
	MArgumentManager mngr(libData, Argc, Args, Res);

	std::vector<mint> v { 3, 5, 7, 9 };
	Tensor<mint> t {std::begin(v), std::end(v), {2, 2}};
	mngr.set(t);

	return ErrorCode::NoError;
}

LIBRARY_LINK_FUNCTION(FlattenThroughVector) {
	MArgumentManager mngr(libData, Argc, Args, Res);
	auto t = mngr.getTensor<mint>(0);
	auto v = t.asVector();

	Tensor<mint> t2 {v};
	mngr.set(t2);

	return ErrorCode::NoError;
}

LIBRARY_LINK_FUNCTION(CopyThroughNumericArray) {
	MArgumentManager mngr(libData, Argc, Args, Res);
	auto t = mngr.getTensor<mint>(0);
	NumericArray<mint> na {std::begin(t), std::end(t), t.dimensions()};

	Tensor<mint> t2{na, na.dimensions()};
	mngr.set(t2);

	return ErrorCode::NoError;
}