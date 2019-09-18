/**
 * @file	NumericArray.h
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @date	6/07/2017
 *
 * @brief	Templated C++ wrapper for MNumericArray
 *
 */
#ifndef LLUTILS_NUMERICARRAY_H_
#define LLUTILS_NUMERICARRAY_H_

#include <type_traits>
#include <initializer_list>

#include "WolframLibrary.h"
#include "WolframNumericArrayLibrary.h"

#include "LLU/Containers/MArray.hpp"
#include "LLU/Containers/Passing/PassingPolicy.hpp"
#include "LLU/Containers/Passing/Automatic.hpp"
#include "LLU/Containers/Passing/Manual.hpp"
#include "LLU/Containers/Generic/NumericArray.hpp"
#include "LLU/Utilities.hpp"

namespace LLU {

	/**
	 *  @brief  Typed interface for NumericArray.
	 *
	 *  Provides iterators, data access and info about dimensions.
	 *  @tparam T - type of data in NumericArray
	 */
    template<typename T>
    class TypedNumericArray : public MArray<T> {
    public:
        using MArray<T>::MArray;

        /**
         *   @brief	Return matching type of MNumericArray
         **/
        static numericarray_data_t getType() noexcept {
            return type;
        }

    private:
        /**
         *   @brief Return raw pointer to underlying data
         **/
        T* getData() const noexcept override {
            return static_cast<T*>(LibraryData::NumericArrayAPI()->MNumericArray_getData(this->getInternal()));
        }

        virtual MNumericArray getInternal() const = 0;

        /// NumericArray data type matching template parameter T
        static const numericarray_data_t type;
    };

	/**
	 * @class NumericArray
	 * @brief This is a class template, where template parameter T is the type of data elements. NumericArray is derived from MArray.
	 *
	 * NumericArray<> classes automate creation and deletion of MNumericArrays.
	 * They are strongly typed (no void* to underlying memory) and almost all functions from \<algorithms\> can be used on NumericArray.
	 *
	 * @tparam	T - type of underlying data
	 */
	template<typename T, class PassingMode = Passing::Manual>
	class NumericArray: public TypedNumericArray<T>, public GenericNumericArray<PassingMode> {
	public:

		/**
		 *   @brief         Constructs flat NumericArray based on a list of elements
		 *   @param[in]     v - initializer list with NumericArray elements
		 *   @throws		see NumericArray<T>::NumericArray(InputIt, InputIt, std::initializer_list<mint>)
		 *
		 *   @warning		It is user's responsibility to make sure that length of v fits into mint!
		 **/
		NumericArray(std::initializer_list<T> v);

		/**
		 * @brief   Constructs flat NumericArray with contents copied from a given collection of data
		 * @tparam  Container - any iterable (begin(), end()) collection of data that has a \c value_type alias member and a size() member function
		 * @param   c - const reference to a collection from which data will be copied to the NumericArray
		 */
		template<class Container, typename = std::enable_if_t<is_iterable_container_with_matching_type_v<Container, T> && has_size_v<Container>>>
		explicit NumericArray(const Container &c) : NumericArray(c, {static_cast<mint>(c.size())}) {}

		/**
		 * @brief   Constructs a NumericArray with contents copied from a given collection of data and dimensions passed as parameter
		 * @tparam  Container  - any iterable (begin(), end()) collection of data that has a \c value_type alias member
		 * @param   c - const reference to a collection from which data will be copied to the NumericArray
		 * @param   dims - dimensions of the NumericArray
		 */
		template<class Container, typename = std::enable_if_t<is_iterable_container_with_matching_type_v<Container, T>>>
		NumericArray(const Container &c, MArrayDimensions dims) : NumericArray(std::begin(c), std::end(c), std::move(dims)) {}
		
		/**
		 *   @brief         Constructs flat NumericArray with elements from range [first, last)
		 *   @param[in]     first - iterator to the beginning of range
		 *   @param[in]		last - iterator past the end of range
		 *   @tparam		InputIt - any iterator conforming to InputIterator concept
		 *   @throws		see NumericArray<T>::NumericArray(InputIt, InputIt, std::initializer_list<mint>)
		 *
		 *   @warning		It is user's responsibility to make sure that length of range fits into mint!
		 *   @note			Be aware that efficiency of this constructor strongly depends on whether the InputIt is also a RandomAccessIterator
		 **/
		template<class InputIt, typename = enable_if_input_iterator<InputIt>>
		NumericArray(InputIt first, InputIt last);

		/**
		 *   @brief         Constructs the NumericArray of given shape with all elements initialized to given value
		 *   @param[in]     init - value of type \b T to initialize all elements of the NumericArray
		 *   @param[in]     dims - container with NumericArray dimensions
		 **/
		NumericArray(T init, MArrayDimensions dims);

