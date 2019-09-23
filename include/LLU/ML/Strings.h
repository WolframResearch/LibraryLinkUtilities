/**
 * @file	Strings.h
 * @date	Mar 22, 2018
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @brief	Definitions of multiple structures and type aliases related to string handling in MathLink
 */
#ifndef LLUTILS_ML_STRINGS_H_
#define LLUTILS_ML_STRINGS_H_

#include <algorithm>
#include <functional>
#include <memory>

#include "mathlink.h"

#include "LLU/ErrorLog/Errors.h"
#include "LLU/ML/EncodingTraits.hpp"
#include "LLU/ML/Utilities.h"
#include "LLU/Utilities.hpp"

namespace LLU {

	namespace ML {

		/**
		 * @brief Contains configuration parameters that paclet developers may modify
		 */
		namespace EncodingConfig {
			/// Global configuration parameter defining a substitute character required in MLGetByteString.
			/// The default value is \b 26
			extern long substituteCodeForByteEncoding;

			/// Global configuration parameter specifying whether to use a faster version of sending UTF8 strings.
			/// It means that if a string only have ASCII characters then much faster MLPutByteString is used. The time of checking if the string
			/// only has ASCII characters is negligible compared to the time taken by MLPut*String, so this parameter is by default set to \b true.
			extern bool useFastUTF8;
		}

		template<Encoding E>
		struct ReleaseString;

		template<Encoding E>
		using StringData = std::unique_ptr<const CharType<E>[], ReleaseString<E>>;

		template<typename T>
		using GetStringFuncT = std::function<int(MLINK, const T**, int*, int*)>;

		template<typename T>
		using PutStringFuncT = std::function<int(MLINK, const T*, int)>;

		template<typename T>
		using ReleaseStringFuncT = std::function<void(MLINK, const T*, int)>;

		template<Encoding E>
		struct String {

			using CharT = CharType<E>;

			static GetStringFuncT<CharT> Get;
			static PutStringFuncT<CharT> Put;
			static ReleaseStringFuncT<CharT> Release;

			static const std::string GetFName;
			static const std::string PutFName;

			template<typename T>
			static void put(MLINK m, const T* string, int len) {
				static_assert(CharacterTypesCompatible<E, T>, "Character type does not match the encoding in ML::String<E>::put");
				auto* expectedStr = reinterpret_cast<const CharT*>(string);
				checkError(m, Put(m, expectedStr, len), ErrorName::MLPutStringError, PutFName);
			}

			static StringData<E> get(MLINK m) {
				const CharT* rawResult;
				int bytes, characters;
				checkError(m, Get(m, &rawResult, &bytes, &characters), ErrorName::MLGetStringError, GetFName);
				return {rawResult, ReleaseString<E> {m, bytes, characters}};
			}

			template<typename T>
			static std::basic_string<T> getString(MLINK m) {
				static_assert(CharacterTypesCompatible<E, T>, "Character type does not match the encoding in ML::String<E>::getString");
				using StringType = std::basic_string<T>;

				auto strData {get(m)};

				auto bytes = strData.get_deleter().getLength();
				auto* expectedData = reinterpret_cast<const T*>(strData.get());
				auto strlen = static_cast<typename StringType::size_type>(bytes);

				return (bytes < 0 ? StringType {expectedData} : StringType {expectedData, strlen});
			}
		};

		/**
		 * @cond
		 * Explicit specialization of String class for undefined encoding.
		 * Its sole purpose is to trigger nice compilation errors.
		 */
		template<>
		struct String<Encoding::Undefined> {

			template<typename T>
			static void put(MLINK, const T*, int) {
				static_assert(dependent_false_v<T>, "Trying to use ML::String<Encoding::Undefined>::put");
			}

			template<typename T = char>
			static T* get(MLINK) {
				static_assert(dependent_false_v<T>, "Trying to use ML::String<Encoding::Undefined>::get");
				return nullptr;
			}

			template<typename T>
			static std::basic_string<T> getString(MLINK) {
				static_assert(dependent_false_v<T>, "Trying to use ML::String<Encoding::Undefined>::getString");
				return {};
			}
		};
		/// @endcond

		template<Encoding E>
		struct ReleaseString {
			ReleaseString() = default;
			ReleaseString(MLINK m, int l, int c) : m(m), length(l), chars(c) {}

			void operator()(const CharType<E>* data) {
				String<E>::Release(m, data, length);
			}

			int getLength() const {
				return length;
			}

			int getCharacters() const {
				return chars;
			}

