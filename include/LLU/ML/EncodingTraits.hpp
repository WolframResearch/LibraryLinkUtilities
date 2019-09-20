/**
 * @file	EncodingTraits.hpp
 * @date	Mar 23, 2018
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @brief	Defines possible encodings together with their properties and related types.
 */
#ifndef LLUTILS_ML_ENCODINGTRAITS_HPP_
#define LLUTILS_ML_ENCODINGTRAITS_HPP_

#include <cstdint>
#include <string>
#include <type_traits>

namespace LLU {

	namespace ML {

		/**
		 * @enum Encoding
		 * List of all string encodings supported by MathLink
		 */
		enum class Encoding : std::uint8_t {
			Undefined,	  //!< Undefined, can be used to denote that certain function is not supposed to deal with strings
			Native,		  //!< Use MLGetString for reading and MLPutString for writing strings
			Byte,		  //!< Use MLGetByteString for reading and MLPutByteString for writing strings
			UTF8,		  //!< Use MLGetUTF8String for reading and MLPutUTF8String for writing strings
			UTF16,		  //!< Use MLGetUTF16String for reading and MLPutUTF16String for writing strings
			UCS2,		  //!< Use MLGetUCS2String for reading and MLPutUCS2String for writing strings
			UTF32		  //!< Use MLGetUTF32String for reading and MLPutUTF32String for writing strings
		};

		/**
		 * @struct CharTypeStruct
		 * @tparam E - encoding
		 *
		 * Defines which character type is bound to encoding E
		 */
		template<Encoding E>
		struct CharTypeStruct {
			static_assert(E != Encoding::Undefined, "Trying to deduce character type for undefined encoding");
			/// static_assert will trigger compilation error, we add a dummy type to prevent further compiler errors
			using type = char;
		};

		/**
		 * Specializations of CharTypeStruct, encoding E has assigned type T iff MLPutEString takes const T* as second parameter
		 * @cond
		 */
		template<>
		struct CharTypeStruct<Encoding::Native> {
			using type = char;
		};
		template<>
		struct CharTypeStruct<Encoding::Byte> {
			using type = unsigned char;
		};
		template<>
		struct CharTypeStruct<Encoding::UTF8> {
			using type = unsigned char;
		};
		template<>
		struct CharTypeStruct<Encoding::UTF16> {
			using type = unsigned short;
		};
		template<>
		struct CharTypeStruct<Encoding::UCS2> {
			using type = unsigned short;
		};
		template<>
		struct CharTypeStruct<Encoding::UTF32> {
			using type = unsigned int;
		};
		/// @endcond

		/**
		 * @typedef CharType
		 * @tparam 	E - encoding
		 *
		 * Utility alias for type member of CharTypeStruct
		 */
		template<Encoding E>
		using CharType = typename CharTypeStruct<E>::type;

		/**
		 * @brief Check whether character type T is compatible with encoding E (i.e. if it can be more or less safely used with MLPut/Get\<E\>String)
		 *
		 * Each MathLink function MLPut/Get*String works with only one specific character type (see CharType<E>). In C++, on the other hand,
		 * there is no notion of encoding so std::string (with character type being \c char) can be used to store strings of any encoding.
		 *
		 * We say that character type T is compatible with encoding E as long as the size of T is the same as size of CharType<E>.
		 */
		template<Encoding E, typename T>
		constexpr bool CharacterTypesCompatible = (sizeof(T) == sizeof(CharType<E>));

		/**
		 * @struct StringTypeStruct
		 * @tparam E - encoding
		 *
		 * Defines which basic_string instantiation naturally corresponds to encoding E
		 */
		template<Encoding E>
		struct StringTypeStruct {
			using type = std::basic_string<CharType<E>>;
		};

		/**
		 * @typedef StringType
		 * @tparam 	E - encoding
		 *
		 * Utility alias for type member of StringTypeStruct
		 */
		template<Encoding E>
		using StringType = typename StringTypeStruct<E>::type;

