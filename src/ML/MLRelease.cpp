/** 
 * @file	MLRelease.h
 * @date	Nov 28, 2017
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @brief	Implementation file with classes responsible for releasing memory allocated by MathLink when receiving data.
 */
#include "MLRelease.h"

namespace LibraryLinkUtils {

	namespace ML {

		template<>
		ReleaseString<char>::Func ReleaseString<char>::Release = [](MLINK m, const char* d, int) {
			MLReleaseString(m, d);
		};

		template<>
		ReleaseArray<unsigned char>::Func ReleaseArray<unsigned char>::Release = MLReleaseInteger8Array;

		template<>
		ReleaseList<unsigned char>::Func ReleaseList<unsigned char>::Release = MLReleaseInteger8List;

		template<>
		ReleaseString<unsigned char>::Func ReleaseString<unsigned char>::Release = MLReleaseUTF8String;

		template<>
		ReleaseArray<short>::Func ReleaseArray<short>::Release = MLReleaseInteger16Array;

		template<>
		ReleaseList<short>::Func ReleaseList<short>::Release = MLReleaseInteger16List;

		template<>
		ReleaseString<unsigned short>::Func ReleaseString<unsigned short>::Release = MLReleaseUTF16String;

		template<>
		ReleaseArray<int>::Func ReleaseArray<int>::Release = MLReleaseInteger32Array;

		template<>
		ReleaseList<int>::Func ReleaseList<int>::Release = MLReleaseInteger32List;

		template<>
		ReleaseString<unsigned int>::Func ReleaseString<unsigned int>::Release = MLReleaseUTF32String;

		template<>
		ReleaseArray<mlint64>::Func ReleaseArray<mlint64>::Release = MLReleaseInteger64Array;

		template<>
		ReleaseList<mlint64>::Func ReleaseList<mlint64>::Release = MLReleaseInteger64List;

		template<>
		ReleaseArray<float>::Func ReleaseArray<float>::Release = MLReleaseReal32Array;

		template<>
		ReleaseList<float>::Func ReleaseList<float>::Release = MLReleaseReal32List;

		template<>
		ReleaseArray<double>::Func ReleaseArray<double>::Release = MLReleaseReal64Array;

		template<>
		ReleaseList<double>::Func ReleaseList<double>::Release = MLReleaseReal64List;

	}

} /* namespace LibraryLinkUtils */
