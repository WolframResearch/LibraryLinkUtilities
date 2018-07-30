/** 
 * @file	RawArray.cpp
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @date	18/04/2017
 *
 * @brief	Template specializations of certain RawArray methods for all underlying data types that we want to support
 *
 */

#include "LLU/Containers/RawArray.h"

#include <complex>
#include <cstdint>

namespace LibraryLinkUtils {

	template<>
	const rawarray_t RawArray<uint8_t>::type = MRawArray_Type_Ubit8;

	template<>
	const rawarray_t RawArray<int8_t>::type = MRawArray_Type_Bit8;

	template<>
	const rawarray_t RawArray<uint16_t>::type = MRawArray_Type_Ubit16;

	template<>
	const rawarray_t RawArray<int16_t>::type = MRawArray_Type_Bit16;

	template<>
	const rawarray_t RawArray<uint32_t>::type = MRawArray_Type_Ubit32;

	template<>
	const rawarray_t RawArray<int32_t>::type = MRawArray_Type_Bit32;

	template<>
	const rawarray_t RawArray<uint64_t>::type = MRawArray_Type_Ubit64;

	template<>
	const rawarray_t RawArray<int64_t>::type = MRawArray_Type_Bit64;

	template<>
	const rawarray_t RawArray<float>::type = MRawArray_Type_Real32;

	template<>
	const rawarray_t RawArray<double>::type = MRawArray_Type_Real64;

	template<>
	const rawarray_t RawArray<std::complex<float>>::type = MRawArray_Type_Float_Complex;

	template<>
	const rawarray_t RawArray<std::complex<double>>::type = MRawArray_Type_Double_Complex;

} /* namespace LibraryLinkUtils */
