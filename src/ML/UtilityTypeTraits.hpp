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

#include "../Utilities.hpp"

namespace LibraryLinkUtils {

	namespace ML {

		template<typename T>
		struct IsSupportedInMLArithmetic : std::false_type {};

		template<> struct IsSupportedInMLArithmetic<unsigned char> : std::true_type {};
		template<> struct IsSupportedInMLArithmetic<short> : std::true_type {};
		template<> struct IsSupportedInMLArithmetic<int> : std::true_type {};
		template<> struct IsSupportedInMLArithmetic<mlint64> : std::true_type {};
		template<> struct IsSupportedInMLArithmetic<float> : std::true_type {};
		template<> struct IsSupportedInMLArithmetic<double> : std::true_type {};


		template<typename T, typename U>
		using ScalarSupportedTypeQ = std::enable_if_t<IsSupportedInMLArithmetic<remove_cv_ref<T>>::value, U>;

		template<typename T, typename U>
		using NotScalarSupportedTypeQ = std::enable_if_t<!IsSupportedInMLArithmetic<remove_cv_ref<T>>::value, U>;

		template<typename T, typename U>
		using ScalarNotSupportedTypeQ = std::enable_if_t<std::is_arithmetic<T>::value && !IsSupportedInMLArithmetic<remove_cv_ref<T>>::value, U>;

		template<typename T>
		struct IsSupportedInMLString : std::false_type {};

		template<> struct IsSupportedInMLString<char> : std::true_type {};
		template<> struct IsSupportedInMLString<unsigned char> : std::true_type {};
		template<> struct IsSupportedInMLString<unsigned short> : std::true_type {};
		template<> struct IsSupportedInMLString<unsigned int> : std::true_type {};


		template<typename T, typename U>
		using StringTypeQ = std::enable_if_t<IsSupportedInMLString<remove_cv_ref<T>>::value, U>;

	}
}

#endif /* LLUTILS_ML_UTILITYTYPETRAITS_HPP_ */
