/** 
 * @file	Utilities.hpp
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @date	8/07/2017
 *
 * @brief	Short but generally useful functions
 *
 */
#ifndef LLUTILS_UTILITIES_HPP_
#define LLUTILS_UTILITIES_HPP_

#include <memory>

namespace LibraryLinkUtils {

	/**
	 * @brief 		Utility function to create a new object on the heap and return \c std::unique_ptr pointing to that object
	 *
	 * @tparam		T - type of object that we want to point to
	 * @tparam		Ts - types of parameters forwarded to the object constructor
	 * @param[in]	params - parameters forwarded to the object constructor
	 *
	 * @see			[std::make_unique](http://en.cppreference.com/w/cpp/memory/unique_ptr/make_unique)
	 * @note		This function can be removed when we switch to C++14
	 */
	template<typename T, typename... Ts>
	std::unique_ptr<T> make_unique(Ts&&... params) {
		return std::unique_ptr<T>(new T(std::forward<Ts>(params)...));
	}

} /* namespace LibraryLinkUtils */

#endif /* LLUTILS_UTILITIES_HPP_ */
