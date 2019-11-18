/**
 * @file	MLTest.cpp
 * @date	Nov 23, 2017
 * @author	rafalc
 * @brief	Source code for MathLinkStream unit tests.
 */

#include "LLU/NoMinMaxWindows.h"

#include <algorithm>
#include <iostream>
#include <map>
#include <random>
#include <regex>
#include <set>
#include <string>
#include <vector>

#include "mathlink.h"

#include "WolframLibrary.h"

#include "LLU/ErrorLog/LibraryLinkError.h"
#include "LLU/LibraryLinkFunctionMacro.h"
#include "LLU/ML/MLStream.hpp"


using namespace LLU;
using MathLinkStream = MLStream<ML::Encoding::UTF8>;

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
	m << (std::numeric_limits<T>::max)();
}

//
// Scalars
//
EXTERN_C DLLEXPORT int SameInts(WolframLibraryData libData, MLINK mlp) {
	auto err = ErrorCode::NoError;
	try {
		MathLinkStream m(mlp, "List", 4);
		m << ML::List(4);
		readAndWriteScalar<unsigned char>(m);
		readAndWriteScalar<short>(m);
		readAndWriteScalar<int>(m);
		readAndWriteScalar<mlint64>(m);
	} catch (const LibraryLinkError& e) {
		err = e.which();
	} catch (...) {
		err = ErrorCode::FunctionError;
	}
	return err;
}

EXTERN_C DLLEXPORT int MaxInts(WolframLibraryData libData, MLINK mlp) {
	auto err = ErrorCode::NoError;
	try {
		MathLinkStream m(mlp, "List", 4);
		m << ML::List(4);
		readAndWriteScalarMax<unsigned char>(m);
		readAndWriteScalarMax<short>(m);
		readAndWriteScalarMax<int>(m);
		readAndWriteScalarMax<mlint64>(m);
	} catch (const LibraryLinkError& e) {
		err = e.which();
	} catch (...) {
		err = ErrorCode::FunctionError;
	}
	return err;
}

EXTERN_C DLLEXPORT int WriteMint(WolframLibraryData libData, MLINK mlp) {
	auto err = ErrorCode::NoError;
	try {
		MathLinkStream m(mlp);
		ML::List l;
		m >> l;
		mint i = -1;
		m << i;
	} catch (const LibraryLinkError& e) {
		err = e.which();
	} catch (...) {
		err = ErrorCode::FunctionError;
	}
	return err;
}

EXTERN_C DLLEXPORT int SameFloats(WolframLibraryData libData, MLINK mlp) {
	auto err = ErrorCode::NoError;
	try {
		MathLinkStream m(mlp, "List", 2);
		m << ML::List(2);
		readAndWriteScalar<float>(m);
		readAndWriteScalar<double>(m);
	} catch (const LibraryLinkError& e) {
		err = e.which();
	} catch (...) {
		err = ErrorCode::FunctionError;
	}
	return err;
}

EXTERN_C DLLEXPORT int BoolAnd(WolframLibraryData libData, MLINK mlp) {
	auto err = ErrorCode::NoError;
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
	} catch (const LibraryLinkError& e) {
		err = e.which();
	} catch (...) {
		err = ErrorCode::FunctionError;
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
	auto err = ErrorCode::NoError;
	try {
		MathLinkStream ml(mlp, "List", 1);
		reverseList<unsigned char>(ml);
	} catch (const LibraryLinkError& e) {
		err = e.which();
	} catch (...) {
		err = ErrorCode::FunctionError;
	}
	return err;
}

EXTERN_C DLLEXPORT int GetReversed16(WolframLibraryData libData, MLINK mlp) {
	auto err = ErrorCode::NoError;
	try {
		MathLinkStream ml(mlp, "List", 1);
		reverseList<short>(ml);
	} catch (const LibraryLinkError& e) {
		err = e.which();
	} catch (...) {
		err = ErrorCode::FunctionError;
	}
	return err;
}

EXTERN_C DLLEXPORT int GetReversed32(WolframLibraryData libData, MLINK mlp) {
	auto err = ErrorCode::NoError;
	try {
		MathLinkStream ml(mlp, "List", 1);
		reverseList<int>(ml);
	} catch (const LibraryLinkError& e) {
		err = e.which();
	} catch (...) {
		err = ErrorCode::FunctionError;
	}
	return err;
}

