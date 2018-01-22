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


	MathLinkStream& MathLinkStream::operator<<(StreamToken f) {
		return f(*this);
	}

	MathLinkStream& MathLinkStream::operator<<(BidirStreamToken f) {
		return f(*this, ML::Direction::Put);
	}

	MathLinkStream& MathLinkStream::operator<<(const ML::Symbol& s) {
		check(
			MLPutSymbol(m, s.getHead().c_str()),
			LLErrorCode::MLPutSymbolError,
			"Cannot put symbol: \"" + s.getHead() + "\""
		);
		return *this;
	}

	MathLinkStream& MathLinkStream::operator<<(const ML::Function& f) {
		check(
			MLPutFunction(m, f.getHead().c_str(), f.getArgc()),
			LLErrorCode::MLPutFunctionError,
			"Cannot put function: \"" + f.getHead() + "\" with " + std::to_string(f.getArgc()) + " arguments"
		);
		return *this;
	}

	MathLinkStream& MathLinkStream::operator<<(bool b) {
		return *this << ML::Symbol(b? "True" : "False");
	}


	MathLinkStream& MathLinkStream::operator<<(const char* s) {
		ML::PutString<char>::put(m, s, std::strlen(s));
		return *this;
	}

	void MathLinkStream::check(int statusOk, int errorCode, const std::string& debugInfo) {
		ML::checkError(m, statusOk, errorCode, debugInfo);
	}

	void MathLinkStream::check(int statusOk, const std::string& errorName, const std::string& debugInfo) {
		// TODO this overload may be unnecessary
		ML::checkError(m, statusOk, errorName, debugInfo);
	}

	int MathLinkStream::testHead(const std::string& head) {
		int argcount;
		check(
			MLTestHead(m, head.c_str(), &argcount),
			LLErrorCode::MLTestHeadError,
			"Expected \"" + head + "\""
		);
		return argcount;
	}

	void MathLinkStream::testHead(const std::string& head, int argc) {
		int argcount = testHead(head);
		if (argc != argcount) {
			ErrorManager::throwException(LLErrorCode::MLTestHeadError, "Expected " + std::to_string(argc) + " arguments but got " + std::to_string(argcount));
		}
	}

	MathLinkStream& MathLinkStream::operator>>(BidirStreamToken f) {
		return f(*this, ML::Direction::Get);
	}

	MathLinkStream& MathLinkStream::operator>>(const ML::Symbol& s) {
		check(
			MLTestSymbol(m, s.getHead().c_str()),
			LLErrorCode::MLTestSymbolError,
			"Cannot get symbol: \"" + s.getHead() + "\""
		);
		return *this;
	}

	MathLinkStream& MathLinkStream::operator>>(ML::Symbol& s) {
		if (!s.getHead().empty()) {
			check(
				MLTestSymbol(m, s.getHead().c_str()),
				LLErrorCode::MLTestSymbolError,
				"Cannot get symbol: \"" + s.getHead() + "\""
			);
		} else {
			const char* head;
			check(
				MLGetSymbol(m, &head),
				LLErrorCode::MLGetSymbolError,
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
				LLErrorCode::MLGetFunctionError,
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
			ErrorManager::throwException(LLErrorCode::MLWrongSymbolForBool, "Expected \"True\" or \"False\", got " + boolean.getHead());
		}
		return *this;
	}


} /* namespace LibraryLinkUtils */
