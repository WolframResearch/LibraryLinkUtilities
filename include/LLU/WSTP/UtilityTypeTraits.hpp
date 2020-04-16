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

namespace LLU {

	namespace WS {

		/**
		 * @struct 	IsSupportedInWSArithmetic
		 * @tparam	T - any type
		 * @brief	Utility trait class that determines whether type T is a suitable data type for functions like WSPut*Array, WSGet*List, WSPutScalar, etc.
		 */
		template<typename T>
		struct IsSupportedInWSArithmetic : std::false_type {};

		/// @cond
		template<>
		struct IsSupportedInWSArithmetic<unsigned char> : std::true_type {};
		template<>
		struct IsSupportedInWSArithmetic<short> : std::true_type {};
		template<>
		struct IsSupportedInWSArithmetic<int> : std::true_type {};
		template<>
		struct IsSupportedInWSArithmetic<wsint64> : std::true_type {};
		template<>
		struct IsSupportedInWSArithmetic<float> : std::true_type {};
		template<>
		struct IsSupportedInWSArithmetic<double> : std::true_type {};
		/// @endcond

		template<typename T, typename U>
		using ScalarSupportedTypeQ = std::enable_if_t<IsSupportedInWSArithmetic<remove_cv_ref<T>>::value, U>;

		template<typename T, typename U>
		using NotScalarSupportedTypeQ = std::enable_if_t<!IsSupportedInWSArithmetic<remove_cv_ref<T>>::value, U>;

		template<typename T, typename U>
		using ScalarNotSupportedTypeQ = std::enable_if_t<std::is_arithmetic<T>::value && !IsSupportedInWSArithmetic<remove_cv_ref<T>>::value, U>;

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
	}
}

#endif /* LLU_WSTP_UTILITYTYPETRAITS_HPP_ */
