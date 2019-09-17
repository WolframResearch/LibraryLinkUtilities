#include "LLU/MArgumentManager.h"
#include "LLU/LibraryLinkFunctionMacro.h"

using namespace LibraryLinkUtils;

LIBRARY_LINK_FUNCTION(EchoTensor) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr(libData, Argc, Args, Res);
		mngr.operateOnTensor(0, [&mngr](auto t1) {
			auto t2{std::move(t1)};  // test move constructor
			auto t3 = std::move(t2);  // test move assignment
			mngr.setTensor(t3);
		});
	} catch (const LibraryLinkError &e) {
		err = e.which();
	}
	return err;
}

LIBRARY_LINK_FUNCTION(EchoFirst) {
	MArgumentManager mngr(libData, Argc, Args, Res);

	auto t = mngr.getTensor<mint>(0);
	mngr.setInteger(t.front());

	return LLErrorCode::NoError;
}

LIBRARY_LINK_FUNCTION(EchoLast) {
	MArgumentManager mngr(libData, Argc, Args, Res);

	auto t = mngr.getTensor<mint>(0);
	mngr.setInteger(t.back());

	return LLErrorCode::NoError;
}

LIBRARY_LINK_FUNCTION(EchoElement) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, Res);
		auto na = mngr.getNumericArray<std::int64_t>(0);
		auto coords = mngr.getTensor<mint>(1);
		std::vector<mint> coordsVec(coords.begin(), coords.end());
		mngr.setInteger(na.at(coordsVec));
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
