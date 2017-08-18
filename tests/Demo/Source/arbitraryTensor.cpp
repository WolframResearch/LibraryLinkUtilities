/*
 An example that demonstrates using Shared memory management for 
 communicating between Mathematica and a Wolfram Library.
*/

#include "WolframLibrary.h"

#include "MArgumentManager.h"
#include "LibraryLinkError.h"
#include "LibraryLinkFunctionMacro.h"

using namespace LibraryLinkUtils;

template<typename T>
static Tensor<T> tensor;

template<>
Tensor<double> tensor<double>;

template<>
Tensor<std::complex<double>> tensor<std::complex<double>>;

template<>
Tensor<mint> tensor<mint>;

EXTERN_C DLLEXPORT mint WolframLibrary_getVersion( ) {
	return WolframLibraryVersion;
}

EXTERN_C DLLEXPORT int WolframLibrary_initialize( WolframLibraryData libData) {
	MArgumentManager::setLibraryData(libData);
	return 0;
}

LIBRARY_LINK_FUNCTION(loadArray) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, Res);
		mngr.operateOnTensor(0, [](auto t) {
			using T = typename decltype(t)::value_type;
			tensor<T> = std::move(t);
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

EXTERN_C DLLEXPORT int getElementShared(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, Res);
		auto pos = mngr.getInteger<mint>(0);
		mngr.setReal(tensor<double>[pos]);
	}
	catch (LibraryLinkError& e) {
		err = e.which();
	}
	catch (std::exception& e) {
		err = LLErrorCode::FunctionError;
	}
	return static_cast<int>(err);
}

EXTERN_C DLLEXPORT int getElementNonShared(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, Res);
		auto pos = mngr.getInteger<mint>(0);
		auto T0 = mngr.getTensor<double>(1);
		mngr.setReal(T0[pos]);
	}
	catch (LibraryLinkError& e) {
		err = e.which();
	}
	catch (std::exception& e) {
		err = LLErrorCode::FunctionError;
	}
	return static_cast<int>(err);
}


EXTERN_C DLLEXPORT int unloadArray(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {

	tensor<double>.disown();
	return 0;
}




