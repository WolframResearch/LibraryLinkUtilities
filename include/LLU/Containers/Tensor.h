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
#include "LLU/Containers/MContainer.hpp"
#include "LLU/Containers/Passing/PassingPolicy.hpp"
#include "LLU/Containers/Passing/Automatic.hpp"
#include "LLU/Containers/Passing/Manual.hpp"
#include "LLU/Utilities.hpp"

namespace LibraryLinkUtils {

    template<typename T>
    class TypedTensor : public MArray<T> {
    public:
        using MArray<T>::MArray;

        /**
         *   @brief         Get channel value at specified position in 2D Tensor
         *   @param[in]     row - pixel row (in Mathematica-style indexing - starting from 1)
         *   @param[in]     col - pixel column (in Mathematica-style indexing - starting from 1)
         *   @param[in]     channel - desired channel (in Mathematica-style indexing - starting from 1)
         *   @throws		LLErrorName::TensorIndexError - if the specified coordinates are out-of-bound
         **/
        T get(mint* pos) const {
            return getValueAt(pos);
        }

        /**
         *   @brief         Set channel value at specified position in 2D Tensor
         *   @param[in]     row - pixel row (in Mathematica-style indexing - starting from 1)
         *   @param[in]     col - pixel column (in Mathematica-style indexing - starting from 1)
         *   @param[in]     channel - desired channel (in Mathematica-style indexing - starting from 1)
         *   @param[in]		newValue - new channel value
         *   @throws		LLErrorName::TensorIndexError - if the specified coordinates are out-of-bound
         **/
        void set(mint* pos, T newValue) {
            setValueAt(pos, newValue);
        }

        /**
		 *   @brief	Return matching type of MTensor
		 **/
        static mint getType() noexcept {
            return type;
        }

    private:

        T* getData() const noexcept override;

        virtual void indexError() const = 0;

        virtual MTensor getInternal() const = 0;

        T getValueAt(mint* pos, mint channel) const;

        void setValueAt(mint* pos, mint channel, T newValue);

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
	class Tensor: public TypedTensor<T>, public MContainer<MArgumentType::Tensor, PassingMode> {
	public:

		/**
		 *   @brief         Constructs flat Tensor based on a container of elements
		 *   @param[in]     v - container
		 *   @tparam		Container - any container with elements convertible to type \b T
		 *   @throws		see Tensor<T>::Tensor(InputIt, InputIt, std::initializer_list<mint>)
		 *
		 *   @warning		It is user's responsibility to make sure that length of v fits into mint!
		 **/
		template<typename Container, typename = disable_if_same_or_derived<Tensor, Container>>
		explicit Tensor(Container&& v);

		/**
		 *   @brief         Constructs flat Tensor based on a list of elements
		 *   @param[in]     v - initializer list with Tensor elements
		 *   @throws		see Tensor<T>::Tensor(InputIt, InputIt, std::initializer_list<mint>)
		 *
		 *   @warning		It is user's responsibility to make sure that length of v fits into mint!
		 **/
		Tensor(std::initializer_list<T> v);

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
		 *   @param[in]     dims - container with Tensor dimensions
		 *   @tparam		Container - any type of container that has member \b value_type and this type is integral
		 *   @throws		see Tensor<T>::createInternal() and MArray<T>::MArray(Container&&)
		 **/
		template<class Container, typename = enable_if_integral_elements<Container>>
		Tensor(T init, Container&& dims);

		/**
		 *   @brief         Constructs the Tensor of given shape with all elements initialized to given value
		 *   @param[in]     init - value of type \b T to initialize all elements of the Tensor
		 *   @param[in]     dims - initializer list with Tensor dimensions
		 *   @throws		see Tensor<T>::createInternal() and MArray<T>::MArray(std::initializer_list<mint>)
		 **/
		Tensor(T init, std::initializer_list<mint> dims);

		/**
		 *   @brief         Constructs the Tensor of given shape with elements from range [first, last)
		 *   @param[in]     first - iterator to the beginning of range
		 *   @param[in]		last - iterator past the end of range
		 *   @param[in]     dims - container with Tensor dimensions
		 *   @tparam		Container - any type of container that has member \b value_type and this type is convertible to mint
		 *   @throws		LLErrorName::TensorNewError - if number of elements in \c v does not match total Tensor size indicated by \c dims
		 *   @throws		see Tensor<T>::createInternal() and MArray<T>::MArray(Container&&)
		 **/
		template<class InputIt, class Container, typename = enable_if_integral_elements<Container>, typename = enable_if_input_iterator<InputIt>>
		Tensor(InputIt first, InputIt last, Container&& dims);

