/** 
 * @file	MLStringEncodings.cpp
 * @date	Mar 30, 2018
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @brief	Test suite for string related functionality of MLStream
 */

#include <iostream>

#include "mathlink.h"

#include "LLU/Error/ErrorManager.h"
#include "LLU/LibraryLinkFunctionMacro.h"
#include "LLU/ML/MLStream.hpp"

using namespace LibraryLinkUtils;

template<class Operation, typename... Args>
void forAllEncodingsDo(Args&&... params) {
	Operation::template run<ML::Encoding::Native>(std::forward<Args>(params)...);
	Operation::template run<ML::Encoding::Byte>(std::forward<Args>(params)...);
	Operation::template run<ML::Encoding::UTF8>(std::forward<Args>(params)...);
	Operation::template run<ML::Encoding::UTF16>(std::forward<Args>(params)...);
	Operation::template run<ML::Encoding::UCS2>(std::forward<Args>(params)...);
	Operation::template run<ML::Encoding::UTF32>(std::forward<Args>(params)...);
}

LIBRARY_MATHLINK_FUNCTION(NestedPutAs) {
	auto err = LLErrorCode::NoError;
	try {
		MLStream<ML::Encoding::Byte, ML::Encoding::UTF32> ml(mlp, 1);

		std::string s;

		ml >> ML::getAs<ML::Encoding::UTF8>(s);

		ml << ML::putAs<ML::Encoding::UTF16>(ML::putAs<ML::Encoding::UTF8>(s)); // the most nested encoding should be the one used
	}
	catch (LibraryLinkError& e) {
		err = e.which();
	}
	catch (...) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}

template<typename T>
std::vector<mlint64> toIntegerCodes(const T* p, std::size_t n) {
	std::vector<mlint64> ret {p, p + n};
	return ret;
}

struct StringToCharCodes {
	template<ML::Encoding E, typename MathLinkStream>
	static void run(MathLinkStream ml, MLINKMark mark) {
		std::basic_string<ML::CharType<E>> s;
		if (MLSeekToMark(ml.get(), mark, 0)) {
			ml >> ML::getAs<E>(s);
			ml << ML::Rule << ML::getEncodingName(E) << toIntegerCodes(s.c_str(), s.length());
		}
	}
};

LIBRARY_MATHLINK_FUNCTION(CharacterCodes) {
	auto err = LLErrorCode::NoError;
	try {
		MLStream<ML::Encoding::Byte> ml(mlp, 1);
		auto mark = MLCreateMark(mlp);
		ml << ML::Association(6); // there are 6 encodings available
		forAllEncodingsDo<StringToCharCodes>(ml, mark);

	}
	catch (LibraryLinkError& e) {
		err = e.which();
	}
	catch (...) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}

struct EncodingRoundtrip {
	template<ML::Encoding E, typename MathLinkStream>
	static void run(MathLinkStream ml, MLINKMark mark) {
		std::basic_string<ML::CharType<E>> s;
		if (MLSeekToMark(ml.get(), mark, 0)) {
			ml >> ML::getAs<E>(s);
			ml << ML::Rule << ML::getEncodingName(E) << ML::putAs<E>(s);
		}
	}
};

LIBRARY_MATHLINK_FUNCTION(AllEncodingsRoundtrip) {
	auto err = LLErrorCode::NoError;
	try {
		MLStream<ML::Encoding::Byte> ml(mlp, 1);
		auto mark = MLCreateMark(mlp);
		ml << ML::Association(6); // there are 6 encodings available
		forAllEncodingsDo<EncodingRoundtrip>(ml, mark);
	}
	catch (LibraryLinkError& e) {
		err = e.which();
	}
	catch (...) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}

