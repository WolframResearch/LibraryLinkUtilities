/** 
 * @file	Encodings.h
 * @date	Mar 22, 2018
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @brief	<brief description>
 */
#ifndef LLUTILS_ML_ENCODINGS_H_
#define LLUTILS_ML_ENCODINGS_H_

#include <functional>
#include <memory>

#include "mathlink.h"

#include "LibraryLinkError.h"
#include "EncodingTraits.hpp"

namespace LibraryLinkUtils {

	namespace ML {

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
				checkError(m, Put(m, expectedStr, len), LLErrorName::MLPutStringError, PutFName);
			}

			static StringData<E> get(MLINK m) {
				const CharT* rawResult;
				int bytes, characters;
				checkError(m, Get(m, &rawResult, &bytes, &characters), LLErrorName::MLGetStringError, GetFName);
				return {rawResult, ReleaseString<E> {m, bytes , characters}};
			}

			template<typename T>
			static std::basic_string<T> getString(MLINK m) {
				static_assert(CharacterTypesCompatible<E, T>, "Character type does not match the encoding in ML::String<E>::getString");
				using StringType = std::basic_string<T>;

				auto strData { get(m) };

				auto bytes = strData.get_deleter().getLength();
				auto* expectedData = reinterpret_cast<const T*>(strData.get());
				auto strlen = static_cast<typename StringType::size_type>(bytes);

				return (bytes < 0? StringType { expectedData } : StringType { expectedData, strlen });
			}
		};

		template<>
		struct String<Encoding::Undefined> {

			template<typename T>
			static void put(MLINK m, const T* string, int len) {
				static_assert(sizeof(T) == 0, "Trying to use ML::String<Encoding::Undefined>::put");
			}

			template<typename T = char>
			static T* get(MLINK m) {
				static_assert(sizeof(T) == 0, "Trying to use ML::String<Encoding::Undefined>::get");
				return nullptr;
			}

			template<typename T>
			static std::basic_string<T> getString(MLINK m) {
				static_assert(sizeof(T) == 0, "Trying to use ML::String<Encoding::Undefined>::getString");
				return {};
			}
		};

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

		constexpr const char* getEncodingName(Encoding e);

#ifndef _WIN32

		template<> GetStringFuncT<CharType<Encoding::Native>> String<Encoding::Native>::Get;
		template<> PutStringFuncT<CharType<Encoding::Native>> String<Encoding::Native>::Put;
		template<> ReleaseStringFuncT<CharType<Encoding::Native>> String<Encoding::Native>::Release;
		template<> const std::string String<Encoding::Native>::GetFName;
		template<> const std::string String<Encoding::Native>::PutFName;

		template<> GetStringFuncT<CharType<Encoding::Byte>> String<Encoding::Byte>::Get;
		template<> PutStringFuncT<CharType<Encoding::Byte>> String<Encoding::Byte>::Put;
		template<> ReleaseStringFuncT<CharType<Encoding::Byte>> String<Encoding::Byte>::Release;

		template<> GetStringFuncT<CharType<Encoding::UTF8>> String<Encoding::UTF8>::Get;
		template<> PutStringFuncT<CharType<Encoding::UTF8>> String<Encoding::UTF8>::Put;
		template<> ReleaseStringFuncT<CharType<Encoding::UTF8>> String<Encoding::UTF8>::Release;

		template<> GetStringFuncT<CharType<Encoding::UTF8Strict>> String<Encoding::UTF8Strict>::Get;
		template<> PutStringFuncT<CharType<Encoding::UTF8Strict>> String<Encoding::UTF8Strict>::Put;
		template<> ReleaseStringFuncT<CharType<Encoding::UTF8Strict>> String<Encoding::UTF8Strict>::Release;

		template<> GetStringFuncT<CharType<Encoding::UTF16>> String<Encoding::UTF16>::Get;
		template<> PutStringFuncT<CharType<Encoding::UTF16>> String<Encoding::UTF16>::Put;
		template<> ReleaseStringFuncT<CharType<Encoding::UTF16>> String<Encoding::UTF16>::Release;

		template<> GetStringFuncT<CharType<Encoding::UCS2>> String<Encoding::UCS2>::Get;
		template<> PutStringFuncT<CharType<Encoding::UCS2>> String<Encoding::UCS2>::Put;
		template<> ReleaseStringFuncT<CharType<Encoding::UCS2>> String<Encoding::UCS2>::Release;

		template<> GetStringFuncT<CharType<Encoding::UTF32>> String<Encoding::UTF32>::Get;
		template<> PutStringFuncT<CharType<Encoding::UTF32>> String<Encoding::UTF32>::Put;
		template<> ReleaseStringFuncT<CharType<Encoding::UTF32>> String<Encoding::UTF32>::Release;
		template<> const std::string String<Encoding::UTF32>::GetFName;
		template<> const std::string String<Encoding::UTF32>::PutFName;

#else


#endif

	} /* namespace ML */

} /* namespace LibraryLinkUtils */

#endif /* LLUTILS_ML_ENCODINGS_H_ */
