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
#include "StringTypeTraits.hpp"

namespace LibraryLinkUtils {

	namespace ML {

		template<Encoding E>
		struct ReleaseString;

		template<Encoding E>
		using StringData = std::unique_ptr<const CharType_t<E>[], ReleaseString<E>>;


		template<typename T>
		using GetStringFuncT = std::function<int(MLINK, const T**, int*, int*)>;

		template<typename T>
		using PutStringFuncT = std::function<int(MLINK, const T*, int)>;

		template<typename T>
		using ReleaseStringFuncT = std::function<void(MLINK, const T*, int)>;

		template<Encoding E, typename T>
		bool CharacterTypesCompatible = (sizeof(T) == sizeof(CharType_t<E>));

		template<Encoding E>
		struct String {

			using CharT = CharType_t<E>;

			static GetStringFuncT<CharT> Get;
			static PutStringFuncT<CharT> Put;
			static ReleaseStringFuncT<CharT> Release;

			static constexpr const char* GetFName;
			static constexpr const char* PutFName;

			template<typename T>
			static void put(MLINK m, const T* string, int len) {
				static_assert(CharacterTypesCompatible<E, T>, "Encoding vs character type mismatch in ML::String<E>::put");
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
				static_assert(CharacterTypesCompatible<E, T>, "Encoding vs character type mismatch in ML::String<E>::getString");
				using StringType = std::basic_string<T>;

				auto strData { get(m) };

				auto bytes = strData.get_deleter().getLength();
				auto* expectedData = reinterpret_cast<const T*>(strData.get());
				auto strlen = static_cast<typename StringType::size_type>(bytes);

				return (bytes < 0? StringType { expectedData } : StringType { expectedData, strlen });
			}
		};


		template<Encoding E>
		struct ReleaseString {
			ReleaseString() = default;
			ReleaseString(MLINK m, int l, int c) : m(m), length(l), chars(c) {}

			void operator()(const CharType_t<E>* data) {
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

		template<> GetStringFuncT<CharType_t<Encoding::MMA>> String<Encoding::MMA>::Get;
		template<> PutStringFuncT<CharType_t<Encoding::MMA>> String<Encoding::MMA>::Put;
		template<> ReleaseStringFuncT<CharType_t<Encoding::MMA>> String<Encoding::MMA>::Release;
		template<> constexpr const char* String<Encoding::MMA>::GetFName;
		template<> constexpr const char* String<Encoding::MMA>::PutFName;

		template<> GetStringFuncT<CharType_t<Encoding::Byte>> String<Encoding::Byte>::Get;
		template<> PutStringFuncT<CharType_t<Encoding::Byte>> String<Encoding::Byte>::Put;
		template<> ReleaseStringFuncT<CharType_t<Encoding::Byte>> String<Encoding::Byte>::Release;

		template<> GetStringFuncT<CharType_t<Encoding::UTF8>> String<Encoding::UTF8>::Get;
		template<> PutStringFuncT<CharType_t<Encoding::UTF8>> String<Encoding::UTF8>::Put;
		template<> ReleaseStringFuncT<CharType_t<Encoding::UTF8>> String<Encoding::UTF8>::Release;

		template<> GetStringFuncT<CharType_t<Encoding::UTF8Strict>> String<Encoding::UTF8Strict>::Get;
		template<> PutStringFuncT<CharType_t<Encoding::UTF8Strict>> String<Encoding::UTF8Strict>::Put;
		template<> ReleaseStringFuncT<CharType_t<Encoding::UTF8Strict>> String<Encoding::UTF8Strict>::Release;

		template<> GetStringFuncT<CharType_t<Encoding::UTF16>> String<Encoding::UTF16>::Get;
		template<> PutStringFuncT<CharType_t<Encoding::UTF16>> String<Encoding::UTF16>::Put;
		template<> ReleaseStringFuncT<CharType_t<Encoding::UTF16>> String<Encoding::UTF16>::Release;

		template<> GetStringFuncT<CharType_t<Encoding::UCS2>> String<Encoding::UCS2>::Get;
		template<> PutStringFuncT<CharType_t<Encoding::UCS2>> String<Encoding::UCS2>::Put;
		template<> ReleaseStringFuncT<CharType_t<Encoding::UCS2>> String<Encoding::UCS2>::Release;

		template<> GetStringFuncT<CharType_t<Encoding::UTF32>> String<Encoding::UTF32>::Get;
		template<> PutStringFuncT<CharType_t<Encoding::UTF32>> String<Encoding::UTF32>::Put;
		template<> ReleaseStringFuncT<CharType_t<Encoding::UTF32>> String<Encoding::UTF32>::Release;

#else


#endif

	} /* namespace ML */

} /* namespace LibraryLinkUtils */

#endif /* LLUTILS_ML_ENCODINGS_H_ */
