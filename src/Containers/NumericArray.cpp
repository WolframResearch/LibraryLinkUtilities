/** 
 * @file	NumericArray.cpp
 * @author	Rafal Chojna <rafalc@wolfram.com>
 *
 * @brief	Template specializations of certain TypedNumericArray methods for all underlying data types that we want to support
 *
 */

#include "LLU/Containers/NumericArray.h"

#include <complex>
#include <cstdint>

namespace LibraryLinkUtils {

	template<>
	const numericarray_data_t TypedNumericArray<uint8_t>::type = MNumericArray_Type_UBit8;

	template<>
	const numericarray_data_t TypedNumericArray<int8_t>::type = MNumericArray_Type_Bit8;

	template<>
	const numericarray_data_t TypedNumericArray<uint16_t>::type = MNumericArray_Type_UBit16;

	template<>
	const numericarray_data_t TypedNumericArray<int16_t>::type = MNumericArray_Type_Bit16;

	template<>
	const numericarray_data_t TypedNumericArray<uint32_t>::type = MNumericArray_Type_UBit32;

	template<>
	const numericarray_data_t TypedNumericArray<int32_t>::type = MNumericArray_Type_Bit32;

	template<>
	const numericarray_data_t TypedNumericArray<uint64_t>::type = MNumericArray_Type_UBit64;

	template<>
	const numericarray_data_t TypedNumericArray<int64_t>::type = MNumericArray_Type_Bit64;

	template<>
	const numericarray_data_t TypedNumericArray<float>::type = MNumericArray_Type_Real32;

	template<>
	const numericarray_data_t TypedNumericArray<double>::type = MNumericArray_Type_Real64;

	template<>
	const numericarray_data_t TypedNumericArray<std::complex<float>>::type = MNumericArray_Type_Complex_Real32;

	template<>
	const numericarray_data_t TypedNumericArray<std::complex<double>>::type = MNumericArray_Type_Complex_Real64;

} /* namespace LibraryLinkUtils */
