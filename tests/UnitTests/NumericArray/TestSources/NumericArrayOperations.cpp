#include <iostream>
#include <memory>
#include <numeric>
#include <type_traits>

#include "LLU/ErrorLog/Logger.h"
#include "LLU/LLU.h"
#include "LLU/LibraryLinkFunctionMacro.h"

namespace NA = LLU::NA;
namespace ErrorCode = LLU::ErrorCode;

using LLU::LibraryLinkError;
using LLU::MArgumentManager;
using LLU::NumericArray;

static std::unique_ptr<LLU::GenericNumericArray<LLU::Passing::Shared>> shared_numeric;

EXTERN_C DLLEXPORT mint WolframLibrary_getVersion() {
	return WolframLibraryVersion;
}

EXTERN_C DLLEXPORT int WolframLibrary_initialize(WolframLibraryData libData) {
	LLU::LibraryData::setLibraryData(libData);
	LLU::ErrorManager::registerPacletErrors({{"InvalidConversionMethod", "NumericArray conversion method `method` is invalid."}});
	return 0;
}

LIBRARY_LINK_FUNCTION(CreateEmptyVector) {
	MArgumentManager mngr(libData, Argc, Args, Res);

	NumericArray<std::uint16_t> out(0, {0});

	mngr.setNumericArray(out);
	return ErrorCode::NoError;
}

LIBRARY_LINK_FUNCTION(CreateEmptyMatrix) {
	MArgumentManager mngr(libData, Argc, Args, Res);

	NumericArray<double> out(0, {3, 5, 0});

	mngr.setNumericArray(out);
	return ErrorCode::NoError;
}

struct MoveTester {
	LLU::DataList<LLU::MArgumentType::NumericArray, LLU::Passing::Manual>& result;

	template<typename T, typename P>
	void operator()(NumericArray<T, P> na) {
		auto na2 {std::move(na)};	 // test move constructor
		NumericArray<T> na3;
		na3 = std::move(na2);	 // test move assignment, but this will only move when P is Passing::Manual, otherwise it has to make a copy
		result.push_back(na3);
	}
};

LIBRARY_LINK_FUNCTION(echoNumericArrays) {
	auto err = ErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, Res);
		LLU::DataList<LLU::MArgumentType::NumericArray, LLU::Passing::Manual> result;

		MoveTester mt {result};

		mngr.operateOnNumericArray(0, mt);
		mngr.operateOnNumericArray<LLU::Passing::Manual>(1, mt);
		mngr.operateOnNumericArray<LLU::Passing::Shared>(2, mt);

		mngr.set(result);
	} catch (const LibraryLinkError& e) {
		err = e.which();
		std::cout << e.what() << std::endl;
	} catch (...) {
		err = ErrorCode::FunctionError;
	}
	return err;
}

/*
 * Numeric array library functions
 */
LIBRARY_LINK_FUNCTION(getNumericArrayLength) {
	auto err = ErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, Res);
		mngr.operateOnNumericArray(0, [&mngr](auto&& rarray) { mngr.setInteger(rarray.size()); });
	} catch (const LibraryLinkError& e) {
		err = e.which();
	} catch (...) {
		err = ErrorCode::FunctionError;
	}
	return err;
}

LIBRARY_LINK_FUNCTION(getNumericArrayRank) {
	auto err = ErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, Res);
		mngr.operateOnNumericArray(0, [&mngr](auto&& rarray) { mngr.setInteger(rarray.rank()); });
	} catch (const LibraryLinkError& e) {
		err = e.which();
	} catch (...) {
		err = ErrorCode::FunctionError;
	}
	return err;
}

// create new numeric array
LIBRARY_LINK_FUNCTION(newNumericArray) {
	auto err = ErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, Res);
		NumericArray<float> ra(0., {3, 3});
		mngr.setNumericArray(ra);
	} catch (const LibraryLinkError& e) {
		err = e.which();
	} catch (...) {
		err = ErrorCode::FunctionError;
	}
	return err;
}

struct CopyTester {
	LLU::DataList<LLU::MArgumentType::NumericArray, LLU::Passing::Manual>& result;

