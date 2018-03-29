/** 
 * @file	Strings.cpp
 * @date	Mar 23, 2018
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @brief	Implementation file for all the functionality related to Strings in MathLink
 */

#include <algorithm>
#include <type_traits>

#include "Strings.h"

namespace LibraryLinkUtils {

	namespace ML {

		/// Definitions of configuration parameters, see header file for detailed description
		namespace EncodingConfig {
			long substituteCodeForByteEncoding = 26;
			bool useFastUTF8 = true;
		}

		constexpr const char* getEncodingName(Encoding e) {
			switch(e) {
				case Encoding::Undefined: return "Undefined";
				case Encoding::Native: return "Native";
				case Encoding::Byte: return "Byte";
				case Encoding::UTF8: return "UTF8";
				case Encoding::UTF16: return "UTF16";
				case Encoding::UCS2: return "UCS2";
				case Encoding::UTF32: return "UTF32";
				default: return "NoSuchEncoding";
			}
		}



		template<>
		GetStringFuncT<CharType<Encoding::Native>> String<Encoding::Native>::Get = [](MLINK m, const char** d, int* l, int* c) {
			*l = *c = -1;
			return MLGetString(m, d);
		};
		template<>
		PutStringFuncT<CharType<Encoding::Native>> String<Encoding::Native>::Put = [](MLINK m, const char* d, int) {
			return MLPutString(m, d);
		};
		template<>
		ReleaseStringFuncT<CharType<Encoding::Native>> String<Encoding::Native>::Release = [](MLINK m, const char* d, int) {
			MLReleaseString(m, d);
		};
		template<>
		const std::string String<Encoding::Native>::GetFName = "MLGetString";
		template<>
		const std::string String<Encoding::Native>::PutFName = "MLPutString";


		template<>
		GetStringFuncT<CharType<Encoding::Byte>> String<Encoding::Byte>::Get = [](MLINK m, const unsigned char** d, int* l, int* c) {
			*c = -1;
			return MLGetByteString(m, d, l, EncodingConfig::substituteCodeForByteEncoding);
		};
		template<>
		PutStringFuncT<CharType<Encoding::Byte>> String<Encoding::Byte>::Put = MLPutByteString;
		template<>
		ReleaseStringFuncT<CharType<Encoding::Byte>> String<Encoding::Byte>::Release = MLReleaseByteString;
		template<>
		const std::string String<Encoding::Byte>::GetFName = "MLGetByteString";
		template<>
		const std::string String<Encoding::Byte>::PutFName = "MLPutByteString";

		template<>
		GetStringFuncT<CharType<Encoding::UTF8>> String<Encoding::UTF8>::Get = MLGetUTF8String;
		template<>
		PutStringFuncT<CharType<Encoding::UTF8>> String<Encoding::UTF8>::Put = [](MLINK m, const unsigned char* d, int len) {
			if (EncodingConfig::useFastUTF8 && std::all_of(d, d + len, [](unsigned char c) { return c <= 127; })) {
				return MLPutByteString(m, d, len);
			} else {
				return MLPutUTF8String(m, d, len);
			}
		};
		template<>
		ReleaseStringFuncT<CharType<Encoding::UTF8>> String<Encoding::UTF8>::Release = MLReleaseUTF8String;
		template<>
		const std::string String<Encoding::UTF8>::GetFName = "MLGetUTF8String";
		template<>
		const std::string String<Encoding::UTF8>::PutFName = "MLPut(UTF8/Byte)String";


		template<>
		GetStringFuncT<CharType<Encoding::UTF16>> String<Encoding::UTF16>::Get = MLGetUTF16String;
		template<>
		PutStringFuncT<CharType<Encoding::UTF16>> String<Encoding::UTF16>::Put = MLPutUTF16String;
		template<>
		ReleaseStringFuncT<CharType<Encoding::UTF16>> String<Encoding::UTF16>::Release = MLReleaseUTF16String;
		template<>
		const std::string String<Encoding::UTF16>::GetFName = "MLGetUTF16String";
		template<>
		const std::string String<Encoding::UTF16>::PutFName = "MLPutUTF16String";


		template<>
		GetStringFuncT<CharType<Encoding::UCS2>> String<Encoding::UCS2>::Get = [](MLINK m, const unsigned short** d, int* l, int* c) {
			*c = -1;
			return MLGetUCS2String(m, d, l);
		};
		template<>
		PutStringFuncT<CharType<Encoding::UCS2>> String<Encoding::UCS2>::Put = MLPutUCS2String;
		template<>
		ReleaseStringFuncT<CharType<Encoding::UCS2>> String<Encoding::UCS2>::Release = MLReleaseUCS2String;
		template<>
		const std::string String<Encoding::UCS2>::GetFName = "MLGetUCS2String";
		template<>
		const std::string String<Encoding::UCS2>::PutFName = "MLPutUCS2String";


		template<>
		GetStringFuncT<CharType<Encoding::UTF32>> String<Encoding::UTF32>::Get = [](MLINK m, const unsigned int** d, int* l, int* c) {
			*c = -1;
			return MLGetUTF32String(m, d, l);
		};
		template<>
		PutStringFuncT<CharType<Encoding::UTF32>> String<Encoding::UTF32>::Put = MLPutUTF32String;
		template<>
		ReleaseStringFuncT<CharType<Encoding::UTF32>> String<Encoding::UTF32>::Release = MLReleaseUTF32String;
		template<>
		const std::string String<Encoding::UTF32>::GetFName = "MLGetUTF32String";
		template<>
		const std::string String<Encoding::UTF32>::PutFName = "MLPutUTF32String";
	}

} /* namespace LibraryLinkUtils */
