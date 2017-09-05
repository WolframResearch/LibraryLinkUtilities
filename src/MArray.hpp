/**
 * @file	MArray.hpp
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @date	6/07/2017
 *
 * @brief	Template base class for C++ wrappers of LibraryLink containers
 *
 */
#ifndef LLUTILS_MARRAY_HPP_
#define LLUTILS_MARRAY_HPP_

#include <iostream>
#include <ostream>
#include <vector>

#include "MArrayBase.h"

namespace LibraryLinkUtils {

	/**
	 * @class MArray
	 * @brief This is a class template, where template parameter T is the type of data elements. MArray is the base class for RawArray, Tensor and Image.
	 *
	 * Each MArray<T> is an abstract class, it provides common interface to RawArrays, Tensors and Images. One of the biggest benefits is that this interface
	 * contains iterators over underlying data together with begin() and end() member functions which makes it possible to use containers derived
	 * from MArray directly in many functions from standard library \<algorithms\>.
	 *
	 * @tparam	T - type of underlying data
	 */
	template<typename T>
	class MArray : public MArrayBase {
	public:
		/// Iterator type
		using iterator = T*;

		/// Constant iterator type
		using const_iterator = const T*;

		/// Type of elements stored
		using value_type = T;

		using MArrayBase::MArrayBase;

		MArray() = default;

		template<typename U>
		MArray(const MArray<U>& ma2) : MArrayBase(ma2) {};

		/**
		 *	@brief Get raw pointer to underlying data
		 **/
		T* data() noexcept {
			return getData();
		}

		/**
		 *	@brief Get raw pointer to const underlying data
		 **/
		const T* data() const noexcept {
			return getData();
		}

		/**
		 *	@brief Get iterator at the beginning of underlying data
		 **/
		iterator begin() noexcept {
			return getData();
		}

		/**
		 *	@brief Get constant iterator at the beginning of underlying data
		 **/
		const_iterator begin() const noexcept {
			return getData();
		}

		/**
		 *	@brief Get constant iterator at the beginning of underlying data
		 **/
		const_iterator cbegin() const noexcept {
			return getData();
		}

		/**
		 *	@brief Get iterator after the end of underlying data
		 **/
		iterator end() noexcept {
			return getData() + size();
		}

		/**
		 *	@brief Get constant iterator after the end of underlying data
		 **/
		const_iterator end() const noexcept {
			return getData() + size();
		}

		/**
		 *	@brief Get constant iterator after the end of underlying data
		 **/
		const_iterator cend() const noexcept {
			return getData() + size();
		}

		/**
		 *	@brief 		Get a reference to the data element at given position
		 *	@param[in]	index - position of desired data element
		 *	@throws		indexError() - if \c index is out-of-bounds
		 **/
		T& operator[](mint index) {
			return at(index);
		}

		/**
		 *	@brief 		Get a constant reference to the data element at given position
		 *	@param[in]	index - position of desired data element
		 *	@throws		indexError() - if \c index is out-of-bounds
		 **/
		const T& operator[](mint index) const {
			return at(index);
		}

		/**
		 *	@brief 		Get a reference to the data element at given position in a multidimensional container
		 *	@param[in]	indices - vector with coordinates of desired data element
		 *	@throws		indexError() - if \c indices are out-of-bounds
		 **/
		T& operator[](const std::vector<mint>& indices) {
			return at(getIndex(indices));
		}

		/**
		 *	@brief 		Get a constant reference to the data element at given position in a multidimensional container
		 *	@param[in]	indices - vector with coordinates of desired data element
		 *	@throws		indexError() - if \c indices are out-of-bounds
		 **/
		const T& operator[](const std::vector<mint>& indices) const {
			return at(getIndex(indices));
		}

	private:
		///  @copydoc MArray::operator[](mint)
		T& at(mint index);

		///  @copydoc MArray::operator[](mint)const
		const T& at(mint index) const;

		/**
		 *	@brief	Get raw pointer to underlying data
		 **/
		virtual T* getData() const noexcept = 0;
	};

	template<typename T>
	T& MArray<T>::at(mint index) {
		if (index >= flattenedLength)
			indexError();
		return *(begin() + index);
	}

	template<typename T>
	const T& MArray<T>::at(mint index) const {
		if (index >= flattenedLength)
			indexError();
		return *(cbegin() + index);
	}

	/**
	 * @brief 		Insertion operator to allow pretty-printing of MArray
	 * @tparam		T - type of elements in the container
	 * @param[out]	os - output stream
	 * @param[in]	c - const& to the MArray we want to print
	 */
	template<typename T>
	std::ostream& operator<<(std::ostream& os, const MArray<T>& c) {
		os << "{ ";
		for (auto elem : c)
			os << elem << " ";
		os << "}";
		return os;
	}

} /* namespace LibraryLinkUtils */


#endif /* LLUTILS_MARRAY_HPP_ */
