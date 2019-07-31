#include "LLU/MArgumentManager.h"

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
	return LLErrorCode::NoError;
}

EXTERN_C DLLEXPORT int CreateEmptyVector(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	MArgumentManager mngr(libData, Argc, Args, Res);

	Tensor<mint> out(0, {0});

	mngr.setTensor(out);
	return LLErrorCode::NoError;
}

EXTERN_C DLLEXPORT int CreateEmptyMatrix(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	MArgumentManager mngr(libData, Argc, Args, Res);

	Tensor<mint> out(0, {3, 5, 0});

	mngr.setTensor(out);
	return LLErrorCode::NoError;
}

//clone Tensor
EXTERN_C DLLEXPORT int CloneTensor(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
		auto err = LLErrorCode::NoError;
		try {
			MArgumentManager mngr(libData, Argc, Args, Res);
			mngr.operateOnTensor(0, [&mngr](auto t1) {
				auto t2{t1};  // test copy constructor
				auto t3 = t2;  // test copy assignment
				mngr.setTensor(t3);
			});
		}
		catch (const LibraryLinkError& e) {
			err = e.which();
		}
		catch (...) {
			err = LLErrorCode::FunctionError;
		}
		return err;
}

EXTERN_C DLLEXPORT int TestDimensions(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
		auto err = LLErrorCode::NoError;
		try {
			MArgumentManager mngr(libData, Argc, Args, Res);
			Tensor<mint> dims = mngr.getTensor<mint>(0);
			Tensor<double> na(0.0, dims);
			mngr.setTensor(na);
		} catch (const LibraryLinkError &e) {
			err = e.which();
		}
		return err;
}


EXTERN_C DLLEXPORT int TestDimensions2(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
		auto err = LLErrorCode::NoError;
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
				naList.push_back(na.getInternal());
				na.setOwner(false);
			}

			mngr.setDataList(naList);
		} catch (const LibraryLinkError &e) {
			err = LLErrorCode::FunctionError;
		}
		return err;
}