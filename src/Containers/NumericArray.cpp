/** 
 * @file	NumericArray.cpp
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @date	18/04/2017
 *
 * @brief	Template specializations of certain NumericArray methods for all underlying data types that we want to support
 *
 */

#include "LLU/Containers/NumericArray.h"

#include <complex>
#include <cstdint>

namespace LibraryLinkUtils {

	template<>
	const numericarray_data_t NumericArray<uint8_t>::type = MNumericArray_Type_UBit8;

	template<>
	const numericarray_data_t NumericArray<int8_t>::type = MNumericArray_Type_Bit8;

	template<>
	const numericarray_data_t NumericArray<uint16_t>::type = MNumericArray_Type_UBit16;

	template<>
	const numericarray_data_t NumericArray<int16_t>::type = MNumericArray_Type_Bit16;

	template<>
	const numericarray_data_t NumericArray<uint32_t>::type = MNumericArray_Type_UBit32;

	template<>
	const numericarray_data_t NumericArray<int32_t>::type = MNumericArray_Type_Bit32;

	template<>
	const numericarray_data_t NumericArray<uint64_t>::type = MNumericArray_Type_UBit64;

	template<>
	const numericarray_data_t NumericArray<int64_t>::type = MNumericArray_Type_Bit64;

	template<>
	const numericarray_data_t NumericArray<float>::type = MNumericArray_Type_Real32;

	template<>
	const numericarray_data_t NumericArray<double>::type = MNumericArray_Type_Real64;

	template<>
	const numericarray_data_t NumericArray<std::complex<float>>::type = MNumericArray_Type_Complex_Real32;

	template<>
	const numericarray_data_t NumericArray<std::complex<double>>::type = MNumericArray_Type_Complex_Real64;

} /* namespace LibraryLinkUtils */
