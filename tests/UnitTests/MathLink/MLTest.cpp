/** 
 * @file	MLTest.cpp
 * @date	Nov 23, 2017
 * @author	rafalc
 * @brief	<brief description>
 */

#include <iostream>
#include <cstring>
#include <string>
#include <vector>

#include "mathlink.h"
#include "WolframLibrary.h"

#include "LLU.h"
#include "ML/MathLinkStream.h"

using LibraryLinkUtils::MathLinkStream;
namespace ML = LibraryLinkUtils::ML;

template<typename T>
void readAndWriteScalar(MathLinkStream& m) {
	T x;
	m >> x;
	m << x;
}

template<typename T>
void readAndWriteScalarMax(MathLinkStream& m) {
	T x;
	m >> x;
	m << std::numeric_limits<T>::max();
}

//
// Scalars
//
EXTERN_C DLLEXPORT int SameInts(WolframLibraryData libData, MLINK mlp) {
	auto err = LLErrorCode::NoError;
	try {
		MathLinkStream m(mlp, "List", 4);
		m << ML::List(4);
		readAndWriteScalar<unsigned char>(m);
		readAndWriteScalar<short>(m);
		readAndWriteScalar<int>(m);
		readAndWriteScalar<mlint64>(m);
	}
	catch (LibraryLinkError& e) {
		err = e.which();
	}
	catch (...) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}

EXTERN_C DLLEXPORT int MaxInts(WolframLibraryData libData, MLINK mlp) {
	auto err = LLErrorCode::NoError;
	try {
		MathLinkStream m(mlp, "List", 4);
		m << ML::List(4);
		readAndWriteScalarMax<unsigned char>(m);
		readAndWriteScalarMax<short>(m);
		readAndWriteScalarMax<int>(m);
		readAndWriteScalarMax<mlint64>(m);
	}
	catch (LibraryLinkError& e) {
		err = e.which();
	}
	catch (std::exception& e) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}

EXTERN_C DLLEXPORT int SameFloats(WolframLibraryData libData, MLINK mlp) {
	auto err = LLErrorCode::NoError;
	try {
		MathLinkStream m(mlp, "List", 2);
		m << ML::List(2);
		readAndWriteScalar<float>(m);
		readAndWriteScalar<double>(m);
	}
	catch (LibraryLinkError& e) {
		err = e.which();
	}
	catch (std::exception& e) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}

EXTERN_C DLLEXPORT int BoolAnd(WolframLibraryData libData, MLINK mlp) {
	auto err = LLErrorCode::NoError;
	try {
		MathLinkStream ml(mlp);
		
		ML::List l;
		ml >> l;

		// Read all bools in a loop and calculate conjunction. No short-circuit
		bool res = true;
		for (auto i = 0; i < l.getArgc(); ++i) {
			bool b;
			ml >> b;
			res &= b;
		}

		ml << ML::NewPacket << res << ML::EndPacket;
	}
	catch (LibraryLinkError& e) {
		err = e.which();
	}
	catch (std::exception& e) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}



//
// Lists
//


template<class T>
void getAndPut(MathLinkStream& m) {
	T t;
	m >> t;
	m << t;
}


template<typename T>
void reverseList(MathLinkStream& m) {
	ML::ListData<T> l;
	m >> l;

	auto listLen = l.get_deleter().getLength();
	std::vector<T> result(listLen);
	std::reverse_copy(l.get(), l.get() + listLen, std::begin(result));
	m << result;
}

EXTERN_C DLLEXPORT int GetReversed8(WolframLibraryData libData, MLINK mlp) {
	auto err = LLErrorCode::NoError;
	try {
		MathLinkStream ml(mlp, "List", 1);
		reverseList<unsigned char>(ml);
	}
	catch (LibraryLinkError& e) {
		err = e.which();
	}
	catch (...) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}