	template<typename T, typename P>
	void operator()(NumericArray<T, P> na) {
		NumericArray<T, LLU::Passing::Manual> na2 {na};	   // test copy constructor
		NumericArray<T> na3;
		na3 = na2;	  // test copy assignment
		result.push_back(na3);
	}
};

// clone NumericArray
LIBRARY_LINK_FUNCTION(cloneNumericArrays) {
	auto err = ErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, Res);
		LLU::DataList<LLU::MArgumentType::NumericArray, LLU::Passing::Manual> result;

		CopyTester mt {result};

		mngr.operateOnNumericArray(0, mt);
		mngr.operateOnNumericArray<LLU::Passing::Manual>(1, mt);
		mngr.operateOnNumericArray<LLU::Passing::Shared>(2, mt);

		mngr.set(result);
	} catch (const LibraryLinkError& e) {
		err = e.which();
	} catch (...) {
		err = ErrorCode::FunctionError;
	}
	return err;
}

LIBRARY_LINK_FUNCTION(changeSharedNumericArray) {
	auto err = ErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, Res);
		auto oldShareCount = shared_numeric ? shared_numeric->shareCount() : 0;
		shared_numeric = std::make_unique<LLU::GenericNumericArray<LLU::Passing::Shared>>(mngr.getGenericNumericArray<LLU::Passing::Shared>(0));
		mngr.set(10 * oldShareCount + shared_numeric->shareCount());
	} catch (const LibraryLinkError& e) {
		err = e.which();
	} catch (...) {
		err = ErrorCode::FunctionError;
	}
	return err;
}

EXTERN_C DLLEXPORT int getSharedNumericArray(WolframLibraryData libData, mint Argc, MArgument* Args, MArgument Res) {
	auto err = ErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, Res);
		if (shared_numeric) {
			mngr.set(*shared_numeric);
		} else {
			return ErrorCode::FunctionError;
		}
	} catch (const LibraryLinkError& e) {
		err = e.which();
	}
	return err;
}

struct ZeroReal64 {
	template<typename T, class P>
	void operator()(NumericArray<T, P>&&, MArgumentManager&) {
		LLU::ErrorManager::throwException(LLU::ErrorName::FunctionError);
	}

	template<class P>
	void operator()(NumericArray<double, P>&& ra, MArgumentManager& mngr) {
		std::fill(ra.begin(), ra.end(), 0.0);
		mngr.setNumericArray(ra);
	}
};

// reset NumericArray
EXTERN_C DLLEXPORT int numericZeroData(WolframLibraryData libData, mint Argc, MArgument* Args, MArgument Res) {
	auto err = ErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, Res);
		mngr.operateOnNumericArray<LLU::Passing::Automatic, ZeroReal64>(0, mngr);
	} catch (const LibraryLinkError& e) {
		err = e.which();
	} catch (...) {
		err = ErrorCode::FunctionError;
	}
	return err;
}

struct AccumulateIntegers {
	template<typename T, class P>
	std::enable_if_t<!std::is_integral<T>::value> operator()(const NumericArray<T, P>&, MArgumentManager&) {
		LLU::ErrorManager::throwException(LLU::ErrorName::FunctionError);
	}

	template<typename T, class P>
	std::enable_if_t<std::is_integral<T>::value> operator()(const NumericArray<T, P>& ra, MArgumentManager& mngr) {
		auto result = std::accumulate(ra.begin(), ra.end(), static_cast<T>(0));
		mngr.setInteger(result);
	}
};

// sum elements of a NumericArray but only if it is of integer type
EXTERN_C DLLEXPORT int accumulateIntegers(WolframLibraryData libData, mint Argc, MArgument* Args, MArgument Res) {
	auto err = ErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, Res);
		mngr.operateOnNumericArray<LLU::Passing::Constant, AccumulateIntegers>(0, mngr);
	} catch (const LibraryLinkError& e) {
		err = e.which();
	} catch (...) {
		err = ErrorCode::FunctionError;
	}
	return err;
}

