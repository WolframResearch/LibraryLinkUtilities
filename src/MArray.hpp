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

#include "WolframLibrary.h"
#include "WolframImageLibrary.h"
#include "WolframRawArrayLibrary.h"

#include <algorithm>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <limits>
#include <numeric>
#include <type_traits>
#include <vector>

#include "LibraryLinkError.h"

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
	class MArray {

		/// Iterator type
		using iterator = T*;

		/// Constant iterator type
		using const_iterator = const T*;

	public:

		/**
		 *	@brief Default constructor
		 **/
		MArray() = default;

		/**
		 * 	@brief		Constructs uninitialized container with given dimensions
		 *	@param[in]	dims - list of MArray dimensions
		 *	@throws		LLErrorCode::DimensionsError - if \c dims are invalid
		 *	@throws		LLErrorCode::FunctionError - if any of Wolfram*Library structures was not initialized
		 **/
		MArray(std::initializer_list<mint> dims);

		/**
		 * 	@brief		Constructs uninitialized container with given dimensions
		 *	@param[in]	dims - container with MArray dimensions
		 *	@tparam		Container - any type of container that has member \b value_type and this type is convertible to mint
		 *	@throws		LLErrorCode::DimensionsError - if \c dims are invalid
		 *	@throws		LLErrorCode::FunctionError - if any of Wolfram*Library structures was not initialized
		 **/
		template<class Container, typename = typename std::enable_if<std::is_convertible<typename Container::value_type, mint>::value>::type>
		MArray(Container&& dims);


		/**
		 *	@brief Default destructor
		 **/
		virtual ~MArray() = default;

		/**
		 *	@brief Get container rank
		 **/
		mint rank() const noexcept {
			return depth;
		}

		/**
		 *	@brief Get raw pointer to container dimensions
		 **/
		const mint* dimensionsData() const noexcept {
			return dims.data();
		}

		/**
		 *	@brief Get container dimensions in the form of const& to \b std::vector
		 **/
		const std::vector<mint>& dimensions() const noexcept {
			return dims;
		}

		/**
		 *	@brief 		Get single dimension
		 *	@param[in]	dim - index of desired dimension
		 *	@throws		indexError() - if \c dim is out-of-bounds
		 **/
		mint dimension(mint dim) const {
			if (dim >= rank() || dim < 0)
				indexError();
			return dims[dim];
		}

		/**
		 *	@brief Get total number of elements in the container
		 **/
		mint size() const noexcept {
			return flattenedLength;
		}

		/**
		 *	@brief Check whether container is empty
		 **/
		mint empty() const noexcept {
			return flattenedLength == 0;
		}

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

		/**
		 *	@brief 		Pass the container as a result to LibraryLink via MArgument
		 *	@param[out]	res - MArgument that will carry the internal container
		 **/
		void passAsResult(MArgument& res) noexcept {
			passInternal(res);
			arrayOwnerQ = false;
		}

		/**
		 *   @brief         Set Wolfram*Library structures as static members of MArray
		 *   @param[in]     ld - WolframLibraryData
		 *   @note			If you use MArgumentManeger you probably will never have to call this function directly
		 **/
		static void setLibraryData(WolframLibraryData ld) noexcept {
			libData = ld;
			raFuns = ld->rawarrayLibraryFunctions;
			imgFuns = ld->imageLibraryFunctions;
		}

	protected:
		/// Total number of elements in the container
		mint flattenedLength = 0;

		/// Container rank
		mint depth = 0;

		/// Container dimensions
		std::vector<mint> dims;

		/// This helps to convert coordinates \f$ (x_1, \ldots, x_n) \f$ in multidimensional MArray to the corresponding index in a flat list of elements
		std::vector<mint> offsets;

		/// Determines if MArray should free the underlying container
		bool arrayOwnerQ = false;

		static WolframLibraryData libData;
		static WolframRawArrayLibrary_Functions raFuns;
		static WolframImageLibrary_Functions imgFuns;

		/// Populate \c offsets member
		void fillOffsets();
	private:
		/**
		 *	@brief 		Convert coordinates of an element in a multidimensional MArray to the corresponding index in a flat list of elements
		 *	@param[in]	indices - vector with coordinates of desired data element
		 *	@throws		indexError() - if \c indices are out-of-bounds
		 **/
		mint getIndex(const std::vector<mint>& indices) const;

		///  @copydoc MArray::operator[](mint)
		T& at(mint index);

		///  @copydoc MArray::operator[](mint)const
		const T& at(mint index) const;

		/**
		 *	@brief 		Check if container size will fit into \b mint
		 *	@param[in]	v - a container
		 *	@throws		LLErrorCode::DimensionsError - if \c v is too big
		 **/
		template<typename Container>
		mint checkContainerSize(Container&& v) const;

		/**
		 *	@brief 		Check if initializer list size will fit into \b mint
		 *	@param[in]	v - an initializer list
		 *	@throws		LLErrorCode::DimensionsError - if \c v is too big
		 **/
		mint checkContainerSize(std::initializer_list<mint> v) const;

		/**
		 *	@brief	Get raw pointer to underlying data
		 **/
		virtual T* getData() const noexcept = 0;

		/**
		 *   @brief 	Purely virtual method for throwing exception concerning index validity within the container
		 **/
		virtual void indexError() const = 0;

		/**
		 *   @brief 	Throw exception relating to container initialization
		 *   @throws 	LibraryLinkError(LLErrorCode::FunctionError)
		 **/
		virtual void initError() const {
			ErrorManager::throwException(LLErrorCode::FunctionError);
		}

		/**
		 *   @brief 	Throw exception relating to container size
		 *   @throws 	LibraryLinkError(LLErrorCode::DimensionsError)
		 **/
		virtual void sizeError() const {
			ErrorManager::throwException(LLErrorCode::DimensionsError);
		}

		/**
		 *   @brief 	Create internal container
		 **/
		virtual void createInternal() = 0;

		/**
		 *   @brief 	Free internal container
		 **/
		virtual void freeInternal() noexcept = 0;

		/**
		 *   @brief 		Set internal container as result for LibraryLink.
		 *   @param[out]	res - MArgument that will carry the internal container
		 **/
		virtual void passInternal(MArgument& res) noexcept = 0;

		/// Calculate total array length based on current value of dims
		mint totalLengthFromDims() const noexcept;
	};

	template<typename T>
	WolframLibraryData MArray<T>::libData = nullptr;
	template<typename T>
	WolframRawArrayLibrary_Functions MArray<T>::raFuns = nullptr;
	template<typename T>
	WolframImageLibrary_Functions MArray<T>::imgFuns = nullptr;

	template<typename T>
	void MArray<T>::fillOffsets() {
		offsets.assign(rank(), 1);
		if (rank() >= 2) {
			auto dimsIt = dims.crbegin() + 1;
			for (auto it = offsets.rbegin() + 1; it != offsets.rend(); ++it)
				*it = *(it - 1) * (*dimsIt++);
		}
	}

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

	template<typename T>
	mint MArray<T>::getIndex(const std::vector<mint>& indices) const {
		if (indices.size() != static_cast<std::make_unsigned<mint>::type>(rank()))
			indexError();
		mint flatIndex = 0;
		auto dimsIt = dims.cbegin();
		auto offset = offsets.cbegin();
		for (auto idx : indices) {
			if (idx < 0 || idx >= *dimsIt++)
				indexError();
			flatIndex += idx * (*offset++);
		}
		return flatIndex;
	}

	template<typename T>
	MArray<T>::MArray(std::initializer_list<mint> dimensions) {
		if (!libData || !raFuns || !imgFuns)
			initError();
		depth = checkContainerSize(dimensions);
		dims = dimensions;
		flattenedLength = totalLengthFromDims();
		fillOffsets();
	}

	template<typename T>
	template<class Container, typename>
	MArray<T>::MArray(Container&& dimensions) {
		if (!libData || !raFuns || !imgFuns)
			initError();
		depth = checkContainerSize(dimensions);
		auto dimsOk = std::all_of(std::begin(dimensions), std::end(dimensions), [](typename Container::value_type d) {
			return (d > 0) && (d <= std::numeric_limits<mint>::max());
		});
		if (!dimsOk)
			ErrorManager::throwException(LLErrorCode::DimensionsError, "Invalid input vector with array dimensions");
		dims.reserve(depth);
		std::copy(std::begin(dimensions), std::end(dimensions), std::back_inserter(dims));
		flattenedLength = totalLengthFromDims();
		fillOffsets();
	}

	template<typename T>
	template<class Container>
	mint MArray<T>::checkContainerSize(Container&& v) const {
		if (v.size() > std::numeric_limits<mint>::max())
			sizeError();
		return static_cast<mint>(v.size());
	}

	template<typename T>
	mint MArray<T>::checkContainerSize(std::initializer_list<mint> v) const {
		if (v.size() > std::numeric_limits<mint>::max())
			sizeError();
		return static_cast<mint>(v.size());
	}

	template<typename T>
	mint MArray<T>::totalLengthFromDims() const noexcept {
		return std::accumulate(std::begin(dims), std::end(dims), static_cast<mint>(1), std::multiplies<mint>());
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