EXTERN_C DLLEXPORT int GetReversed64(WolframLibraryData libData, MLINK mlp) {
	auto err = ErrorCode::NoError;
	try {
		MathLinkStream ml(mlp, "List", 1);
		reverseList<mlint64>(ml);
	} catch (const LibraryLinkError& e) {
		err = e.which();
	} catch (...) {
		err = ErrorCode::FunctionError;
	}
	return err;
}

EXTERN_C DLLEXPORT int GetReversedDouble(WolframLibraryData libData, MLINK mlp) {
	auto err = ErrorCode::NoError;
	try {
		MathLinkStream ml(mlp, "List", 1);
		reverseList<double>(ml);
	} catch (const LibraryLinkError& e) {
		err = e.which();
	} catch (...) {
		err = ErrorCode::FunctionError;
	}
	return err;
}

EXTERN_C DLLEXPORT int GetFloatList(WolframLibraryData libData, MLINK mlp) {
	auto err = ErrorCode::NoError;
	try {
		MathLinkStream ml(mlp, "List", 1);
		getAndPut<ML::ListData<float>>(ml);
	} catch (const LibraryLinkError& e) {
		err = e.which();
	} catch (...) {
		err = ErrorCode::FunctionError;
	}
	return err;
}

//
// Arrays
//

EXTERN_C DLLEXPORT int GetSame8(WolframLibraryData libData, MLINK mlp) {
	auto err = ErrorCode::NoError;
	try {
		MathLinkStream ml(mlp, "List", 1);
		getAndPut<ML::ArrayData<unsigned char>>(ml);
	} catch (const LibraryLinkError& e) {
		err = e.which();
	} catch (...) {
		err = ErrorCode::FunctionError;
	}
	return err;
}

EXTERN_C DLLEXPORT int GetSame16(WolframLibraryData libData, MLINK mlp) {
	auto err = ErrorCode::NoError;
	try {
		MathLinkStream ml(mlp, "List", 1);
		getAndPut<ML::ArrayData<short>>(ml);
	} catch (const LibraryLinkError& e) {
		err = e.which();
	} catch (...) {
		err = ErrorCode::FunctionError;
	}
	return err;
}

EXTERN_C DLLEXPORT int GetSame32(WolframLibraryData libData, MLINK mlp) {
	auto err = ErrorCode::NoError;
	try {
		MathLinkStream ml(mlp, "List", 1);
		getAndPut<ML::ArrayData<int>>(ml);
	} catch (const LibraryLinkError& e) {
		err = e.which();
	} catch (...) {
		err = ErrorCode::FunctionError;
	}
	return err;
}

EXTERN_C DLLEXPORT int GetSame64(WolframLibraryData libData, MLINK mlp) {
	auto err = ErrorCode::NoError;
	try {
		MathLinkStream ml(mlp, "List", 1);
		getAndPut<ML::ArrayData<mlint64>>(ml);
	} catch (const LibraryLinkError& e) {
		err = e.which();
	} catch (...) {
		err = ErrorCode::FunctionError;
	}
	return err;
}

