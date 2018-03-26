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
		struct CharTypeStruct;

		template<>
		struct CharTypeStruct<Encoding::Native> { using type = char; };
		template<>
		struct CharTypeStruct<Encoding::Byte> { using type = unsigned char; };
		template<>
		struct CharTypeStruct<Encoding::UTF8> { using type = unsigned char; };
		template<>
		struct CharTypeStruct<Encoding::UTF8Strict> { using type = unsigned char; };
		template<>
		struct CharTypeStruct<Encoding::UTF16> { using type = unsigned short; };
		template<>
		struct CharTypeStruct<Encoding::UCS2> { using type = unsigned short; };
		template<>
		struct CharTypeStruct<Encoding::UTF32> { using type = unsigned int; };

		template<Encoding E>
		using CharType = typename CharTypeStruct<E>::type;


		template<Encoding E>
		struct StringTypeStruct {
			using type = std::basic_string<CharType<E>>;
		};

		template<Encoding E>
		using StringType = typename StringTypeStruct<E>::type;

	} /* namespace ML */

} /* namespace LibraryLinkUtils */

#endif /* LLUTILS_ML_STRINGTYPETRAITS_HPP_ */
