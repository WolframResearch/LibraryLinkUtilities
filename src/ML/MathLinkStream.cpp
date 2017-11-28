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


	template<typename T>
	MathLinkStream::MLPutListFunc<T> MathLinkStream::MLPut<T>::List = [] (auto&&...) {
		static_assert(sizeof(T) < 0, "Trying to use MLPut<T>::List for unsupported type T");
		return 0;
	};

	template<typename T>
	MathLinkStream::MLPutListFunc<T> MathLinkStream::MLPut<T>::String = [] (auto&&...) {
		static_assert(sizeof(T) < 0, "Trying to use MLPut<T>::String for unsupported type T");
		return 0;
	};

	template<typename T>
	MathLinkStream::MLPutScalarFunc<T> MathLinkStream::MLPut<T>::Scalar = [] (auto&&...) {
		static_assert(sizeof(T) < 0, "Trying to use MLPut<T>::Scalar for unsupported type T");
		return 0;
	};


	template<>
	MathLinkStream::MLPutListFunc<char> MathLinkStream::MLPut<char>::String = [](MLINK m, const char* d, int l) {
		return MLPutString(m, d);
	};


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




	template<typename T>
	MathLinkStream::MLGetListFunc<T> MathLinkStream::MLGet<T>::List  = [] (auto&&...) {
		static_assert(sizeof(T) < 0, "Trying to use MLGet<T>::List for unsupported type T");
		return 0;
	};

	template<typename T>
	MathLinkStream::MLGetStringFunc<T> MathLinkStream::MLGet<T>::String = [] (auto&&...) {
		static_assert(sizeof(T) < 0, "Trying to use MLGet<T>::String for unsupported type T");
		return 0;
	};

	template<typename T>
	MathLinkStream::MLGetScalarFunc<T> MathLinkStream::MLGet<T>::Scalar = [] (auto&&...) {
		static_assert(sizeof(T) < 0, "Trying to use MLGet<T>::Scalar for unsupported type T");
		return 0;
	};


	template<>
	MathLinkStream::MLGetStringFunc<char> MathLinkStream::MLGet<char>::String = [](MLINK m, const char** d, int* l, int* c) {
		*l = *c = -1;
		return MLGetString(m, d);
	};


	template<>
	MathLinkStream::MLGetListFunc<unsigned char> MathLinkStream::MLGet<unsigned char>::List = MLGetInteger8List;

	template<>
	MathLinkStream::MLGetStringFunc<unsigned char> MathLinkStream::MLGet<unsigned char>::String = MLGetUTF8String;

	template<>
	MathLinkStream::MLGetScalarFunc<unsigned char> MathLinkStream::MLGet<unsigned char>::Scalar = MLGetInteger8;

	template<>
	MathLinkStream::MLGetListFunc<short> MathLinkStream::MLGet<short>::List = MLGetInteger16List;

	template<>
	MathLinkStream::MLGetStringFunc<unsigned short> MathLinkStream::MLGet<unsigned short>::String = MLGetUTF16String;

	template<>
	MathLinkStream::MLGetScalarFunc<short> MathLinkStream::MLGet<short>::Scalar = MLGetInteger16;

	template<>
	MathLinkStream::MLGetListFunc<int> MathLinkStream::MLGet<int>::List = MLGetInteger32List;

	template<>
	MathLinkStream::MLGetStringFunc<unsigned int> MathLinkStream::MLGet<unsigned int>::String = [](MLINK m, const unsigned int** d, int* l, int* c) {
		*c = -1;
		return MLGetUTF32String(m, d, l);
	};

	template<>
	MathLinkStream::MLGetScalarFunc<int> MathLinkStream::MLGet<int>::Scalar = MLGetInteger32;

	template<>
	MathLinkStream::MLGetListFunc<mlint64> MathLinkStream::MLGet<mlint64>::List = MLGetInteger64List;

	template<>
	MathLinkStream::MLGetScalarFunc<mlint64> MathLinkStream::MLGet<mlint64>::Scalar = MLGetInteger64;

	template<>
	MathLinkStream::MLGetListFunc<float> MathLinkStream::MLGet<float>::List = MLGetReal32List;

	template<>
	MathLinkStream::MLGetScalarFunc<float> MathLinkStream::MLGet<float>::Scalar = MLGetReal32;

	template<>
	MathLinkStream::MLGetListFunc<double> MathLinkStream::MLGet<double>::List = MLGetReal64List;

	template<>
	MathLinkStream::MLGetScalarFunc<double> MathLinkStream::MLGet<double>::Scalar = MLGetReal64;



	template<typename T>
	MathLinkStream::MLReleaseListFunc<T> MathLinkStream::MLRelease<T>::List  = [] (auto&&...) {
		static_assert(sizeof(T) < 0, "Trying to use MLRelease<T>::List for unsupported type T");
	};

	template<typename T>
	MathLinkStream::MLReleaseStringFunc<T> MathLinkStream::MLRelease<T>::String = [] (auto&&...) {
		static_assert(sizeof(T) < 0, "Trying to use MLRelease<T>::String for unsupported type T");
	};


	template<>
	MathLinkStream::MLReleaseStringFunc<char> MathLinkStream::MLRelease<char>::String = [](MLINK m, const char* d, int) {
		MLReleaseString(m, d);
	};


	template<>
	MathLinkStream::MLReleaseListFunc<unsigned char> MathLinkStream::MLRelease<unsigned char>::List = MLReleaseInteger8List;

	template<>
	MathLinkStream::MLReleaseStringFunc<unsigned char> MathLinkStream::MLRelease<unsigned char>::String = MLReleaseUTF8String;

	template<>
	MathLinkStream::MLReleaseListFunc<short> MathLinkStream::MLRelease<short>::List = MLReleaseInteger16List;

	template<>
	MathLinkStream::MLReleaseStringFunc<unsigned short> MathLinkStream::MLRelease<unsigned short>::String = MLReleaseUTF16String;

	template<>
	MathLinkStream::MLReleaseListFunc<int> MathLinkStream::MLRelease<int>::List = MLReleaseInteger32List;

	template<>
	MathLinkStream::MLReleaseStringFunc<unsigned int> MathLinkStream::MLRelease<unsigned int>::String = MLReleaseUTF32String;

	template<>
	MathLinkStream::MLReleaseListFunc<mlint64> MathLinkStream::MLRelease<mlint64>::List = MLReleaseInteger64List;

	template<>
	MathLinkStream::MLReleaseListFunc<float> MathLinkStream::MLRelease<float>::List = MLReleaseReal32List;

	template<>
	MathLinkStream::MLReleaseListFunc<double> MathLinkStream::MLRelease<double>::List = MLReleaseReal64List;




	MathLinkStream::MathLinkStream(MLINK mlp) {
		links.push(mlp);
	}

	MathLinkStream::MathLinkStream(MLINK mlp, const std::string& head, int argc) {
		links.push(mlp);
		testHead(head, argc);
	}


	MathLinkStream& MathLinkStream::operator<<(StreamToken f) {
		return f(*this);
	}

	MathLinkStream& MathLinkStream::operator<<(BidirStreamToken f) {
		return f(*this, ML::Direction::Put);
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

	MathLinkStream& MathLinkStream::operator<<(bool b) {
		return *this << ML::Symbol(b? "True" : "False");
	}

	std::string getMLErrorText(MLINK mlp) {
		std::string err = "Error code reported by MathLink: " + std::to_string(MLError(mlp)) + "\n";
		auto mlErrorMsg = MLErrorMessage(mlp);
		err += "\"" + std::string(mlErrorMsg) + "\"\nAdditional debug info: ";
		MLReleaseErrorMessage(mlp, mlErrorMsg);
		MLClearError(mlp);
		return err;
	}

	void MathLinkStream::checkMLError(int statusOk, int errorCode, const std::string& debugInfo) {
		if (!statusOk) {
			ErrorManager::throwException(errorCode, getMLErrorText(get()) + debugInfo);
		}
	}

	void MathLinkStream::checkMLError(int statusOk, const std::string& errorName, const std::string& debugInfo) {
		// TODO this overload may be unnecessary
		if (!statusOk) {
			ErrorManager::throwException(errorName, getMLErrorText(get()) + debugInfo);
		}
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

	MathLinkStream& MathLinkStream::operator>>(BidirStreamToken f) {
		return f(*this, ML::Direction::Get);
	}

	MathLinkStream& MathLinkStream::operator>>(const ML::Symbol& s) {
		checkMLError(MLTestSymbol(get(), s.getHead().c_str()), LLErrorCode::MLTestSymbolError, "Cannot get symbol: \"" + s.getHead() + "\"");
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
