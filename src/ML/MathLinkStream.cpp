/** 
 * @file	MathLinkStream.cpp
 * @date	Nov 23, 2017
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @brief	Implementation file for MathLinkStream class.
 */
#include "MathLinkStream.h"

#include <cstring>
#include <string>

#include "../LibraryLinkError.h"

namespace LibraryLinkUtils {


	MathLinkStream::MathLinkStream(MLINK mlp) : m(mlp) {
	}

	MathLinkStream::MathLinkStream(MLINK mlp, int argc) : MathLinkStream(mlp, "List", argc) {
	}

	MathLinkStream::MathLinkStream(MLINK mlp, const std::string& head, int argc) : m(mlp) {
		testHead(head, argc);
	}

	MathLinkStream& MathLinkStream::operator<<(ML::Encoding e) {
		EncodingOut = e;
		return *this;
	}

	MathLinkStream& MathLinkStream::operator<<(StreamToken f) {
		return f(*this);
	}

	MathLinkStream& MathLinkStream::operator<<(BidirStreamToken f) {
		return f(*this, ML::Direction::Put);
	}

	MathLinkStream& MathLinkStream::operator<<(const ML::Symbol& s) {
		check(
			MLPutSymbol(m, s.getHead().c_str()),
			LLErrorName::MLPutSymbolError,
			"Cannot put symbol: \"" + s.getHead() + "\""
		);
		return *this;
	}

	MathLinkStream& MathLinkStream::operator<<(const ML::Function& f) {
		check(
			MLPutFunction(m, f.getHead().c_str(), f.getArgc()),
			LLErrorName::MLPutFunctionError,
			"Cannot put function: \"" + f.getHead() + "\" with " + std::to_string(f.getArgc()) + " arguments"
		);
		return *this;
	}

	MathLinkStream& MathLinkStream::operator<<(const ML::Missing& f) {
		check(
			MLPutFunction(m, f.getHead().c_str(), 1), //f.getArgc() could be 0 but we still want to send f.reason, even if it's an empty string
			LLErrorName::MLPutFunctionError,
			"Cannot put function: \"" + f.getHead() + "\" with 1 argument"
		);
		*this << ML::PutAsUTF8(f.why());
		return *this;
	}

	MathLinkStream& MathLinkStream::operator<<(bool b) {
		return *this << ML::Symbol(b? "True" : "False");
	}

	MathLinkStream& MathLinkStream::operator<<(const ML::PutAsUTF8& s) {
		ML::String<ML::Encoding::UTF8>::put(m, s.str, static_cast<int>(s.len));
		return *this;
	}

	MathLinkStream& MathLinkStream::operator<<(const char* s) {
		PutStringDispatch(s, static_cast<int>(std::strlen(s)));
		return *this;
	}

	void MathLinkStream::check(int statusOk, const std::string& errorName, const std::string& debugInfo) {
		ML::checkError(m, statusOk, errorName, debugInfo);
	}

	int MathLinkStream::testHead(const std::string& head) {
		int argcount;
		check(
			MLTestHead(m, head.c_str(), &argcount),
			LLErrorName::MLTestHeadError,
			"Expected \"" + head + "\""
		);
		return argcount;
	}

	void MathLinkStream::testHead(const std::string& head, int argc) {
		int argcount = testHead(head);
		if (argc != argcount) {
			ErrorManager::throwException(LLErrorName::MLTestHeadError, "Expected " + std::to_string(argc) + " arguments but got " + std::to_string(argcount));
		}
	}

	MathLinkStream& MathLinkStream::operator>>(ML::Encoding e) {
		EncodingIn = e;
		return *this;
	}

	MathLinkStream& MathLinkStream::operator>>(BidirStreamToken f) {
		return f(*this, ML::Direction::Get);
	}

	MathLinkStream& MathLinkStream::operator>>(const ML::Symbol& s) {
		check(
			MLTestSymbol(m, s.getHead().c_str()),
			LLErrorName::MLTestSymbolError,
			"Cannot get symbol: \"" + s.getHead() + "\""
		);
		return *this;
	}

	MathLinkStream& MathLinkStream::operator>>(ML::Symbol& s) {
		if (!s.getHead().empty()) {
			check(
				MLTestSymbol(m, s.getHead().c_str()),
				LLErrorName::MLTestSymbolError,
				"Cannot get symbol: \"" + s.getHead() + "\""
			);
		} else {
			const char* head;
			check(
				MLGetSymbol(m, &head),
				LLErrorName::MLGetSymbolError,
				"Cannot get symbol"
			);
			s.setHead(head);
			MLReleaseSymbol(m, head);
		}
		return *this;
	}

	MathLinkStream& MathLinkStream::operator>>(const ML::Function& f) {
		testHead(f.getHead().c_str(), f.getArgc());
		return *this;
	}

	MathLinkStream& MathLinkStream::operator>>(ML::Function& f) {
		if (!f.getHead().empty()) {
			if (f.getArgc() < 0) {
				f.setArgc(testHead(f.getHead().c_str()));
			} else {
				testHead(f.getHead().c_str(), f.getArgc());
			}
		} else {
			const char* head;
			int argc;
			check(
				MLGetFunction(m, &head, &argc),
				LLErrorName::MLGetFunctionError,
				"Cannot get function"
			);
			f.setHead(head);
			MLReleaseSymbol(m, head);
			f.setArgc(argc);
		}
		return *this;
	}

	MathLinkStream& MathLinkStream::operator>>(bool& b) {
		ML::Symbol boolean;
		*this >> boolean;
		if (boolean.getHead() == "True") {
			b = true;
		} else if (boolean.getHead() == "False") {
			b = false;
		} else {
			ErrorManager::throwException(LLErrorName::MLWrongSymbolForBool, "Expected \"True\" or \"False\", got " + boolean.getHead());
		}
		return *this;
	}

	MathLinkStream& MathLinkStream::operator>>(ML::GetAsUTF8 s) {
		auto stringData = ML::String<ML::Encoding::UTF8>::get(m);
		s.str = std::string { reinterpret_cast<const char*>(stringData.get()) };
		return *this;
	}

	template<>
	void MathLinkStream::PutStringDispatch(const unsigned short* strData, int strLen, ML::Encoding e) {
		switch(e) {
			case ML::Encoding::UCS2:
				return ML::String<ML::Encoding::UCS2>::put(m, strData, strLen);
			case ML::Encoding::UTF16:
				//[[fallthrough]]; //uncomment when we start using C++17
			default:
				return ML::String<ML::Encoding::UTF16>::put(m, strData, strLen);
		}
	}

	template<>
	void MathLinkStream::PutStringDispatch(const unsigned int* strData, int strLen, ML::Encoding) {
		ML::String<ML::Encoding::UTF32>::put(m, strData, strLen);
	}


	template<>
	std::basic_string<unsigned short> MathLinkStream::GetStringDispatch(ML::Encoding e) {
		switch(e) {
			case ML::Encoding::UCS2:
				return ML::String<ML::Encoding::UCS2>::getString<unsigned short>(m);
			case ML::Encoding::UTF16:
				//[[fallthrough]]; //uncomment when we start using C++17
			default:
				return ML::String<ML::Encoding::UTF16>::getString<unsigned short>(m);
		}
	}

	template<>
	std::basic_string<unsigned int> MathLinkStream::GetStringDispatch(ML::Encoding) {
		return ML::String<ML::Encoding::UTF32>::getString<unsigned int>(m);
	}

} /* namespace LibraryLinkUtils */