EXTERN_C DLLEXPORT int GetReversed16(WolframLibraryData libData, MLINK mlp) {
	auto err = LLErrorCode::NoError;
	try {
		MathLinkStream ml(mlp, "List", 1);
		reverseList<short>(ml);
	}
	catch (LibraryLinkError& e) {
		err = e.which();
	}
	catch (...) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}

EXTERN_C DLLEXPORT int GetReversed32(WolframLibraryData libData, MLINK mlp) {
	auto err = LLErrorCode::NoError;
	try {
		MathLinkStream ml(mlp, "List", 1);
		reverseList<int>(ml);
	}
	catch (LibraryLinkError& e) {
		err = e.which();
	}
	catch (...) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}

EXTERN_C DLLEXPORT int GetReversed64(WolframLibraryData libData, MLINK mlp) {
	auto err = LLErrorCode::NoError;
	try {
		MathLinkStream ml(mlp, "List", 1);
		reverseList<mlint64>(ml);
	}
	catch (LibraryLinkError& e) {
		err = e.which();
	}
	catch (...) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}

EXTERN_C DLLEXPORT int GetReversedDouble(WolframLibraryData libData, MLINK mlp) {
	auto err = LLErrorCode::NoError;
	try {
		MathLinkStream ml(mlp, "List", 1);
		reverseList<double>(ml);
	}
	catch (LibraryLinkError& e) {
		err = e.which();
	}
	catch (...) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}

EXTERN_C DLLEXPORT int GetFloatList(WolframLibraryData libData, MLINK mlp) {
	auto err = LLErrorCode::NoError;
	try {
		MathLinkStream ml(mlp, "List", 1);
		getAndPut<ML::ListData<float>>(ml);
	}
	catch (LibraryLinkError& e) {
		err = e.which();
	}
	catch (...) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}


//
// Arrays
//

EXTERN_C DLLEXPORT int GetSame8(WolframLibraryData libData, MLINK mlp) {
	auto err = LLErrorCode::NoError;
	try {
		MathLinkStream ml(mlp, "List", 1);
		getAndPut<ML::ArrayData<unsigned char>>(ml);
	}
	catch (LibraryLinkError& e) {
		err = e.which();
	}
	catch (...) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}

EXTERN_C DLLEXPORT int GetSame16(WolframLibraryData libData, MLINK mlp) {
	auto err = LLErrorCode::NoError;
	try {
		MathLinkStream ml(mlp, "List", 1);
		getAndPut<ML::ArrayData<short>>(ml);
	}
	catch (LibraryLinkError& e) {
		err = e.which();
	}
	catch (...) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}

EXTERN_C DLLEXPORT int GetSame32(WolframLibraryData libData, MLINK mlp) {
	auto err = LLErrorCode::NoError;
	try {
		MathLinkStream ml(mlp, "List", 1);
		getAndPut<ML::ArrayData<int>>(ml);
	}
	catch (LibraryLinkError& e) {
		err = e.which();
	}
	catch (...) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}

EXTERN_C DLLEXPORT int GetSame64(WolframLibraryData libData, MLINK mlp) {
	auto err = LLErrorCode::NoError;
	try {
		MathLinkStream ml(mlp, "List", 1);
		getAndPut<ML::ArrayData<mlint64>>(ml);
	}
	catch (LibraryLinkError& e) {
		err = e.which();
	}
	catch (...) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}