EXTERN_C DLLEXPORT int GetSameDouble(WolframLibraryData libData, MLINK mlp) {
	auto err = ErrorCode::NoError;
	try {
		MathLinkStream ml(mlp, "List", 1);
		getAndPut<ML::ArrayData<double>>(ml);
	} catch (const LibraryLinkError& e) {
		err = e.which();
	} catch (...) {
		err = ErrorCode::FunctionError;
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
	auto err = ErrorCode::NoError;
	try {
		MathLinkStream ml(mlp, "List", 1);
		reshapeArray<unsigned char>(ml);
	} catch (const LibraryLinkError& e) {
		err = e.which();
	} catch (...) {
		err = ErrorCode::FunctionError;
	}
	return err;
}

EXTERN_C DLLEXPORT int Reshape16(WolframLibraryData libData, MLINK mlp) {
	auto err = ErrorCode::NoError;
	try {
		MathLinkStream ml(mlp, "List", 1);
		reshapeArray<short>(ml);
	} catch (const LibraryLinkError& e) {
		err = e.which();
	} catch (...) {
		err = ErrorCode::FunctionError;
	}
	return err;
}

EXTERN_C DLLEXPORT int Reshape32(WolframLibraryData libData, MLINK mlp) {
	auto err = ErrorCode::NoError;
	try {
		MathLinkStream ml(mlp, "List", 1);
		reshapeArray<int>(ml);
	} catch (const LibraryLinkError& e) {
		err = e.which();
	} catch (...) {
		err = ErrorCode::FunctionError;
	}
	return err;
}

EXTERN_C DLLEXPORT int Reshape64(WolframLibraryData libData, MLINK mlp) {
	auto err = ErrorCode::NoError;
	try {
		MathLinkStream ml(mlp, "List", 1);
		reshapeArray<mlint64>(ml);
	} catch (const LibraryLinkError& e) {
		err = e.which();
	} catch (...) {
		err = ErrorCode::FunctionError;
	}
	return err;
}

EXTERN_C DLLEXPORT int ReshapeDouble(WolframLibraryData libData, MLINK mlp) {
	auto err = ErrorCode::NoError;
	try {
		MathLinkStream ml(mlp, "List", 1);
		reshapeArray<double>(ml);
	} catch (const LibraryLinkError& e) {
		err = e.which();
	} catch (...) {
		err = ErrorCode::FunctionError;
	}
	return err;
}

EXTERN_C DLLEXPORT int ComplexToList(WolframLibraryData libData, MLINK mlp) {
	auto err = ErrorCode::NoError;
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
	} catch (const LibraryLinkError& e) {
		err = e.which();
	} catch (...) {
		err = ErrorCode::FunctionError;
	}
	return err;
}

EXTERN_C DLLEXPORT int ReceiveAndFreeArray(WolframLibraryData libData, MLINK mlp) {
	auto err = ErrorCode::NoError;
	try {
		MathLinkStream ml(mlp, "List", 1);
		ML::ArrayData<double> a;
		ml >> a;
		ml << ML::Null << ML::EndPacket;
	} catch (const LibraryLinkError& e) {
		err = e.which();
	} catch (...) {
		err = ErrorCode::FunctionError;
	}
	return err;
}

//
// Strings
//

template<ML::Encoding EIn, ML::Encoding EOut>
void repeatString(MLStream<EIn, EOut>& ml) {
	std::string s;
	ml >> s;
	ml << s + s;
}

// Specialize for UTF16 and UTF32 because WSTP prepends BOM to the string
template<>
void repeatString(MLStream<ML::Encoding::UTF16>& ml) {
	std::basic_string<unsigned short> s;
	ml >> s;
	auto t = s;
	ml << t + s.erase(0, 1);
}

template<>
void repeatString(MLStream<ML::Encoding::UTF32>& ml) {
	std::basic_string<unsigned int> s;
	ml >> s;
	auto t = s;
	ml << t + s.erase(0, 1);
}

EXTERN_C DLLEXPORT int RepeatString(WolframLibraryData libData, MLINK mlp) {
	auto err = ErrorCode::NoError;
	try {
		MLStream<ML::Encoding::Native> ml(mlp, "List", 1);
		repeatString(ml);
	} catch (const LibraryLinkError& e) {
		err = e.which();
	} catch (...) {
		err = ErrorCode::FunctionError;
	}
	return err;
}

EXTERN_C DLLEXPORT int RepeatUTF8(WolframLibraryData libData, MLINK mlp) {
	auto err = ErrorCode::NoError;
	try {
		MLStream<ML::Encoding::UTF8> ml(mlp, "List", 1);
		repeatString(ml);
	} catch (const LibraryLinkError& e) {
		err = e.which();
	} catch (...) {
		err = ErrorCode::FunctionError;
	}
	return err;
}

EXTERN_C DLLEXPORT int RepeatUTF16(WolframLibraryData libData, MLINK mlp) {
	auto err = ErrorCode::NoError;
	try {
		MLStream<ML::Encoding::UTF16> ml(mlp, "List", 1);
		repeatString(ml);
	} catch (const LibraryLinkError& e) {
		err = e.which();
	} catch (...) {
		err = ErrorCode::FunctionError;
	}
	return err;
}

EXTERN_C DLLEXPORT int RepeatUTF32(WolframLibraryData libData, MLINK mlp) {
	auto err = ErrorCode::NoError;
	try {
		MLStream<ML::Encoding::UTF32> ml(mlp, "List", 1);
		repeatString(ml);
	} catch (const LibraryLinkError& e) {
		err = e.which();
	} catch (...) {
		err = ErrorCode::FunctionError;
	}
	return err;
}

