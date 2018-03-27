/** 
 * @file	StringTypeTraits.hpp
 * @date	Mar 23, 2018
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @brief	<brief description>
 */
#ifndef LLUTILS_ML_STRINGTYPETRAITS_HPP_
#define LLUTILS_ML_STRINGTYPETRAITS_HPP_

#include <cstdint>
#include <string>

namespace LibraryLinkUtils {

	namespace ML {

		enum class Encoding : std::uint8_t {
			Undefined,
			Native,
			Byte,
			UTF8,
			UTF8Strict,
			UTF16,
			UCS2,
			UTF32
		};

		template<Encoding E>
		struct CharTypeStruct {
			static_assert(E != Encoding::Undefined, "Trying to deduce character type for undefined encoding");
			using type = char;
		};

		template<> struct CharTypeStruct<Encoding::Native> 		{ using type = char; };
		template<> struct CharTypeStruct<Encoding::Byte> 		{ using type = unsigned char; };
		template<> struct CharTypeStruct<Encoding::UTF8> 		{ using type = unsigned char; };
		template<> struct CharTypeStruct<Encoding::UTF8Strict> 	{ using type = unsigned char; };
		template<> struct CharTypeStruct<Encoding::UTF16> 		{ using type = unsigned short; };
		template<> struct CharTypeStruct<Encoding::UCS2> 		{ using type = unsigned short; };
		template<> struct CharTypeStruct<Encoding::UTF32> 		{ using type = unsigned int; };

		template<Encoding E>
		using CharType = typename CharTypeStruct<E>::type;

		template<Encoding E, typename T>
		constexpr bool CharacterTypesCompatible = (sizeof(T) == sizeof(CharType<E>));

		template<Encoding E>
		struct StringTypeStruct {
			using type = std::basic_string<CharType<E>>;
		};

		template<Encoding E>
		using StringType = typename StringTypeStruct<E>::type;


		/**
		 *	@struct PutAs
		 *	@brief	Utility structure used to enforce sending given string as UTF8 string via MathLink
		 */
		template<Encoding E, typename T>
		struct PutAs {

			/**
			 * @brief	Take a _const char*_ to be sent with MLPutUTF8String.
			 * @param 	s - C-style string encoded in UTF8
			 *
			 * @warning	You are entirely responsible for making sure that the string is actually UTF8 encoded!
			 */
			explicit PutAs(const T& o) : obj(o) {}

			explicit PutAs(T&& o) : obj(std::move(o)) {}

			/// String data after casting to a type that MLPutUTF8String accepts
			const T& obj;
		};

		template<Encoding E, typename T>
		PutAs<E, T> putAs(const T& obj) {
			return PutAs<E, T>(obj);
		}

		/**
		 *	@struct GetAsUTF8
		 *	@brief	Utility structure to facilitate reading UTF8 strings into std::string
		 */
		template<Encoding E, typename T>
		struct GetAs {

			/**
			 * @brief	Read a string from MathLink with MLGetUTF8String and assign to std::string
			 * @param 	s - reference to a string that will be read from MathLink
			 */
			explicit GetAs(T& o) : obj(o) {}

			/// Store a reference to which later a UTF8 string from MathLink will be assigned
			T& obj;
		};

		template<Encoding E, typename T>
		GetAs<E, T> getAs(T& obj) {
			return GetAs<E, T>(obj);
		}
	} /* namespace ML */

} /* namespace LibraryLinkUtils */

#endif /* LLUTILS_ML_STRINGTYPETRAITS_HPP_ */
