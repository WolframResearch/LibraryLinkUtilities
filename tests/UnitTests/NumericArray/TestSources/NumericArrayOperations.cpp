#include "WolframLibrary.h"
#include "WolframNumericArrayLibrary.h"

#include <iostream>
#include <numeric>
#include <type_traits>

#include "LLU/MArgumentManager.h"
#include "LLU/LibraryLinkFunctionMacro.h"
#include "../../../../include/LLU/Containers/NumericArray.h"

using namespace LibraryLinkUtils;

static MNumericArray shared_raw = 0;

EXTERN_C DLLEXPORT mint WolframLibrary_getVersion() {
	return WolframLibraryVersion;
}

EXTERN_C DLLEXPORT int WolframLibrary_initialize(WolframLibraryData libData) {
	MArgumentManager::setLibraryData(libData);
	return 0;
}

LIBRARY_LINK_FUNCTION(echoNumericArray) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, Res);
		mngr.operateOnNumericArray(0, [&mngr](auto&& rarray1) {
			auto rarray2(std::move(rarray1));
			mngr.setNumericArray(rarray2);
		});
	}
	catch (const LibraryLinkError& e) {
		err = e.which();
		std::cout << e.what() << std::endl;
	}
	catch (...) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}

/*
 * Numeric array library functions
 */
LIBRARY_LINK_FUNCTION(getNumericArrayLength) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, Res);
		mngr.operateOnNumericArray(0, [&mngr](auto&& rarray) {
			mngr.setInteger(rarray.size());
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

LIBRARY_LINK_FUNCTION(getNumericArrayRank) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, Res);
		mngr.operateOnNumericArray(0, [&mngr](auto&& rarray) {
			mngr.setInteger(rarray.rank());
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

//create new numeric array
LIBRARY_LINK_FUNCTION(newNumericArray) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, Res);
		NumericArray<float> ra(0., { 3, 3 });
		mngr.setNumericArray(ra);
	}
	catch (const LibraryLinkError& e) {
		err = e.which();
	}
	catch (...) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}

//clone NumericArray
LIBRARY_LINK_FUNCTION(cloneNumericArray) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, Res);
		mngr.operateOnNumericArray(0, [&mngr](auto&& rarray1) {
			auto rarray2 = rarray1;
			mngr.setNumericArray(rarray2);
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

LIBRARY_LINK_FUNCTION(changeSharedNumericArray) {
	WolframNumericArrayLibrary_Functions rawArrayFunctions = libData->numericarrayLibraryFunctions;
	int err = LIBRARY_NO_ERROR;

	if (shared_raw) {
		rawArrayFunctions->MNumericArray_disown(shared_raw);
		shared_raw = 0;
	}

	shared_raw = MArgument_getMNumericArray(Args[0]);

	return err;
}

EXTERN_C DLLEXPORT int getSharedNumericArray(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	int err = LIBRARY_NO_ERROR;

	if (shared_raw) {
		MArgument_setMNumericArray(Res, shared_raw);
	}
	else
		err = LIBRARY_FUNCTION_ERROR;

	return err;
}

struct ZeroReal64 {
	template<typename T>
	void operator()(NumericArray<T>, MArgumentManager&) {
		ErrorManager::throwException(LLErrorName::FunctionError);
	}

	void operator()(NumericArray<double>& ra, MArgumentManager& mngr) {
		std::fill(ra.begin(), ra.end(), 0.0);
		mngr.setNumericArray(ra);
	}
};

//reset NumericArray
EXTERN_C DLLEXPORT int rawZeroData(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, Res);
		mngr.operateOnNumericArray<ZeroReal64>(0, mngr);
	}
	catch (const LibraryLinkError& e) {
		err = e.which();
	}
	catch (...) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}

struct AccumulateIntegers {
	template<typename T>
	std::enable_if_t<!std::is_integral<T>::value> operator()(NumericArray<T>, MArgumentManager&) {
		ErrorManager::throwException(LLErrorName::FunctionError);
	}

	template<typename T>
	std::enable_if_t<std::is_integral<T>::value> operator()(NumericArray<T> ra, MArgumentManager& mngr) {
		auto result = std::accumulate(ra.begin(), ra.end(), 0L);
		mngr.setInteger(result);
	}
};

//sum elements of a NumericArray but only if it is of integer type
EXTERN_C DLLEXPORT int accumulateIntegers(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, Res);
		mngr.operateOnNumericArray<AccumulateIntegers>(0, mngr);
	}
	catch (const LibraryLinkError& e) {
		err = e.which();
	}
	catch (...) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}

// convert NumericArray
LIBRARY_LINK_FUNCTION(convert) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, Res);
		mngr.operateOnNumericArray(0, [&mngr](const auto& numArr) {
			NumericArray<std::uint16_t> converted { numArr, static_cast<NA::ConversionMethod>(mngr.getInteger<int>(1))};
			mngr.setNumericArray(converted);
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