		/**
		 *   @brief         Constructs the NumericArray of given shape with elements from range [first, last)
		 *   @param[in]     first - iterator to the beginning of range
		 *   @param[in]		last - iterator past the end of range
		 *   @param[in]     dims - container with NumericArray dimensions
		 *   @tparam		Container - any type of container that has member \b value_type and this type is convertible to mint
		 *   @throws		ErrorName::NumericArrayNewError - if number of elements in \c v does not match total NumericArray size indicated by \c dims
		 *   @throws		see NumericArray<T>::createInternal() and MArray<T>::MArray(Container&&)
		 **/
		template<class InputIt, typename = enable_if_input_iterator<InputIt>>
		NumericArray(InputIt first, InputIt last, MArrayDimensions dims);

		/**
		 *
		 * @param na
		 */
		explicit NumericArray(MNumericArray na);

		/**
		 *   @brief     Create new NumericArray from a GenericNumericArray
		 *   @param[in] na - generic NumericArray to be wrapped into NumericArray class
		 *   @throws	ErrorName::NumericArrayTypeError - if the NumericArray template type \b T does not match the actual data type of the generic NumericArray
		 **/
		explicit NumericArray(GenericNumericArray<PassingMode> na);

		/**
		 *   @brief         Create NumericArray from generic NumericArray
		 *   @param[in]     other - const reference to a generic NumericArray
		 *   @param[in]		method - conversion method to be used
		 *   @param[in]     param - conversion tolerance
		 **/
		template<class P>
		explicit NumericArray(const GenericNumericArray<P>& other, NA::ConversionMethod method = NA::ConversionMethod::ClipRound, double param = 0.0);

		/**
		 * Default constructor, creates a "hollow" NumericArray that does not have underlying MNumericArray
		 */
		NumericArray() = default;

		/**
		 *   @brief        	Copy constructor
		 *   @param[in]     other - const reference to a NumericArray of matching type
		 **/
		NumericArray(const NumericArray& other) = default;

		/**
		 *   @brief         Move constructor
		 *   @param[in]     other - rvalue reference to a NumericArray of matching type
		 **/
		NumericArray(NumericArray &&other) noexcept = default;

		/**
		 *    @brief	Free internal MNumericArray if necessary
		 **/
		~NumericArray() = default;

		/// Default copy-assignment operator
		NumericArray& operator=(const NumericArray&) = default;

		/// Default move-assignment operator
		NumericArray& operator=(NumericArray&&) noexcept = default;

		/**
		 *   @brief         Copy-assignment operator with passing mode change
		 *   @param[in]     other - const reference to a NumericArray of matching type
		 **/
        template<class P>
		NumericArray& operator=(const NumericArray<T, P>& other) {
            TypedNumericArray<T>::operator=(other);
			GenericBase::operator=(other);
            return *this;
		}

	private:
		using GenericBase = GenericNumericArray<PassingMode>;

        MNumericArray getInternal() const noexcept override {
            return this->getContainer();
        }
	};


	template<typename T, class PassingMode>
	NumericArray<T, PassingMode>::NumericArray(std::initializer_list<T> v) :
			NumericArray(std::begin(v), std::end(v), { static_cast<mint>(v.size()) }) {
	}

	template<typename T, class PassingMode>
	template<class InputIt, typename>
	NumericArray<T, PassingMode>::NumericArray(InputIt first, InputIt last) :
			NumericArray(first, last, { static_cast<mint>(std::distance(first, last)) }) {
	}

	template<typename T, class PassingMode>
	NumericArray<T, PassingMode>::NumericArray(T init, MArrayDimensions dims) : TypedNumericArray<T>(std::move(dims)),
	        GenericBase(TypedNumericArray<T>::getType(), this->rank(), this->dims.data()) {
		std::fill(this->begin(), this->end(), init);
	}

	template<typename T, class PassingMode>
	template<class InputIt, typename>
	NumericArray<T, PassingMode>::NumericArray(InputIt first, InputIt last, MArrayDimensions dims) : TypedNumericArray<T>(std::move(dims)),
			GenericBase(TypedNumericArray<T>::getType(), this->rank(), this->dims.data()) {
		if (std::distance(first, last) != this->dims.flatCount())
			ErrorManager::throwException(ErrorName::NumericArrayNewError, "Length of data range does not match specified dimensions");
		std::copy(first, last, this->begin());
	}

	template<typename T, class PassingMode>
	NumericArray<T, PassingMode>::NumericArray(GenericBase na) : TypedNumericArray<T>({na.getDimensions(), na.getRank()}),
			GenericBase(std::move(na)) {
		if (TypedNumericArray<T>::getType() != GenericBase::type())
			ErrorManager::throwException(ErrorName::NumericArrayTypeError);
	}

	template<typename T, class PassingMode>
	NumericArray<T, PassingMode>::NumericArray(MNumericArray na) : NumericArray(GenericBase { na }) {}

	template<typename T, class PassingMode>
	template<class P>
	NumericArray<T, PassingMode>::NumericArray(const GenericNumericArray<P>& other, NA::ConversionMethod method, double param) :
			TypedNumericArray<T>({other.getDimensions(), other.getRank()}), GenericBase(other.convert(this->getType(), method, param)) {
	}

} /* namespace LLU */

#endif /* LLUTILS_NUMERICARRAY_H_ */
