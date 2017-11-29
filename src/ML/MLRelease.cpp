/** 
 * @file	MLRelease.cpp
 * @date	Nov 28, 2017
 * @author	rafal
 * @brief	<brief description>
 */
#include "MLRelease.h"

namespace LibraryLinkUtils {

	namespace ML {

		template<typename T>
		typename ReleaseList<T>::Func ReleaseList<T>::Release  = [] (auto&&...) {
			static_assert(sizeof(T) < 0, "Trying to use ML::ReleaseList<T>::Release for unsupported type T");
		};

		template<typename T>
		typename ReleaseString<T>::Func ReleaseString<T>::Release = [] (auto&&...) {
			static_assert(sizeof(T) < 0, "Trying to use ML::ReleaseString<T>::Release for unsupported type T");
		};


		template<>
		ReleaseString<char>::Func ReleaseString<char>::Release = [](MLINK m, const char* d, int) {
			MLReleaseString(m, d);
		};


		template<>
		ReleaseList<unsigned char>::Func ReleaseList<unsigned char>::Release = MLReleaseInteger8List;

		template<>
		ReleaseString<unsigned char>::Func ReleaseString<unsigned char>::Release = MLReleaseUTF8String;

		template<>
		ReleaseList<short>::Func ReleaseList<short>::Release = MLReleaseInteger16List;

		template<>
		ReleaseString<unsigned short>::Func ReleaseString<unsigned short>::Release = MLReleaseUTF16String;

		template<>
		ReleaseList<int>::Func ReleaseList<int>::Release = MLReleaseInteger32List;

		template<>
		ReleaseString<unsigned int>::Func ReleaseString<unsigned int>::Release = MLReleaseUTF32String;

		template<>
		ReleaseList<mlint64>::Func ReleaseList<mlint64>::Release = MLReleaseInteger64List;

		template<>
		ReleaseList<float>::Func ReleaseList<float>::Release = MLReleaseReal32List;

		template<>
		ReleaseList<double>::Func ReleaseList<double>::Release = MLReleaseReal64List;

	}

} /* namespace LibraryLinkUtils */
