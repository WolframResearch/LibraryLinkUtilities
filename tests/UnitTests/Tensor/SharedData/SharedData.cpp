#include "WolframLibrary.h"

#include "LLU/LibraryLinkFunctionMacro.h"
#include "LLU/MArgumentManager.h"
#include "LLU/LibraryLinkError.h"
#include "LLU/Containers/Tensor.h"

using namespace LibraryLinkUtils;

static Tensor<double> tensor;

EXTERN_C DLLEXPORT mint WolframLibrary_getVersion() {
	return WolframLibraryVersion;
}

EXTERN_C DLLEXPORT int WolframLibrary_initialize(WolframLibraryData libData) {
	MArgumentManager::setLibraryData(libData);
	return 0;
}

EXTERN_C DLLEXPORT int loadRealArray(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, Res);
		tensor = mngr.getTensor<double>(0);
	}
	catch (const LibraryLinkError& e) {
		err = e.which();
	}
	catch (...) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}

EXTERN_C DLLEXPORT int getRealArray(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, Res);
		auto& out = tensor;
		if (out.getInternal() != nullptr)
			mngr.setTensor(out);
		else
			return LIBRARY_FUNCTION_ERROR;
	}
	catch (const LibraryLinkError& e) {
		err = e.which();
	}
	catch (...) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}

EXTERN_C DLLEXPORT int doubleRealArray(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, Res);
		auto& out = tensor;

		for (auto& elem : out) {
			elem *= 2;
		}
		mngr.setTensor(out);
	}
	catch (const LibraryLinkError& e) {
		err = e.which();
	}
	catch (...) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}

EXTERN_C DLLEXPORT int unloadRealArray(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, Res);
		tensor.disown();
		mngr.setInteger(0);
	}
	catch (const LibraryLinkError& e) {
		err = e.which();
	}
	catch (...) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}

//Modify the contents of tensor in C function
EXTERN_C DLLEXPORT int add1(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, Res);
		auto tx = mngr.getTensor<double>(0);

		for (auto& elem : tx) {
			elem++;
		}
	}
	catch (const LibraryLinkError& e) {
		err = e.which();
	}
	catch (...) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}

LIBRARY_LINK_FUNCTION(copyShared) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, Res);
		auto sharedTensor = mngr.getTensor<double>(0);
		auto sc = sharedTensor.shareCount();
		auto copy = sharedTensor; // create deep copy of the shared Tensor
		mngr.setInteger(100 * sc + 10 * sharedTensor.shareCount() + copy.shareCount());
	}
	catch (const LibraryLinkError& e) {
		err = e.which();
	}
	catch (...) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}