/**
 * @file	UtilityTypeTraits.hpp
 * @date	Feb 7, 2018
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @brief	Type traits used by WSStream to identify types supported by WSTP
 */
#ifndef LLU_WSTP_UTILITYTYPETRAITS_HPP_
#define LLU_WSTP_UTILITYTYPETRAITS_HPP_

#include <type_traits>

#include "wstp.h"

#include "LLU/Utilities.hpp"

namespace LLU::WS {

	/**
	 * @struct 	IsSupportedInWSArithmetic
	 * @tparam	T - any type
	 * @brief	Utility trait class that determines whether type T is a suitable data type for functions like WSPut*Array, WSGet*List, WSPutScalar, etc.
	 */
	template<typename T>
	inline constexpr bool IsSupportedInWSArithmetic = false;

	/// @cond
	template<>
	inline constexpr bool IsSupportedInWSArithmetic<unsigned char> = true;
	template<>
	inline constexpr bool IsSupportedInWSArithmetic<short> = true;
	template<>
	inline constexpr bool IsSupportedInWSArithmetic<int> = true;
	template<>
	inline constexpr bool IsSupportedInWSArithmetic<wsint64> = true;
	template<>
	inline constexpr bool IsSupportedInWSArithmetic<float> = true;
	template<>
	inline constexpr bool IsSupportedInWSArithmetic<double> = true;
	/// @endcond

	template<typename T>
	constexpr bool ScalarSupportedTypeQ = IsSupportedInWSArithmetic<remove_cv_ref<T>>;

	/**
	 * @struct 	IsSupportedInWSString
	 * @tparam	T - any type
	 * @brief	Utility trait class that determines whether type T is a suitable character type for WSPut*String and WSGet*String
	 */
	template<typename T>
	struct IsSupportedInWSString : std::false_type {};

	/// @cond
	template<>
	struct IsSupportedInWSString<char> : std::true_type {};
	template<>
	struct IsSupportedInWSString<unsigned char> : std::true_type {};
	template<>
	struct IsSupportedInWSString<unsigned short> : std::true_type {};
	template<>
	struct IsSupportedInWSString<unsigned int> : std::true_type {};
	/// @endcond

	template<typename T, typename U>
	using StringTypeQ = std::enable_if_t<IsSupportedInWSString<remove_cv_ref<T>>::value, U>;
}  // namespace LLU::WS

#endif /* LLU_WSTP_UTILITYTYPETRAITS_HPP_ */
