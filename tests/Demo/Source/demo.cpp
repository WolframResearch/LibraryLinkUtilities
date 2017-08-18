/* Include required header */
#include "WolframLibrary.h"

#include "MArgumentManager.h"
#include "LibraryLinkError.h"
#include "LibraryLinkFunctionMacro.h"

using namespace LibraryLinkUtils;

/* Return the version of Library Link */
EXTERN_C DLLEXPORT mint WolframLibrary_getVersion() {
	return WolframLibraryVersion;
}

/* Initialize Library */
EXTERN_C DLLEXPORT int WolframLibrary_initialize(WolframLibraryData libData) {
	MArgumentManager::setLibraryData(libData);
	return LIBRARY_NO_ERROR;
}

/* Uninitialize Library */
EXTERN_C DLLEXPORT void WolframLibrary_uninitialize(WolframLibraryData libData) {
	return;
}

/* Adds one to the input, returning the result  */
EXTERN_C DLLEXPORT int demo_I_I(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, Res);
		auto i = mngr.getInteger<mint>(0);
		mngr.setInteger(i + 1);
	}
	catch (LibraryLinkError& e) {
		err = e.which();
	}
	catch (std::exception& e) {
		err = LLErrorCode::FunctionError;
	}
	return static_cast<int>(err);
}

/* Adds one to the input, returning the result */
EXTERN_C DLLEXPORT int demo1_I_I(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, Res);
		auto i = mngr.getInteger<mint>(0);
		mngr.setInteger(i + 1);
	}
	catch (LibraryLinkError& e) {
		err = e.which();
	}
	catch (std::exception& e) {
		err = LLErrorCode::FunctionError;
	}
	return static_cast<int>(err);
}

/* Multiply two reals together, returning the result */
EXTERN_C DLLEXPORT int demo_R_R(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, Res);
		auto r1 = mngr.getReal(0);
		mngr.setReal(r1 * r1);
	}
	catch (LibraryLinkError& e) {
		err = e.which();
	}
	catch (std::exception& e) {
		err = LLErrorCode::FunctionError;
	}
	return static_cast<int>(err);
}

/* Multiply two reals together, returning the result */
EXTERN_C DLLEXPORT int demo1_R_R(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, Res);
		auto r1 = mngr.getReal(0);
		mngr.setReal(r1 * r1);
	}
	catch (LibraryLinkError& e) {
		err = e.which();
	}
	catch (std::exception& e) {
		err = LLErrorCode::FunctionError;
	}
	return static_cast<int>(err);
}

/* Returns Sum[i*R0, {i, I0}] */
EXTERN_C DLLEXPORT int demo_IR_R(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, Res);
		auto i0 = mngr.getInteger<mint>(0);
		auto r0 = mngr.getReal(1);
		double r1 = 0.;
		for (mint i = 0; i < i0; i++) {
			r1 += i * r0;
		}
		mngr.setReal(r1);
	}
	catch (LibraryLinkError& e) {
		err = e.which();
	}
	catch (std::exception& e) {
		err = LLErrorCode::FunctionError;
	}
	return static_cast<int>(err);
}

/* Returns Sum[i*R0, {i, I0}] */
EXTERN_C DLLEXPORT int demo1_IR_R(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, Res);
		auto i0 = mngr.getInteger<mint>(0);
		auto r0 = mngr.getReal(1);
		double r1 = 0.;
		for (mint i = 0; i < i0; i++) {
			r1 += i * r0;
		}
		mngr.setReal(r1);
	}
	catch (LibraryLinkError& e) {
		err = e.which();
	}
	catch (std::exception& e) {
		err = LLErrorCode::FunctionError;
	}
	return static_cast<int>(err);
}

/* Gets the I0 th Real number from the rank 1 tensor T0 */
EXTERN_C DLLEXPORT int demo_TI_R(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, Res);
		auto t0 = mngr.getTensor<double>(0);
		auto i0 = mngr.getInteger<mint>(1) - 1;
		mngr.setReal(t0[i0]);
	}
	catch (LibraryLinkError& e) {
		err = e.which();
	}
	catch (std::exception& e) {
		err = LLErrorCode::FunctionError;
	}
	return static_cast<int>(err);
}

/**
 * Same as demo_TI_R, but pass in a packed array then manually free it.
 **/