		private:
			MLINK m = nullptr;
			int length = 0;
			int chars = 0;
		};

#ifndef _WIN32

/// Macro for declaring specializations of static members for ML::String<Encoding::E>
/// For internal use only.
#define ML_STRING_DECLARE_SPECIALIZATIONS_OF_STATIC_MEMBERS(E)              \
	template<>                                                              \
	GetStringFuncT<CharType<Encoding::E>> String<Encoding::E>::Get;         \
	template<>                                                              \
	PutStringFuncT<CharType<Encoding::E>> String<Encoding::E>::Put;         \
	template<>                                                              \
	ReleaseStringFuncT<CharType<Encoding::E>> String<Encoding::E>::Release; \
	template<>                                                              \
	const std::string String<Encoding::E>::GetFName;                        \
	template<>                                                              \
	const std::string String<Encoding::E>::PutFName;

		ML_STRING_DECLARE_SPECIALIZATIONS_OF_STATIC_MEMBERS(Native)
		ML_STRING_DECLARE_SPECIALIZATIONS_OF_STATIC_MEMBERS(Byte)
		ML_STRING_DECLARE_SPECIALIZATIONS_OF_STATIC_MEMBERS(UTF8)
		ML_STRING_DECLARE_SPECIALIZATIONS_OF_STATIC_MEMBERS(UTF16)
		ML_STRING_DECLARE_SPECIALIZATIONS_OF_STATIC_MEMBERS(UCS2)
		ML_STRING_DECLARE_SPECIALIZATIONS_OF_STATIC_MEMBERS(UTF32)

#else

		template<>
		GetStringFuncT<CharType<Encoding::Native>> String<Encoding::Native>::Get = [](MLINK m, const char** strData, int* len, int* charCnt) {
			*len = *charCnt = -1;
			return MLGetString(m, strData);
		};
		template<>
		PutStringFuncT<CharType<Encoding::Native>> String<Encoding::Native>::Put = [](MLINK m, const char* strData, int) { return MLPutString(m, strData); };
		template<>
		ReleaseStringFuncT<CharType<Encoding::Native>>
			String<Encoding::Native>::Release = [](MLINK m, const char* strData, int) { MLReleaseString(m, strData); };
		template<>
		const std::string String<Encoding::Native>::GetFName = "MLGetString";
		template<>
		const std::string String<Encoding::Native>::PutFName = "MLPutString";

		template<>
		GetStringFuncT<CharType<Encoding::Byte>> String<Encoding::Byte>::Get = [](MLINK m, const unsigned char** strData, int* len, int* charCnt) {
			*charCnt = -1;
			return MLGetByteString(m, strData, len, EncodingConfig::substituteCodeForByteEncoding);
		};
		template<>
		PutStringFuncT<CharType<Encoding::Byte>> String<Encoding::Byte>::Put = MLPutByteString;
		template<>
		ReleaseStringFuncT<CharType<Encoding::Byte>> String<Encoding::Byte>::Release = MLReleaseByteString;
		template<>
		const std::string String<Encoding::Byte>::GetFName = "MLGetByteString";
		template<>
		const std::string String<Encoding::Byte>::PutFName = "MLPutByteString";

		namespace {
			// Nested lambdas are too much for MSVC, so we define this helper function separately
			bool allASCIIQ(const unsigned char* strData, int len) {
				return std::all_of(strData, strData + len, [](unsigned char c) -> bool { return c <= 127; });
			}
		}

		template<>
		GetStringFuncT<CharType<Encoding::UTF8>> String<Encoding::UTF8>::Get = MLGetUTF8String;
		template<>
		PutStringFuncT<CharType<Encoding::UTF8>> String<Encoding::UTF8>::Put = [](MLINK m, const unsigned char* strData, int len) -> int {
			if (EncodingConfig::useFastUTF8 && allASCIIQ(strData, len)) {
				return MLPutByteString(m, strData, len);
			} else {
				return MLPutUTF8String(m, strData, len);
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
		GetStringFuncT<CharType<Encoding::UCS2>> String<Encoding::UCS2>::Get = [](MLINK m, const unsigned short** strData, int* len, int* charCnt) {
			*charCnt = -1;
			return MLGetUCS2String(m, strData, len);
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
		GetStringFuncT<CharType<Encoding::UTF32>> String<Encoding::UTF32>::Get = [](MLINK m, const unsigned int** strData, int* len, int* charCnt) {
			*charCnt = -1;
			return MLGetUTF32String(m, strData, len);
		};
		template<>
		PutStringFuncT<CharType<Encoding::UTF32>> String<Encoding::UTF32>::Put = MLPutUTF32String;
		template<>
		ReleaseStringFuncT<CharType<Encoding::UTF32>> String<Encoding::UTF32>::Release = MLReleaseUTF32String;
		template<>
		const std::string String<Encoding::UTF32>::GetFName = "MLGetUTF32String";
		template<>
		const std::string String<Encoding::UTF32>::PutFName = "MLPutUTF32String";

#endif

	} /* namespace ML */

} /* namespace LLU */

#endif /* LLUTILS_ML_STRINGS_H_ */
