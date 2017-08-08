/** 
 * @file	Utilities.h
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

#include "WolframLibrary.h"
#include "WolframRawArrayLibrary.h"

namespace LibraryLinkUtils {

	template<typename A, typename B>
	using disable_if_same_or_derived = typename std::enable_if_t<!std::is_base_of<A,typename std::remove_reference_t<B>>::value>;

	template<rawarray_t>
	struct RawArrayFromEnum;

	template<> struct RawArrayFromEnum<MRawArray_Type_Bit8> {
		using type = std::int8_t;
	};
	template<> struct RawArrayFromEnum<MRawArray_Type_Ubit8> {
		using type = std::uint8_t;
	};
	template<> struct RawArrayFromEnum<MRawArray_Type_Bit16> {
		using type = std::int16_t;
	};
	template<> struct RawArrayFromEnum<MRawArray_Type_Ubit16> {
		using type = std::uint16_t;
	};
	template<> struct RawArrayFromEnum<MRawArray_Type_Bit32> {
		using type = std::int32_t;
	};
	template<> struct RawArrayFromEnum<MRawArray_Type_Ubit32> {
		using type = std::uint32_t;
	};
	template<> struct RawArrayFromEnum<MRawArray_Type_Bit64> {
		using type = std::int64_t;
	};
	template<> struct RawArrayFromEnum<MRawArray_Type_Ubit64> {
		using type = std::uint64_t;
	};
	template<> struct RawArrayFromEnum<MRawArray_Type_Real32> {
		using type = float;
	};
	template<> struct RawArrayFromEnum<MRawArray_Type_Real64> {
		using type = double;
	};
	template<> struct RawArrayFromEnum<MRawArray_Type_Float_Complex> {
		using type = std::complex<float>;
	};
	template<> struct RawArrayFromEnum<MRawArray_Type_Double_Complex> {
		using type = std::complex<double>;
	};

	template<rawarray_t rat>
	using RawArrayTypeFromEnum = typename RawArrayFromEnum<rat>::type;

	template<typename Callable, typename ... Args>
	void applyToRawArray(rawarray_t raType, Callable&& f, Args&&... args) {
		switch (raType) {
			case MRawArray_Type_Bit8:
				std::forward<Callable>(f).template operator()<RawArrayTypeFromEnum<MRawArray_Type_Bit8>>(std::forward<Args>(args)...);
				break;
			case MRawArray_Type_Ubit8:
				std::forward<Callable>(f).template operator()<RawArrayTypeFromEnum<MRawArray_Type_Ubit8>>(std::forward<Args>(args)...);
				break;
			case MRawArray_Type_Bit16:
				std::forward<Callable>(f).template operator()<RawArrayTypeFromEnum<MRawArray_Type_Bit16>>(std::forward<Args>(args)...);
				break;
			case MRawArray_Type_Ubit16:
				std::forward<Callable>(f).template operator()<RawArrayTypeFromEnum<MRawArray_Type_Ubit16>>(std::forward<Args>(args)...);
				break;
			case MRawArray_Type_Bit32:
				std::forward<Callable>(f).template operator()<RawArrayTypeFromEnum<MRawArray_Type_Bit32>>(std::forward<Args>(args)...);
				break;
			case MRawArray_Type_Ubit32:
				std::forward<Callable>(f).template operator()<RawArrayTypeFromEnum<MRawArray_Type_Ubit32>>(std::forward<Args>(args)...);
				break;
			case MRawArray_Type_Bit64:
				std::forward<Callable>(f).template operator()<RawArrayTypeFromEnum<MRawArray_Type_Bit64>>(std::forward<Args>(args)...);
				break;
			case MRawArray_Type_Ubit64:
				std::forward<Callable>(f).template operator()<RawArrayTypeFromEnum<MRawArray_Type_Ubit64>>(std::forward<Args>(args)...);
				break;
			case MRawArray_Type_Real32:
				std::forward<Callable>(f).template operator()<RawArrayTypeFromEnum<MRawArray_Type_Real32>>(std::forward<Args>(args)...);
				break;
			case MRawArray_Type_Real64:
				std::forward<Callable>(f).template operator()<RawArrayTypeFromEnum<MRawArray_Type_Real64>>(std::forward<Args>(args)...);
				break;
			case MRawArray_Type_Float_Complex:
				std::forward<Callable>(f).template operator()<RawArrayTypeFromEnum<MRawArray_Type_Float_Complex>>(std::forward<Args>(args)...);
				break;
			case MRawArray_Type_Double_Complex:
				std::forward<Callable>(f).template operator()<RawArrayTypeFromEnum<MRawArray_Type_Double_Complex>>(std::forward<Args>(args)...);
				break;
			default:
				return;
		}
	}

} /* namespace LibraryLinkUtils */

#endif /* LLUTILS_UTILITIES_H_ */
