#include "LLU/LLU.h"
#include "LLU/LibraryLinkFunctionMacro.h"

#include <type_traits>
#include <cstdint>

namespace LLErrorCode = LLU::ErrorCode;

LIBRARY_LINK_FUNCTION(EchoImage1) {
	auto err = LLErrorCode::NoError;
	try {
		LLU::MArgumentManager mngr(libData, Argc, Args, Res);
		mngr.operateOnImage(0, [&mngr](auto&& im1) {
			using T = typename std::remove_reference_t<decltype(im1)>::value_type;
			auto im2{std::move(im1)};  // test move constructor
			LLU::Image<T> im3;
			im3 = std::move(im2);  // test move assignment
			mngr.setImage(im3);
		});
	}
	catch (const LLU::LibraryLinkError& e) {
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
		LLU::MArgumentManager mngr(libData, Argc, Args, Res);

		mngr.operateOnImage(0, [&mngr](auto&& in) {
			auto slices = in.is3D()? in.slices() : 0;
			auto columns = in.columns();
			auto rows = in.rows();
			auto channels = in.channels();
			auto colorspace = in.colorspace();
			auto interleaving = in.interleavedQ();

			using T = typename std::remove_reference_t<decltype(in)>::value_type;
			LLU::Image<T> out(slices, columns, rows, channels, colorspace, interleaving);
			
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
	catch (const LLU::LibraryLinkError& e) {
		err = e.which();
	}
	catch (...) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}

LIBRARY_LINK_FUNCTION(EchoImage3) {
	auto err = LLErrorCode::NoError;
	try {
		LLU::MArgumentManager mngr(libData, Argc, Args, Res);
		auto img = mngr.getGenericImage(0);
		mngr.set(img);
	}
	catch (const LLU::LibraryLinkError &e) {
		err = e.which();
	}
	return err;
}

LIBRARY_LINK_FUNCTION(ConvertImageToByte) {
	auto err = LLErrorCode::NoError;
	try {
		LLU::MArgumentManager mngr(libData, Argc, Args, Res);
		mngr.operateOnImage(0, [&mngr](auto&& in) {
			LLU::Image<std::uint8_t> out(in);
			mngr.setImage(out);
		});
	}
	catch (const LLU::LibraryLinkError& e) {
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
		LLU::MArgumentManager mngr(libData, Argc, Args, Res);
		mngr.operateOnImage(0, [&mngr](auto&& in) {
			using T = typename std::remove_reference_t<decltype(in)>::value_type;
			T x = 0;
			mngr.operateOnImage(1, [&mngr, x](auto&& in2) {
				LLU::Image<decltype(x)> out(in2);
				mngr.setImage(out);
			});

		});
	}
	catch (const LLU::LibraryLinkError& e) {
		err = e.which();
	}
	catch (...) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}

LIBRARY_LINK_FUNCTION(CloneImage) {
	auto err = LLErrorCode::NoError;
	try {
		LLU::MArgumentManager mngr(libData, Argc, Args, Res);
		mngr.operateOnImage(0, [&mngr](auto&& im1) {
			using T = typename std::remove_reference_t<decltype(im1)>::value_type;
			LLU::Image<T, LLU::Passing::Manual> im2 {im1};  // test copy constructor
			LLU::Image<T> im3;
			im3 = im2;  // test copy assignment
			mngr.setImage(im3);
		});
	}
	catch (const LLU::LibraryLinkError &e) {
		err = e.which();
	}
	catch (...) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}

LIBRARY_LINK_FUNCTION(EmptyWrapper) {
	auto err = LLErrorCode::NoError;
	try {
		LLU::MArgumentManager mngr(libData, Argc, Args, Res);
		LLU::Image<std::uint8_t> im { nullptr }; // this should trigger an exception
	}
	catch (const LLU::LibraryLinkError &e) {
		err = e.which();
	}
	catch (...) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}
