/**
 * @file	Utilities.hpp
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @date	8/07/2017
 *
 * @brief	Short but generally useful functions
 *
 */
#ifndef LLUTILS_UTILITIES_H_
#define LLUTILS_UTILITIES_H_

#include <complex>
#include <cstdint>
#include <type_traits>
#include <utility>

#include "LLU/LibraryData.h"

namespace LLU {

	/**
	 * @brief 	Utility type that strips any given type from reference and cv qualifiers
	 * @tparam	T - any type
	 */
	template<typename T>
	using remove_cv_ref = std::remove_cv_t<std::remove_reference_t<T>>;

	/**
	 * @brief 	Utility type that is valid only if B is not A and not a subclass of A
	 * @tparam	A - any type
	 * @tparam	B - any type, will be stripped of reference and cv-qualifiers before comparing with A
	 */
	template<typename A, typename B>
	using disable_if_same_or_derived = typename std::enable_if_t<!std::is_same<A, B>::value && !std::is_base_of<A, remove_cv_ref<B>>::value>;

	/**
	 * @brief 	Utility type that is valid only if B is A or a subclass of A
	 * @tparam	A - any type
	 * @tparam	B - any type, will be stripped of reference and cv-qualifiers before comparing with A
	 */
	template<typename A, typename B>
	using enable_if_same_or_derived = typename std::enable_if_t<std::is_same<A, B>::value || std::is_base_of<A, remove_cv_ref<B>>::value>;

	/**
	 * @brief 	Utility type that checks if given type can be treated as input iterator
	 * @tparam	Iterator - iterator type
	 */
	template<typename Iterator>
	using enable_if_input_iterator = enable_if_same_or_derived<std::input_iterator_tag, typename std::iterator_traits<Iterator>::iterator_category>;

	/**
	 * @brief 	Utility type that checks if given container type has elements that are integers (and therefore can be used as Tensor or NumericArray dimensions)
	 * @tparam	Container - container type
	 */
	template<typename Container>
	using enable_if_integral_elements = typename std::enable_if_t<std::is_integral<typename std::remove_reference_t<Container>::value_type>::value>;

	template<class...>
	using void_t = void;

	template<class...>
	struct conjunction : std::true_type {};
	template<class B1>
	struct conjunction<B1> : B1 {};
	template<class B1, class... Bn>
	struct conjunction<B1, Bn...> : std::conditional_t<bool(B1::value), conjunction<Bn...>, B1> {};

	template<typename Container, typename = void_t<>>
	struct has_value_type : std::false_type {};

	template<typename Container>
	struct has_value_type<Container, void_t<typename Container::value_type>> : std::true_type {};

	template<typename Container, typename T>
	struct has_matching_type : std::is_same<typename Container::value_type, T> {};

	template<typename Container, typename = void_t<>>
	struct has_size : std::false_type {};

	template<typename Container>
	struct has_size<Container, void_t<decltype(std::declval<Container>().size())>> : std::true_type {};

	template<typename Container>
	constexpr bool has_size_v = has_size<Container>::value;

	template<typename Container, typename = void_t<>>
	struct is_iterable : std::false_type {};

	template<typename Container>
	struct is_iterable<Container, void_t<decltype(*std::begin(std::declval<Container>())), decltype(std::end(std::declval<Container>()))>> : std::true_type {};

	template<typename Container, typename T>
	constexpr bool is_iterable_container_with_matching_type_v =
		conjunction<std::is_class<Container>, has_value_type<Container>, is_iterable<Container>, has_matching_type<Container, T>>::value;

	/**
	 * @brief 	Dummy function called on otherwise unused parameters to eliminate compiler warnings.
	 * @tparam 	Ts - variadic template parameter, any number of arbitrary types
	 */
	template<typename... Ts>
	void Unused(Ts&&...) {}

	/**
	 * @brief	Get a type that inherits from false_type and ignores the template parameter completely
	 * @tparam 	T - any type
	 */
	template<typename T>
	struct dependent_false : std::false_type {};

	/**
	 * @brief	Compile-time boolean constant false that "depends" on a template parameter.
	 * Useful utility for static_assert.
	 */
	template<typename T>
	constexpr bool dependent_false_v = dependent_false<T>::value;