EXTERN_C DLLEXPORT int demo1_TI_R(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, Res);
		auto t0 = mngr.getTensor<double>(0);
		auto i0 = mngr.getInteger<mint>(1) - 1;
		mngr.setReal(t0[i0]);
	}
	catch (LibraryLinkError& e) {
		err = e.which();
	}
	catch (std::exception& e) {
		err = LLErrorCode::FunctionError;
	}
	return static_cast<int>(err);
}

/**
 * Same as demo_TI_R, but just to avoid copying, so we need 
 * to disown the tensor input.
 **/
EXTERN_C DLLEXPORT int demo2_TI_R(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	auto err = LLErrorCode::NoError;
	Tensor<double> t0;
	try {
		MArgumentManager mngr(Argc, Args, Res);
		t0 = mngr.getTensor<double>(0);
		auto i0 = mngr.getInteger<mint>(1) - 1;
		mngr.setReal(t0[i0]);
	}
	catch (LibraryLinkError& e) {
		err = e.which();
	}
	catch (std::exception& e) {
		err = LLErrorCode::FunctionError;
	}
	t0.disown();
	return static_cast<int>(err);
}

/**
 * Constructs a new rank 1 tensor of length I0, and sets the
 * ith element of the vector to 2*i
 **/
EXTERN_C DLLEXPORT int demo_I_T(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, Res);
		auto i0 = mngr.getInteger<mint>(0);
		Tensor<mint> t0(static_cast<mint>(0), { i0 });
		for (mint i = 1; i <= i0; i++)
			t0[i - 1] = 2 * i;
		mngr.setTensor(t0);
	}
	catch (LibraryLinkError& e) {
		err = e.which();
	}
	catch (std::exception& e) {
		err = LLErrorCode::FunctionError;
	}
	return static_cast<int>(err);
}

/**
 * Same as demo_TI_R, but just to avoid copying
 **/
EXTERN_C DLLEXPORT int demo_TT_T(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, Res);
		auto t0 = mngr.getTensor<double>(0);
		auto t1 = mngr.getTensor<mint>(1);
		auto r0 = t1[t0[0] - 1];

		Tensor<double> t2(0., { });
		t2[0] = r0;

		mngr.setTensor(t2);
	}
	catch (LibraryLinkError& e) {
		err = e.which();
	}
	catch (std::exception& e) {
		err = LLErrorCode::FunctionError;
	}
	return static_cast<int>(err);
}

/**
 * Intended to demonstrate working with rank 0 tensors.
 *
 * The arguments are three tensors.  
 * The first is a rank > 0 real tensor.
 * The second is a rank 0 integer tensor.
 * The third a rank 0 real tensor.
 *
 * The second argument is used as an index to find
 * an element of the first, which is added to the
 * third argument to form the result.
 *
 * The result is returned as a rank 0 real tensor.
 **/
EXTERN_C DLLEXPORT int demo_TTT_T(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, Res);
		auto T_arg = mngr.getTensor<double>(0);
		auto T_I_arg = mngr.getTensor<mint>(1);
		auto T_R_arg = mngr.getTensor<double>(2);

		auto R0 = T_R_arg[0];
		R0 += T_arg[T_I_arg[0] - 1];

		Tensor<mint> T_res(static_cast<mint>(0), { });
		T_res[0] = static_cast<mint>(R0);

		mngr.setTensor(T_res);
	}
	catch (LibraryLinkError& e) {
		err = e.which();
	}
	catch (std::exception& e) {
		err = LLErrorCode::FunctionError;
	}
	return static_cast<int>(err);
}

/**
 * Constructs a copy of the input tensor with the number of elements,
 * rank, and type appended at the end.
 **/
LIBRARY_LINK_FUNCTION(demo_T_T) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, Res);
		mngr.operateOnTensor(0, [&mngr](auto T_arg) {
			using TensorType = typename decltype(T_arg)::value_type;

			auto rank = T_arg.rank();
			const auto& dims = T_arg.dimensions();
			auto num = T_arg.size();

			/*
			 The result is going to have all the elements, also
			 the length of each dimension, number of elems, rank and type.
			 */
			mint len = num + rank + 3;

			Tensor<TensorType> T_res(static_cast<TensorType>(0), {len});

			std::copy(std::begin(T_arg), std::end(T_arg), std::begin(T_res));
			std::copy(std::begin(dims), std::end(dims), std::begin(T_res) + num);

			T_res[num + rank] = num;
			T_res[num + rank + 1] = rank;
			T_res[num + rank + 2] = T_res.getType();

			mngr.setTensor(T_res);
		});
	}
	catch (LibraryLinkError& e) {
		err = e.which();
	}
	catch (std::exception& e) {
		err = LLErrorCode::FunctionError;
	}
	return static_cast<int>(err);
}