template<ML::Encoding E>
void appendString(MLStream<E>& ml) {
	ML::StringType<E> s;
	ml >> s;
	ML::StringType<E> appendix {{'\a', '\b', '\f', '\r', '\n', '\t', '\v', '\\', '\'', '\"', '\?'}};
	ml << s + appendix;
}

template<>
void appendString(MLStream<ML::Encoding::Native>& ml) {
	std::string s;
	ml >> s;
	std::string appendix {{'\a', '\b', '\f', '\r', '\n', '\t', '\v', '\\', '\\', '\'', '\"', '\?'}};
	ml << s + appendix;
}

EXTERN_C DLLEXPORT int AppendString(WolframLibraryData libData, MLINK mlp) {
	auto err = ErrorCode::NoError;
	try {
		MLStream<ML::Encoding::Native> ml(mlp, "List", 1);
		appendString(ml);
	} catch (const LibraryLinkError& e) {
		err = e.which();
	} catch (std::runtime_error& e) {
		std::cout << e.what() << std::endl;
		err = ErrorCode::FunctionError;
	}
	return err;
}

EXTERN_C DLLEXPORT int AppendUTF8(WolframLibraryData libData, MLINK mlp) {
	auto err = ErrorCode::NoError;
	try {
		MLStream<ML::Encoding::UTF8> ml(mlp, "List", 1);
		appendString(ml);
	} catch (const LibraryLinkError& e) {
		err = e.which();
	} catch (...) {
		err = ErrorCode::FunctionError;
	}
	return err;
}

EXTERN_C DLLEXPORT int AppendUTF16(WolframLibraryData libData, MLINK mlp) {
	auto err = ErrorCode::NoError;
	try {
		MLStream<ML::Encoding::UTF16> ml(mlp, "List", 1);
		appendString(ml);
	} catch (const LibraryLinkError& e) {
		err = e.which();
	} catch (...) {
		err = ErrorCode::FunctionError;
	}
	return err;
}

EXTERN_C DLLEXPORT int AppendUTF32(WolframLibraryData libData, MLINK mlp) {
	auto err = ErrorCode::NoError;
	try {
		MLStream<ML::Encoding::UTF32> ml(mlp, "List", 1);
		appendString(ml);
	} catch (const LibraryLinkError& e) {
		err = e.which();
	} catch (...) {
		err = ErrorCode::FunctionError;
	}
	return err;
}

EXTERN_C DLLEXPORT int ReceiveAndFreeString(WolframLibraryData libData, MLINK mlp) {
	auto err = ErrorCode::NoError;
	try {
		MathLinkStream ml(mlp, "List", 1);
		ML::StringData<ML::Encoding::Native> s;
		ml >> s;
		ml << ML::Null << ML::EndPacket;
	} catch (const LibraryLinkError& e) {
		err = e.which();
	} catch (...) {
		err = ErrorCode::FunctionError;
	}
	return err;
}

EXTERN_C DLLEXPORT int GetAndPutUTF8(WolframLibraryData libData, MLINK mlp) {
	auto err = ErrorCode::NoError;
	try {
		MLStream<ML::Encoding::UTF8> ml(mlp, "List", 2);

		ML::StringData<ML::Encoding::UTF8> sUChar;
		ml >> sUChar;

		std::cout << "Unsigned char string bytes: ";
		for (int i = 0; i < sUChar.get_deleter().getLength(); ++i) {
			std::cout << std::to_string(+sUChar[i]) << " ";
		}
		std::cout << std::endl;

		std::string sChar;
		ml >> sChar;

		std::cout << sChar << std::endl;

		ml << R"(This will be sent as UTF8 encoded string. No need to escape backslashes \o/. Some weird characters: ą©łóßµ)" << ML::EndPacket;

	} catch (const LibraryLinkError& e) {
		err = e.which();
	} catch (...) {
		err = ErrorCode::FunctionError;
	}
	return err;
}

//
// Symbols and Arbitrary Functions
//

