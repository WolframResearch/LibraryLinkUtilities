/** 
 * @file	MathLinkStream.cpp
 * @date	Nov 23, 2017
 * @author	rafal
 * @brief	<brief description>
 */
#include "MathLinkStream.h"

#include <string>

#include "../LibraryLinkError.h"

namespace LibraryLinkUtils {


	MathLinkStream::MathLinkStream(MLINK mlp) : m(mlp) {
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
		check(MLPutSymbol(m, s.getHead().c_str()), LLErrorCode::MLPutSymbolError, "Cannot put symbol: \"" + s.getHead() + "\"");
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

	void MathLinkStream::check(int statusOk, int errorCode, const std::string& debugInfo) {
		ML::checkError(m, statusOk, errorCode, debugInfo);
	}

	void MathLinkStream::check(int statusOk, const std::string& errorName, const std::string& debugInfo) {
		// TODO this overload may be unnecessary
		ML::checkError(m, statusOk, errorName, debugInfo);
	}

	int MathLinkStream::testHead(const std::string& head) {
		int argcount;
		check(MLTestHead(m, head.c_str(), &argcount), LLErrorCode::MLTestHeadError, "Expected \"" + head + "\"");
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
		check(MLTestSymbol(m, s.getHead().c_str()), LLErrorCode::MLTestSymbolError, "Cannot get symbol: \"" + s.getHead() + "\"");
		return *this;
	}

	MathLinkStream& MathLinkStream::operator>>(const ML::Function& f) {
		testHead(f.getHead().c_str(), f.getArgc());
		return *this;
	}

	MathLinkStream& MathLinkStream::operator>>(ML::Function& f) {
		f.setArgc(testHead(f.getHead().c_str()));
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
