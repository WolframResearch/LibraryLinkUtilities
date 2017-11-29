/** 
 * @file	MLPut.cpp
 * @date	Nov 28, 2017
 * @author	rafalc
 * @brief	<brief description>
 */
#include "MLPut.h"

#include "mathlink.h"

namespace LibraryLinkUtils {

	namespace ML {

		template<typename T>
		typename PutList<T>::Func PutList<T>::ListF = [] (auto&&...) {
			static_assert(sizeof(T) < 0, "Trying to use ML::PutList<T> for unsupported type T");
			return 0;
		};

		template<typename T>
		typename PutString<T>::Func PutString<T>::StringF = [] (auto&&...) {
			static_assert(sizeof(T) < 0, "Trying to use ML::PutString<T> for unsupported type T");
			return 0;
		};

		template<typename T>
		typename PutScalar<T>::Func PutScalar<T>::ScalarF = [] (auto&&...) {
			static_assert(sizeof(T) < 0, "Trying to use ML::PutScalar<T> for unsupported type T");
			return 0;
		};


		template<>
		PutString<char>::Func PutString<char>::StringF = [](MLINK m, const char* d, int l) {
			return MLPutString(m, d);
		};

		template<>
		const std::string PutString<char>::StringFName = "MLPutString";


		template<>
		PutList<unsigned char>::Func PutList<unsigned char>::ListF = MLPutInteger8List;

		template<>
		const std::string PutList<unsigned char>::ListFName = "MLPutInteger8List";

		template<>
		PutString<unsigned char>::Func PutString<unsigned char>::StringF = MLPutUTF8String;

		template<>
		const std::string PutString<unsigned char>::StringFName = "MLPutUTF8String";

		template<>
		PutScalar<unsigned char>::Func PutScalar<unsigned char>::ScalarF = MLPutInteger8;

		template<>
		const std::string PutScalar<unsigned char>::ScalarFName = "MLPutInteger8";


		template<>
		PutList<short>::Func PutList<short>::ListF = MLPutInteger16List;

		template<>
		const std::string PutList<short>::ListFName = "MLPutInteger16List";

		template<>
		PutString<unsigned short>::Func PutString<unsigned short>::StringF = MLPutUTF16String;

		template<>
		const std::string PutString<unsigned short>::StringFName = "MLPutUTF16String";

		template<>
		PutScalar<short>::Func PutScalar<short>::ScalarF = MLPutInteger16;

		template<>
		const std::string PutScalar<short>::ScalarFName = "MLPutInteger16";


		template<>
		PutList<int>::Func PutList<int>::ListF = MLPutInteger32List;

		template<>
		const std::string PutList<int>::ListFName = "MLPutInteger32List";

		template<>
		PutString<unsigned int>::Func PutString<unsigned int>::StringF = MLPutUTF32String;

		template<>
		const std::string PutString<unsigned int>::StringFName = "MLPutUTF32String";

		template<>
		PutScalar<int>::Func PutScalar<int>::ScalarF = MLPutInteger32;

		template<>
		const std::string PutScalar<int>::ScalarFName = "MLPutInteger32";


		template<>
		PutList<mlint64>::Func PutList<mlint64>::ListF = MLPutInteger64List;

		template<>
		const std::string PutList<mlint64>::ListFName = "MLPutInteger64List";

		template<>
		PutScalar<mlint64>::Func PutScalar<mlint64>::ScalarF = MLPutInteger64;

		template<>
		const std::string PutScalar<mlint64>::ScalarFName = "MLPutInteger64";


		template<>
		PutList<float>::Func PutList<float>::ListF = MLPutReal32List;

		template<>
		const std::string PutList<float>::ListFName = "MLPutReal32List";

		template<>
		PutScalar<float>::Func PutScalar<float>::ScalarF = MLPutReal32;

		template<>
		const std::string PutScalar<float>::ScalarFName = "MLPutReal32";


		template<>
		PutList<double>::Func PutList<double>::ListF = MLPutReal64List;

		template<>
		const std::string PutList<double>::ListFName = "MLPutReal64List";

		template<>
		PutScalar<double>::Func PutScalar<double>::ScalarF = MLPutReal64;

		template<>
		const std::string PutScalar<double>::ScalarFName = "MLPutReal64";

	}

} /* namespace LibraryLinkUtils */