/**
 * Constructs a copy of the input tensor with the number of elements,
 * rank, and type appended at the end.
 **/
LIBRARY_LINK_FUNCTION(demo1_T_T) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, Res);
		mngr.operateOnTensor(0, [&mngr](auto T_arg) {
			using TensorType = typename decltype(T_arg)::value_type;

			auto rank = T_arg.rank();
			const auto& dims = T_arg.dimensions();
			auto num = T_arg.size();

			/*
			 The result is going to have all the elements, also
			 the length of each dimension, number of elems, rank and type.
			 */
			mint len = num + rank + 3;

			Tensor<TensorType> T_res(static_cast<TensorType>(0), {len} );

			std::copy(std::begin(T_arg), std::end(T_arg), std::begin(T_res));
			std::copy(std::begin(dims), std::end(dims), std::begin(T_res) + num);

			T_res[num + rank] = num;
			T_res[num + rank + 1] = rank;
			T_res[num + rank + 2] = T_res.getType();

			mngr.setTensor(T_res);
		});
	}
	catch (LibraryLinkError& e) {
		err = e.which();
	}
	catch (std::exception& e) {
		err = LLErrorCode::FunctionError;
	}
	return static_cast<int>(err);
}

/**
 * Constructs a new rank 1 tensor with dimension I0, and sets the
 * ith element with the value 2*i. The newly constructed tensor is
 * returned.
 **/
EXTERN_C DLLEXPORT int demo1_I_T(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, Res);
		auto i = mngr.getInteger<mint>(0);

		Tensor<mint> T0(static_cast<mint>(0), { i });
		mint k = 1;
		for (auto& elem : T0)
			elem = 2 * k++;
		mngr.setTensor(T0);
	}
	catch (LibraryLinkError& e) {
		err = e.which();
	}
	catch (std::exception& e) {
		err = LLErrorCode::FunctionError;
	}
	return static_cast<int>(err);
}

/* Gets the I0th element of T0, returning that value */
EXTERN_C DLLEXPORT int demo_T_I(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, Res);
		auto t0 = mngr.getTensor<mint>(0);
		auto i0 = mngr.getInteger<mint>(1);
		mngr.setInteger(t0[i0 - 1]);
	}
	catch (LibraryLinkError& e) {
		err = e.which();
	}
	catch (std::exception& e) {
		err = LLErrorCode::FunctionError;
	}
	return static_cast<int>(err);
}

EXTERN_C DLLEXPORT int demo_MintSize(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, Res);
		mngr.setInteger(sizeof(mint));
	}
	catch (LibraryLinkError& e) {
		err = e.which();
	}
	catch (std::exception& e) {
		err = LLErrorCode::FunctionError;
	}
	return static_cast<int>(err);
}

/* Gets the I0,I1 th integer element of T0 returning that value */
EXTERN_C DLLEXPORT int demo_TII_I(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, Res);
		auto t0 = mngr.getTensor<mint>(0);
		auto i0 = mngr.getInteger<mint>(1) - 1;
		auto i1 = mngr.getInteger<mint>(2) - 1;
		mngr.setInteger(t0[ { i0, i1 }]);
	}
	catch (LibraryLinkError& e) {
		err = e.which();
	}
	catch (std::exception& e) {
		err = LLErrorCode::FunctionError;
	}
	return static_cast<int>(err);
}

/* Sets the I0,I1 th integer element of T0 with value, returning that position */
EXTERN_C DLLEXPORT int demo_TIII_I(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, Res);
		auto t0 = mngr.getTensor<mint>(0);
		auto i0 = mngr.getInteger<mint>(1) - 1;
		auto i1 = mngr.getInteger<mint>(2) - 1;
		auto value = mngr.getInteger<mint>(3);
		t0[ { i0, i1 }] = value;
		mngr.setInteger(t0[ { i0, i1 }]);
	}
	catch (LibraryLinkError& e) {
		err = e.which();
	}
	catch (std::exception& e) {
		err = LLErrorCode::FunctionError;
	}
	return static_cast<int>(err);
}

