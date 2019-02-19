#include "WolframLibrary.h"

#include "LLU/MArgumentManager.h"
#include "LLU/Containers/Image.h"
#include "LLU/LibraryLinkError.h"
#include "LLU/LibraryLinkFunctionMacro.h"
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
	template<typename T, class P>
	void operator()(Image<T, P> in, MArgumentManager& mngr) {
		Image<T> out(in.is3D()? in.slices() : 0, in.columns(), in.rows(), in.channels(), in.colorspace(), in.interleavedQ());

		std::transform(std::cbegin(in), std::cend(in), std::begin(out), [](T inElem) {
			return negator<T> & ~inElem;
		});
		mngr.setImage(out);
	}

	template<class P>
	void operator()(Image<float, P> in, MArgumentManager& mngr) {
		throw std::runtime_error("Cannot negate Real32 image");
	}

	template<class P>
	void operator()(Image<double, P> in, MArgumentManager& mngr) {
		throw std::runtime_error("Cannot negate Real image");
	}
};


LIBRARY_LINK_FUNCTION(ImageNegate) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr(libData, Argc, Args, Res);
		mngr.operateOnImage<ImageNegator>(0, mngr);
	}
	catch (const LibraryLinkError& e) {
		err = e.which();
	}
	catch (...) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}
