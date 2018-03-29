/** 
 * @file	Strings.h
 * @date	Mar 22, 2018
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @brief	Definitions of multiple structures and type aliases related to string handling in MathLink
 */
#ifndef LLUTILS_ML_STRINGS_H_
#define LLUTILS_ML_STRINGS_H_

#include <functional>
#include <memory>

#include "mathlink.h"

#include "LibraryLinkError.h"
#include "EncodingTraits.hpp"

namespace LibraryLinkUtils {

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

		/**
		 * @cond
		 * Explicit specialization of String class for undefined encoding.
		 * Its sole purpose is to trigger nice compilation errors.
		 */
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

		constexpr const char* getEncodingName(Encoding e);

#ifndef _WIN32

/// Macro for declaring specializations of static members for ML::String<Encoding::E>
/// For internal use only.
#define ML_STRING_DECLARE_SPECIALIZATIONS_OF_STATIC_MEMBERS(E) \
	template<> GetStringFuncT<CharType<Encoding::E>> String<Encoding::E>::Get;\
	template<> PutStringFuncT<CharType<Encoding::E>> String<Encoding::E>::Put;\
	template<> ReleaseStringFuncT<CharType<Encoding::E>> String<Encoding::E>::Release;\
	template<> const std::string String<Encoding::E>::GetFName;\
	template<> const std::string String<Encoding::E>::PutFName;

		ML_STRING_DECLARE_SPECIALIZATIONS_OF_STATIC_MEMBERS(Native)
		ML_STRING_DECLARE_SPECIALIZATIONS_OF_STATIC_MEMBERS(Byte)
		ML_STRING_DECLARE_SPECIALIZATIONS_OF_STATIC_MEMBERS(UTF8)
		ML_STRING_DECLARE_SPECIALIZATIONS_OF_STATIC_MEMBERS(UTF16)
		ML_STRING_DECLARE_SPECIALIZATIONS_OF_STATIC_MEMBERS(UCS2)
		ML_STRING_DECLARE_SPECIALIZATIONS_OF_STATIC_MEMBERS(UTF32)

#else


#endif

	} /* namespace ML */

} /* namespace LibraryLinkUtils */

#endif /* LLUTILS_ML_STRINGS_H_ */
