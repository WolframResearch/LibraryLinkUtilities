/** 
 * @file	Strings.cpp
 * @date	Mar 23, 2018
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @brief	<brief description>
 */

#include "Strings.h"

namespace LibraryLinkUtils {

	namespace ML {

		template<>
		ReleaseStringFuncT<CharType_t<Encoding::MMA>> String<Encoding::MMA>::Release = [](MLINK m, const char* d, int) {
			MLReleaseString(m, d);
		};

		template<>
		ReleaseStringFuncT<CharType_t<Encoding::UTF8>> String<Encoding::UTF8>::Release = MLReleaseByteString;

		template<>
		ReleaseStringFuncT<CharType_t<Encoding::UTF8>> String<Encoding::UTF8>::Release = MLReleaseUTF8String;

		template<>
		ReleaseStringFuncT<CharType_t<Encoding::UTF8Strict>> String<Encoding::UTF8Strict>::Release = MLReleaseUTF8String;

		template<>
		ReleaseStringFuncT<CharType_t<Encoding::UTF16>> String<Encoding::UTF16>::Release = MLReleaseUTF16String;

		template<>
		ReleaseStringFuncT<CharType_t<Encoding::UCS2>> String<Encoding::UCS2>::Release = MLReleaseUCS2String;

		template<>
		ReleaseStringFuncT<CharType_t<Encoding::UTF32>> String<Encoding::UTF32>::Release = MLReleaseUTF32String;
	}

} /* namespace LibraryLinkUtils */
