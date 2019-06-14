#include <memory>

#include "WolframLibrary.h"

#include "LLU/LibraryLinkFunctionMacro.h"
#include "LLU/MArgumentManager.h"
#include "LLU/Containers/Tensor.h"
#include "LLU/Containers/Passing/Shared.hpp"

using namespace LLU;

using SharedTensorPtr = std::unique_ptr<Tensor<double, Passing::Shared>>;

static SharedTensorPtr tensor {};

EXTERN_C DLLEXPORT mint WolframLibrary_getVersion() {
	return WolframLibraryVersion;
}

EXTERN_C DLLEXPORT int WolframLibrary_initialize(WolframLibraryData libData) {
	LibraryData::setLibraryData(libData);
	return 0;
}

EXTERN_C DLLEXPORT int loadRealArray(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	auto err = ErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, Res);
		tensor = std::make_unique<Tensor<double, Passing::Shared>>(mngr.getTensor<double, Passing::Shared>(0));
	}
	catch (const LibraryLinkError& e) {
		err = e.which();
	}
	catch (...) {
		err = ErrorCode::FunctionError;
	}
	return err;
}

EXTERN_C DLLEXPORT int getRealArray(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	auto err = ErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, Res);
		if (!tensor) {
            return ErrorCode::FunctionError;
		}
		auto& out = *tensor;
		mngr.setTensor(out);
	}
	catch (const LibraryLinkError& e) {
		err = e.which();
	}
	catch (...) {
		err = ErrorCode::FunctionError;
	}
	return err;
}

EXTERN_C DLLEXPORT int doubleRealArray(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	auto err = ErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, Res);
        if (!tensor) {
            return ErrorCode::FunctionError;
        }
		auto& out = *tensor;

		for (auto& elem : out) {
			elem *= 2;
		}
		mngr.setTensor(out);
	}
	catch (const LibraryLinkError& e) {
		err = e.which();
	}
	catch (...) {
		err = ErrorCode::FunctionError;
	}
	return err;
}

EXTERN_C DLLEXPORT int unloadRealArray(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	auto err = ErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, Res);
        if (!tensor) {
            return ErrorCode::FunctionError;
        }
		mngr.setInteger(tensor->shareCount());
        tensor.release();
	}
	catch (const LibraryLinkError& e) {
		err = e.which();
	}
	catch (...) {
		err = ErrorCode::FunctionError;
	}
	return err;
}

//Modify the contents of tensor in C function
EXTERN_C DLLEXPORT int add1(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	auto err = ErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, Res);
		auto tx = mngr.getTensor<double, Passing::Shared>(0);

		for (auto& elem : tx) {
			elem++;
		}
	}
	catch (const LibraryLinkError& e) {
		err = e.which();
	}
	catch (...) {
		err = ErrorCode::FunctionError;
	}
	return err;
}

LIBRARY_LINK_FUNCTION(copyShared) {
	auto err = ErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, Res);
		auto sharedTensor = mngr.getTensor<double, Passing::Shared>(0);
		auto sc = sharedTensor.shareCount();
		Tensor<double> copy { sharedTensor }; // create deep copy of the shared Tensor. The new Tensor is not Shared
		mngr.setInteger(100 * sc + 10 * sharedTensor.shareCount() + copy.shareCount());
	}
	catch (const LibraryLinkError& e) {
		err = e.which();
	}
	catch (...) {
		err = ErrorCode::FunctionError;
	}
	return err;
}
