#include <cstdint>

#include "LLU/LLU.h"
#include "LLU/LibraryLinkFunctionMacro.h"

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
	void operator()(LLU::Image<T, P> in, LLU::MArgumentManager& mngr) {
		LLU::Image<T> out(in.is3D() ? in.slices() : 0, in.columns(), in.rows(), in.channels(), in.colorspace(), in.interleavedQ());

		std::transform(std::cbegin(in), std::cend(in), std::begin(out), [](T inElem) { return negator<T> & ~inElem; });
		mngr.setImage(out);
	}

	template<class P>
	void operator()(LLU::Image<float, P>, LLU::MArgumentManager&) {
		throw std::runtime_error("Cannot negate Real32 image");
	}

	template<class P>
	void operator()(LLU::Image<double, P>, LLU::MArgumentManager&) {
		throw std::runtime_error("Cannot negate Real image");
	}
};

LIBRARY_LINK_FUNCTION(ImageNegate) {
	auto err = LLU::ErrorCode::NoError;
	try {
		LLU::MArgumentManager mngr(libData, Argc, Args, Res);
		mngr.operateOnImage<LLU::Passing::Automatic, ImageNegator>(0, mngr);
	} catch (const LLU::LibraryLinkError& e) {
		err = e.which();
	} catch (...) {
		err = LLU::ErrorCode::FunctionError;
	}
	return err;
}
