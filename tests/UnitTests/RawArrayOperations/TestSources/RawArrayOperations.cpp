#include "WolframLibrary.h"
#include "WolframRawArrayLibrary.h"

#include "MArgumentManager.h"

using namespace LibraryLinkUtils;

static MRawArray shared_raw = 0;

EXTERN_C DLLEXPORT mint WolframLibrary_getVersion() {
	return WolframLibraryVersion;
}

EXTERN_C DLLEXPORT int WolframLibrary_initialize(WolframLibraryData libData) {
	MArgumentManager::setLibraryData(libData);
	return 0;
}

EXTERN_C DLLEXPORT int echoRawArray(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, Res);
		mngr.operateOnRawArray(0, [&mngr](auto&& rarray1) {
			std::cout << "RawArray to be moved" << std::endl;
			auto rarray2(std::move(rarray1));
			std::cout << "RawArray moved" << std::endl;
			mngr.setRawArray(rarray2);
			std::cout << "RawArray returned" << std::endl;
		});
	}
	catch (LibraryLinkError<LLErrorCode>& e) {
		err = e.which();
		std::cout << e.what() << std::endl;
	}
	catch (std::exception& e) {
		err = LLErrorCode::FunctionError;
	}
	return static_cast<int>(err);
}

/*
 * Raw array library functions
 */
EXTERN_C DLLEXPORT int getRawArrayLength(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, Res);
		mngr.operateOnRawArray(0, [&mngr](auto&& rarray) {
			mngr.setInteger(rarray.size());
		});
	}
	catch (LibraryLinkError<LLErrorCode>& e) {
		err = e.which();
	}
	catch (std::exception& e) {
		err = LLErrorCode::FunctionError;
	}
	return static_cast<int>(err);
}

EXTERN_C DLLEXPORT int getRawArrayRank(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, Res);
		mngr.operateOnRawArray(0, [&mngr](auto&& rarray) {
			mngr.setInteger(rarray.rank());
		});
	}
	catch (LibraryLinkError<LLErrorCode>& e) {
		err = e.which();
	}
	catch (std::exception& e) {
		err = LLErrorCode::FunctionError;
	}
	return static_cast<int>(err);
}

//create new raw array
EXTERN_C DLLEXPORT int newRawArray(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, Res);
		RawArray<float> ra(0., { 3, 3 });
		mngr.setRawArray(ra);
	}
	catch (LibraryLinkError<LLErrorCode>& e) {
		err = e.which();
	}
	catch (std::exception& e) {
		err = LLErrorCode::FunctionError;
	}
	return static_cast<int>(err);
}

//clone RawArray
EXTERN_C DLLEXPORT int cloneRawArray(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, Res);
		mngr.operateOnRawArray(0, [&mngr](auto&& rarray1) {
			auto rarray2 = rarray1;
			mngr.setRawArray(rarray2);
		});
	}
	catch (LibraryLinkError<LLErrorCode>& e) {
		err = e.which();
	}
	catch (std::exception& e) {
		err = LLErrorCode::FunctionError;
	}
	return static_cast<int>(err);
}

EXTERN_C DLLEXPORT int changeSharedRawArray(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	WolframRawArrayLibrary_Functions rawArrayFunctions = libData->rawarrayLibraryFunctions;
	int err = LIBRARY_NO_ERROR;

	if (shared_raw) {
		rawArrayFunctions->MRawArray_disown(shared_raw);
		shared_raw = 0;
	}

	shared_raw = MArgument_getMRawArray(Args[0]);

	return err;
}

EXTERN_C DLLEXPORT int getSharedRawArray(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	int err = LIBRARY_NO_ERROR;

	if (shared_raw) {
		MArgument_setMRawArray(Res, shared_raw);
	}
	else
		err = LIBRARY_FUNCTION_ERROR;

	return err;
}

struct ZeroReal64 {
	template<typename T>
	void operator()(RawArray<T>, MArgumentManager&) {
		throw LibraryLinkError<LLErrorCode>(LLErrorCode::FunctionError);
	}

	void operator()(RawArray<double>& ra, MArgumentManager& mngr) {
		std::fill(ra.begin(), ra.end(), 0.0);
		mngr.setRawArray(ra);
	}
};

//reset rawArray
EXTERN_C DLLEXPORT int rawZeroData(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, Res);
		mngr.operateOnRawArray<ZeroReal64>(0, mngr);
	}
	catch (LibraryLinkError<LLErrorCode>& e) {
		err = e.which();
	}
	catch (std::exception& e) {
		err = LLErrorCode::FunctionError;
	}
	return static_cast<int>(err);
}
