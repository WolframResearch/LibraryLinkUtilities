/** 
 * @file	MathLinkStream.cpp
 * @date	Nov 23, 2017
 * @author	rafal
 * @brief	<brief description>
 */
#include "MathLinkStream.h"

#include <string>

#include "LibraryLinkError.h"

namespace LibraryLinkUtils {

	template<typename T>
	int MLPutListError(MLINK, const T*, int) {
		static_assert(sizeof(T) < 0, "sdsdsd");
		//TODO
		return 0;
	}

	template<typename T>
	int MLPutScalarError(MLINK, T) {
		static_assert(sizeof(T) < 0, "sdsdsd");
		//TODO
		return 0;
	}

	int MLPutStringWithLength(MLINK m, const char* s, int len) {
		return MLPutString(m, s);
	}

	template<typename T>
	MathLinkStream::MLPutListFunc<T> MathLinkStream::MLPut<T>::List = MLPutListError<T>;

	template<typename T>
	MathLinkStream::MLPutListFunc<T> MathLinkStream::MLPut<T>::String = MLPutListError<T>;

	template<typename T>
	MathLinkStream::MLPutScalarFunc<T> MathLinkStream::MLPut<T>::Scalar = MLPutScalarError<T>;


	template<>
	MathLinkStream::MLPutListFunc<char> MathLinkStream::MLPut<char>::String = MLPutStringWithLength;


	template<>
	MathLinkStream::MLPutListFunc<unsigned char> MathLinkStream::MLPut<unsigned char>::List = MLPutInteger8List;

	template<>
	MathLinkStream::MLPutListFunc<unsigned char> MathLinkStream::MLPut<unsigned char>::String = MLPutUTF8String;

	template<>
	MathLinkStream::MLPutScalarFunc<unsigned char> MathLinkStream::MLPut<unsigned char>::Scalar = MLPutInteger8;


	template<>
	MathLinkStream::MLPutListFunc<short> MathLinkStream::MLPut<short>::List = MLPutInteger16List;

	template<>
	MathLinkStream::MLPutListFunc<unsigned short> MathLinkStream::MLPut<unsigned short>::String = MLPutUTF16String;

	template<>
	MathLinkStream::MLPutScalarFunc<short> MathLinkStream::MLPut<short>::Scalar = MLPutInteger16;


	template<>
	MathLinkStream::MLPutListFunc<int> MathLinkStream::MLPut<int>::List = MLPutInteger32List;

	template<>
	MathLinkStream::MLPutListFunc<unsigned int> MathLinkStream::MLPut<unsigned int>::String = MLPutUTF32String;

	template<>
	MathLinkStream::MLPutScalarFunc<int> MathLinkStream::MLPut<int>::Scalar = MLPutInteger32;


	template<>
	MathLinkStream::MLPutListFunc<mlint64> MathLinkStream::MLPut<mlint64>::List = MLPutInteger64List;

	template<>
	MathLinkStream::MLPutScalarFunc<mlint64> MathLinkStream::MLPut<mlint64>::Scalar = MLPutInteger64;


	template<>
	MathLinkStream::MLPutListFunc<float> MathLinkStream::MLPut<float>::List = MLPutReal32List;

	template<>
	MathLinkStream::MLPutScalarFunc<float> MathLinkStream::MLPut<float>::Scalar = MLPutReal32;


	template<>
	MathLinkStream::MLPutListFunc<double> MathLinkStream::MLPut<double>::List = MLPutReal64List;

	template<>
	MathLinkStream::MLPutScalarFunc<double> MathLinkStream::MLPut<double>::Scalar = MLPutReal64;




	MathLinkStream::MathLinkStream(MLINK mlp) {
		links.push(mlp);
	}

	MathLinkStream::MathLinkStream(MLINK mlp, const std::string& head, int argc) {
		links.push(mlp);
		testHead(head, argc);
	}


	int MathLinkStream::testHead(const std::string& head) {
		int argcount;
		checkMLError(MLTestHead(get(), head.c_str(), &argcount), LLErrorCode::MLTestHeadError, "Expected \"" + head + "\"");
		return argcount;
	}

	void MathLinkStream::testHead(const std::string& head, int argc) {
		int argcount = testHead(head);
		if (argc != argcount) {
			ErrorManager::throwException(LLErrorCode::MLTestHeadError, "Expected " + std::to_string(argc) + " arguments but got " + std::to_string(argcount));
		}
	}

	MathLinkStream& MathLinkStream::operator<<(std::function<MathLinkStream& (MathLinkStream&)> f) {
		return f(*this);
	}


	MathLinkStream& MathLinkStream::operator<<(const ML::Symbol& s) {
		checkMLError(MLPutSymbol(get(), s.getHead().c_str()), LLErrorCode::MLPutSymbolError, "Cannot put symbol: \"" + s.getHead() + "\"");
		return *this;
	}

	MathLinkStream& MathLinkStream::operator<<(const ML::Function& f) {
		checkMLError(
			MLPutFunction(get(), f.getHead().c_str(), f.getArgc()),
			LLErrorCode::MLPutFunctionError,
			"Cannot put function: \"" + f.getHead() + "\" with " + std::to_string(f.getArgc()) + " arguments"
		);
		return *this;
	}

	void MathLinkStream::checkMLError(int statusOk, int errorCode, const std::string& debugInfo) {
		if (!statusOk) {
			//TODO make use of MLError() and MLErrorMessage() here
			ErrorManager::throwException(errorCode, debugInfo);
		}
	}

	void MathLinkStream::checkMLError(int statusOk, const std::string& errorName, const std::string& debugInfo) {
		// TODO this overload may be unnecessary
		if (!statusOk) {
			ErrorManager::throwException(errorName, debugInfo);
		}
	}

	namespace ML {

		const std::string& Symbol::getHead() const {
			return head;
		}

		int Function::getArgc() const {
			return argc;
		}

		MathLinkStream& NewPacket(MathLinkStream& ms) {
			MLNewPacket(ms.get()); //TODO error checking
			return ms;
		}

		MathLinkStream& EndPacket(MathLinkStream& ms) {
			MLEndPacket(ms.get()); //TODO error checking
			return ms;
		}

		MathLinkStream& Flush(MathLinkStream& ms) {
			MLFlush(ms.get()); //TODO error checking
			return ms;
		}

		MathLinkStream& Rule(MathLinkStream& ms) {
			return ms << Function("Rule", 2);
		}
	}


} /* namespace LibraryLinkUtils */
