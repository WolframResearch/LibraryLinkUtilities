#include "WolframLibrary.h"

#include "MArgumentManager.h"
#include "Image.h"
#include "LibraryLinkError.h"
#include "LibraryLinkFunctionMacro.h"
#include <cstdint>

using namespace LibraryLinkUtils;

template<typename T>
T negator;

template<>
int8_t negator<int8_t> = 1;

template<>
uint8_t negator<uint8_t> = 0xFF;

template<>
uint16_t negator<uint16_t> = 0xFFFF;

struct ImageNegator {
	template<typename T>
	void operator()(Image<T> in, MArgumentManager& mngr) {
		Image<T> out(in.is3D()? in.slices() : 0, in.columns(), in.rows(), in.channels(), in.colorspace(), in.interleavedQ());

		std::transform(std::cbegin(in), std::cend(in), std::begin(out), [](T inElem) {
			return negator<T> & ~inElem;
		});
		mngr.setImage(out);
	}

	void operator()(Image<float> in, MArgumentManager& mngr) {
		throw std::runtime_error("Cannot negate Real32 image");
	}

	void operator()(Image<double> in, MArgumentManager& mngr) {
		throw std::runtime_error("Cannot negate Real image");
	}
};


LIBRARY_LINK_FUNCTION(ImageNegate) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr(libData, Argc, Args, Res);
		mngr.operateOnImage<ImageNegator>(0, mngr);
	}
	catch (LibraryLinkError& e) {
		err = e.which();
	}
	catch (std::exception& e) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}