/* Gets the I0,I1 th real element of T0 returning that value */
EXTERN_C DLLEXPORT int demo_TII_R(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, Res);
		auto t0 = mngr.getTensor<double>(0);
		auto i0 = mngr.getInteger<mint>(1) - 1;
		auto i1 = mngr.getInteger<mint>(2) - 1;
		mngr.setReal(t0[ { i0, i1 }]);
	}
	catch (LibraryLinkError& e) {
		err = e.which();
	}
	catch (std::exception& e) {
		err = LLErrorCode::FunctionError;
	}
	return static_cast<int>(err);
}

/* Sets the I0,I1 th real element of T0 with value, returning that position */
EXTERN_C DLLEXPORT int demo_TIIR_R(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, Res);
		auto t0 = mngr.getTensor<double>(0);
		auto i0 = mngr.getInteger<mint>(1) - 1;
		auto i1 = mngr.getInteger<mint>(2) - 1;
		auto value = mngr.getReal(3);
		t0[ { i0, i1 }] = value;
		mngr.setReal(t0[ { i0, i1 }]);
	}
	catch (LibraryLinkError& e) {
		err = e.which();
	}
	catch (std::exception& e) {
		err = LLErrorCode::FunctionError;
	}
	return static_cast<int>(err);
}

/* Gets the subpart of the input tensor starting at the I0 th position */
EXTERN_C DLLEXPORT int demo_TI_T(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	MTensor T0, T1 = 0;
	mint I0;
	int err = LIBRARY_NO_ERROR;

	T0 = MArgument_getMTensor(Args[0]);
	I0 = MArgument_getInteger(Args[1]);
	err = libData->MTensor_getMTensor(T0, &I0, 1, &T1);
	MArgument_setMTensor(Res, T1);
	return err;
}

/* Sets the I0 th element in T0 to its value in T1 */
EXTERN_C DLLEXPORT int demo_TTI_T(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	MTensor T0, T1;
	mint I0;
	int err = LIBRARY_NO_ERROR;

	T0 = MArgument_getMTensor(Args[0]);
	T1 = MArgument_getMTensor(Args[1]);
	I0 = MArgument_getInteger(Args[2]);
	err = libData->MTensor_setMTensor(T0, T1, &I0, 1);
	MArgument_setMTensor(Res, T0);
	return err;
}

/* Gets the subpart of the input tensor starting at the I0,I1 th position */
EXTERN_C DLLEXPORT int demo_TII_T(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
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

/* Sets the element in the I0,I1 position in T0 to its value in T1, returning T0 */
EXTERN_C DLLEXPORT int demo_TTII_T(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
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

/* Accepts no inputs, but returns a result */
EXTERN_C DLLEXPORT int demoNoArguments(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	MArgumentManager mngr(Argc, Args, Res);
	mngr.setInteger(Argc);
	return LIBRARY_NO_ERROR;
}

/* Accepts inputs, but returns nothing */
EXTERN_C DLLEXPORT int demoNoResult(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	MArgumentManager mngr(Argc, Args, Res);
	auto T = mngr.getTensor<mint>(0);

	T[0] = MArgument_getAddress(Res) == 0 ? 0 : 1;
	return LIBRARY_NO_ERROR;
}

/* Returns BitNot[b1] of the input*/
EXTERN_C DLLEXPORT int demoBoolean1(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	MArgumentManager mngr(Argc, Args, Res);
	auto b = mngr.getBoolean(0);
	mngr.setBoolean(!b);
	return LIBRARY_NO_ERROR;
}

/* Returns the complex conjugate of the complex number inputed */
EXTERN_C DLLEXPORT int demoComplex1(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	MArgumentManager mngr(Argc, Args, Res);
	auto c = mngr.getComplex(0);
	mngr.setComplex(std::conj(c));
	return LIBRARY_NO_ERROR;
}

/* Sets the output to the number of arguments passed in to the function */
EXTERN_C DLLEXPORT int demoNoInput(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	MArgumentManager mngr(Argc, Args, Res);
	mngr.setInteger(Argc);
	return LIBRARY_NO_ERROR;
}

/* Accepts and returns nothing */
EXTERN_C DLLEXPORT int demoVoid(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	MArgumentManager mngr(Argc, Args, Res);
	return LIBRARY_NO_ERROR;
}

/* Accepts and returns nothing */
EXTERN_C DLLEXPORT int demoNoInputVoid(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	MArgumentManager mngr(Argc, Args, Res);
	return LIBRARY_NO_ERROR;
}

