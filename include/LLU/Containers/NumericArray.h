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
#include "LLU/LibraryLinkError.h"
#include "LLU/Containers/MContainer.hpp"
#include "LLU/Utilities.hpp"

namespace LibraryLinkUtils {

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

    protected:
        /// Functions from WolframNumericArrayLibrary
        using MArray<T>::naFuns;

    private:
        /**
         *   @brief Return raw pointer to underlying data
         **/
        T* getData() const noexcept override {
            return static_cast<T*>(naFuns->MNumericArray_getData(this->getInternal()));
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
		template<typename Container, typename = disable_if_same_or_derived<NumericArray<T>, Container>>
		NumericArray(Container&& v);

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
		 *   @brief         Constructs NumericArray based on MNumericArray
		 *   @param[in]     na - LibraryLink structure to be wrapped
		 *   @throws        LLErrorName::NumericArrayInitError - if structure with WolframNumericArrayLibrary functions is not initialized
		 *   @throws		LLErrorName::NumericArrayTypeError - if template parameter \b T does not match MNumericArray data type
		 **/
		NumericArray(const MNumericArray na);

		/**
		 *   @brief         Create NumericArray from NumericArray of different type
		 *   @tparam		U - data type of the NumericArray to be converted
		 *   @param[in]     other - const reference to a NumericArray any type
		 *   @param[in]		method - conversion method to be used
		 **/
		template<typename U, class P>
		NumericArray(const NumericArray<U, P>& other, NA::ConversionMethod method = NA::ConversionMethod::ClipRound);

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
			ErrorManager::throwException(LLErrorName::NumericArrayIndexError);
		}

		/**
		 *   @brief 	Sub-class implementation of virtual void MArray<T>::initError()
		 *   @throws 	LibraryLinkError(LLErrorName::NumericArrayInitError)
		 **/
		void initError() const override {
			ErrorManager::throwException(LLErrorName::NumericArrayInitError);
		}

		/**
		 *   @brief 	Sub-class implementation of virtual void MArray<T>::sizeError()
		 *   @throws 	LibraryLinkError(LLErrorName::NumericArraySizeError)
		 **/
		void sizeError() const override {
			ErrorManager::throwException(LLErrorName::NumericArraySizeError);
		}

		/**
		 * @brief 	Set \p na as internal container and extract properties like dimensions, depth and total length.
		 * @param 	na - MNumericArray of matching type
		 */
		void extractPropertiesFromInternal();
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
			ErrorManager::throwException(LLErrorName::NumericArrayNewError, "Length of data range does not match specified dimensions");
		std::copy(first, last, this->begin());
	}

	template<typename T, class PassingMode>
	template<class InputIt, typename>
	NumericArray<T, PassingMode>::NumericArray(InputIt first, InputIt last, std::initializer_list<mint> dims) :TypedNumericArray<T>(dims),
	        GenericNumericArray(TypedNumericArray<T>::getType(), this->depth, this->dimensionsData()) {
		if (std::distance(first, last) != this->flattenedLength)
			ErrorManager::throwException(LLErrorName::NumericArrayNewError, "Length of data range does not match specified dimensions");
		std::copy(first, last, this->begin());
	}

	template<typename T, class PassingMode>
	void NumericArray<T, PassingMode>::extractPropertiesFromInternal() {
		auto internalNA = this->getContainer();
		this->depth = this->naFuns->MNumericArray_getRank(internalNA);
		this->flattenedLength = this->naFuns->MNumericArray_getFlattenedLength(internalNA);
		const mint* rawDims = this->naFuns->MNumericArray_getDimensions(internalNA);
		this->dims.assign(rawDims, rawDims + this->rank());
		this->fillOffsets();
	}

	template<typename T, class PassingMode>
	NumericArray<T, PassingMode>::NumericArray(const MNumericArray na) : GenericNumericArray(na) {
		if (!this->naFuns)
			initError();
		if (TypedNumericArray<T>::getType() != this->naFuns->MNumericArray_getType(na))
			ErrorManager::throwException(LLErrorName::NumericArrayTypeError);
		extractPropertiesFromInternal();
	}

	template<typename T, class PassingMode>
	template<typename U, class P>
	NumericArray<T, PassingMode>::NumericArray(const NumericArray<U, P>& other, NA::ConversionMethod method) : MArray<T>(other),
	        GenericNumericArray(other.convert()) {
		if (!this->naFuns) {
			initError();
		}
		extractPropertiesFromInternal();
	}

} /* namespace LibraryLinkUtils */

#endif /* LLUTILS_NUMERICARRAY_H_ */