	/// Utility structure that matches an MNumericArray data type with corresponding C++ type
	template<numericarray_data_t>
	struct NumericArrayFromEnum;

	/// @cond
	template<>
	struct NumericArrayFromEnum<MNumericArray_Type_Bit8> {
		using type = std::int8_t;
		static constexpr const char* typeName = "Integer8";
	};
	template<>
	struct NumericArrayFromEnum<MNumericArray_Type_UBit8> {
		using type = std::uint8_t;
		static constexpr const char* typeName = "UnsignedInteger8";
	};
	template<>
	struct NumericArrayFromEnum<MNumericArray_Type_Bit16> {
		using type = std::int16_t;
		static constexpr const char* typeName = "Integer16";
	};
	template<>
	struct NumericArrayFromEnum<MNumericArray_Type_UBit16> {
		using type = std::uint16_t;
		static constexpr const char* typeName = "UnsignedInteger16";
	};
	template<>
	struct NumericArrayFromEnum<MNumericArray_Type_Bit32> {
		using type = std::int32_t;
		static constexpr const char* typeName = "Integer32";
	};
	template<>
	struct NumericArrayFromEnum<MNumericArray_Type_UBit32> {
		using type = std::uint32_t;
		static constexpr const char* typeName = "UnsignedInteger32";
	};
	template<>
	struct NumericArrayFromEnum<MNumericArray_Type_Bit64> {
		using type = std::int64_t;
		static constexpr const char* typeName = "Integer64";
	};
	template<>
	struct NumericArrayFromEnum<MNumericArray_Type_UBit64> {
		using type = std::uint64_t;
		static constexpr const char* typeName = "UnsignedInteger64";
	};
	template<>
	struct NumericArrayFromEnum<MNumericArray_Type_Real32> {
		using type = float;
		static constexpr const char* typeName = "Real32";
	};
	template<>
	struct NumericArrayFromEnum<MNumericArray_Type_Real64> {
		using type = double;
		static constexpr const char* typeName = "Real64";
	};
	template<>
	struct NumericArrayFromEnum<MNumericArray_Type_Complex_Real32> {
		using type = std::complex<float>;
		static constexpr const char* typeName = "ComplexReal32";
	};
	template<>
	struct NumericArrayFromEnum<MNumericArray_Type_Complex_Real64> {
		using type = std::complex<double>;
		static constexpr const char* typeName = "ComplexReal64";
	};
	/// @endcond

	/// Simple type alias to easily extract type from NumericArrayFromEnum
	template<numericarray_data_t rat>
	using NumericArrayTypeFromEnum = typename NumericArrayFromEnum<rat>::type;