// check if conversion methods are mapped correctly
LIBRARY_LINK_FUNCTION(convertMethodName) {
	auto err = ErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, Res);
		auto method = mngr.getInteger<NA::ConversionMethod>(0);
		std::string methodStr;
		switch (method) {
			case NA::ConversionMethod::Check: methodStr = "Check"; break;
			case NA::ConversionMethod::ClipCheck: methodStr = "ClipCheck"; break;
			case NA::ConversionMethod::Coerce: methodStr = "Coerce"; break;
			case NA::ConversionMethod::ClipCoerce: methodStr = "ClipCoerce"; break;
			case NA::ConversionMethod::Round: methodStr = "Round"; break;
			case NA::ConversionMethod::ClipRound: methodStr = "ClipRound"; break;
			case NA::ConversionMethod::Scale: methodStr = "Scale"; break;
			case NA::ConversionMethod::ClipScale: methodStr = "ClipScale"; break;
			default: LLU::ErrorManager::throwException("InvalidConversionMethod", static_cast<int>(method));
		}
		mngr.setString(std::move(methodStr));
	} catch (const LibraryLinkError& e) {
		err = e.which();
	} catch (...) {
		err = ErrorCode::FunctionError;
	}
	return err;
}

// convert NumericArray
LIBRARY_LINK_FUNCTION(convert) {
	auto err = ErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, Res);
		mngr.operateOnNumericArray(0, [&mngr](auto&& numArr) {
			NumericArray<std::uint16_t> converted {numArr, mngr.getInteger<NA::ConversionMethod>(1), mngr.getReal(2)};
			mngr.setNumericArray(converted);
		});
	} catch (const LibraryLinkError& e) {
		err = e.which();
	} catch (...) {
		err = ErrorCode::FunctionError;
	}
	return err;
}

// convert generic NumericArray
LIBRARY_LINK_FUNCTION(convertGeneric) {
	auto err = ErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, Res);
		auto numArr = mngr.getGenericNumericArray(0);
		NumericArray<std::uint16_t> converted {numArr, mngr.getInteger<NA::ConversionMethod>(1), mngr.getReal(2)};
		mngr.set(converted);
	} catch (const LibraryLinkError& e) {
		err = e.which();
	} catch (...) {
		err = ErrorCode::FunctionError;
	}
	return err;
}

LIBRARY_LINK_FUNCTION(TestDimensions) {
	auto err = ErrorCode::NoError;
	try {
		MArgumentManager mngr(libData, Argc, Args, Res);
		auto dims = mngr.getTensor<mint>(0);
		NumericArray<float> na(0.0, dims.asVector());
		mngr.setNumericArray(na);
	} catch (const LibraryLinkError& e) {
		err = e.which();
	}
	return err;
}

LIBRARY_LINK_FUNCTION(TestDimensions2) {
	auto err = ErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, Res);
		LLU::DataList<LLU::MArgumentType::NumericArray> naList;

		std::vector<std::vector<mint>> dimsList {{0}, {3}, {3, 0}, {3, 2}, {3, 2, 0}, {3, 2, 4}};

		for (auto& dims : dimsList) {
			NumericArray<float> na(0.0f, dims);
			naList.push_back(na);
		}
		mngr.setDataList(naList);
	} catch (const LibraryLinkError& e) {
		err = ErrorCode::FunctionError;
	}
	return err;
}

LIBRARY_LINK_FUNCTION(FlattenThroughList) {
	auto err = ErrorCode::NoError;
	try {
		MArgumentManager mngr(libData, Argc, Args, Res);
		LLU_DEBUG("NumericArray type is ", NA::typeToString(mngr.getNumericArrayType(0)));
		auto na = mngr.getNumericArray<std::int32_t>(0);
		std::list<std::int32_t> l {na.begin(), na.end()};
		NumericArray<std::int32_t> na2 {l};
		mngr.set(na2);
	} catch (const LibraryLinkError& e) {
		err = e.which();
	}
	return err;
}

LIBRARY_LINK_FUNCTION(CopyThroughTensor) {
	auto err = ErrorCode::NoError;
	try {
		MArgumentManager mngr(libData, Argc, Args, Res);
		LLU_DEBUG("NumericArray type is ", NA::typeToString(mngr.getNumericArrayType(0)));
		auto na = mngr.getNumericArray<double>(0);
		LLU::Tensor<double> t {na, na.dimensions()};
		NumericArray<double> na2 {t, t.dimensions()};
		mngr.set(na2);
	} catch (const LibraryLinkError& e) {
		err = e.which();
	}
	return err;
}
