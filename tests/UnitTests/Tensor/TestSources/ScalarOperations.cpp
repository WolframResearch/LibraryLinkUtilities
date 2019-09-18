/* Include required header */
#include "LLU/MArgumentManager.h"

using namespace LLU;

/* Gets the I0 th Real number from the rank 1 tensor T0 */
EXTERN_C DLLEXPORT int getNthRealFromTR1(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	auto err = ErrorCode::NoError;
	try {
		MArgumentManager mngr(libData, Argc, Args, Res);

		auto t = mngr.getTensor<double>(0);
		auto i = mngr.getInteger<mint>(1) - 1;

		mngr.setReal(t.at(i)); // we use at() here to verify if the index i is not out-of-bounds
	}
	catch (const LibraryLinkError& e) {
		err = e.which();
	}
	catch (...) {
		err = ErrorCode::FunctionError;
	}
	return err;
}

/* Gets the (m,n) Real number from the rank 2 tensor T0 */
EXTERN_C DLLEXPORT int getNthRealFromTR2(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	auto err = ErrorCode::NoError;
	try {
		MArgumentManager mngr(libData, Argc, Args, Res);

		auto t = mngr.getTensor<double>(0);
		auto i = mngr.getInteger<mint>(1) - 1;
		auto j = mngr.getInteger<mint>(2) - 1;

		mngr.setReal(t[{ i, j }]);

	}
	catch (const LibraryLinkError& e) {
		err = e.which();
	}
	catch (...) {
		err = ErrorCode::FunctionError;
	}
	return err;
}

/* Gets the (m,n) Integer number from the rank 2 tensor T0 */
EXTERN_C DLLEXPORT int getNthIntegerFromTR2(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	auto err = ErrorCode::NoError;
	try {
		MArgumentManager mngr(libData, Argc, Args, Res);

		auto t = mngr.getTensor<mint>(0);
		auto i = mngr.getInteger<mint>(1) - 1;
		auto j = mngr.getInteger<mint>(2) - 1;

		mngr.setInteger(t[{ i, j }]);
	}
	catch (const LibraryLinkError& e) {
		err = e.which();
	}
	catch (...) {
		err = ErrorCode::FunctionError;
	}
	return err;
}

/**
 * Constructs a new rank 1 tensor of length I0, and sets the
 * ith element of the vector to 2*i
 **/
EXTERN_C DLLEXPORT int setNthIntegerT(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	auto err = ErrorCode::NoError;
	try {
		MArgumentManager mngr(libData, Argc, Args, Res);

		auto len = mngr.getInteger<mint>(0);

		Tensor<mint> t(0, { len });
		mint val = 2;
		std::for_each(t.begin(), t.end(), [&val](mint& elem) {
			elem = val;
			val += 2;
		});
		mngr.setTensor(t);
	}
	catch (const LibraryLinkError& e) {
		err = e.which();
	}
	catch (...) {
		err = ErrorCode::FunctionError;
	}
	return err;
}

/* Sets the element in the I0,I1 position in T0 to its value in T1, returning T0 */
EXTERN_C DLLEXPORT int setI0I1T(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	MTensor T0, T1;
	mint I0, I1;
	mint pos[2];
	int err = LIBRARY_NO_ERROR;

	T0 = MArgument_getMTensor(Args[0]);
	T1 = MArgument_getMTensor(Args[1]);
	I0 = MArgument_getInteger(Args[2]);
	I1 = MArgument_getInteger(Args[3]);
	pos[0] = I0;
	pos[1] = I1;
	err = libData->MTensor_setMTensor(T0, T1, pos, 2);
	MArgument_setMTensor(Res, T0);
	return err;
}

/* Gets the subpart of the input tensor starting at the I0,I1 th position */
EXTERN_C DLLEXPORT int getSubpartT(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	MTensor T0, T1 = 0;
	mint I0, I1;
	mint pos[2];
	int err = LIBRARY_NO_ERROR;

	T0 = MArgument_getMTensor(Args[0]);
	I0 = MArgument_getInteger(Args[1]);
	I1 = MArgument_getInteger(Args[2]);
	pos[0] = I0;
	pos[1] = I1;
	err = libData->MTensor_getMTensor(T0, pos, 2, &T1);
	MArgument_setMTensor(Res, T1);
	return err;
}