	/**
	 * @brief Calls an overload of \c f() with template type matching an MNumericArray type \c rat
	 * @param raType - MNumericArray type
	 * @param f - any callable structure
	 * @param args - additional arguments for f::operator()
	 * @warning This function is a prototype, has not been tested yet and is likely to change in the future.
	 */
	template<typename Callable, typename... Args>
	void applyToNumericArray(numericarray_data_t raType, Callable&& f, Args&&... args) {
		switch (raType) {
			case MNumericArray_Type_Bit8:
				std::forward<Callable>(f).template operator()<NumericArrayTypeFromEnum<MNumericArray_Type_Bit8>>(std::forward<Args>(args)...);
				break;
			case MNumericArray_Type_UBit8:
				std::forward<Callable>(f).template operator()<NumericArrayTypeFromEnum<MNumericArray_Type_UBit8>>(std::forward<Args>(args)...);
				break;
			case MNumericArray_Type_Bit16:
				std::forward<Callable>(f).template operator()<NumericArrayTypeFromEnum<MNumericArray_Type_Bit16>>(std::forward<Args>(args)...);
				break;
			case MNumericArray_Type_UBit16:
				std::forward<Callable>(f).template operator()<NumericArrayTypeFromEnum<MNumericArray_Type_UBit16>>(std::forward<Args>(args)...);
				break;
			case MNumericArray_Type_Bit32:
				std::forward<Callable>(f).template operator()<NumericArrayTypeFromEnum<MNumericArray_Type_Bit32>>(std::forward<Args>(args)...);
				break;
			case MNumericArray_Type_UBit32:
				std::forward<Callable>(f).template operator()<NumericArrayTypeFromEnum<MNumericArray_Type_UBit32>>(std::forward<Args>(args)...);
				break;
			case MNumericArray_Type_Bit64:
				std::forward<Callable>(f).template operator()<NumericArrayTypeFromEnum<MNumericArray_Type_Bit64>>(std::forward<Args>(args)...);
				break;
			case MNumericArray_Type_UBit64:
				std::forward<Callable>(f).template operator()<NumericArrayTypeFromEnum<MNumericArray_Type_UBit64>>(std::forward<Args>(args)...);
				break;
			case MNumericArray_Type_Real32:
				std::forward<Callable>(f).template operator()<NumericArrayTypeFromEnum<MNumericArray_Type_Real32>>(std::forward<Args>(args)...);
				break;
			case MNumericArray_Type_Real64:
				std::forward<Callable>(f).template operator()<NumericArrayTypeFromEnum<MNumericArray_Type_Real64>>(std::forward<Args>(args)...);
				break;
			case MNumericArray_Type_Complex_Real32:
				std::forward<Callable>(f).template operator()<NumericArrayTypeFromEnum<MNumericArray_Type_Complex_Real32>>(std::forward<Args>(args)...);
				break;
			case MNumericArray_Type_Complex_Real64:
				std::forward<Callable>(f).template operator()<NumericArrayTypeFromEnum<MNumericArray_Type_Complex_Real64>>(std::forward<Args>(args)...);
				break;
			default: return;
		}
	}

	/// Small namespace for NumericArray related utilities
	namespace NA {
		/**
		 * @brief Possible methods of handling out-of-range data when converting a NumericArray to different type.
		 */
		enum class ConversionMethod {
			Check = MNumericArray_Convert_Check,
			ClipCheck = MNumericArray_Convert_Clip_Check,
			Coerce = MNumericArray_Convert_Coerce,
			ClipCoerce = MNumericArray_Convert_Clip_Coerce,
			Round = MNumericArray_Convert_Round,
			ClipRound = MNumericArray_Convert_Clip_Round,
			Scale = MNumericArray_Convert_Scale,
			ClipScale = MNumericArray_Convert_Clip_Scale,
		};

		inline std::string typeToString(numericarray_data_t t) {
			switch (t) {
				case MNumericArray_Type_Undef: return "Undefined";
				case MNumericArray_Type_Bit8: return NumericArrayFromEnum<MNumericArray_Type_Bit8>::typeName;
				case MNumericArray_Type_UBit8: return NumericArrayFromEnum<MNumericArray_Type_UBit8>::typeName;
				case MNumericArray_Type_Bit16: return NumericArrayFromEnum<MNumericArray_Type_Bit16>::typeName;
				case MNumericArray_Type_UBit16: return NumericArrayFromEnum<MNumericArray_Type_UBit16>::typeName;
				case MNumericArray_Type_Bit32: return NumericArrayFromEnum<MNumericArray_Type_Bit32>::typeName;
				case MNumericArray_Type_UBit32: return NumericArrayFromEnum<MNumericArray_Type_UBit32>::typeName;
				case MNumericArray_Type_Bit64: return NumericArrayFromEnum<MNumericArray_Type_Bit64>::typeName;
				case MNumericArray_Type_UBit64: return NumericArrayFromEnum<MNumericArray_Type_UBit64>::typeName;
				case MNumericArray_Type_Real32: return NumericArrayFromEnum<MNumericArray_Type_Real32>::typeName;
				case MNumericArray_Type_Real64: return NumericArrayFromEnum<MNumericArray_Type_Real64>::typeName;
				case MNumericArray_Type_Complex_Real32: return NumericArrayFromEnum<MNumericArray_Type_Complex_Real32>::typeName;
				case MNumericArray_Type_Complex_Real64: return NumericArrayFromEnum<MNumericArray_Type_Complex_Real64>::typeName;
			}
			return "Undefined";
		}
	}

} /* namespace LLU */

#endif /* LLUTILS_UTILITIES_H_ */
