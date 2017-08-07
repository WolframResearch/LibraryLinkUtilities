#include "WolframLibrary.h"
#include "MArgumentManager.h"
#include "Image.h"
#include "LibraryLinkError.hpp"

#include <type_traits>
#include <iostream>

using namespace LibraryLinkUtils;

EXTERN_C DLLEXPORT int EchoImage1(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr(libData, Argc, Args, Res);
		mngr.operateOnImage(0, [&mngr](auto&& in) {
			auto& out = in;
			mngr.setImage(out);
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

EXTERN_C DLLEXPORT int EchoImage2(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
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
			std::cout << std::boolalpha << in.is3D() << std::endl;
			std::cout << slices << std::endl;

			using T = typename std::remove_reference_t<decltype(in)>::value_type;
			Image<T> out(slices, columns, rows, channels, colorspace, interleaving);
			std::cout << std::boolalpha << out.is3D() << std::endl;

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
			std::cout << "Image copied" << std::endl;
			mngr.setImage(out);
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
