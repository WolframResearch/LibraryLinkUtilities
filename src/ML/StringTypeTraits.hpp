/** 
 * @file	StringTypeTraits.hpp
 * @date	Mar 23, 2018
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @brief	<brief description>
 */
#ifndef LLUTILS_ML_STRINGTYPETRAITS_HPP_
#define LLUTILS_ML_STRINGTYPETRAITS_HPP_

#include <cstdint>

namespace LibraryLinkUtils {

	namespace ML {

		enum class Encoding : std::uint8_t {
			Undefined,
			MMA,
			Byte,
			UTF8,
			UTF8Strict,
			UTF16,
			UCS2,
			UTF32
		};

		template<Encoding E>
		struct CharType;

		template<>
		struct CharType<Encoding::MMA> { using type = char; };
		template<>
		struct CharType<Encoding::Byte> { using type = unsigned char; };
		template<>
		struct CharType<Encoding::UTF8> { using type = unsigned char; };
		template<>
		struct CharType<Encoding::UTF8Strict> { using type = unsigned char; };
		template<>
		struct CharType<Encoding::UTF16> { using type = unsigned short; };
		template<>
		struct CharType<Encoding::UCS2> { using type = unsigned short; };
		template<>
		struct CharType<Encoding::UTF32> { using type = unsigned int; };


		template<Encoding E>
		using CharType_t = typename CharType<E>::type;

	} /* namespace ML */

} /* namespace LibraryLinkUtils */

#endif /* LLUTILS_ML_STRINGTYPETRAITS_HPP_ */
