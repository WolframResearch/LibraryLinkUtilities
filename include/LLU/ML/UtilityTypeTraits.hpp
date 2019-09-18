/** 
 * @file	UtilityTypeTraits.hpp
 * @date	Feb 7, 2018
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @brief	Type traits used by MathLinkStream to identify types supported by MathLink
 */
#ifndef LLUTILS_ML_UTILITYTYPETRAITS_HPP_
#define LLUTILS_ML_UTILITYTYPETRAITS_HPP_

#include <type_traits>

#include "mathlink.h"

#include "LLU/Utilities.hpp"

namespace LLU {

	namespace ML {

		/**
		 * @struct 	IsSupportedInMLArithmetic
		 * @tparam	T - any type
		 * @brief	Utility trait class that determines whether type T is a suitable data type for functions like MLPut*Array, MLGet*List, MLPutScalar, etc.
		 */
		template<typename T>
		struct IsSupportedInMLArithmetic : std::false_type {};

		/// @cond
		template<> struct IsSupportedInMLArithmetic<unsigned char> : std::true_type {};
		template<> struct IsSupportedInMLArithmetic<short> : std::true_type {};
		template<> struct IsSupportedInMLArithmetic<int> : std::true_type {};
		template<> struct IsSupportedInMLArithmetic<mlint64> : std::true_type {};
		template<> struct IsSupportedInMLArithmetic<float> : std::true_type {};
		template<> struct IsSupportedInMLArithmetic<double> : std::true_type {};
		/// @endcond

        template<typename T>
        constexpr bool scalarSupportedTypeQ = IsSupportedInMLArithmetic<remove_cv_ref<T>>::value;

		template<typename T, typename U>
		using ScalarSupportedTypeQ = std::enable_if_t<scalarSupportedTypeQ<T>, U>;

		template<typename T, typename U>
		using NotScalarSupportedTypeQ = std::enable_if_t<!scalarSupportedTypeQ<T>, U>;

		template<typename T, typename U>
		using ScalarNotSupportedTypeQ = std::enable_if_t<std::is_arithmetic_v<T> && !scalarSupportedTypeQ<T>, U>;

		/**
		 * @struct 	IsSupportedInMLString
		 * @tparam	T - any type
		 * @brief	Utility trait class that determines whether type T is a suitable character type for MLPut*String and MLGet*String
		 */
		template<typename T>
		struct IsSupportedInMLString : std::false_type {};

		/// @cond
		template<> struct IsSupportedInMLString<char> : std::true_type {};
		template<> struct IsSupportedInMLString<unsigned char> : std::true_type {};
		template<> struct IsSupportedInMLString<unsigned short> : std::true_type {};
		template<> struct IsSupportedInMLString<unsigned int> : std::true_type {};
		/// @endcond

		template<typename T, typename U>
		using StringTypeQ = std::enable_if_t<IsSupportedInMLString<remove_cv_ref<T>>::value, U>;
	}
}

#endif /* LLUTILS_ML_UTILITYTYPETRAITS_HPP_ */
