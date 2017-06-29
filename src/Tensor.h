/** 
 * @file	Tensor.h
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @date	18/04/2017
 *
 * @brief	Templated C++ wrapper for MTensor
 *
 */
#ifndef LLUTILS_TENSOR_H_
#define LLUTILS_TENSOR_H_

#include "WolframLibrary.h"

#include <vector>

#include "LibraryLinkError.hpp"
#include "MArray.hpp"

namespace LibraryLinkUtils {

	/**
	 * @class Tensor
	 * @brief This is a class template, where template parameter T is the type of data elements. Tensor is derived from MArray.
	 *
	 * Tensor<> classes automate creation and deletion of MTensors.
	 * They are strongly typed (no void* to underlying memory) and almost all functions from \<algorithms\> can be used on Tensor.
	 *
	 * @tparam	T - type of underlying data
	 */
	template<typename T>
	class Tensor: public MArray<T> {

		///	Short name for exception class
		using LibraryLinkError = LibraryLinkUtils::LibraryLinkError<LLErrorCode>;

	public:
		/**
		 *   @brief         Constructs flat Tensor based on a vector of elements
		 *   @param[in]     v - const& to vector of elements of type \b T
		 *   @throws		see Tensor<T>::Tensor(T init, const std::vector<mint>&)
		 *
		 *   @note			It is user's responsibility to make sure that length of v fits into mint!
		 **/
		Tensor(const std::vector<T>& v);

		/// @copydoc Tensor::Tensor(const std::vector<T>&)
		Tensor(std::initializer_list<T> v);

		/**
		 *   @brief         Constructs the Tensor of given shape with all elements initialized to given value
		 *   @param[in]     init - value of type \b T to initialize all elements of the Tensor
		 *   @param[in]     dims - const& to vector of dimensions
		 *   @tparam		U - any type convertible to mint
		 *   @throws		see Tensor<T>::createInternal() and MArray<T>::MArray(const std::vector<U>&)
		 **/
		template<class Container, typename = typename std::enable_if<std::is_convertible<typename Container::value_type, mint>::value>::type>
		Tensor(T init, Container&& dims);

		/**
		 *   @brief         Constructs the Tensor of given shape with all elements initialized to given value
		 *   @param[in]     init - value of type \b T to initialize all elements of the Tensor
		 *   @param[in]     dims - initializer list with Tensor dimensions
		 *   @throws		see Tensor<T>::createInternal() and MArray<T>::MArray(const std::vector<U>&)
		 **/
		Tensor(T init, std::initializer_list<mint> dims);

		/**
		 *   @brief         Constructs the Tensor of given shape with element values taken from a given vector
		 *   @param[in]     v - const& to vector of values of type \b T to initialize all elements of the Tensor
		 *   @param[in]     dims - const& to vector of dimensions
		 *   @tparam		U - any type convertible to mint
		 *   @throws		LLErrorCode::TensorNewError - if number of elements in \c v does not match total Tensor size indicated by \c dims
		 *   @throws		see Tensor<T>::createInternal() and MArray<T>::MArray(const std::vector<U>&)
		 **/
		template<class Container, typename = typename std::enable_if<std::is_convertible<typename Container::value_type, mint>::value>::type>
		Tensor(const std::vector<T>& v, Container&& dims);

		/**
		 *   @brief         Constructs the Tensor of given shape with element values taken from a given vector
		 *   @param[in]     v - const& to vector of values of type \b T to initialize all elements of the Tensor
		 *   @param[in]     dims - initializer list with Tensor dimensions
		 *   @throws		LLErrorCode::TensorNewError - if number of elements in \c v does not match total Tensor size indicated by \c dims
		 *   @throws		see Tensor<T>::createInternal() and MArray<T>::MArray(const std::vector<U>&)
		 **/
		Tensor(const std::vector<T>& v, std::initializer_list<mint> dims);

		/**
		 *   @brief         Constructs Tensor based on MTensor
		 *   @param[in]     v - LibraryLink structure to be wrapped
		 *   @throws        LLErrorCode::TensorInitError - if WolframLibraryData structure is not initialized
		 *   @throws		LLErrorCode::TensorTypeError - if template parameter \b T does not match MTensor data type
		 **/
		Tensor(const MTensor v);

		/**
		 *   @brief         Move constructor
		 *   @param[in]     t2 - rvalue reference to a Tensor of matching type
		 **/
		Tensor(Tensor<T>&& t2);

		/**
		 *   @brief	Free internal MTensor if necessary
		 **/
		virtual ~Tensor();

		/**
		 *   @brief	Return matching type of MTensor
		 **/
		unsigned char getType() const noexcept {
			return type;
		}
	private:
		/**
		 *   @brief Return raw pointer to underlying data
		 **/
		T* getData() const noexcept override;

		/**
		 *   @brief 	Sub-class implementation of virtual void MArray<T>::indexError()
		 *   @throws 	LibraryLinkError(LLErrorCode::TensorIndexError)
		 **/
		void indexError() const override {
			throw LibraryLinkError(LLErrorCode::TensorIndexError);
		}