EXTERN_C DLLEXPORT int GetList(WolframLibraryData libData, MLINK mlp) {
	auto err = ErrorCode::NoError;
	try {
		MLStream<ML::Encoding::Byte> ml(mlp, "List", 0);
		ml << ML::List(5);
		ml << std::vector<int> {1, 2, 3} << ML::Missing() << ML::putAs<ML::Encoding::UTF8>(std::vector<float> {1.5, 2.5, 3.5})
		   << ML::putAs<ML::Encoding::UTF8>("Hello!") << ML::Missing("Deal with it");
		ml << ML::EndPacket;
	} catch (const LibraryLinkError& e) {
		err = e.which();
	} catch (...) {
		err = ErrorCode::FunctionError;
	}
	return err;
}

EXTERN_C DLLEXPORT int ReverseSymbolsOrder(WolframLibraryData libData, MLINK mlp) {
	auto err = ErrorCode::NoError;
	try {
		MathLinkStream ml(mlp);
		ML::List l;
		ml >> l;

		// use unique_ptr only to test if MathLinkStream handles it correctly
		std::vector<std::unique_ptr<ML::Symbol>> v;

		// Read all symbols in a loop
		for (auto i = 0; i < l.getArgc(); ++i) {
			ML::Symbol s;
			ml >> s;
			v.push_back(std::make_unique<ML::Symbol>(std::move(s)));
		}

		ml << ML::List(l.getArgc());
		for (auto it = v.crbegin(); it != v.crend(); ++it) {
			ml << *it;
		}
		ml << ML::EndPacket;
	} catch (const LibraryLinkError& e) {
		err = e.which();
	} catch (...) {
		err = ErrorCode::FunctionError;
	}
	return err;
}

EXTERN_C DLLEXPORT int TakeLibraryFunction(WolframLibraryData libData, MLINK mlp) {
	auto err = ErrorCode::NoError;
	try {
		MLStream<ML::Encoding::UTF8> ml(mlp, "List", 1);
		ML::Function llf {"LibraryFunction", 3};
		ml >> llf;

		std::string libDir;
		ml >> libDir;

		std::string fname;
		ml >> fname;

		ML::Symbol s {"LinkObject"};
		ml >> s;

		ml << ML::NewPacket << llf << libDir << "ReverseSymbolsOrder" << s << ML::EndPacket;
	} catch (const LibraryLinkError& e) {
		err = e.which();
	} catch (...) {
		err = ErrorCode::FunctionError;
	}
	return err;
}

EXTERN_C DLLEXPORT int GetSet(WolframLibraryData libData, MLINK mlp) {
	auto err = ErrorCode::NoError;
	try {
		MLStream<ML::Encoding::UTF8> ml(mlp);
		ML::List l;
		ml >> l;

		std::set<std::string> ss;
		ML::List innerL;
		ml >> innerL;
		for (auto i = 0; i < innerL.getArgc(); ++i) {
			std::string s;
			ml >> s;
			std::cout << s << std::endl;
			ss.insert(std::move(s));
		}

		if (l.getArgc() == 2) {	   // send with custom head
			std::string head;
			ml >> head;
			ml.sendRange(ss.cbegin(), ss.cend(), head);
		} else {
			ml << ss;
		}

		ml << ML::EndPacket;
	} catch (const LibraryLinkError& e) {
		err = e.which();
	} catch (...) {
		err = ErrorCode::FunctionError;
	}
	return err;
}

//
// Associations/Maps
//

LIBRARY_MATHLINK_FUNCTION(ReadNestedMap) {
	auto err = ErrorCode::NoError;
	try {
		MLStream<ML::Encoding::Byte> ml(mlp, "List", 1);

		std::map<std::string, std::map<int, std::vector<double>>> myNestedMap;

		ml >> ML::getAs<ML::Encoding::UTF8>(myNestedMap);

		for (auto& outerRule : myNestedMap) {
			const auto& outerKey = outerRule.first;
			auto& innerMap = outerRule.second;
			for (auto& innerRule : innerMap) {
				auto& innerMapVal = innerRule.second;
				if (outerKey == "Negate") {
					std::transform(innerMapVal.cbegin(), innerMapVal.cend(), innerMapVal.begin(), std::negate<double>());
				} else if (outerKey == "Add") {
					std::transform(innerMapVal.cbegin(), innerMapVal.cend(), innerMapVal.begin(), [&innerRule](auto d) { return d + innerRule.first; });
				} else if (outerKey == "Multiply") {
					std::transform(innerMapVal.cbegin(), innerMapVal.cend(), innerMapVal.begin(), [&innerRule](auto d) { return d * innerRule.first; });
				}
			}
		}

		ml << ML::putAs<ML::Encoding::UTF8>(myNestedMap);
	} catch (const LibraryLinkError& e) {
		err = e.which();
	} catch (...) {
		err = ErrorCode::FunctionError;
	}
	return err;
}

