/**
 * @file	WSStringEncodings.cpp
 * @date	Mar 30, 2018
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @brief	Test suite for string related functionality of WSStream
 */

#include <iostream>

#include "wstp.h"

#include "LLU/ErrorLog/ErrorManager.h"
#include "LLU/LibraryLinkFunctionMacro.h"
#include "LLU/WSTP/WSStream.hpp"

using namespace LLU;

template<class Operation, typename... Args>
void forAllEncodingsDo(Args&&... params) {
	Operation::template run<WS::Encoding::Native>(std::forward<Args>(params)...);
	Operation::template run<WS::Encoding::Byte>(std::forward<Args>(params)...);
	Operation::template run<WS::Encoding::UTF8>(std::forward<Args>(params)...);
	Operation::template run<WS::Encoding::UTF16>(std::forward<Args>(params)...);
	Operation::template run<WS::Encoding::UCS2>(std::forward<Args>(params)...);
	Operation::template run<WS::Encoding::UTF32>(std::forward<Args>(params)...);
}

LIBRARY_WSTP_FUNCTION(NestedPutAs) {
	auto err = ErrorCode::NoError;
	try {
		WSStream<WS::Encoding::Byte, WS::Encoding::UTF32> ml(mlp, 1);

		std::string s;

		ml >> WS::getAs<WS::Encoding::UTF8>(s);

		ml << WS::putAs<WS::Encoding::UTF16>(WS::putAs<WS::Encoding::UTF8>(s));	   // the most nested encoding should be the one used
	} catch (LibraryLinkError& e) {
		err = e.which();
	} catch (...) {
		err = ErrorCode::FunctionError;
	}
	return err;
}

template<typename T>
std::vector<wsint64> toIntegerCodes(const T* p, std::size_t n) {
	std::vector<wsint64> ret {p, p + n};
	return ret;
}

struct StringToCharCodes {
	template<WS::Encoding E, typename WSTPStream>
	static void run(WSTPStream ml, WSMARK mark) {
		std::basic_string<WS::CharType<E>> s;
		if (WSSeekToMark(ml.get(), mark, 0)) {
			ml >> WS::getAs<E>(s);
			ml << WS::Rule << WS::getEncodingName(E) << toIntegerCodes(s.c_str(), s.length());
		}
	}
};

LIBRARY_WSTP_FUNCTION(CharacterCodes) {
	auto err = ErrorCode::NoError;
	try {
		WSStream<WS::Encoding::Byte> ml(mlp, 1);
		auto* mark = WSCreateMark(mlp);
		ml << WS::Association(6);	 // there are 6 encodings available
		forAllEncodingsDo<StringToCharCodes>(ml, mark);

	} catch (LibraryLinkError& e) {
		err = e.which();
	} catch (...) {
		err = ErrorCode::FunctionError;
	}
	return err;
}

struct EncodingRoundtrip {
	template<WS::Encoding E, typename WSTPStream>
	static void run(WSTPStream ml, WSMARK mark) {
		std::basic_string<WS::CharType<E>> s;
		if (WSSeekToMark(ml.get(), mark, 0)) {
			ml >> WS::getAs<E>(s);
			ml << WS::Rule << WS::getEncodingName(E) << WS::putAs<E>(s);
		}
	}
};

LIBRARY_WSTP_FUNCTION(AllEncodingsRoundtrip) {
	auto err = ErrorCode::NoError;
	try {
		WSStream<WS::Encoding::Byte> ml(mlp, 1);
		auto* mark = WSCreateMark(mlp);
		ml << WS::Association(6);	 // there are 6 encodings available
		forAllEncodingsDo<EncodingRoundtrip>(ml, mark);
	} catch (LibraryLinkError& e) {
		err = e.which();
	} catch (...) {
		err = ErrorCode::FunctionError;
	}
	return err;
}