		/**
		 *   @brief         Constructs the Tensor of given shape with elements from range [first, last)
		 *   @param[in]     first - iterator to the beginning of range
		 *   @param[in]		last - iterator past the end of range
		 *   @param[in]     dims - initializer list with Tensor dimensions
		 *   @throws		LLErrorName::TensorNewError - if number of elements in \c v does not match total Tensor size indicated by \c dims
		 *   @throws		see Tensor<T>::createInternal() and MArray<T>::MArray(const std::vector<U>&)
		 **/
		template<class InputIt, typename = enable_if_input_iterator<InputIt>>
		Tensor(InputIt first, InputIt last, std::initializer_list<mint> dims);

		/**
		 *   @brief
		 *   @param[in]
		 *   @throws		LLErrorName::TensorTypeError - if template parameter \b T does not match MTensor data type
		 **/
		Tensor(MContainer<MArgumentType::Tensor, PassingMode> t);

		Tensor(MTensor t);

		Tensor(const Tensor&) = default;

		Tensor(Tensor&&) = default;

		/**
		 *   @brief	Free internal MTensor if necessary
		 **/
		~Tensor() = default;

	private:
		using GenericTensor = MContainer<MArgumentType::Tensor, PassingMode>;

        MTensor getInternal() const noexcept override {
            return this->getContainer();
        }

		/**
		 *   @brief 	Sub-class implementation of virtual void MArray<T>::indexError()
		 *   @throws 	LibraryLinkError(LLErrorName::TensorIndexError)
		 **/
		void indexError() const override {
			ErrorManager::throwException(LLErrorName::TensorIndexError);
		}

		/**
		 *   @brief 	Sub-class implementation of virtual void MArray<T>::sizeError()
		 *   @throws 	LibraryLinkError(LLErrorName::TensorSizeError)
		 **/
		void sizeError() const override {
			ErrorManager::throwException(LLErrorName::TensorSizeError);
		}

	};

	template<typename T, class PassingMode>
	template<typename Container, typename>
	Tensor<T, PassingMode>::Tensor(Container&& v) :
			Tensor(std::begin(v), std::end(v), { static_cast<mint>(v.size()) }) {
	}

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
	template<class Container, typename>
	Tensor<T, PassingMode>::Tensor(T init, Container&& dims) : TypedTensor<T>(std::forward<Container>(dims)),
	            GenericTensor(TypedTensor<T>::getType(), this->depth, this->dimensionsData()) {
		std::fill(this->begin(), this->end(), init);
	}

	template<typename T, class PassingMode>
	Tensor<T, PassingMode>::Tensor(T init, std::initializer_list<mint> dims) : TypedTensor<T>(dims),
	        GenericTensor(TypedTensor<T>::getType(), this->depth, this->dimensionsData()) {
		std::fill(this->begin(), this->end(), init);
	}

	template<typename T, class PassingMode>
	template<class InputIt, class Container, typename, typename>
	Tensor<T, PassingMode>::Tensor(InputIt first, InputIt last, Container&& dims) : TypedTensor<T>(std::forward<Container>(dims),
	        GenericTensor(TypedTensor<T>::getType(), this->depth, this->dimensionsData())) {
		if (std::distance(first, last) != this->flattenedLength)
			ErrorManager::throwException(LLErrorName::TensorNewError, "Length of data range does not match specified dimensions");
		std::copy(first, last, this->begin());
	}

	template<typename T, class PassingMode>
	template<class InputIt, typename>
	Tensor<T, PassingMode>::Tensor(InputIt first, InputIt last, std::initializer_list<mint> dims) : TypedTensor<T>(dims),
	        GenericTensor(TypedTensor<T>::getType(), this->depth, this->dimensionsData()) {
		if (std::distance(first, last) != this->flattenedLength)
			ErrorManager::throwException(LLErrorName::TensorNewError, "Length of data range does not match specified dimensions");
		std::copy(first, last, this->begin());
	}

	template<typename T, class PassingMode>
	Tensor<T, PassingMode>::Tensor(GenericTensor t) : TypedTensor<T>(t.getDimensions(), t.getRank()), GenericTensor(std::move(t)) {
		if (TypedTensor<T>::getType() != GenericTensor::type())
			ErrorManager::throwException(LLErrorName::TensorTypeError);
	}

	template<typename T, class PassingMode>
	Tensor<T, PassingMode>::Tensor(MTensor na) : Tensor(GenericTensor{ na }) {}
	
} /* namespace LibraryLinkUtils */

#endif /* LLUTILS_TENSOR_H_ */