EXTERN_C DLLEXPORT int GetSameDouble(WolframLibraryData libData, MLINK mlp) {
	auto err = LLErrorCode::NoError;
	try {
		MathLinkStream ml(mlp, "List", 1);
		getAndPut<ML::ArrayData<double>>(ml);
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
void reshapeArray(MathLinkStream& m) {
	ML::ArrayData<T> a;
	m >> a;

	auto rank = a.get_deleter().getRank();
	if (rank >= 2) {
		auto* dims = a.get_deleter().getDims();
		std::swap(dims[rank - 2], dims[rank - 1]);
	}
	m << a;
}

EXTERN_C DLLEXPORT int Reshape8(WolframLibraryData libData, MLINK mlp) {
	auto err = LLErrorCode::NoError;
	try {
		MathLinkStream ml(mlp, "List", 1);
		reshapeArray<unsigned char>(ml);
	}
	catch (LibraryLinkError& e) {
		err = e.which();
	}
	catch (...) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}

EXTERN_C DLLEXPORT int Reshape16(WolframLibraryData libData, MLINK mlp) {
	auto err = LLErrorCode::NoError;
	try {
		MathLinkStream ml(mlp, "List", 1);
		reshapeArray<short>(ml);
	}
	catch (LibraryLinkError& e) {
		err = e.which();
	}
	catch (...) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}

EXTERN_C DLLEXPORT int Reshape32(WolframLibraryData libData, MLINK mlp) {
	auto err = LLErrorCode::NoError;
	try {
		MathLinkStream ml(mlp, "List", 1);
		reshapeArray<int>(ml);
	}
	catch (LibraryLinkError& e) {
		err = e.which();
	}
	catch (...) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}

EXTERN_C DLLEXPORT int Reshape64(WolframLibraryData libData, MLINK mlp) {
	auto err = LLErrorCode::NoError;
	try {
		MathLinkStream ml(mlp, "List", 1);
		reshapeArray<mlint64>(ml);
	}
	catch (LibraryLinkError& e) {
		err = e.which();
	}
	catch (...) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}

EXTERN_C DLLEXPORT int ReshapeDouble(WolframLibraryData libData, MLINK mlp) {
	auto err = LLErrorCode::NoError;
	try {
		MathLinkStream ml(mlp, "List", 1);
		reshapeArray<double>(ml);
	}
	catch (LibraryLinkError& e) {
		err = e.which();
	}
	catch (...) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}

EXTERN_C DLLEXPORT int ComplexToList(WolframLibraryData libData, MLINK mlp) {
	auto err = LLErrorCode::NoError;
	try {
		MathLinkStream ml(mlp, "List", 1);
		ML::ArrayData<double> a;
		ml >> a;

		auto rank = a.get_deleter().getRank();
		char** heads = a.get_deleter().getHeads();
		if (rank >= 1) {
			using namespace std::string_literals;
			auto listHead = "List"s;
			std::vector<const char*> newHeads(heads, heads + rank);
			newHeads[rank - 1] = listHead.c_str();
			ML::PutArray<double>::put(mlp, a.get(), a.get_deleter().getDims(), newHeads.data(), rank);
		} else {
			ml << a;
		}
	}
	catch (LibraryLinkError& e) {
		err = e.which();
	}
	catch (...) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}

EXTERN_C DLLEXPORT int ReceiveAndFreeArray(WolframLibraryData libData, MLINK mlp) {
	auto err = LLErrorCode::NoError;
	try {
		MathLinkStream ml(mlp, "List", 1);
		ML::ArrayData<double> a;
		ml >> a;
		ml << ML::Symbol("Null") << ML::EndPacket;
	}
	catch (LibraryLinkError& e) {
		err = e.which();
	}
	catch (...) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}


//
// Strings
//

template<typename T>
void repeatString(MathLinkStream& ml) {
	std::basic_string<T> s;
	ml >> s;
	ml << s + s;
}

// Specialize for UTF16 and UTF32 because WSTP prepends BOM to the string
template<>
void repeatString<unsigned short>(MathLinkStream& ml) {
	std::basic_string<unsigned short> s;
	ml >> s;
	auto t = s;
	ml << t + s.erase(0, 1);
}

template<>
void repeatString<unsigned int>(MathLinkStream& ml) {
	std::basic_string<unsigned int> s;
	ml >> s;
	auto t = s;
	ml << t + s.erase(0, 1);
}

EXTERN_C DLLEXPORT int RepeatString(WolframLibraryData libData, MLINK mlp) {
	auto err = LLErrorCode::NoError;
	try {
		MathLinkStream ml(mlp, "List", 1);
		repeatString<char>(ml);
	}
	catch (LibraryLinkError& e) {
		err = e.which();
	}
	catch (...) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}

EXTERN_C DLLEXPORT int RepeatUTF8(WolframLibraryData libData, MLINK mlp) {
	auto err = LLErrorCode::NoError;
	try {
		MathLinkStream ml(mlp, "List", 1);
		repeatString<unsigned char>(ml);
	}
	catch (LibraryLinkError& e) {
		err = e.which();
	}
	catch (...) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}

EXTERN_C DLLEXPORT int RepeatUTF16(WolframLibraryData libData, MLINK mlp) {
	auto err = LLErrorCode::NoError;
	try {
		MathLinkStream ml(mlp, "List", 1);
		repeatString<unsigned short>(ml);
	}
	catch (LibraryLinkError& e) {
		err = e.which();
	}
	catch (...) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}

EXTERN_C DLLEXPORT int RepeatUTF32(WolframLibraryData libData, MLINK mlp) {
	auto err = LLErrorCode::NoError;
	try {
		MathLinkStream ml(mlp, "List", 1);
		repeatString<unsigned int>(ml);
	}
	catch (LibraryLinkError& e) {
		err = e.which();
	}
	catch (...) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}

EXTERN_C DLLEXPORT int ReceiveAndFreeString(WolframLibraryData libData, MLINK mlp) {
	auto err = LLErrorCode::NoError;
	try {
		MathLinkStream ml(mlp, "List", 1);
		ML::StringData<char> s;
		ml >> s;
		ml << ML::Symbol("Null") << ML::EndPacket;
	}
	catch (LibraryLinkError& e) {
		err = e.which();
	}
	catch (...) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}


//
// Symbols and Arbitrary Functions
//

EXTERN_C DLLEXPORT int GetList(WolframLibraryData libData, MLINK mlp) {
	auto err = LLErrorCode::NoError;
	try {
		MathLinkStream ml(mlp, "List", 0);
		ml << ML::List(3);
		ml << std::vector<int> { 1, 2, 3 } << std::vector<float> { 1.5, 2.5, 3.5 } << "Hello!";
		ml << ML::EndPacket;
	}
	catch (LibraryLinkError& e) {
		err = e.which();
	}
	catch (...) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}

EXTERN_C DLLEXPORT int ReverseSymbolsOrder(WolframLibraryData libData, MLINK mlp) {
	auto err = LLErrorCode::NoError;
	try {
		MathLinkStream ml(mlp);
		ML::List l;
		ml >> l;

		std::vector<ML::Symbol> v;
		// Read all symbols in a loop
		for (auto i = 0; i < l.getArgc(); ++i) {
			ML::Symbol s;
			ml >> s;
			v.push_back(std::move(s));
		}

		ml << ML::List(l.getArgc());
		for (auto it = v.crbegin(); it != v.crend(); ++it) {
			ml << *it;
		}
		ml << ML::EndPacket;
	}
	catch (LibraryLinkError& e) {
		err = e.which();
		std::cerr << e.debug() << std::endl;
	}
	catch (...) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}


EXTERN_C DLLEXPORT int TakeLibraryFunction(WolframLibraryData libData, MLINK mlp) {
	auto err = LLErrorCode::NoError;
	try {
		MathLinkStream ml(mlp, "List", 1);
		ML::Function llf { "LibraryFunction", 3 };
		ml >> llf;

		std::string libDir;
		ml >> libDir;

		std::string fname;
		ml >> fname;

		ML::Symbol s { "LinkObject" };
		ml >> s;

		ml << ML::NewPacket << llf << libDir << "ReverseSymbolsOrder" << s << ML::EndPacket;
	}
	catch (LibraryLinkError& e) {
		err = e.which();
		std::cerr << e.debug() << std::endl;
	}
	catch (...) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}


//
// Associations/Maps
//
