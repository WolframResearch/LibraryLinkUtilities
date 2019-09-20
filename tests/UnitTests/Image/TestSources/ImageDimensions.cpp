#include "LLU/LLU.h"
#include "LLU/LibraryLinkFunctionMacro.h"

LIBRARY_LINK_FUNCTION(ImageColumnCount) {
	auto err = LLU::ErrorCode::NoError;
	try {
		LLU::MArgumentManager mngr(libData, Argc, Args, Res);
		auto image = mngr.getGenericImage(0);
		mngr.setInteger(image.columns());
	} catch (const LLU::LibraryLinkError& e) {
		err = e.which();
	} catch (...) {
		err = LLU::ErrorCode::FunctionError;
	}
	return err;
}

LIBRARY_LINK_FUNCTION(ImageRank) {
	auto err = LLU::ErrorCode::NoError;
	try {
		LLU::MArgumentManager mngr(libData, Argc, Args, Res);
		mngr.operateOnImage(0, [&mngr](auto&& image) { mngr.setInteger(image.rank()); });
	} catch (const LLU::LibraryLinkError& e) {
		err = e.which();
	} catch (...) {
		err = LLU::ErrorCode::FunctionError;
	}
	return err;
}

LIBRARY_LINK_FUNCTION(ImageRowCount) {
	auto err = LLU::ErrorCode::NoError;
	try {
		LLU::MArgumentManager mngr(libData, Argc, Args, Res);
		auto image = mngr.getGenericImage(0);
		mngr.setInteger(image.rows());
	} catch (const LLU::LibraryLinkError& e) {
		err = e.which();
	} catch (...) {
		err = LLU::ErrorCode::FunctionError;
	}
	return err;
}
