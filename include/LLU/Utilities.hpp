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
#include <variant>

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
	 *
	 * @tparam VariantType
	 * @tparam T
	 * @tparam index
	 * @return
	 * @see     https://stackoverflow.com/questions/52303316/get-index-by-type-in-stdvariant
	 */
	template<typename VariantType, typename T, std::size_t index = 0>
	constexpr std::size_t variant_index() {
		if constexpr (index >= std::variant_size_v<VariantType>) {
			return index;
		} else if(std::is_same_v<std::variant_alternative_t<index, VariantType>, T>) {
			return index;
		} else {
			return variant_index<VariantType, T, index + 1>();
		}
	}


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
				default:
					// In V12.2 MNumericArray_Type_Real16 and MNumericArray_Type_Complex_Real16 have been introduced but they are not supported in the Kernel.
					// We add a default case to avoid compiler warnings.
					return "Undefined";
			}
			return "Undefined";
		}
	}

	/// @cond
	template<typename T>
	struct ImageTypeImpl {
		static constexpr imagedata_t type = MImage_Type_Undef;
	};
	template<>
	struct ImageTypeImpl<int8_t> {
		static constexpr imagedata_t type = MImage_Type_Bit;
	};
	template<>
	struct ImageTypeImpl<uint8_t> {
		static constexpr imagedata_t type = MImage_Type_Bit8;
	};
	template<>
	struct ImageTypeImpl<uint16_t> {
		static constexpr imagedata_t type = MImage_Type_Bit16;
	};
	template<>
	struct ImageTypeImpl<float> {
		static constexpr imagedata_t type = MImage_Type_Real32;
	};
	template<>
	struct ImageTypeImpl<double> {
		static constexpr imagedata_t type = MImage_Type_Real;
	};
	/// @endcond


	/// Utility variable template that matches a C++ type with a corresponding MImage data type
	template<typename T>
	constexpr imagedata_t ImageType = ImageTypeImpl<T>::type;

	/// @cond
	template<typename T>
	struct NumericArrayTypeImpl {
		static constexpr numericarray_data_t type = MNumericArray_Type_Undef;
	};
	template<>
	struct NumericArrayTypeImpl<int8_t> {
		static constexpr numericarray_data_t type = MNumericArray_Type_Bit8;
	};
	template<>
	struct NumericArrayTypeImpl<uint8_t> {
		static constexpr numericarray_data_t type = MNumericArray_Type_UBit8;
	};
	template<>
	struct NumericArrayTypeImpl<int16_t> {
		static constexpr numericarray_data_t type = MNumericArray_Type_Bit16;
	};
	template<>
	struct NumericArrayTypeImpl<uint16_t> {
		static constexpr numericarray_data_t type = MNumericArray_Type_UBit16;
	};
	template<>
	struct NumericArrayTypeImpl<int32_t> {
		static constexpr numericarray_data_t type = MNumericArray_Type_Bit32;
	};
	template<>
	struct NumericArrayTypeImpl<uint32_t> {
		static constexpr numericarray_data_t type = MNumericArray_Type_UBit32;
	};
	template<>
	struct NumericArrayTypeImpl<int64_t> {
		static constexpr numericarray_data_t type = MNumericArray_Type_Bit64;
	};
	template<>
	struct NumericArrayTypeImpl<uint64_t> {
		static constexpr numericarray_data_t type = MNumericArray_Type_UBit64;
	};
	template<>
	struct NumericArrayTypeImpl<float> {
		static constexpr numericarray_data_t type = MNumericArray_Type_Real32;
	};
	template<>
	struct NumericArrayTypeImpl<double> {
		static constexpr numericarray_data_t type = MNumericArray_Type_Real64;
	};
	template<>
	struct NumericArrayTypeImpl<std::complex<float>> {
		static constexpr numericarray_data_t type = MNumericArray_Type_Complex_Real32;
	};
	template<>
	struct NumericArrayTypeImpl<std::complex<double>> {
		static constexpr numericarray_data_t type = MNumericArray_Type_Complex_Real64;
	};
	/// @endcond

	/// Utility structure that matches a C++ type with a corresponding MNumericArray data type
	template<typename T>
	constexpr numericarray_data_t NumericArrayType = NumericArrayTypeImpl<T>::type;

	/// @cond
	template<typename T>
	struct TensorTypeImpl {
		static constexpr mint type = MType_Undef;
	};
	template<>
	struct TensorTypeImpl<mint> {
		static constexpr mint type = MType_Integer;
	};
	template<>
	struct TensorTypeImpl<double> {
		static constexpr mint type = MType_Real;
	};
	template<>
	struct TensorTypeImpl<std::complex<double>> {
		static constexpr mint type = MType_Complex;
	};
	/// @endcond

	/// Utility structure that matches a C++ type with a corresponding MTensor data type
	template<typename T>
	constexpr mint TensorType = TensorTypeImpl<T>::type;

} /* namespace LLU */

#endif /* LLUTILS_UTILITIES_H_ */
