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

        virtual void indexError() const = 0;

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
	class NumericArray: public TypedNumericArray<T>, public MContainer<MArgumentType::NumericArray, PassingMode> {
	public:

		/**
		 *   @brief         Constructs flat NumericArray based on a container of elements
		 *   @param[in]     v - container
		 *   @tparam		Container - any container with elements convertible to type \b T
		 *   @throws		see NumericArray<T>::NumericArray(InputIt, InputIt, std::initializer_list<mint>)
		 *
		 *   @warning		It is user's responsibility to make sure that length of v fits into mint!
		 **/
		template<typename Container, typename = disable_if_same_or_derived<NumericArray, Container>>
		explicit NumericArray(Container&& v);

		/**
		 *   @brief         Constructs flat NumericArray based on a list of elements
		 *   @param[in]     v - initializer list with NumericArray elements
		 *   @throws		see NumericArray<T>::NumericArray(InputIt, InputIt, std::initializer_list<mint>)
		 *
		 *   @warning		It is user's responsibility to make sure that length of v fits into mint!
		 **/
		NumericArray(std::initializer_list<T> v);

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
		 *   @tparam		Container - any type of container that has member \b value_type and this type is convertible to mint
		 *   @throws		see NumericArray<T>::createInternal() and MArray<T>::MArray(Container&&)
		 **/
		template<class Container, typename = enable_if_integral_elements<Container>>
		NumericArray(T init, Container&& dims);

		/**
		 *   @brief         Constructs the NumericArray of given shape with all elements initialized to given value
		 *   @param[in]     init - value of type \b T to initialize all elements of the NumericArray
		 *   @param[in]     dims - initializer list with NumericArray dimensions
		 *   @throws		see NumericArray<T>::createInternal() and MArray<T>::MArray(std::initializer_list<mint>)
		 **/
		NumericArray(T init, std::initializer_list<mint> dims);

		/**
		 *   @brief         Constructs the NumericArray of given shape with elements from range [first, last)
		 *   @param[in]     first - iterator to the beginning of range
		 *   @param[in]		last - iterator past the end of range
		 *   @param[in]     dims - container with NumericArray dimensions
		 *   @tparam		Container - any type of container that has member \b value_type and this type is convertible to mint
		 *   @throws		LLErrorName::NumericArrayNewError - if number of elements in \c v does not match total NumericArray size indicated by \c dims
		 *   @throws		see NumericArray<T>::createInternal() and MArray<T>::MArray(Container&&)
		 **/
		template<
			class InputIt,
			class Container,
			typename = enable_if_integral_elements<Container>,
			typename = enable_if_input_iterator<InputIt>
		>
		NumericArray(InputIt first, InputIt last, Container&& dims);

		/**
		 *   @brief         Constructs the NumericArray of given shape with elements from range [first, last)
		 *   @param[in]     first - iterator to the beginning of range
		 *   @param[in]		last - iterator past the end of range
		 *   @param[in]     dims - initializer list with NumericArray dimensions
		 *   @throws		LLErrorName::NumericArrayNewError - if number of elements in \c v does not match total NumericArray size indicated by \c dims
		 *   @throws		see NumericArray<T>::createInternal() and MArray<T>::MArray(const std::vector<U>&)
		 **/
		template<class InputIt, typename = enable_if_input_iterator<InputIt>>
		NumericArray(InputIt first, InputIt last, std::initializer_list<mint> dims);

		/**
		 *   @brief
		 *   @param[in]     na -
		 *   @throws		LLErrorName::NumericArrayTypeError - if template parameter \b T does not match MNumericArray data type
		 **/
		NumericArray(MContainer<MArgumentType::NumericArray, PassingMode> na);

		NumericArray(MNumericArray na);

		/**
		 *   @brief         Create NumericArray from generic NumericArray
		 *   @param[in]     other - const reference to a generic NumericArray
		 *   @param[in]		method - conversion method to be used
		 **/
		template<class P>
		NumericArray(const MContainer<MArgumentType::NumericArray, P>& other, NA::ConversionMethod method = NA::ConversionMethod::ClipRound, double param = 0.0);

		/**
		 *   @brief         Copy constructor
		 *   @param[in]     other - const reference to a NumericArray of matching type
		 **/
        template<class P>
		NumericArray(const NumericArray<T, P>& other) : TypedNumericArray<T>(other), GenericNumericArray(other) {}
		
		/**
		 *   @brief         Move constructor
		 *   @param[in]     other - rvalue reference to a NumericArray of matching type
		 **/
        template<class P>
		NumericArray(NumericArray<T, P>&& other) : TypedNumericArray<T>(std::move(other)), GenericNumericArray(std::move(other)) {}


		/**
		 *   @brief         Copy-assignment operator
		 *   @param[in]     other - const reference to a NumericArray of matching type
		 **/
        template<class P>
		NumericArray& operator=(const NumericArray<T, P>& other) {
            TypedNumericArray<T>::operator=(other);
            GenericNumericArray::operator=(other);
            return *this;
		}

		/**
		 *   @brief         Move-assignment operator
		 *   @param[in]     other - rvalue reference to a NumericArray of matching type
		 **/
        template<class P>
        NumericArray& operator=(NumericArray<T, P>&& other) {
            TypedNumericArray<T>::operator=(std::move(other));
            GenericNumericArray::operator=(std::move(other));
            return *this;
        }


		/**
		 *    @brief	Free internal MNumericArray if necessary
		 **/
		~NumericArray() = default;

	private:
		using GenericNumericArray = MContainer<MArgumentType::NumericArray, PassingMode>;

        MNumericArray getInternal() const noexcept override {
            return this->getContainer();
        }

		/**
		 *   @brief 	Sub-class implementation of virtual void MArray<T>::indexError()
		 *   @throws 	LibraryLinkError(LLErrorName::NumericArrayIndexError)
		 **/
		void indexError() const override {
			ErrorManager::throwException(ErrorName::NumericArrayIndexError);
		}

		/**
		 *   @brief 	Sub-class implementation of virtual void MArray<T>::sizeError()
		 *   @throws 	LibraryLinkError(LLErrorName::NumericArraySizeError)
		 **/
		void sizeError() const override {
			ErrorManager::throwException(ErrorName::NumericArraySizeError);
		}
	};



	template<typename T, class PassingMode>
	template<typename Container, typename>
	NumericArray<T, PassingMode>::NumericArray(Container&& v) :
			NumericArray(std::begin(v), std::end(v), { static_cast<mint>(v.size()) }) {
	}

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
	template<class Container, typename>
	NumericArray<T, PassingMode>::NumericArray(T init, Container&& dims) : TypedNumericArray<T>(std::forward<Container>(dims)),
	        GenericNumericArray(TypedNumericArray<T>::getType(), this->depth, this->dimensionsData()) {
		std::fill(this->begin(), this->end(), init);
	}

	template<typename T, class PassingMode>
	NumericArray<T, PassingMode>::NumericArray(T init, std::initializer_list<mint> dims) : TypedNumericArray<T>(dims),
	        GenericNumericArray(TypedNumericArray<T>::getType(), this->depth, this->dimensionsData()) {
		std::fill(this->begin(), this->end(), init);
	}

	template<typename T, class PassingMode>
	template<class InputIt, class Container, typename, typename>
	NumericArray<T, PassingMode>::NumericArray(InputIt first, InputIt last, Container&& dims) : TypedNumericArray<T>(std::forward<Container>(dims)),
	        GenericNumericArray(TypedNumericArray<T>::getType(), this->depth, this->dimensionsData()) {
		if (std::distance(first, last) != this->flattenedLength)
			ErrorManager::throwException(ErrorName::NumericArrayNewError, "Length of data range does not match specified dimensions");
		std::copy(first, last, this->begin());
	}

	template<typename T, class PassingMode>
	template<class InputIt, typename>
	NumericArray<T, PassingMode>::NumericArray(InputIt first, InputIt last, std::initializer_list<mint> dims) :TypedNumericArray<T>(dims),
	        GenericNumericArray(TypedNumericArray<T>::getType(), this->depth, this->dimensionsData()) {
		if (std::distance(first, last) != this->flattenedLength)
			ErrorManager::throwException(ErrorName::NumericArrayNewError, "Length of data range does not match specified dimensions");
		std::copy(first, last, this->begin());
	}

	template<typename T, class PassingMode>
	NumericArray<T, PassingMode>::NumericArray(GenericNumericArray na) : TypedNumericArray<T>(na.getDimensions(), na.getRank()),
			GenericNumericArray(std::move(na)) {
		if (TypedNumericArray<T>::getType() != GenericNumericArray::type())
			ErrorManager::throwException(ErrorName::NumericArrayTypeError);
	}

	template<typename T, class PassingMode>
	NumericArray<T, PassingMode>::NumericArray(MNumericArray na) : NumericArray(GenericNumericArray { na }) {}

	template<typename T, class PassingMode>
	template<class P>
	NumericArray<T, PassingMode>::NumericArray(const MContainer<MArgumentType::NumericArray, P>& other, NA::ConversionMethod method, double param) :
			TypedNumericArray<T>(other.getDimensions(), other.getRank()), GenericNumericArray(other.convert(this->getType(), method, param)) {
	}

} /* namespace LLU */

#endif /* LLUTILS_NUMERICARRAY_H_ */
