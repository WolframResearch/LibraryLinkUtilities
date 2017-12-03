/** 
 * @file	MLGet.cpp
 * @date	Nov 28, 2017
 * @author	rafalc
 * @brief	<brief description>
 */
#include "MLGet.h"

#include "mathlink.h"

namespace LibraryLinkUtils {

	namespace ML {

		template<typename T>
		typename GetArray<T>::Func GetArray<T>::ArrayF  = [] (auto&&...) {
			static_assert(sizeof(T) < 0, "Trying to use ML::GetArray<T> for unsupported type T");
			return 0;
		};

		template<typename T>
		typename GetList<T>::Func GetList<T>::ListF  = [] (auto&&...) {
			static_assert(sizeof(T) < 0, "Trying to use ML::GetList<T> for unsupported type T");
			return 0;
		};

		template<typename T>
		typename GetString<T>::Func GetString<T>::StringF = [] (auto&&...) {
			static_assert(sizeof(T) < 0, "Trying to use ML::GetString<T> for unsupported type T");
			return 0;
		};

		template<typename T>
		typename GetScalar<T>::Func GetScalar<T>::ScalarF = [] (auto&&...) {
			static_assert(sizeof(T) < 0, "Trying to use ML::GetScalar<T> for unsupported type T");
			return 0;
		};


		template<>
		GetString<char>::Func GetString<char>::StringF = [](MLINK m, const char** d, int* l, int* c) {
			*l = *c = -1;
			return MLGetString(m, d);
		};

		template<>
		const std::string GetString<char>::StringFName = "MLGetString";


		template<>
		GetArray<unsigned char>::Func GetArray<unsigned char>::ArrayF = MLGetInteger8Array;

		template<>
		const std::string GetArray<unsigned char>::ArrayFName = "MLGetInteger8Array";

		template<>
		GetList<unsigned char>::Func GetList<unsigned char>::ListF = MLGetInteger8List;

		template<>
		const std::string GetList<unsigned char>::ListFName = "MLGetInteger8List";

		template<>
		GetString<unsigned char>::Func GetString<unsigned char>::StringF = MLGetUTF8String;

		template<>
		const std::string GetString<unsigned char>::StringFName = "MLGetUTF8String";

		template<>
		GetScalar<unsigned char>::Func GetScalar<unsigned char>::ScalarF = MLGetInteger8;

		template<>
		const std::string GetScalar<unsigned char>::ScalarFName = "MLGetInteger8";


		template<>
		GetArray<short>::Func GetArray<short>::ArrayF = MLGetInteger16Array;

		template<>
		const std::string GetArray<short>::ArrayFName = "MLGetInteger16Array";

		template<>
		GetList<short>::Func GetList<short>::ListF = MLGetInteger16List;

		template<>
		const std::string GetList<short>::ListFName = "MLGetInteger16List";

		template<>
		GetString<unsigned short>::Func GetString<unsigned short>::StringF = MLGetUTF16String;

		template<>
		const std::string GetString<unsigned short>::StringFName = "MLGetUTF16String";

		template<>
		GetScalar<short>::Func GetScalar<short>::ScalarF = MLGetInteger16;

		template<>
		const std::string GetScalar<short>::ScalarFName = "MLGetInteger16";


		template<>
		GetArray<int>::Func GetArray<int>::ArrayF = MLGetInteger32Array;

		template<>
		const std::string GetArray<int>::ArrayFName = "MLGetInteger32Array";

		template<>
		GetList<int>::Func GetList<int>::ListF = MLGetInteger32List;

		template<>
		const std::string GetList<int>::ListFName = "MLGetInteger32List";


		template<>
		GetString<unsigned int>::Func GetString<unsigned int>::StringF = [](MLINK m, const unsigned int** d, int* l, int* c) {
			*c = -1;
			return MLGetUTF32String(m, d, l);
		};

		template<>
		const std::string GetString<unsigned int>::StringFName = "MLGetUTF32String";

		template<>
		GetScalar<int>::Func GetScalar<int>::ScalarF = MLGetInteger32;

		template<>
		const std::string GetScalar<int>::ScalarFName = "MLGetInteger32";


		template<>
		GetArray<mlint64>::Func GetArray<mlint64>::ArrayF = MLGetInteger64Array;

		template<>
		const std::string GetArray<mlint64>::ArrayFName = "MLGetInteger64Array";

		template<>
		GetList<mlint64>::Func GetList<mlint64>::ListF = MLGetInteger64List;

		template<>
		const std::string GetList<mlint64>::ListFName = "MLGetInteger64List";

		template<>
		GetScalar<mlint64>::Func GetScalar<mlint64>::ScalarF = MLGetInteger64;

		template<>
		const std::string GetScalar<mlint64>::ScalarFName = "MLGetInteger64";



		template<>
		GetArray<float>::Func GetArray<float>::ArrayF = MLGetReal32Array;

		template<>
		const std::string GetArray<float>::ArrayFName = "MLGetReal32Array";

		template<>
		GetList<float>::Func GetList<float>::ListF = MLGetReal32List;

		template<>
		const std::string GetList<float>::ListFName = "MLGetReal32List";

		template<>
		GetScalar<float>::Func GetScalar<float>::ScalarF = MLGetReal32;

		template<>
		const std::string GetScalar<float>::ScalarFName = "MLGetReal32";


		template<>
		GetArray<double>::Func GetArray<double>::ArrayF = MLGetReal64Array;

		template<>
		const std::string GetArray<double>::ArrayFName = "MLGetReal64Array";

		template<>
		GetList<double>::Func GetList<double>::ListF = MLGetReal64List;

		template<>
		const std::string GetList<double>::ListFName = "MLGetReal64List";

		template<>
		GetScalar<double>::Func GetScalar<double>::ScalarF = MLGetReal64;

		template<>
		const std::string GetScalar<double>::ScalarFName = "MLGetReal64";

	}

} /* namespace LibraryLinkUtils */
