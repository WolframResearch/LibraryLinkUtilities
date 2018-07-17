#include "WolframLibrary.h"

#include "MArgumentManager.h"
#include "LibraryLinkError.h"
#include "LibraryLinkFunctionMacro.h"

using namespace LibraryLinkUtils;

LIBRARY_LINK_FUNCTION(ImageColumnCount) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr(libData, Argc, Args, Res);
		mngr.operateOnImage(0, [&mngr](auto&& image) {
			mngr.setInteger(image.columns());
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


LIBRARY_LINK_FUNCTION(ImageRank) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr(libData, Argc, Args, Res);
		mngr.operateOnImage(0, [&mngr](auto&& image) {
			mngr.setInteger(image.rank());
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


LIBRARY_LINK_FUNCTION(ImageRowCount) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr(libData, Argc, Args, Res);
		mngr.operateOnImage(0, [&mngr](auto&& image) {
			mngr.setInteger(image.rows());
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