		/**
		 *   @brief 	Sub-class implementation of virtual void MArray<T>::initError()
		 *   @throws 	LibraryLinkError(LLErrorCode::TensorInitError)
		 **/
		void initError() const override {
			throw LibraryLinkError(LLErrorCode::TensorInitError);
		}

		/**
		 *   @brief 	Sub-class implementation of virtual void MArray<T>::sizeError()
		 *   @throws 	LibraryLinkError(LLErrorCode::TensorSizeError)
		 **/
		void sizeError() const override {
			throw LibraryLinkError(LLErrorCode::TensorSizeError);
		}


		/**
		 *   @brief 	Sub-class implementation of virtual void MArray<T>::createInternal(). It creates a new flat MTensor.
		 *   @throws 	LibraryLinkError(LLErrorCode::TensorNewError)
		 *
		 *   @see 		<http://reference.wolfram.com/language/LibraryLink/ref/callback/MTensor_new.html>
		 *
		 **/
		void createInternal() override {
			if (this->libData->MTensor_new(type, 1, &(this->flattenedLength), &internalMT))
				throw LibraryLinkError(LLErrorCode::TensorNewError);
		}

		/**
		 *   @brief 	Sub-class implementation of virtual void MArray<T>::freeInternal(). It frees internal MTensor.
		 *   @see 		<http://reference.wolfram.com/language/LibraryLink/ref/callback/MTensor_free.html>
		 **/
		void freeInternal() noexcept override {
			this->libData->MTensor_free(internalMT);
		}

		/**
		 *   @brief 	Sub-class implementation of virtual void MArray<T>::passInternal(MArgument&). Passes internal MTensor as result to LibraryLink.
		 **/
		void passInternal(MArgument& res) noexcept override {
			MArgument_setMTensor(res, internalMT);
		}

		/// MTensor type matching template parameter T
		static const unsigned char type;

		/// Internal container
		MTensor internalMT = nullptr;
	};

	template<typename T>
	Tensor<T>::Tensor(const std::vector<T>& v) :
			Tensor<T>(v, { static_cast<mint>(v.size()) }) {
	}

	template<typename T>
	Tensor<T>::Tensor(std::initializer_list<T> v) :
			Tensor<T>(v, { static_cast<mint>(v.size()) }) {
	}

	template<typename T>
	template<class Container, typename>
	Tensor<T>::Tensor(T init, Container&& dims) : MArray<T>(dims) {
		createInternal();
		this->arrayOwnerQ = true;
		std::fill(this->begin(), this->end(), init);
	}

	template<typename T>
	Tensor<T>::Tensor(T init, std::initializer_list<mint> dims) : MArray<T>(dims) {
		createInternal();
		this->arrayOwnerQ = true;
		std::fill(this->begin(), this->end(), init);
	}


	template<typename T>
	template<class Container, typename>
	Tensor<T>::Tensor(const std::vector<T>& v, Container&& dims) : MArray<T>(dims) {
		createInternal();
		if (v.size() != this->flattenedLength)
			LibraryLinkError(LLErrorCode::TensorNewError, "Input data vector does not match specified dimensions");
		this->arrayOwnerQ = true;
		std::copy(std::begin(v), std::end(v), this->begin());
	}

	template<typename T>
	Tensor<T>::Tensor(const std::vector<T>& v, std::initializer_list<mint> dims) : MArray<T>(dims) {
		createInternal();
		if (v.size() != this->flattenedLength)
			LibraryLinkError(LLErrorCode::TensorNewError, "Input data vector does not match specified dimensions");
		this->arrayOwnerQ = true;
		std::copy(std::begin(v), std::end(v), this->begin());
	}

	template<typename T>
	Tensor<T>::Tensor(const MTensor t) {
		if (!this->libData)
			initError();
		if (type != this->libData->MTensor_getType(t))
			throw LibraryLinkError(LLErrorCode::TensorTypeError);
		this->arrayOwnerQ = false;
		internalMT = t;
		this->depth = this->libData->MTensor_getRank(internalMT);
		this->flattenedLength = this->libData->MTensor_getFlattenedLength(internalMT);
		const mint* rawDims = this->libData->MTensor_getDimensions(internalMT);
		this->dims.assign(rawDims, rawDims + this->rank());
		this->fillOffsets();
	}

	template<typename T>
	Tensor<T>::Tensor(Tensor<T> && t2) : MArray<T>(std::move(t2)) {
		this->internalMT = t2.internalMT;
		t2.internalMT = nullptr;
		t2.arrayOwnerQ = false;
	}

	template<typename T>
	Tensor<T>::~Tensor() {
		if (this->arrayOwnerQ)
			this->freeInternal();
	}
} /* namespace LibraryLinkUtils */

#endif /* LLUTILS_TENSOR_H_ */
