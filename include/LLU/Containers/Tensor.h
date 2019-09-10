/** 
 * @file	Tensor.h
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @date	6/07/2017
 *
 * @brief	Templated C++ wrapper for MTensor
 *
 */
#ifndef LLUTILS_TENSOR_H_
#define LLUTILS_TENSOR_H_

#include <initializer_list>
#include <type_traits>

#include "WolframLibrary.h"

#include "LLU/Containers/MArray.hpp"
#include "LLU/Containers/Generic/Tensor.hpp"
#include "LLU/Containers/Passing/PassingPolicy.hpp"
#include "LLU/Containers/Passing/Automatic.hpp"
#include "LLU/Containers/Passing/Manual.hpp"
#include "LLU/Utilities.hpp"

namespace LLU {

	/**
	 *  @brief  Typed interface for Tensor.
	 *
	 *  Provides iterators, data access and info about dimensions.
	 *  @tparam T - type of data in Tensor
	 */
    template<typename T>
    class TypedTensor : public MArray<T> {
    public:
        using MArray<T>::MArray;

        /// Return matching type of MTensor
        static mint getType() noexcept {
            return type;
        }

    private:
		/// @copydoc MArray<T>::getData()
        T* getData() const noexcept override;

        /// Get the raw MTensor, must be implemented in subclasses.
        virtual MTensor getInternal() const = 0;

        /// Tensor data type matching template parameter T
        static const mint type;
    };

	/**
	 * @class Tensor
	 * @brief This is a class template, where template parameter T is the type of data elements. Tensor is derived from MArray.
	 *
	 * Tensor<> classes automate creation and deletion of MTensors.
	 * They are strongly typed (no void* to underlying memory) and almost all functions from \<algorithms\> can be used on Tensor.
	 *
	 * @tparam	T - type of underlying data
	 */
	template<typename T, class PassingMode = Passing::Manual>
	class Tensor: public TypedTensor<T>, public GenericTensor<PassingMode> {
	public:

		/**
		 *   @brief         Constructs flat Tensor based on a list of elements
		 *   @param[in]     v - initializer list with Tensor elements
		 *   @throws		see Tensor<T>::Tensor(InputIt, InputIt, std::initializer_list<mint>)
		 *
		 *   @warning		It is user's responsibility to make sure that length of v fits into mint!
		 **/
		Tensor(std::initializer_list<T> v);

		/**
		 * @brief   Constructs flat Tensor with contents copied from a given collection of data
		 * @tparam  Container - any iterable (begin(), end()) collection of data that has a \c value_type alias member and a size() member function
		 * @param   c - const reference to a collection from which data will be copied to the Tensor
		 */
		template<class Container, typename = std::enable_if_t<is_iterable_container_with_matching_type_v<Container, T> && has_size_v<Container>>>
		explicit Tensor(const Container& c) : Tensor(c, {static_cast<mint>(c.size()) }) {}

		/**
		 * @brief   Constructs a Tensor with contents copied from a given collection of data and dimensions passed as parameter
		 * @tparam  Container  - any iterable (begin(), end()) collection of data that has a \c value_type alias member
		 * @param   c - const reference to a collection from which data will be copied to the Tensor
		 * @param   dims - dimensions of the Tensor
		 */
		template<class Container, typename = std::enable_if_t<is_iterable_container_with_matching_type_v<Container, T>>>
		Tensor(const Container& c, MArrayDimensions dims) : Tensor(std::begin(c), std::end(c), std::move(dims)) {}

		/**
		 *   @brief         Constructs flat Tensor with elements from range [first, last)
		 *   @param[in]     first - iterator to the beginning of range
		 *   @param[in]		last - iterator past the end of range
		 *   @tparam		InputIt - any iterator conforming to InputIterator concept
		 *   @throws		see Tensor<T>::Tensor(InputIt, InputIt, std::initializer_list<mint>)
		 *
		 *   @warning		It is user's responsibility to make sure that length of range fits into mint!
		 *   @note			Be aware that efficiency of this constructor strongly depends on whether the InputIt is also a RandomAccessIterator
		 **/
		template<class InputIt, typename = enable_if_input_iterator<InputIt>>
		Tensor(InputIt first, InputIt last);

		/**
		 *   @brief         Constructs the Tensor of given shape with all elements initialized to given value
		 *   @param[in]     init - value of type \b T to initialize all elements of the Tensor
		 *   @param[in]     dims - MArrayDimensions object with Tensor dimensions
		 **/
		Tensor(T init, MArrayDimensions dims);

