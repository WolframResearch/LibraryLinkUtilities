#include "LLU/MArgumentManager.h"
#include "LLU/LibraryLinkFunctionMacro.h"

using namespace LibraryLinkUtils;

LIBRARY_LINK_FUNCTION(EchoTensor) {
	MArgumentManager mngr(libData, Argc, Args, Res);

	mngr.operateOnTensor(0, [&mngr](auto&& t) {
		mngr.setTensor(t);
	});
	return LIBRARY_NO_ERROR;
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
		auto na { mngr.getNumericArray<std::int64_t>(0) };
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
