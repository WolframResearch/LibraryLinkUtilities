#include "WolframLibrary.h"
#include "LLU/MArgumentManager.h"
#include "LLU/Containers/Image.h"
#include "LLU/LibraryLinkError.h"
#include "LLU/LibraryLinkFunctionMacro.h"

#include <type_traits>
#include <cstdint>

using namespace LibraryLinkUtils;

LIBRARY_LINK_FUNCTION(EchoImage1) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr(libData, Argc, Args, Res);
		mngr.operateOnImage(0, [&mngr](auto&& in) {
			auto& out = in;
			mngr.setImage(out);
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

LIBRARY_LINK_FUNCTION(EchoImage2) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr(libData, Argc, Args, Res);

		mngr.operateOnImage(0, [&mngr](auto&& in) {
			auto slices = in.is3D()? in.slices() : 0;
			auto columns = in.columns();
			auto rows = in.rows();
			auto channels = in.channels();
			auto colorspace = in.colorspace();
			auto interleaving = in.interleavedQ();

			using T = typename std::remove_reference_t<decltype(in)>::value_type;
			Image<T> out(slices, columns, rows, channels, colorspace, interleaving);
			
			for (mint column = 1; column <= columns; column++) {
				for (mint row = 1; row <= rows; row++) {
					if (in.is3D()) {
						for (mint slice = 1; slice <= slices; slice++) {
							out.set(slice, row, column, 1, in.get(slice, row, column, 1));
						}
					} else {
						out.set(row, column, 1, in.get(row, column, 1));
					}
				}
			}
			mngr.setImage(out);
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

LIBRARY_LINK_FUNCTION(ConvertImageToByte) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr(libData, Argc, Args, Res);
		mngr.operateOnImage(0, [&mngr](auto&& in) {
			Image<std::uint8_t> out(in);
			mngr.setImage(out);
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

LIBRARY_LINK_FUNCTION(UnifyImageTypes) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr(libData, Argc, Args, Res);
		mngr.operateOnImage(0, [&mngr](auto&& in) {
			using T = typename std::remove_reference_t<decltype(in)>::value_type;
			T x = 0;
			mngr.operateOnImage(1, [&mngr, x](auto&& in2) {
				Image<decltype(x)> out(in2);
				mngr.setImage(out);
			});

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