//
// BeginExpr - DropExpr - EndExpr
//

LIBRARY_MATHLINK_FUNCTION(UnknownLengthList) {
	auto err = ErrorCode::NoError;
	try {
		MathLinkStream ml(mlp, 1);

		int modulus;
		ml >> modulus;

		std::random_device rd;
		std::mt19937 gen {rd()};
		std::uniform_int_distribution<> distr(0, std::max(1'000'000, modulus + 1));

		ml << ML::BeginExpr("List");

		int r;
		while ((r = distr(gen)) % modulus != 0) {
			ml << r;
		}
		ml << ML::EndExpr();

	} catch (const LibraryLinkError& e) {
		err = e.which();
		std::cout << e.what() << std::endl;
	} catch (...) {
		err = ErrorCode::FunctionError;
	}
	return err;
}

LIBRARY_MATHLINK_FUNCTION(RaggedArray) {
	auto err = ErrorCode::NoError;
	try {
		MathLinkStream ml(mlp, 1);

		int len;
		ml >> len;

		ml << ML::BeginExpr("List");
		for (int i = 0; i < len; ++i) {
			ml << ML::BeginExpr("List");
			for (int j = 0; j < i; ++j) {
				ml << j;
			}
			ml << ML::EndExpr();
		}
		ml << ML::EndExpr();

	} catch (const LibraryLinkError& e) {
		err = e.which();
	} catch (...) {
		err = ErrorCode::FunctionError;
	}
	return err;
}

LIBRARY_MATHLINK_FUNCTION(FactorsOrFailed) {
	auto err = ErrorCode::NoError;
	try {
		MathLinkStream ml(mlp, 1);

		std::vector<int> numbers;
		ml >> numbers;

		ml << ML::BeginExpr("Association");
		for (auto&& n : numbers) {
			ml << ML::Rule << n;
			ml << ML::BeginExpr("List");
			int divisors = 0;
			for (int j = 1; j <= n; ++j) {
				if (n % j == 0) {
					if (divisors < 15) {
						ml << j;
						divisors++;
					} else {
						ml << ML::DropExpr();
						ml << ML::Symbol("$Failed");	// $Failed is now sent to the "parent" loopback link
						break;
					}
				}
			}
			ml << ML::EndExpr();
		}
		ml << ML::EndExpr();

	} catch (const LibraryLinkError& e) {
		err = e.which();
	} catch (...) {
		err = ErrorCode::FunctionError;
	}
	return err;
}

LIBRARY_MATHLINK_FUNCTION(Empty) {
	auto err = ErrorCode::NoError;
	try {
		MathLinkStream ml(mlp, 1);

		std::string head;
		ml >> head;

		ml << ML::BeginExpr(head);
		ml << ML::EndExpr();

	} catch (const LibraryLinkError& e) {
		err = e.which();
	} catch (...) {
		err = ErrorCode::FunctionError;
	}
	return err;
}

LIBRARY_MATHLINK_FUNCTION(ListOfStringsTiming) {
	auto err = ErrorCode::NoError;
	constexpr int repetitions = 100;
	try {
		MathLinkStream ml(mlp, 2);

		std::vector<std::string> listOfStrings;
		ml >> listOfStrings;

		bool useBeginExpr;
		ml >> useBeginExpr;

		useBeginExpr ? (ml << ML::BeginExpr("List")) : (ml << ML::List(static_cast<int>(repetitions * listOfStrings.size())));

		for (int i = 0; i < repetitions; ++i) {
			for (auto&& s : listOfStrings)
				ml << s;
		}

		if (useBeginExpr) {
			ml << ML::EndExpr();
		}

		ml << ML::EndPacket;
	} catch (const LibraryLinkError& e) {
		err = e.which();
	} catch (...) {
		err = ErrorCode::FunctionError;
	}
	return err;
}