		/**
		 * Get the name of encoding
		 * @param e - value of ML::Encoding enum
		 * @return C-string containing the name of encoding e
		 */
		constexpr const char* getEncodingName(Encoding e) {
			switch (e) {
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

		/**
		 *	@struct PutAs
		 *	@tparam E - desired encoding
		 *	@tparam T - actual type of the expression
		 *
		 *	@brief	Utility structure used to enforce sending given value with encoding E via MLStream.
		 *
		 *	This structure is only supposed to be used as a wrapper for arguments to MLStream::operator<<
		 *
		 *	@note	It's recommended not to use ML::PutAs constructor directly, but rather utilize a helper function ML::putAs.
		 *
		 *	@code
		 *		MLStream<ML::Encoding::Byte> mls { mlink }; // mls will send all strings as if they were ascii-encoded by default
		 *		std::string stringWithNonAsciiChars = ...;  // oops, now we have to use different encoding
		 *		mls << ML::putAs<ML::Encoding::UTF8>(stringWithNonAsciiChars);   	// this will use UTF8 encoding when sending the string
		 *	@endcode
		 */
		template<Encoding E, typename T, bool = std::is_lvalue_reference<T>::value>
		struct PutAs;

		/**
		 * @cond
		 * Explicit specialization of ML::PutAs for lvalues
		 */
		template<Encoding E, typename T>
		struct PutAs<E, T, true> {

			/**
			 * Constructor that takes a const& and stores it as a data member
			 * @param o - reference to const of type T
			 */
			explicit PutAs(const T& o) : obj(o) {}

			/// Reference to an object which will be later passed to appropriate MLStream::operator<<
			const T& obj;
		};

		/**
		 * Explicit specialization of ML::PutAs for rvalues
		 */
		template<Encoding E, typename T>
		struct PutAs<E, T, false> {

			/**
			 * Constructor that takes a temporary and moves it to a data member
			 * @param o - rvalue reference to temporary object of type T
			 */
			explicit PutAs(T&& o) : obj(std::move(o)) {}

			/// An object which will be later passed to appropriate MLStream::operator<<
			T obj;
		};
		/// @endcond

		/**
		 * This is a helper function to facilitate constructing ML::PutAs wrapper.
		 * @param obj - forwarding reference to object of type T
		 * @return a PutAs wrapper
		 */
		template<Encoding E, typename T>
		PutAs<E, T> putAs(T&& obj) {
			return PutAs<E, T>(std::forward<T>(obj));
		}

		/**
		 *	@struct GetAs
		 *	@tparam E - desired encoding
		 *	@tparam T - actual type of the expression
		 *
		 *	@brief	Utility structure used to enforce receiving given value via MLStream with encoding E.
		 *
		 *	This structure is only supposed to be used as a wrapper for arguments to MLStream::operator>>
		 *
		 *	@note	It's recommended not to use ML::GetAs constructor directly, but rather utilize a helper function ML::getAs.
		 *
		 *	@code
		 *		MLStream<ML::Encoding::Byte> mls { mlink }; // mls will receive all strings as if they were ascii-encoded by default
		 *		std::string stringWithNonAsciiChars;  		// we expect a string with non-ascii characters to come from MathLink
		 *		mls << ML::getAs<ML::Encoding::UTF8>(stringWithNonAsciiChars);   	// this will use UTF8 encoding when receiving the string
		 *	@endcode
		 */
		template<Encoding E, typename T>
		struct GetAs {

			/**
			 * Constructor that takes a reference to value of type T and stores it
			 * @param o - reference to the object to be read from MLStream
			 */
			explicit GetAs(T& o) : obj(o) {}

			/// A reference to which later a resulting value from MathLink function will be assigned
			T& obj;
		};

		/**
		 * This is a helper function to facilitate constructing ML::GetAs wrapper.
		 * @param obj - a reference to value of type T
		 * @return a GetAs wrapper
		 */
		template<Encoding E, typename T>
		GetAs<E, T> getAs(T& obj) {
			return GetAs<E, T>(obj);
		}
	} /* namespace ML */

} /* namespace LLU */

#endif /* LLUTILS_ML_ENCODINGTRAITS_HPP_ */
