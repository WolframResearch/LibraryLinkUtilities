#include "WolframLibrary.h"
#include "WolframImageLibrary.h"

#include "MArgumentManager.h"
#include "Image.h"
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
	catch (LibraryLinkError& e) {
		err = e.which();
	}
	catch (std::exception& e) {
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
	catch (LibraryLinkError& e) {
		err = e.which();
	}
	catch (std::exception& e) {
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
	catch (LibraryLinkError& e) {
		err = e.which();
	}
	catch (std::exception& e) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}
