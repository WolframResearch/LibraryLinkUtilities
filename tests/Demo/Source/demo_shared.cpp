/*
 An example that demonstrates using Shared memory management for 
 communicating between Mathematica and a DLL.
*/

#include <algorithm>
#include <type_traits>

#include "WolframLibrary.h"

#include "MArgumentManager.h"
#include "Tensor.h"
#include "LibraryLinkError.hpp"

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

EXTERN_C DLLEXPORT int loadArray(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	MArgumentManager mngr(Argc, Args, Res);
	mngr.operateOnTensor(0, [&](auto&& t) {
		using T = typename std::remove_reference_t<decltype(t)>::value_type;
		tensor<T> = std::move(t);
	});
	mngr.setInteger(0);
	return 0;
}

EXTERN_C DLLEXPORT int setElementVector(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	MArgumentManager mngr(Argc, Args, Res);
	auto pos = mngr.getInteger<mint>(0);
	auto value = mngr.getReal(1);
	
	tensor<double>[pos - 1] = value;
	
	mngr.setInteger(0);
	return 0;
}


EXTERN_C DLLEXPORT int getElementVector(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, Res);
		auto pos = mngr.getInteger<mint>(0);
		mngr.setReal(tensor<double>[pos - 1]);
	}
	catch (LibraryLinkError<LLErrorCode>& e) {
		err = e.which();
	}
	catch (std::exception& e) {
		err = LLErrorCode::FunctionError;
	}
	return static_cast<int>(err);
}


EXTERN_C DLLEXPORT int setElement(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	MArgumentManager mngr(Argc, Args, Res);
	auto pos1 = mngr.getInteger<mint>(0) - 1;
	auto pos2 = mngr.getInteger<mint>(1) - 1;
	auto value = mngr.getReal(2);
	
	tensor<double>[{pos1, pos2}] = value;
	
	mngr.setInteger(0);
	return 0;
}


EXTERN_C DLLEXPORT int getElement(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, Res);
		auto pos1 = mngr.getInteger<mint>(0) - 1;
		auto pos2 = mngr.getInteger<mint>(1) - 1;
		mngr.setReal(tensor<double>[{pos1, pos2}]);
	}
	catch (LibraryLinkError<LLErrorCode>& e) {
		err = e.which();
	}
	catch (std::exception& e) {
		err = LLErrorCode::FunctionError;
	}
	return static_cast<int>(err);
}




EXTERN_C DLLEXPORT int getArray(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	MArgumentManager mngr(Argc, Args, Res);
	mngr.setTensor(tensor<double>);
	return 0;
}



EXTERN_C DLLEXPORT int unloadArray(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	MArgumentManager mngr(Argc, Args, Res);
	tensor<double>.disown();
	mngr.setInteger(0);
	return 0;
}