		/**
		 *   @brief         Constructs the Tensor of given shape with elements from range [first, last)
		 *   @param[in]     first - iterator to the beginning of range
		 *   @param[in]		last - iterator past the end of range
		 *   @param[in]     dims - container with Tensor dimensions
		 *   @tparam		InputIt - any iterator conforming to InputIterator concept
		 *   @throws		ErrorName::TensorNewError - if number of elements in \c v does not match total Tensor size indicated by \c dims
		 **/
		template<class InputIt, typename = enable_if_input_iterator<InputIt>>
		Tensor(InputIt first, InputIt last, MArrayDimensions dims);

		/**
		 *  @brief  Create new Tensor from MTensor
		 *  @param  t - MTensor
		 */
		explicit Tensor(MTensor t);

		/**
		 *   @brief     Create new Tensor from a GenericTensor
		 *   @param[in] t - generic Tensor to be wrapped into Tensor class
		 *   @throws	ErrorName::TensorTypeError - if the Tensor template type \b T does not match the actual data type of the generic Tensor
		 **/
		explicit Tensor(GenericTensor<PassingMode> t);

		/**
		 *   @brief         Create new Tensor from Tensor of different passing policy by making a copy
		 *   @param[in]     other - const reference to a generic Tensor
		 **/
		template<class P>
		explicit Tensor(const Tensor<T, P>& other);
		
		/**
		 *  @brief  Default constructor, creates a Tensor that does not wrap over any raw MTensor
		 */
		Tensor() = default;

		/**
		 *   @brief        	Copy constructor
		 *   @param[in]     other - const reference to a Tensor of matching type
		 **/
		Tensor(const Tensor& other) = default;

		/**
		 *   @brief         Move constructor
		 *   @param[in]     other - rvalue reference to a Tensor of matching type
		 **/
		Tensor(Tensor&& other) noexcept = default;

		/**
		 *   @brief	Free internal MTensor if necessary
		 **/
		~Tensor() = default;

		/// Default move-assignment operator
		Tensor& operator=(Tensor&&) noexcept = default;

		/// Default copy-assignment operator
		Tensor& operator=(const Tensor&) = default;

		/**
		 *   @brief         Copy-assignment operator
		 *   @param[in]     other - const reference to a Tensor of matching type
		 **/
		template<class P>
		Tensor& operator=(const Tensor<T, P>& other) {
			TypedTensor<T>::operator=(other);
			GenericBase::operator=(other);
			return *this;
		}

	private:
		using GenericBase = MContainer<MArgumentType::Tensor, PassingMode>;

		/// @copydoc MContainerBase::getContainer()
        MTensor getInternal() const noexcept override {
            return this->getContainer();
        }
	};

	template<typename T, class PassingMode>
	Tensor<T, PassingMode>::Tensor(std::initializer_list<T> v) :
			Tensor(std::begin(v), std::end(v), { static_cast<mint>(v.size()) }) {
	}


	template<typename T, class PassingMode>
	template<class InputIt, typename>
	Tensor<T, PassingMode>::Tensor(InputIt first, InputIt last)  :
			Tensor(first, last, { static_cast<mint>(std::distance(first, last)) }) {
	}

	template<typename T, class PassingMode>
	Tensor<T, PassingMode>::Tensor(T init, MArrayDimensions dims) : TypedTensor<T>(std::move(dims)),
	            GenericBase(TypedTensor<T>::getType(), this->rank(), this->dims.data()) {
		std::fill(this->begin(), this->end(), init);
	}

	template<typename T, class PassingMode>
	template<class InputIt, typename>
	Tensor<T, PassingMode>::Tensor(InputIt first, InputIt last, MArrayDimensions dims) : TypedTensor<T>(std::move(dims)),
	        GenericBase(TypedTensor<T>::getType(), this->rank(), this->dims.data()) {
		if (std::distance(first, last) != this->getFlattenedLength())
			ErrorManager::throwException(ErrorName::TensorNewError, "Length of data range does not match specified dimensions");
		std::copy(first, last, this->begin());
	}

	template<typename T, class PassingMode>
	Tensor<T, PassingMode>::Tensor(GenericBase t) : TypedTensor<T>({t.getDimensions(), t.getRank()}), GenericBase(std::move(t)) {
		if (TypedTensor<T>::getType() != GenericBase::type())
			ErrorManager::throwException(ErrorName::TensorTypeError);
	}

	template<typename T, class PassingMode>
	template<class P>
	Tensor<T, PassingMode>::Tensor(const Tensor<T, P>& other) : TypedTensor<T>(other), GenericBase(other) {
	}

	template<typename T, class PassingMode>
	Tensor<T, PassingMode>::Tensor(MTensor t) : Tensor(GenericBase{ t }) {}

} /* namespace LLU */

#endif /* LLUTILS_TENSOR_H_ */
